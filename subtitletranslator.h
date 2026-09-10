#ifndef SUBTITLETRANSLATOR_H
#define SUBTITLETRANSLATOR_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QDir>
#include <QProcess>
#include <QJsonObject>

#include "subtitleparser.h"

// ── Language pair structure ──────────────────────────────────────────────────
struct LanguagePair {
    QString sourceCode;     // "en", "es", "fr", "pt"
    QString targetCode;     // "es", "en", "fr", "pt"
    QString modelCode;      // "en-es-tiny", "es-en-base", etc.
    QString displayName;    // "English → Spanish"
    qint64  sizeBytes;      // Model file size
    bool    isInstalled;    // Whether model is available locally
};

// ── Main translator class ───────────────────────────────────────────────────
class SubtitleTranslator : public QObject
{
    Q_OBJECT

public:
    explicit SubtitleTranslator(QObject *parent = nullptr);
    ~SubtitleTranslator();

    // ── Configuration ───────────────────────────────────────────────────────
    void setModelDirectory(const QString &dir);
    QString modelDirectory() const;

    void setBergamotBinary(const QString &path);
    QString bergamotBinary() const;

    // ── Available languages ─────────────────────────────────────────────────
    static QStringList supportedSourceLanguages();
    static QStringList supportedTargetLanguages();
    static QString languageName(const QString &code);

    // ── Model management ────────────────────────────────────────────────────
    QList<LanguagePair> availableModels();
    QList<LanguagePair> installedModels();
    bool isModelInstalled(const QString &sourceLang, const QString &targetLang);

    // ── Translation path ────────────────────────────────────────────────────
    // Returns the language code chain for translation.
    // Direct: ["en", "es"]  |  Triangular: ["pt", "en", "es"]  |  Empty: no path
    QStringList findTranslationPath(const QString &sourceLang, const QString &targetLang);

    // ── Translation ─────────────────────────────────────────────────────────
    bool translateFile(const QString &inputPath,
                       const QString &outputPath,
                       const QString &sourceLang,
                       const QString &targetLang);

    void translateBatch(const QStringList &inputPaths,
                        const QString &outputDir,
                        const QString &sourceLang,
                        const QString &targetLang);

    void cancel();

signals:
    void progress(int current, int total);
    void fileTranslated(const QString &input, const QString &output);
    void batchFinished();
    void translationError(const QString &message);
    void modelLoadProgress(const QString &modelName, int percent);
    void triangularTranslationUsed(const QStringList &path);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessReadyRead();
    void onProcessError(QProcess::ProcessError error);

private:
    QString m_modelDir;
    QString m_bergamotBinary;
    QProcess *m_process;
    bool m_cancelled;

    // Batch state
    QList<QPair<QString, QString>> m_batchQueue;  // (inputPath, outputPath)
    QString m_batchSourceLang;
    QString m_batchTargetLang;
    int m_batchIndex;

    // Current file translation state
    QString m_currentInputPath;
    QString m_currentOutputPath;
    QList<SubtitleCue> m_currentCues;
    QList<int> m_translatableIndices;
    int m_currentLineTotal;
    bool m_blocking;  // true = wait for process to finish (triangular hops)

    QString findBergamotBinary();
    QString modelPathForPair(const QString &src, const QString &trg);
    bool translateFileDirect(const QString &inputPath,
                             const QString &outputPath,
                             const QString &sourceLang,
                             const QString &targetLang);
    void processNextInBatch();
    void translateFileContent();
};

#endif // SUBTITLETRANSLATOR_H
