#ifndef MODELDOWNLOADER_H
#define MODELDOWNLOADER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QProcess>
#include <QTimer>
#include <QSettings>

// ── Remote model info ───────────────────────────────────────────────────────
struct RemoteModel {
    QString id;              // "en-es-tiny"
    QString name;            // "English-Spanish tiny"
    QString sourceLang;      // "en"
    QString targetLang;      // "es"
    QString type;            // "tiny" or "base"
    QString downloadUrl;     // URL to download the model archive
    QString checksum;        // SHA-256 checksum
    qint64  sizeBytes;       // Archive size
    bool    isInstalled;     // Whether already downloaded
    int     version;         // Model version number
};

// ── Model downloader class ──────────────────────────────────────────────────
class ModelDownloader : public QObject
{
    Q_OBJECT

public:
    explicit ModelDownloader(QObject *parent = nullptr);
    ~ModelDownloader();

    // ── Configuration ───────────────────────────────────────────────────────
    void setModelsDirectory(const QString &dir);
    QString modelsDirectory() const;

    void setRegistryUrl(const QString &url);
    QString registryUrl() const;

    // ── Model listing ───────────────────────────────────────────────────────
    QList<RemoteModel> fetchAvailableModels();
    QList<RemoteModel> installedModels();

    bool isModelInstalled(const QString &modelId);
    bool isModelInstalled(const QString &sourceLang, const QString &targetLang);
    bool isModelUpdateAvailable(const RemoteModel &model) const;

    // ── Download management ─────────────────────────────────────────────────
    void downloadModel(const QString &modelId);
    void downloadModelWithInfo(const RemoteModel &model);
    void cancelDownload();
    void deleteModel(const QString &modelId);

    // ── Utility ─────────────────────────────────────────────────────────────
    static QString modelIdForPair(const QString &sourceLang, const QString &targetLang,
                                  const QString &type = "tiny");

signals:
    void modelsFetched(const QList<RemoteModel> &models);
    void fetchError(const QString &message);

    void downloadProgress(const QString &modelId, qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished(const QString &modelId);
    void downloadError(const QString &modelId, const QString &message);

    void deleteFinished(const QString &modelId);
    void deleteError(const QString &modelId, const QString &message);

private slots:
    void onRegistryReply(QNetworkReply *reply);
    void onDownloadProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onDownloadProgressTimer();

private:
    QNetworkAccessManager *m_networkManager;
    QString m_modelsDir;
    QString m_registryUrl;

    // curl-based download
    QProcess *m_downloadProcess;
    QString   m_currentDownloadModelId;
    QString   m_currentDownloadArchivePath;
    qint64    m_currentDownloadExpectedSize;
    QTimer   *m_downloadProgressTimer;

    QList<RemoteModel> m_cachedModels;

    void extractModelArchive(const QString &archivePath, const QString &modelId);
    bool verifyChecksum(const QString &filePath, const QString &expectedChecksum);
};

#endif // MODELDOWNLOADER_H
