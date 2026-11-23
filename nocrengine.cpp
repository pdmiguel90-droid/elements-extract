#include "nocrengine.h"

#include <QColor>
#include <QtMath>


#include <algorithm>
#include <climits>
#include <cmath>
#include <limits>

namespace {

int roundAway(double value)
{
    return value >= 0 ? static_cast<int>(std::floor(value + 0.5))
                      : static_cast<int>(std::ceil(value - 0.5));
}

bool isSensitive(const QString &text)
{
    static const QString sensitive = QStringLiteral("Oo0'-:\"");
    return text.size() == 1 && sensitive.contains(text.at(0));
}

class Cursor
{
public:
    Cursor(const QByteArray &data, int pos = 0) : m_data(data), m_pos(pos) {}

    bool ok() const { return m_ok; }
    int pos() const { return m_pos; }

    quint8 u8()
    {
        if (!has(1)) return 0;
        return static_cast<quint8>(m_data.at(m_pos++));
    }

    quint16 u16()
    {
        if (!has(2)) return 0;
        const quint16 value = (static_cast<quint16>(static_cast<quint8>(m_data.at(m_pos))) << 8)
                            | static_cast<quint8>(m_data.at(m_pos + 1));
        m_pos += 2;
        return value;
    }

    QByteArray take(int count)
    {
        if (!has(count)) return QByteArray();
        const QByteArray value = m_data.mid(m_pos, count);
        m_pos += count;
        return value;
    }

private:
    bool has(int count) const
    {
        if (count < 0 || m_pos < 0 || m_pos > m_data.size() - count) {
            m_ok = false;
            return false;
        }
        return true;
    }

    const QByteArray &m_data;
    int m_pos;
    mutable bool m_ok = true;
};

} // namespace

NOcrEngine::NOcrEngine() = default;

QList<NOcrEngine::Glyph> NOcrEngine::parseDatabase(const QByteArray &raw, QString *error)
{
    const bool v2 = raw.startsWith("V2");
    Cursor cursor(raw, v2 ? 2 : 0);
    QList<Glyph> glyphs;

    const auto readSegments = [&cursor](bool shortForm, QVector<OcrSegment> &out) {
        const int count = shortForm ? cursor.u8() : cursor.u16();
        if (!cursor.ok()) return false;
        out.reserve(count);
        for (int i = 0; i < count; ++i) {
            OcrSegment line;
            if (shortForm) {
                const QByteArray bytes = cursor.take(4);
                if (bytes.size() != 4) return false;
                line.start = OcrPoint{static_cast<quint8>(bytes.at(0)),
                                      static_cast<quint8>(bytes.at(1))};
                line.end   = OcrPoint{static_cast<quint8>(bytes.at(2)),
                                      static_cast<quint8>(bytes.at(3))};
            } else {
                line.start = OcrPoint{static_cast<int>(cursor.u16()), static_cast<int>(cursor.u16())};
                line.end   = OcrPoint{static_cast<int>(cursor.u16()), static_cast<int>(cursor.u16())};
            }
            if (!cursor.ok()) return false;
            out.append(line);
        }
        return true;
    };

    while (cursor.ok() && cursor.pos() + 4 < raw.size()) {
        Glyph glyph;
        bool shortForm = false;
        if (v2) {
            const quint8 flags = cursor.u8();
            shortForm = (flags & 0x10) != 0;
            glyph.italic = (flags & 0x20) != 0;
            glyph.expand = flags & 0x0F;
            glyph.width  = shortForm ? cursor.u8() : cursor.u16();
            glyph.height = shortForm ? cursor.u8() : cursor.u16();
            glyph.marginTop = shortForm ? cursor.u8() : cursor.u16();
        } else {
            glyph.width  = cursor.u16();
            glyph.height = cursor.u16();
            glyph.marginTop = cursor.u16();
            glyph.italic = cursor.u8() != 0;
            glyph.expand = cursor.u8();
        }
        if (!cursor.ok()) break;

        const int textLength = cursor.u8();
        const QByteArray textBytes = cursor.take(textLength);
        if (textBytes.size() != textLength) break;
        glyph.text = QString::fromUtf8(textBytes);
        if (glyph.text.contains(QChar('\0'))) break;

        if (!readSegments(shortForm, glyph.foreground)) break;
        if (!readSegments(shortForm, glyph.background)) break;
        if (glyph.width <= 0 || glyph.width > 1920 || glyph.height <= 0 || glyph.height > 1080)
            break;
        glyphs.append(glyph);
    }

    if (glyphs.isEmpty() && error)
        *error = QStringLiteral("base nOCR vacía o con formato desconocido");
    return glyphs;
}

bool NOcrEngine::loadDatabase(const QByteArray &data, QString *error)
{
    m_glyphs.clear();
    m_bySize.clear();

    // La base se distribuye sin comprimir: así no hace falta zlib en Windows.
    QString parseError;
    m_glyphs = parseDatabase(data, &parseError);
    if (m_glyphs.isEmpty()) {
        if (error) *error = parseError;
        return false;
    }

    for (int i = 0; i < m_glyphs.size(); ++i) {
        const Glyph &g = m_glyphs.at(i);
        if (g.expand == 0)
            m_bySize[(static_cast<quint32>(g.width) << 16) | static_cast<quint32>(g.height)] << i;
    }
    return true;
}

NOcrEngine::Mask NOcrEngine::makeMask(const QImage &frame)
{
    Mask mask;
    mask.width = frame.width();
    mask.height = frame.height();
    if (mask.width <= 0 || mask.height <= 0) return mask;

    const QImage rgba = frame.convertToFormat(QImage::Format_RGBA8888);
    mask.bits.fill(0, mask.width * mask.height);

    bool allOpaque = true;
    for (int y = 0; y < mask.height && allOpaque; ++y) {
        const uchar *row = rgba.constScanLine(y);
        for (int x = 0; x < mask.width; ++x) {
            if (row[x * 4 + 3] <= 150) {
                allOpaque = false;
                break;
            }
        }
    }

    bool brightIsText = false;
    bool brightFillOverOutline = false;
    int opaque = 0, dark = 0, bright = 0;
    for (int y = 0; y < mask.height; ++y) {
        const uchar *row = rgba.constScanLine(y);
        for (int x = 0; x < mask.width; ++x) {
            if (row[x * 4 + 3] <= 150) continue;
            ++opaque;
            const int gray = qGray(qRgb(row[x * 4], row[x * 4 + 1], row[x * 4 + 2]));
            if (gray < 80) ++dark;
            if (gray > 160) ++bright;
        }
    }
    if (allOpaque) {
        brightIsText = bright < dark;
    } else if (opaque > 0 && dark * 100 > opaque * 5 && bright * 100 > opaque * 15) {
        brightFillOverOutline = true;
    }

    for (int y = 0; y < mask.height; ++y) {
        const uchar *row = rgba.constScanLine(y);
        for (int x = 0; x < mask.width; ++x) {
            bool foreground;
            if (brightFillOverOutline) {
                const int gray = qGray(qRgb(row[x * 4], row[x * 4 + 1], row[x * 4 + 2]));
                foreground = row[x * 4 + 3] > 150 && gray > 160;
            } else if (allOpaque) {
                const int gray = qGray(qRgb(row[x * 4], row[x * 4 + 1], row[x * 4 + 2]));
                foreground = brightIsText ? gray > 128 : gray < 128;
            } else {
                foreground = row[x * 4 + 3] > 150;
            }
            if (foreground) mask.bits[y * mask.width + x] = 1;
        }
    }
    return mask;
}

NOcrEngine::Mask NOcrEngine::cropMask(const Mask &source, int left, int top, int width, int height)
{
    Mask out;
    out.width = width;
    out.height = height;
    out.bits.fill(0, width * height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x)
            if (source.at(left + x, top + y)) out.bits[y * width + x] = 1;
    }
    return out;
}

QList<NOcrEngine::Item> NOcrEngine::segmentMask(const Mask &mask)
{
    QList<Item> items;
    auto spansOf = [](int length, const auto &hasContent) {
        QList<QPair<int, int>> spans;
        int start = -1;
        for (int i = 0; i < length; ++i) {
            if (hasContent(i)) {
                if (start < 0) start = i;
            } else if (start >= 0) {
                spans.append(qMakePair(start, i - 1));
                start = -1;
            }
        }
        if (start >= 0) spans.append(qMakePair(start, length - 1));
        return spans;
    };

    const auto rows = spansOf(mask.height, [&](int y) {
        for (int x = 0; x < mask.width; ++x)
            if (mask.at(x, y)) return true;
        return false;
    });

    for (const QPair<int, int> &row : rows) {
        const int top = row.first, bottom = row.second;
        if (bottom - top + 1 < 2) continue;

        const auto columns = spansOf(mask.width, [&](int x) {
            for (int y = top; y <= bottom; ++y)
                if (mask.at(x, y)) return true;
            return false;
        });

        int previousRight = 0;
        bool first = true;
        for (const QPair<int, int> &col : columns) {
            const int left = col.first, right = col.second;
            int t = top, b = bottom;
            while (t < b) {
                bool occupied = false;
                for (int x = left; x <= right && !occupied; ++x)
                    if (mask.at(x, t)) occupied = true;
                if (occupied) break;
                ++t;
            }
            while (b > t) {
                bool occupied = false;
                for (int x = left; x <= right && !occupied; ++x)
                    if (mask.at(x, b)) occupied = true;
                if (occupied) break;
                --b;
            }

            Item item;
            item.mask = cropMask(mask, left, t, right - left + 1, b - t + 1);
            item.x = left;
            item.topMargin = t - top;
            item.gapBefore = first ? -1 : left - previousRight - 1;
            item.lineHeight = bottom - top + 1;
            item.newLine = first;
            items.append(item);
            previousRight = right;
            first = false;
        }
    }
    return items;
}

NOcrEngine::OcrPoint NOcrEngine::scalePoint(const OcrPoint &point, const Glyph &glyph,
                                            int width, int height)
{
    return OcrPoint{roundAway(static_cast<double>(point.x) * width / glyph.width),
                    roundAway(static_cast<double>(point.y) * height / glyph.height)};
}

QVector<NOcrEngine::OcrPoint> NOcrEngine::scaledPoints(const OcrSegment &line,
                                                       const Glyph &glyph,
                                                       int width, int height)
{
    QVector<OcrPoint> points;
    const OcrPoint p1 = scalePoint(line.start, glyph, width, height);
    const OcrPoint p2 = scalePoint(line.end, glyph, width, height);
    if (p1.x == p2.x && p1.y == p2.y) {
        points.append(p1);
        return points;
    }

    if (qAbs(p2.x - p1.x) > qAbs(p2.y - p1.y)) {
        const OcrPoint a = p2.x > p1.x ? p1 : p2;
        const OcrPoint b = p2.x > p1.x ? p2 : p1;
        const double factor = static_cast<double>(b.y - a.y) / (b.x - a.x);
        for (int x = a.x; x <= b.x; ++x)
            points.append(OcrPoint{x, roundAway(a.y + factor * (x - a.x))});
    } else {
        const OcrPoint a = p2.y > p1.y ? p1 : p2;
        const OcrPoint b = p2.y > p1.y ? p2 : p1;
        const double factor = static_cast<double>(b.x - a.x) / (b.y - a.y);
        for (int y = a.y; y <= b.y; ++y)
            points.append(OcrPoint{roundAway(a.x + factor * (y - a.y)), y});
    }
    return points;
}

int NOcrEngine::matchErrors(const Mask &mask, const Glyph &glyph, int allowed)
{
    if (glyph.foreground.isEmpty() && glyph.background.isEmpty()) return -1;
    int errors = 0;
    const QVector<QPair<const QVector<OcrSegment> *, bool>> groups = {
        qMakePair(&glyph.foreground, true), qMakePair(&glyph.background, false)
    };
    for (const auto &group : groups) {
        for (const OcrSegment &line : *group.first) {
            const QVector<OcrPoint> points = scaledPoints(line, glyph, mask.width, mask.height);
            for (const OcrPoint &point : points) {
                if (point.x < 0 || point.x >= mask.width || point.y < 0 || point.y >= mask.height)
                    continue;
                if (mask.at(point.x, point.y) != group.second) {
                    if (++errors > allowed) return -1;
                }
            }
        }
    }
    return errors;
}

int NOcrEngine::matchGlyph(const Mask &mask, int topMargin, bool deepSeek) const
{
    if (m_glyphs.isEmpty() || mask.width <= 0 || mask.height <= 0) return -1;
    const quint32 key = (static_cast<quint32>(mask.width) << 16)
                      | static_cast<quint32>(mask.height);
    const auto exact = m_bySize.constFind(key);
    if (exact != m_bySize.constEnd()) {
        for (int index : exact.value()) {
            const Glyph &glyph = m_glyphs.at(index);
            if (qAbs(glyph.marginTop - topMargin) < 5 && matchErrors(mask, glyph, 0) >= 0)
                return index;
        }
    }

    const double aspect = static_cast<double>(mask.height) * 100.0 / mask.width;
    const double inf = std::numeric_limits<double>::infinity();
    static const MatchPass passes[] = {
        {15,  5,  5,  0,  0,  0, 0, false},
        {inf, inf, 8,  0,  0,  1, 1, false},
        {inf, inf, 8,  0,  0,  1, 1, false},
        {20,  inf, 15, 0,  0,  2, 3, false},
        {20,  inf, 15, 41, -1, 10, 4, false},
        {30,  inf, 15, 41,  1, 10, 5, false},
        {60,  inf, 17, 51,  0, 10, 6, true},
    };

    for (const MatchPass &pass : passes) {
        if (pass.deepSeek && !deepSeek) continue;
        if (m_maxWrong < pass.allowance) continue;
        int budget = 0;
        switch (pass.errorKind) {
        case 3:  budget = qMin(3, m_maxWrong); break;
        case 4:  budget = qMin(20, m_maxWrong); break;
        case 6:  budget = m_maxWrong; break;
        case 5:  budget = 10; break;
        default: budget = pass.errorKind == 0 ? 0 : 1; break;
        }
        budget = qMin(budget, 40);

        int best = -1, bestErrors = INT_MAX, bestFit = INT_MAX;
        for (int i = 0; i < m_glyphs.size(); ++i) {
            const Glyph &glyph = m_glyphs.at(i);
            const double glyphAspect = static_cast<double>(glyph.height) * 100.0 / glyph.width;
            if (pass.aspect >= 0 && qAbs(aspect - glyphAspect) >= pass.aspect) continue;
            if (pass.size >= 0 && (qAbs(mask.width - glyph.width) >= pass.size
                                   || qAbs(mask.height - glyph.height) >= pass.size)) continue;
            if (pass.margin > 0 && qAbs(glyph.marginTop - topMargin) >= pass.margin) continue;
            if (pass.minLines > 0
                && glyph.foreground.size() + glyph.background.size() < pass.minLines) continue;
            if (pass.sensitivity < 0 && isSensitive(glyph.text)) continue;
            if (pass.sensitivity > 0 && !isSensitive(glyph.text)) continue;

            const int errors = matchErrors(mask, glyph, qMin(budget, bestErrors));
            if (errors < 0) continue;
            const int fit = qAbs(glyph.marginTop - topMargin)
                          + qAbs(glyph.height - mask.height)
                          + qAbs(glyph.width - mask.width);
            if (errors < bestErrors || (errors == bestErrors && fit < bestFit)) {
                best = i;
                bestErrors = errors;
                bestFit = fit;
            }
        }
        if (best >= 0) return best;
    }
    return -1;
}

int NOcrEngine::median(QList<int> values)
{
    if (values.isEmpty()) return 0;
    std::sort(values.begin(), values.end());
    const int n = values.size();
    if (n % 2) return values.at(n / 2);
    return (values.at(n / 2 - 1) + values.at(n / 2)) / 2;
}

QString NOcrEngine::recognize(const QImage &frame, int *unknownCount,
                              int *glyphCount) const
{
    if (unknownCount) *unknownCount = 0;
    if (glyphCount) *glyphCount = 0;
    if (!isLoaded()) return QString();

    QImage content = frame.convertToFormat(QImage::Format_RGBA8888);
    int left = content.width(), top = content.height(), right = -1, bottom = -1;
    for (int y = 0; y < content.height(); ++y) {
        const uchar *row = content.constScanLine(y);
        for (int x = 0; x < content.width(); ++x) {
            if (row[x * 4 + 3] == 0) continue;
            left = qMin(left, x); right = qMax(right, x);
            top = qMin(top, y); bottom = qMax(bottom, y);
        }
    }
    if (right < left) return QString();
    if ((right - left + 1) * (bottom - top + 1) < content.width() * content.height() * 9 / 10) {
        content = content.copy(left, top, right - left + 1, bottom - top + 1);
    }

    const Mask mask = makeMask(content);
    if (mask.width <= 0 || mask.height <= 0) return QString();
    const QList<Item> items = segmentMask(mask);
    if (items.isEmpty()) return QString();
    if (glyphCount) *glyphCount = items.size();

    QList<int> matched;
    int unknowns = 0;
    for (const Item &item : items) {
        const int index = matchGlyph(item.mask, item.topMargin, true);
        if (index < 0) ++unknowns;
        matched.append(index);
    }
    if (unknownCount) *unknownCount = unknowns;

    static const QString xHeightLetters = QStringLiteral("aemnr");
    static const QString tallLetters = QStringLiteral("bdfhkltABDEFGHIJKLMNPQRTY");
    static const QString sizeTwins = QStringLiteral("cosuvwxz");
    QList<int> xHeights, capHeights;
    for (int i = 0; i < items.size(); ++i) {
        if (matched.at(i) < 0) continue;
        const QString text = m_glyphs.at(matched.at(i)).text;
        if (text.size() != 1) continue;
        if (xHeightLetters.contains(text.at(0))) xHeights.append(items.at(i).mask.height);
        if (tallLetters.contains(text.at(0))) capHeights.append(items.at(i).mask.height);
    }
    const int xHeight = median(xHeights);
    const int capHeight = median(capHeights);
    const bool canJudge = xHeight > 0 && capHeight > xHeight;

    QList<int> lineHeights, gaps;
    for (const Item &item : items) {
        lineHeights.append(item.lineHeight);
        if (!item.newLine && item.gapBefore >= 0) gaps.append(item.gapBefore);
    }
    std::sort(lineHeights.begin(), lineHeights.end());
    std::sort(gaps.begin(), gaps.end());
    const int medianLineHeight = median(lineHeights);
    int spaceGap = qMax(6, qRound(m_spaceGapFactor * medianLineHeight));
    if (gaps.size() >= 3 && medianLineHeight > 0) {
        int bestJump = 0, bestIndex = -1;
        for (int i = 1; i < gaps.size(); ++i) {
            if (gaps.at(i) > medianLineHeight || gaps.at(i - 1) < 1) continue;
            const int jump = gaps.at(i) - gaps.at(i - 1);
            if (jump > bestJump) { bestJump = jump; bestIndex = i; }
        }
        if (bestIndex > 0 && bestJump >= 2)
            spaceGap = (gaps.at(bestIndex - 1) + gaps.at(bestIndex)) / 2;
    }

    QString text;
    for (int i = 0; i < items.size(); ++i) {
        const Item &item = items.at(i);
        if (item.newLine && !text.isEmpty()) {
            text.append(QChar('\n'));
        } else if (!item.newLine && item.gapBefore >= 0) {
            if (item.gapBefore >= spaceGap)
                text.append(QChar(' '));
        }
        if (matched.at(i) < 0) {
            text.append(QChar(0x25A1));
            continue;
        }
        QString value = m_glyphs.at(matched.at(i)).text;
        if (canJudge && value.size() == 1 && sizeTwins.contains(value.at(0), Qt::CaseInsensitive)) {
            const int height = item.mask.height;
            value = qAbs(height - capHeight) < qAbs(height - xHeight)
                  ? value.toUpper() : value.toLower();
        }
        text.append(value);
    }
    return text;
}

QList<NOcrEngine::OcrLineResult> NOcrEngine::recognizeFrames(
    const QStringList &pngFiles, const QList<qint64> &pts90k,
    const std::function<void(int, int)> &progress) const
{
    QList<OcrLineResult> results;
    for (int i = 0; i < pngFiles.size(); ++i) {
        if (progress) progress(i, pngFiles.size());
        OcrLineResult result;
        result.pts90k = i < pts90k.size() ? pts90k.at(i) : 0;
        const QImage frame(pngFiles.at(i));
        if (frame.isNull()) {
            result.unknownCount = 0;
            results.append(result);
            continue;
        }
        result.text = recognize(frame, &result.unknownCount, &result.glyphCount);
        results.append(result);
    }
    return results;
}
