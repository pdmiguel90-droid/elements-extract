#ifndef SUBTITLEPARSER_H
#define SUBTITLEPARSER_H

#include <QString>
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QFileInfo>
#include <QDir>
#include <QTextCodec>

// ── Subtitle cue data structure ─────────────────────────────────────────────
struct SubtitleCue {
    int index;
    qint64 startMs;   // Start time in milliseconds
    qint64 endMs;     // End time in milliseconds
    QString text;     // Dialogue text (plain, no tags)
    QString rawText;  // Original text with styling tags (for ASS)
    QString style;    // ASS style name (empty for SRT/VTT)
};

// ── Subtitle format enum ────────────────────────────────────────────────────
enum class SubtitleFormat {
    SRT,
    ASS,
    SSA,
    VTT,
    Unknown
};

// ── Parser class (header-only) ──────────────────────────────────────────────
class SubtitleParser
{
public:
    // ── Parsing ─────────────────────────────────────────────────────────────
    static QList<SubtitleCue> parseFile(const QString &filePath)
    {
        SubtitleFormat fmt = detectFormat(filePath);
        switch (fmt) {
            case SubtitleFormat::SRT:  return parseSrt(filePath);
            case SubtitleFormat::ASS:
            case SubtitleFormat::SSA:  return parseAss(filePath);
            case SubtitleFormat::VTT:  return parseVtt(filePath);
            default:                   return QList<SubtitleCue>();
        }
    }

    static QList<SubtitleCue> parseSrt(const QString &filePath)
    {
        QList<SubtitleCue> cues;
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return cues;

        QTextStream in(&file);
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        if (codec) in.setCodec(codec);

        static QRegularExpression reTime(
            QRegularExpression::anchoredPattern(
                R"((\d{1,2}):(\d{2}):(\d{2}),(\d{3})\s*-->\s*(\d{1,2}):(\d{2}):(\d{2}),(\d{3}))"));

        int idx = 0;
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.isEmpty()) continue;

            bool ok;
            int cueIndex = line.toInt(&ok);
            if (!ok || cueIndex <= 0) continue;

            QString timeLine = in.readLine().trimmed();
            QRegularExpressionMatch m = reTime.match(timeLine);
            if (!m.hasMatch()) continue;

            qint64 startMs = timeToMs(m.captured(1).toInt(), m.captured(2).toInt(),
                                      m.captured(3).toInt(), m.captured(4).toInt());
            qint64 endMs   = timeToMs(m.captured(5).toInt(), m.captured(6).toInt(),
                                      m.captured(7).toInt(), m.captured(8).toInt());

            QString text;
            while (!in.atEnd()) {
                QString l = in.readLine();
                if (l.trimmed().isEmpty()) break;
                if (!text.isEmpty()) text += "\n";
                text += l.trimmed();
            }

            if (!text.isEmpty()) {
                SubtitleCue cue;
                cue.index   = cueIndex;
                cue.startMs = startMs;
                cue.endMs   = endMs;
                cue.text    = stripHtmlTags(text);
                cue.rawText = text;
                cues.append(cue);
            }
        }

        file.close();
        return cues;
    }

    static QList<SubtitleCue> parseAss(const QString &filePath)
    {
        QList<SubtitleCue> cues;
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return cues;

        QTextStream in(&file);
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        if (codec) in.setCodec(codec);

        bool inEvents = false;
        int startCol = 1, endCol = 3, textCol = 9;
        int idx = 1;

        static QRegularExpression reTime(R"((\d+):(\d{2}):(\d{2})\.(\d{2}))");

        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();

            if (line.startsWith('[')) {
                inEvents = (line.compare("[Events]", Qt::CaseInsensitive) == 0);
                continue;
            }

            if (!inEvents) continue;

            if (line.startsWith("Format:", Qt::CaseInsensitive)) {
                QStringList cols = line.mid(7).split(',');
                for (int i = 0; i < cols.size(); ++i) {
                    QString c = cols[i].trimmed();
                    if (c.compare("Start", Qt::CaseInsensitive) == 0) startCol = i;
                    else if (c.compare("End", Qt::CaseInsensitive) == 0) endCol = i;
                    else if (c.compare("Text", Qt::CaseInsensitive) == 0) textCol = i;
                }
                continue;
            }

            if (!line.startsWith("Dialogue:", Qt::CaseInsensitive)) continue;

            QStringList parts = line.mid(9).split(',');
            if (parts.size() <= textCol) continue;

            QRegularExpressionMatch mStart = reTime.match(parts[startCol].trimmed());
            QRegularExpressionMatch mEnd   = reTime.match(parts[endCol].trimmed());
            if (!mStart.hasMatch() || !mEnd.hasMatch()) continue;

            qint64 startMs = assTimeToMs(mStart.captured(1).toInt(),
                                         mStart.captured(2).toInt(),
                                         mStart.captured(3).toInt(),
                                         mStart.captured(4).toInt());
            qint64 endMs = assTimeToMs(mEnd.captured(1).toInt(),
                                       mEnd.captured(2).toInt(),
                                       mEnd.captured(3).toInt(),
                                       mEnd.captured(4).toInt());

            QString text = parts.mid(textCol).join(',').trimmed();
            text.remove(QRegularExpression(R"(\{\\[^}]*\})"));
            text.replace("\\N", "\n");
            text.replace("\\n", "\n");

            if (!text.isEmpty()) {
                SubtitleCue cue;
                cue.index   = idx++;
                cue.startMs = startMs;
                cue.endMs   = endMs;
                cue.text    = text;
                cue.rawText = text;
                cues.append(cue);
            }
        }

        file.close();
        return cues;
    }

    static QList<SubtitleCue> parseVtt(const QString &filePath)
    {
        QList<SubtitleCue> cues;
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return cues;

        QTextStream in(&file);
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        if (codec) in.setCodec(codec);

        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.isEmpty()) break;
            if (line.startsWith("WEBVTT")) continue;
        }

        static QRegularExpression reTime(
            R"((\d{1,2}):(\d{2}):(\d{2})\.(\d{3})\s*-->\s*(\d{1,2}):(\d{2}):(\d{2})\.(\d{3}))");

        int idx = 1;
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.isEmpty()) continue;

            QRegularExpressionMatch m = reTime.match(line);
            if (!m.hasMatch()) {
                if (!in.atEnd()) {
                    line = in.readLine().trimmed();
                    m = reTime.match(line);
                }
                if (!m.hasMatch()) continue;
            }

            qint64 startMs = timeToMs(m.captured(1).toInt(), m.captured(2).toInt(),
                                      m.captured(3).toInt(), m.captured(4).toInt());
            qint64 endMs   = timeToMs(m.captured(5).toInt(), m.captured(6).toInt(),
                                      m.captured(7).toInt(), m.captured(8).toInt());

            QString text;
            while (!in.atEnd()) {
                QString l = in.readLine();
                if (l.trimmed().isEmpty()) break;
                if (!text.isEmpty()) text += "\n";
                text += l.trimmed();
            }

            if (!text.isEmpty()) {
                SubtitleCue cue;
                cue.index   = idx++;
                cue.startMs = startMs;
                cue.endMs   = endMs;
                cue.text    = stripHtmlTags(text);
                cue.rawText = text;
                cues.append(cue);
            }
        }

        file.close();
        return cues;
    }

    // ── Writing (always SRT for maximum compatibility) ──────────────────────
    static bool writeSrt(const QString &filePath, const QList<SubtitleCue> &cues)
    {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return false;

        QTextStream out(&file);
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        if (codec) out.setCodec(codec);

        for (int i = 0; i < cues.size(); ++i) {
            const SubtitleCue &cue = cues[i];
            out << (i + 1) << "\n";
            out << msToSrtTime(cue.startMs) << " --> " << msToSrtTime(cue.endMs) << "\n";
            out << cue.text << "\n";
            out << "\n";
        }

        file.close();
        return true;
    }

    // ── Utilities ───────────────────────────────────────────────────────────
    static SubtitleFormat detectFormat(const QString &filePath)
    {
        QString suffix = QFileInfo(filePath).suffix().toLower();
        if (suffix == "srt")  return SubtitleFormat::SRT;
        if (suffix == "ass")  return SubtitleFormat::ASS;
        if (suffix == "ssa")  return SubtitleFormat::SSA;
        if (suffix == "vtt")  return SubtitleFormat::VTT;
        return SubtitleFormat::Unknown;
    }

    static QString formatName(SubtitleFormat fmt)
    {
        switch (fmt) {
            case SubtitleFormat::SRT:  return "SRT";
            case SubtitleFormat::ASS:  return "ASS";
            case SubtitleFormat::SSA:  return "SSA";
            case SubtitleFormat::VTT:  return "VTT";
            default:                   return "Unknown";
        }
    }

    // ── Time conversion helpers ─────────────────────────────────────────────
    static qint64 timeToMs(int h, int m, int s, int ms)
    {
        return static_cast<qint64>(h) * 3600000 +
               static_cast<qint64>(m) * 60000 +
               static_cast<qint64>(s) * 1000 +
               ms;
    }

    static qint64 assTimeToMs(int h, int m, int s, int cs)
    {
        return static_cast<qint64>(h) * 3600000 +
               static_cast<qint64>(m) * 60000 +
               static_cast<qint64>(s) * 1000 +
               cs * 10;
    }

    static QString msToSrtTime(qint64 ms)
    {
        int h  = static_cast<int>(ms / 3600000);
        int m  = static_cast<int>((ms % 3600000) / 60000);
        int s  = static_cast<int>((ms % 60000) / 1000);
        int ml = static_cast<int>(ms % 1000);
        return QString("%1:%2:%3,%4")
            .arg(h, 2, 10, QChar('0'))
            .arg(m, 2, 10, QChar('0'))
            .arg(s, 2, 10, QChar('0'))
            .arg(ml, 3, 10, QChar('0'));
    }

    // ── Text processing ─────────────────────────────────────────────────────
    static QString stripHtmlTags(const QString &text)
    {
        QString result = text;
        result.remove(QRegularExpression(R"(<[^>]*>)"));
        return result.trimmed();
    }
};

#endif // SUBTITLEPARSER_H
