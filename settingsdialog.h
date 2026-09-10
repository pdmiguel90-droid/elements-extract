#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QMap>
#include <QSet>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class SubtitleTranslator;
class ModelDownloader;
struct RemoteModel;

struct LocalModel {
    QString id;
    QString sourceLang;
    QString targetLang;
    QString size;
    bool    isActive;
    QString path;
};

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(SubtitleTranslator *translator,
                            ModelDownloader *downloader,
                            QWidget *parent = nullptr);

signals:
    void settingsChanged();

private slots:
    void onBrowseModelsDir();
    void onFetchOnlineModels();
    void onOnlineModelsReply(QNetworkReply *reply);
    void onDownloadModel(const QString &modelId);
    void onDeleteModel(const QString &modelId);
    void onActivateModel(const QString &modelId);
    void onModelDownloadProgress(const QString &modelId, qint64 received, qint64 total);
    void onModelDownloaded(const QString &modelId);
    void onModelDownloadError(const QString &modelId, const QString &message);

private:
    void refreshLocalModels();
    void refreshOnlineModels();
    QString formatSize(qint64 bytes);

    SubtitleTranslator *m_translator;
    ModelDownloader *m_downloader;
    QNetworkAccessManager *m_netManager;

    QLineEdit *m_modelsDirEdit;
    QComboBox *m_defaultSrcLangCombo;
    QComboBox *m_defaultDstLangCombo;
    QCheckBox *m_autoTranslateCheck;

    QListWidget *m_localListWidget;
    QListWidget *m_onlineListWidget;
    QLabel *m_statusLabel;
    QProgressBar *m_downloadProgress;

    QList<RemoteModel> m_onlineModels;
    QSet<QString> m_installedIds;
};

#endif // SETTINGSDIALOG_H
