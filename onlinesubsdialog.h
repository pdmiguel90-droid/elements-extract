#ifndef ONLINESUBSDIALOG_H
#define ONLINESUBSDIALOG_H

#include <QDialog>
#include <QList>
#include <QString>

#include "subtitledownloader.h"

class QLineEdit;
class QComboBox;
class QPushButton;
class QTableWidget;
class QLabel;

class OnlineSubsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OnlineSubsDialog(SubtitleDownloader *downloader,
                              const QString &videoPath,
                              QWidget *parent = nullptr);

signals:
    void subtitleReady(const QString &filePath, const QString &videoPath,
                       const QString &language, bool translate);

private slots:
    void doSearch();
    void onSearchFinished(const QList<OnlineSubtitle> &results, const QString &error);
    void onDownloadFinished(const QString &filePath, const QString &error);
    void downloadAndTranslate();
    void downloadOnly();

private:
    void startDownload(bool translate);
    QString cleanQuery(const QString &fileName) const;
    QString downloadDirectory() const;

    SubtitleDownloader *m_downloader;
    QString m_videoPath;

    QLineEdit *m_queryEdit;
    QComboBox *m_languageCombo;
    QPushButton *m_searchButton;
    QTableWidget *m_resultsTable;
    QLabel *m_statusLabel;
    QPushButton *m_translateButton;
    QPushButton *m_downloadButton;
    QPushButton *m_cancelButton;

    QList<OnlineSubtitle> m_results;
    QString m_pendingLanguage;
    bool m_pendingTranslate = false;
};

#endif // ONLINESUBSDIALOG_H
