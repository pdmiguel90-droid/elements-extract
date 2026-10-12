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
#include <QGroupBox>
#include <QMap>
#include <QSet>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEvent>

class SubtitleTranslator;
class ModelDownloader;
class ThemeManager;
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
    void uiLanguageChanged(const QString &langCode);

private slots:
    void onBrowseModelsDir();
    void onFetchOnlineModels();
    void onDownloadModel(const QString &modelId);
    void onDeleteModel(const QString &modelId);
    void onActivateModel(const QString &modelId);
    void onModelDownloadProgress(const QString &modelId, qint64 received, qint64 total);
    void onModelDownloaded(const QString &modelId);
    void onModelDownloadError(const QString &modelId, const QString &message);

protected:
    void changeEvent(QEvent *event) override;

private:
    void refreshLocalModels();
    void refreshOnlineModels();
    void retranslateUi();
    QString formatSize(qint64 bytes);

    SubtitleTranslator *m_translator;
    ModelDownloader *m_downloader;

    QTabWidget *m_tabs;
    QGroupBox  *m_dirBox;
    QPushButton *m_browseBtn;
    QGroupBox  *m_langBox;
    QLabel     *m_srcLbl;
    QLabel     *m_dstLbl;
    QGroupBox  *m_optBox;
    QGroupBox  *m_appearanceBox;
    QLabel     *m_themeLbl;
    QLabel     *m_langLbl;
    QLabel     *m_leftTitle;
    QLabel     *m_rightTitle;
    QPushButton *m_fetchBtn;
    QPushButton *m_deleteBtn;
    QLabel     *m_infoLbl;
    QPushButton *m_closeBtn;

    QLineEdit *m_modelsDirEdit;
    QComboBox *m_defaultSrcLangCombo;
    QComboBox *m_defaultDstLangCombo;
    QCheckBox *m_autoTranslateCheck;
    QCheckBox *m_cleanupAfterMuxCheck;
    QComboBox *m_themeCombo;
    QComboBox *m_uiLangCombo;

    QListWidget *m_localListWidget;
    QListWidget *m_onlineListWidget;
    QLabel *m_statusLabel;
    QProgressBar *m_downloadProgress;

    QList<RemoteModel> m_onlineModels;
    QSet<QString> m_installedIds;
};

#endif // SETTINGSDIALOG_H
