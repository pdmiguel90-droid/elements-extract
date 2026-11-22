#ifndef SUBTITLEDOWNLOADER_H
#define SUBTITLEDOWNLOADER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>

// Resultado de una búsqueda en OpenSubtitles.com
struct OnlineSubtitle {
    QString fileId;
    QString language;
    QString release;
    QString year;
    QString uploader;
    QString url;
    int downloads = 0;
    bool hashMatch = false;
};

class SubtitleDownloader : public QObject
{
    Q_OBJECT

public:
    explicit SubtitleDownloader(QObject *parent = nullptr);
    ~SubtitleDownloader() override;

    void setCredentials(const QString &apiKey, const QString &user,
                        const QString &password);
    bool hasCredentials() const;

    void search(const QString &videoPath, const QString &query,
                const QString &language);
    void download(const OnlineSubtitle &item, const QString &targetPath);
    void cancel();

    // Respuesta de OnlineSubsDialog cuando un ZIP contiene varias candidateas
    // de subtítulos y no hay un claro ganador automático.
    void setChosenArchiveEntry(const QString &entry);

signals:
    void searchFinished(const QList<OnlineSubtitle> &results, const QString &error);
    void downloadFinished(const QString &filePath, const QString &error);
    void statusMessage(const QString &message);
    void needFileChoice(const QStringList &files, const QString &language);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    enum class Step {
        None,
        SearchByHash,
        SearchByQuery,
        LoginForDownload,
        RequestDownload,
        FetchLink
    };

    struct DownloadState {
        OnlineSubtitle item;
        QString targetPath;
        bool authRetried = false;   // un solo reintento tras un 401
    };

    QNetworkAccessManager *m_manager = nullptr;
    QNetworkReply *m_activeReply = nullptr;

    QString m_baseUrl;              // override de prueba: ELEMENTS_EXTRACT_OS_API_URL
    QString m_apiKey;
    QString m_user;
    QString m_password;
    QString m_token;                // solo en memoria, nunca en QSettings

    Step m_step = Step::None;
    bool m_cancelled = false;

    QString m_videoPath;
    QString m_query;
    QString m_language;
    DownloadState m_download;
    QString m_chosenEntry;   // si el usuario eligió una entrada dentro del ZIP

    QNetworkRequest makeRequest(const QUrl &url, bool post) const;
    void startSearchByHash();
    void startSearchByQuery();
    void startLogin();
    void startDownloadRequest();
    void startLinkFetch(const QUrl &url);

    void handleSearchReply(QNetworkReply *reply, bool wasHashSearch);
    void handleLoginReply(QNetworkReply *reply);
    void handleDownloadRequestReply(QNetworkReply *reply);
    void handleLinkReply(QNetworkReply *reply);

    QString replyErrorMessage(QNetworkReply *reply);
    static QString computeMovieHash(const QString &videoPath, qint64 *fileSize = nullptr);
    bool writeSubtitleBytes(const QByteArray &raw, const QString &targetPath,
                            QString *outPath, QString *error);
    static QString detectSubtitleExtension(const QByteArray &data);

    static bool isRarData(const QByteArray &data);
    static QStringList listZipEntries(const QString &archivePath, QString *error);
    static QStringList subtitleEntries(const QStringList &entries);
    static int entryScore(const QString &entry, const QString &language);
    QString selectArchiveEntry(const QStringList &entries);
};

#endif // SUBTITLEDOWNLOADER_H
