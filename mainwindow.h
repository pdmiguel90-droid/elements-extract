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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

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

private:
    void setupUi();
    void setupMenuBar();
    void applyStyle();
    void clearStreamsList();
    void saveCurrentSelections();
    void updateCheckboxesFromGlobal();
    void updateMetadataPanel(double durSecs, qint64 sizeBytes,
                             const QString &videoInfo, int nAudio, int nSubs);
    void loadSettings();
    void saveSettings();
    QString getUniqueFilePath(const QString &fullPath);
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

    // ── Procesos ─────────────────────────────────────────────────────────────
    QProcess   *process;
    QProcess   *probeProcess;
    QByteArray  probeData;

    // ── Estado ───────────────────────────────────────────────────────────────
    QString  lastSelectedFile;
    // file → { streamIndex → streamType }
    QMap<QString, QMap<int, QString>> fileSelections;
    // file → { streamIndex → codecName }
    QMap<QString, QMap<int, QString>> fileStreamCodecs;
    QSet<QString>  globalSelectionTypes;
    QStringList    batchFiles;
    int            currentBatchIndex;
    QMap<QString, qint64> fileDurations; // duración en ms por archivo
    qint64 currentFileDuration;
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
};

#endif // MAINWINDOW_H
