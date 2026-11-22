#ifndef ONLINESUBSDIALOG_H
#define ONLINESUBSDIALOG_H

#include <QDialog>
#include <QList>
#include <QString>
#include <QStringList>

#include "subtitledownloader.h"

class QComboBox;
class QPushButton;
class QTableWidget;
class QLabel;

class OnlineSubsDialog : public QDialog
{
    Q_OBJECT

public:
    // Estado de cada vídeo del lote. La consulta se puede editar en la tabla.
    struct FileState {
        QString path;
        QString query;
        QString status;
        int resultCount = 0;
    };

    // Cada resultado recuerda a qué vídeo pertenece para descargas por lote.
    struct ResultState {
        QString videoPath;
        OnlineSubtitle sub;
        bool selected = false;
        QString status;
    };

    explicit OnlineSubsDialog(SubtitleDownloader *downloader,
                              const QStringList &videoPaths,
                              const QString &preferredLanguage,
                              QWidget *parent = nullptr);

signals:
    void subtitleReady(const QString &filePath, const QString &videoPath,
                       const QString &language, bool translate);

protected:
    void reject() override;

private slots:
    void searchSelected();
    void searchAll();
    void onSearchFinished(const QString &videoPath,
                          const QList<OnlineSubtitle> &results,
                          const QString &error);
    void onDownloadFinished(const QString &videoPath, const QString &filePath,
                            const QString &error);
    void downloadAndTranslate();
    void downloadOnly();
    void applyTriangularChoice();

private:
    void startSearch(bool selectedOnly);
    void startNextSearch();
    void finishSearch();
    void startDownload(bool translate);
    void startNextDownload();
    void finishDownload();
    void addResultsFor(const QString &videoPath,
                       const QList<OnlineSubtitle> &results);
    void removeResultsFor(const QString &videoPath);
    void setFileStatus(const QString &videoPath, const QString &status,
                       int resultCount = -1);
    void setResultStatus(int row, const QString &status);
    void updateDownloadButtons() const;
    void setBusy(bool busy);
    void updateTriangularHint(const QStringList &affectedFiles);
    void clearTriangularHint();
    bool fileHasPreferredResults(const QString &videoPath) const;
    int bestOtherLanguageRow(const QString &videoPath) const;
    int fileRowFor(const QString &videoPath) const;
    QString languageFor(const ResultState &result) const;
    QString cleanQuery(const QString &fileName) const;
    QString downloadDirectory(const QString &videoPath) const;
    static QString normalizedLanguage(const QString &language);
    static QString languageDisplayName(const QString &code);

    SubtitleDownloader *m_downloader = nullptr;
    QList<FileState> m_files;
    QList<ResultState> m_results;
    QString m_preferredLanguage = QStringLiteral("es");
    QStringList m_triangularFiles;   // archivos con resultados, pero sin el idioma preferido

    QStringList m_searchQueue;
    QString m_currentSearchPath;
    int m_nextSearch = 0;

    QList<int> m_downloadQueue;   // filas de m_results marcadas para descargar
    int m_nextDownload = 0;
    int m_downloadedCount = 0;
    int m_failedCount = 0;

    bool m_pendingTranslate = false;
    bool m_busy = false;

    QLabel *m_filesLabel;
    QComboBox *m_languageCombo;
    QTableWidget *m_filesTable;
    QPushButton *m_searchSelectedButton;
    QPushButton *m_searchAllButton;
    QTableWidget *m_resultsTable;
    QLabel *m_statusLabel;
    QLabel *m_triangularHintLabel;
    QPushButton *m_translateButton;
    QPushButton *m_downloadButton;
    QPushButton *m_triangularButton;
    QPushButton *m_closeButton;
};

#endif // ONLINESUBSDIALOG_H
