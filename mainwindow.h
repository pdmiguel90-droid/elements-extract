#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QCheckBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QProcess>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QFile>
#include <QDir>
#include <QMap>
#include <QSet>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QComboBox>
#include <QAction>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>
#include <QCoreApplication>
#include <QGroupBox>

// Forward declarations for translation classes
class SubtitleTranslator;
class ModelDownloader;
class SettingsDialog;

// Sub extraído que puede traducirse/incrustarse de vuelta en su vídeo
struct SubExtractInfo {
    QString videoInput;
    QString subPath;
    QString subLang;      // código normalizado (ej: "es")
};

// Trabajo de remux: sub ya traducido → incrustar en el vídeo original
struct MuxJobInfo {
    QString videoInput;
    QString translatedSub;
    QString langCode;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void loadFiles(const QStringList &files);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent *event) override;

private slots:
    void addFiles();
    void removeFiles();
    void selectOutputDir();
    void startExtraction();
    void cancelExtraction();
    void readProcessOutput();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onFileSelectionChanged();
    void onStreamToggled(int state);
    void onStreamClicked(bool checked);
    void selectAllVideo();
    void selectAllAudio();
    void selectAllSubs();
    void clearAllSelections();
    void showAbout();
    void showHelp();
    void readProbeOutput();
    void probeFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processNextInBatch();
    void openOutputFolder();
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

    // ── Translation slots ──────────────────────────────────────────────────
    void onTranslationProgress(int current, int total);
    void onFileTranslated(const QString &input, const QString &output);
    void onTranslationError(const QString &message);
    void onTranslationFinished();
    void translateExtractedSubtitles(const QString &subtitlePath);
    void translateExistingFiles();

    // ── Filtro por idioma + remux ──────────────────────────────────────────
    void refreshSubLangFilter();
    void onSubLangFilterChanged(int index);
    void startMuxQueue();
    void processNextMux();

    // ── Model management slots ─────────────────────────────────────────────
    void showModelManager();
    void onModelsFetched();
    void onModelDownloadProgress(const QString &modelId, qint64 received, qint64 total);
    void onModelDownloaded(const QString &modelId);
    void onModelDownloadError(const QString &modelId, const QString &message);

    // ── UI language slots ──────────────────────────────────────────────────
    void changeUILanguage(const QString &langCode);

    // ── Settings slots ────────────────────────────────────────────────────
    void showSettings();
    void onSettingsChanged();

private:
    void setupUi();
    void setupMenuBar();
    void retranslateUi();
    void showLogArea();
    void clearStreamsList();
    void saveCurrentSelections();
    void updateCheckboxesFromGlobal();
    void updateMetadataPanel(double durSecs, qint64 sizeBytes,
                             const QString &videoInfo, int nAudio, int nSubs);
    void loadSettings();
    void saveSettings();
    QString getUniqueFilePath(const QString &fullPath);
    QString ensureOutputSubdir(const QString &subName);
    int     pruneEmptyExtractions(const QString &videoInput);
    // Ruta instalada (directa o triangular) para src→dst; si no hay, registra
    // una vez el motivo accionable (descargable / sin conexión / inexistente)
    // y devuelve lista vacía.
    QStringList checkInstalledTranslationPath(const QString &srcLang, const QString &dstLang);
    int     applySubtitleFilterToFile(const QString &path);
    QString ffmpegExecutable() const;
    qint64  parseTimestamp(const QString &ts);
    void    updateProgressFromLine(const QString &line);
    QString extensionForType(const QString &type);
    QString extensionForCodec(const QString &codec, const QString &type);

    // ── Widgets ──────────────────────────────────────────────────────────────
    QListWidget  *fileListWidget;
    QLineEdit    *outputLineEdit;
    QWidget      *streamsContainer;
    QVBoxLayout  *streamsLayout;
    QLabel       *selectedFileLabel;
    QLabel       *metaInfoLabel;
    QProgressBar *progressBar;
    QList<QCheckBox*> streamCheckBoxes;
    QTextEdit    *logArea;
    QPushButton  *extractButton;
    QPushButton  *cancelButton;
    QPushButton  *openFolderButton;
    QComboBox    *audioFormatCombo;
    QComboBox    *videoFormatCombo;
    QComboBox    *videoResCombo;
    QComboBox    *subsFormatCombo;
    QCheckBox    *copyModeCheckBox;

    // ── Widgets traducibles (retranslateUi) ──────────────────────────────────
    QMenu        *menuArchivo;
    QMenu        *menuAjustes;
    QMenu        *menuAyuda;
    QAction      *actAnadir;
    QAction      *actSalir;
    QAction      *actSettings;
    QAction      *actAyuda;
    QAction      *actAcerca;
    QLabel       *loteLabel;
    QLabel       *pistasLabel;
    QPushButton  *addButton;
    QPushButton  *removeButton;
    QPushButton  *videoAllButton;
    QPushButton  *audioAllButton;
    QPushButton  *subsAllButton;
    QPushButton  *clearAllButton;
    QPushButton  *logToggleButton;
    QGroupBox    *formatGroupBox;
    QLabel       *audioFmtLabel;
    QLabel       *videoFmtLabel;
    QLabel       *resFmtLabel;
    QLabel       *subsFmtLabel;
    QLabel       *srcPromptLabel;
    QLabel       *dstPromptLabel;
    QGroupBox    *destGroupBox;
    QPushButton  *browseButton;

    // ── Translation widgets ─────────────────────────────────────────────────
    QGroupBox    *translationGroupBox;
    QComboBox    *srcLangCombo;
    QComboBox    *dstLangCombo;
    QCheckBox    *translateOnExtractCheck;
    QCheckBox    *muxOnExtractCheck;
    QPushButton  *manageModelsButton;
    QPushButton  *translateFilesButton;
    QProgressBar *translationProgressBar;
    QLabel       *translationStatusLabel;
    QLabel       *subFilterLabel;
    QComboBox    *subFilterCombo;

    // ── Procesos ─────────────────────────────────────────────────────────────
    QProcess   *process;
    QProcess   *probeProcess;
    QByteArray  probeData;

    // ── Translation engine ───────────────────────────────────────────────────
    SubtitleTranslator *subtitleTranslator;
    ModelDownloader *modelDownloader;

    // ── Estado ───────────────────────────────────────────────────────────────
    QString  lastSelectedFile;
    QString  m_probedFile;      // archivo que está siendo analizado por ffprobe
    // file → { streamIndex → streamType }
    QMap<QString, QMap<int, QString>> fileSelections;
    // file → { streamIndex → codecName }
    QMap<QString, QMap<int, QString>> fileStreamCodecs;
    // file → { streamIndex → streamType }
    QMap<QString, QMap<int, QString>> fileStreamTypes;
    // file → { streamIndex → languageCode }
    QMap<QString, QMap<int, QString>> fileStreamLanguages;
    QSet<QString>  globalSelectionTypes;
    QStringList    batchFiles;
    QList<SubExtractInfo> m_extractedSubs;   // subs extraídos + su vídeo de origen
    QList<MuxJobInfo>     m_muxQueue;        // subs traducidos pendientes de remux
    int            m_muxIndex;
    bool           m_muxActive;
    int            m_muxOk = 0;           // remux correctos
    int            m_muxFailed = 0;       // omitidos o con error
    QStringList    m_muxReasons;          // "archivo → motivo" (omitidos y errores)
    QStringList    m_muxTail;             // últimas líneas de ffmpeg durante el remux
    QStringList    m_detectedSubLangs;       // idiomas de subs detectados en el lote
    QString        m_subLangFilter;          // "" = todos; si no, código a filtrar
    QMap<QString, QSet<int>> m_filterDeselected; // desmarcados a mano que el filtro no repone
    int            currentBatchIndex;
    QMap<QString, qint64> fileDurations; // duración en ms por archivo
    qint64 currentFileDuration;
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;

    void autoDetectSourceLanguage();
};

#endif // MAINWINDOW_H
