#ifndef NOCRENGINE_H
#define NOCRENGINE_H

#include <QByteArray>
#include <QHash>
#include <QImage>
#include <QList>
#include <QString>
#include <QVector>

#include <functional>

class NOcrEngine
{
public:
    struct OcrLineResult {
        QString text;
        int glyphCount = 0;
        int unknownCount = 0;
        qint64 pts90k = 0;
    };

    NOcrEngine();

    bool loadDatabase(const QByteArray &data, QString *error = nullptr);
    bool isLoaded() const { return !m_glyphs.isEmpty(); }

    void setMaxWrong(int value) { m_maxWrong = value < 1 ? 1 : value; }
    void setSpaceGapFactor(double value) { m_spaceGapFactor = value > 0 ? value : 0.35; }

    QString recognize(const QImage &frame, int *unknownCount = nullptr,
                      int *glyphCount = nullptr) const;
    QList<OcrLineResult> recognizeFrames(
        const QStringList &pngFiles, const QList<qint64> &pts90k,
        const std::function<void(int, int)> &progress = {}) const;

private:
    struct OcrPoint { int x = 0; int y = 0; };
    struct OcrSegment { OcrPoint start; OcrPoint end; };

    struct Glyph {
        QString text;
        int width = 0;
        int height = 0;
        int marginTop = 0;
        bool italic = false;
        int expand = 0;
        QVector<OcrSegment> foreground;
        QVector<OcrSegment> background;
    };

    struct Mask {
        int width = 0;
        int height = 0;
        QVector<quint8> bits;
        bool at(int x, int y) const { return bits.at(y * width + x) != 0; }
    };

    struct Item {
        Mask mask;
        int x = 0;
        int topMargin = 0;
        int gapBefore = -1;
        int lineHeight = 0;
        bool newLine = true;
    };

    struct MatchPass {
        double aspect = -1;
        double size = -1;
        int margin = 0;
        int minLines = 0;
        int sensitivity = 0;
        int allowance = 0;
        int errorKind = 0;
        bool deepSeek = false;
    };

    static QByteArray gunzip(const QByteArray &data, QString *error);
    static QList<Glyph> parseDatabase(const QByteArray &raw, QString *error);
    static Mask makeMask(const QImage &frame);
    static Mask cropMask(const Mask &source, int left, int top, int width, int height);
    static QList<Item> segmentMask(const Mask &mask);
    static OcrPoint scalePoint(const OcrPoint &point, const Glyph &glyph,
                               int width, int height);
    static QVector<OcrPoint> scaledPoints(const OcrSegment &line, const Glyph &glyph,
                                          int width, int height);
    static int matchErrors(const Mask &mask, const Glyph &glyph, int allowed);
    int matchGlyph(const Mask &mask, int topMargin, bool deepSeek) const;
    static int median(QList<int> values);

    QList<Glyph> m_glyphs;
    QHash<quint32, QList<int>> m_bySize;
    int m_maxWrong = 50;
    double m_spaceGapFactor = 0.35;
};

#endif // NOCRENGINE_H
