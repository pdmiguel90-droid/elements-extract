#include "mainwindow.h"
#include "aboutdialog.h"
#include "helpdialog.h"
#include "settingsdialog.h"
#include "subtitletranslator.h"
#include "modeldownloader.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QScrollArea>
#include <QGroupBox>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QDesktopServices>
#include <QUrl>
#include <QRegularExpression>
#include <QSettings>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QSplitter>
#include <QPixmap>
#include <QPainter>
#include <QStyle>
#include <QStandardPaths>

// ─────────────────────────────────────────────────────────────────────────────
// Constructor / Destructor
// ─────────────────────────────────────────────────────────────────────────────
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentBatchIndex(-1), currentFileDuration(0)
{
    setupUi();
    setupMenuBar();
    applyStyle();
    setAcceptDrops(true);
    loadSettings();

    // Initialize translation engine
    subtitleTranslator = new SubtitleTranslator(this);
    // Check development path first, then installed path
    QString devModelsPath = QCoreApplication::applicationDirPath() + "/../models";
    QString installedModelsPath = QCoreApplication::applicationDirPath() + "/../share/elements-extract/models";
    QString modelsPath;
    if (QDir(devModelsPath).exists()) {
        modelsPath = devModelsPath;
    } else {
        modelsPath = installedModelsPath;
    }
    subtitleTranslator->setModelDirectory(modelsPath);

    // bergamot binary found automatically by findBergamotBinary()

    // Initialize model downloader — use SAME directory as translator
    modelDownloader = new ModelDownloader(this);
    modelDownloader->setModelsDirectory(modelsPath);

    // Connect translation signals
    connect(subtitleTranslator, &SubtitleTranslator::progress,
            this, &MainWindow::onTranslationProgress);
    connect(subtitleTranslator, &SubtitleTranslator::fileTranslated,
            this, &MainWindow::onFileTranslated);
    connect(subtitleTranslator, &SubtitleTranslator::translationError,
            this, &MainWindow::onTranslationError);
    connect(subtitleTranslator, &SubtitleTranslator::batchFinished,
            this, &MainWindow::onTranslationFinished);
    connect(subtitleTranslator, &SubtitleTranslator::triangularTranslationUsed,
            this, [this](const QStringList &path) {
        QString pathStr;
        for (int i = 0; i < path.size(); ++i) {
            if (i > 0) pathStr += " → ";
            pathStr += SubtitleTranslator::languageName(path[i]);
        }
        logArea->append(QString("<font color='#FFD700'>↻ Traducción triangular: %1</font>").arg(pathStr));
    });

    // Connect model downloader signals
    connect(modelDownloader, &ModelDownloader::downloadProgress,
            this, &MainWindow::onModelDownloadProgress);
    connect(modelDownloader, &ModelDownloader::downloadFinished,
            this, &MainWindow::onModelDownloaded);
    connect(modelDownloader, &ModelDownloader::downloadError,
            this, &MainWindow::onModelDownloadError);

    // Icono de la aplicación
    QIcon appIcon(":/ffmpeg-extractor.svg");
    setWindowIcon(appIcon);

    trayIcon = new QSystemTrayIcon(appIcon, this);
    trayMenu = new QMenu(this);

    QAction *restoreAction = trayMenu->addAction("Mostrar / Ocultar ventana");
    connect(restoreAction, &QAction::triggered, this, [this]() {
        if (isVisible() && !isMinimized()) hide();
        else { showNormal(); activateWindow(); }
    });

    QAction *cancelAction = trayMenu->addAction("Cancelar proceso actual");
    connect(cancelAction, &QAction::triggered, this, &MainWindow::cancelExtraction);

    trayMenu->addSeparator();

    QAction *quitAction = trayMenu->addAction("Salir");
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    trayIcon->setContextMenu(trayMenu);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);
    trayIcon->show();

    process = new QProcess(this);
    connect(process, &QProcess::readyReadStandardOutput, this, &MainWindow::readProcessOutput);
    connect(process, &QProcess::readyReadStandardError,  this, &MainWindow::readProcessOutput);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::processFinished);

    probeProcess = new QProcess(this);
    connect(probeProcess, &QProcess::readyReadStandardOutput, this, &MainWindow::readProbeOutput);
    connect(probeProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::probeFinished);
}

MainWindow::~MainWindow() {}

// ─────────────────────────────────────────────────────────────────────────────
// Estilo
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::applyStyle()
{
    setStyleSheet(R"(
        QMainWindow { background-color: #121212; }
        QWidget { background-color: #121212; color: #D0D0D0;
                  font-family: 'Segoe UI', sans-serif; font-size: 13px; }
        QLabel  { font-weight: bold; color: #FF8800; }
        QGroupBox { border: 1px solid #2A2A2A; border-radius: 5px;
                    margin-top: 8px; padding-top: 6px; color: #FF8800;
                    font-weight: bold; font-size: 11px; }
        QGroupBox::title { subcontrol-origin: margin; left: 10px; }
        QLineEdit, QListWidget, QTextEdit, QScrollArea {
            background-color: #1A1A1A; border: 1px solid #2A2A2A;
            border-radius: 4px; padding: 5px; color: #D0D0D0; }
        QLineEdit:focus, QListWidget:focus { border: 1px solid #FF8800; }
        QPushButton {
            background-color: #1A1A1A; border: 1px solid #39FF14;
            border-radius: 5px; padding: 7px 14px;
            color: #39FF14; font-weight: bold; }
        QPushButton:hover { background-color: #39FF14; color: #000; }
        QPushButton:disabled { border-color: #333; color: #444; }
        QPushButton#extractButton {
            border: 2px solid #FF8800; color: #FF8800;
            font-size: 15px; padding: 10px; }
        QPushButton#extractButton:hover { background-color: #FF8800; color: #000; }
        QPushButton#cancelButton {
            border: 1px solid #CC3333; color: #CC3333; }
        QPushButton#cancelButton:hover { background-color: #CC3333; color: #FFF; }
        QCheckBox { padding: 4px; color: #D0D0D0; font-weight: normal; }
        QCheckBox::indicator { width: 16px; height: 16px;
            background-color: #1A1A1A; border: 1px solid #39FF14; }
        QCheckBox::indicator:checked { background-color: #39FF14; }
        QComboBox { background-color: #1A1A1A; border: 1px solid #2A2A2A;
                    border-radius: 4px; padding: 4px 8px; color: #D0D0D0; }
        QComboBox:hover { border-color: #FF8800; }
        QComboBox QAbstractItemView { background-color: #1A1A1A;
            border: 1px solid #FF8800; color: #D0D0D0; }
        QProgressBar { border: 1px solid #2A2A2A; border-radius: 4px;
                       background-color: #1A1A1A; text-align: center;
                       color: #D0D0D0; }
        QProgressBar::chunk { background-color: #FF8800; border-radius: 3px; }
        QMenuBar { background-color: #1A1A1A; color: #D0D0D0; }
        QMenuBar::item:selected { background-color: #FF8800; color: #000; }
        QMenu { background-color: #1A1A1A; border: 1px solid #2A2A2A; color: #D0D0D0; }
        QMenu::item:selected { background-color: #FF8800; color: #000; }
        QStatusBar { background-color: #1A1A1A; color: #888; font-size: 11px; }
    )");
}

// ─────────────────────────────────────────────────────────────────────────────
// Menú
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::setupMenuBar()
{
    QMenu *menuArchivo = menuBar()->addMenu("Archivo");
    QAction *actAnadir = menuArchivo->addAction(
        style()->standardIcon(QStyle::SP_FileDialogNewFolder), "Añadir archivos…");
    actAnadir->setShortcut(QKeySequence("Ctrl+O"));
    connect(actAnadir, &QAction::triggered, this, &MainWindow::addFiles);
    menuArchivo->addSeparator();
    QAction *actSalir = menuArchivo->addAction(
        style()->standardIcon(QStyle::SP_DialogCloseButton), "Salir");
    actSalir->setShortcut(QKeySequence("Ctrl+Q"));
    connect(actSalir, &QAction::triggered, this, &QWidget::close);

    QMenu *menuAjustes = menuBar()->addMenu("Ajustes");
    QAction *actSettings = menuAjustes->addAction(
        style()->standardIcon(QStyle::SP_FileDialogDetailedView), "Configuración y Modelos…");
    actSettings->setShortcut(QKeySequence("Ctrl+,"));
    connect(actSettings, &QAction::triggered, this, &MainWindow::showSettings);

    QMenu *menuAyuda = menuBar()->addMenu("Ayuda");
    QAction *actAyuda = menuAyuda->addAction(
        style()->standardIcon(QStyle::SP_DialogHelpButton), "Manual de uso");
    actAyuda->setShortcut(Qt::Key_F1);
    connect(actAyuda, &QAction::triggered, this, &MainWindow::showHelp);
    menuAyuda->addSeparator();
    QAction *actAcerca = menuAyuda->addAction(
        style()->standardIcon(QStyle::SP_MessageBoxInformation), "Acerca de…");
    connect(actAcerca, &QAction::triggered, this, &MainWindow::showAbout);
}

// ─────────────────────────────────────────────────────────────────────────────
// UI
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::setupUi()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *root = new QVBoxLayout(central);
    root->setSpacing(6);
    root->setContentsMargins(8, 6, 8, 6);

    // Título
    QLabel *titleLabel = new QLabel("ELEMENTS EXTRACT FFMPEG");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 20px; letter-spacing: 3px; color: #FF8800; margin-bottom: 2px;");
    root->addWidget(titleLabel);

    // ── Panel central: QSplitter para redimensionar entre archivos y pistas ──
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal);
    mainSplitter->setChildrenCollapsible(false);

    // Panel izquierdo: lista de archivos
    QWidget *leftWidget = new QWidget();
    QVBoxLayout *leftPanel = new QVBoxLayout(leftWidget);
    leftPanel->setContentsMargins(0, 0, 0, 0);
    QLabel *lbLote = new QLabel("LOTE DE ARCHIVOS");
    lbLote->setStyleSheet("font-size: 11px; letter-spacing: 1px;");
    leftPanel->addWidget(lbLote);

    fileListWidget = new QListWidget();
    fileListWidget->setDragDropMode(QAbstractItemView::InternalMove);
    connect(fileListWidget, &QListWidget::itemSelectionChanged,
            this, &MainWindow::onFileSelectionChanged);
    leftPanel->addWidget(fileListWidget, 1);

    QHBoxLayout *fileBtns = new QHBoxLayout();
    QPushButton *addBtn = new QPushButton(
        style()->standardIcon(QStyle::SP_FileDialogNewFolder), " Añadir");
    QPushButton *remBtn = new QPushButton(
        style()->standardIcon(QStyle::SP_TrashIcon), " Quitar");
    addBtn->setToolTip("Añadir archivos de video (Ctrl+O)");
    remBtn->setToolTip("Quitar archivos seleccionados (Del)");
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::addFiles);
    connect(remBtn, &QPushButton::clicked, this, &MainWindow::removeFiles);
    fileBtns->addWidget(addBtn);
    fileBtns->addWidget(remBtn);
    leftPanel->addLayout(fileBtns);
    mainSplitter->addWidget(leftWidget);

    // Panel derecho: pistas del archivo
    QWidget *rightWidget = new QWidget();
    QVBoxLayout *rightPanel = new QVBoxLayout(rightWidget);
    rightPanel->setContentsMargins(0, 0, 0, 0);
    selectedFileLabel = new QLabel("Selecciona un archivo para ver sus pistas");
    selectedFileLabel->setStyleSheet("color: #39FF14; font-weight: bold;"
                                     "border-bottom: 1px solid #2A2A2A; padding-bottom: 4px;");
    rightPanel->addWidget(selectedFileLabel);

    metaInfoLabel = new QLabel("");
    metaInfoLabel->setStyleSheet(
        "color: #888888; font-size: 11px; font-weight: normal; padding: 2px 0;");
    metaInfoLabel->setWordWrap(true);
    rightPanel->addWidget(metaInfoLabel);

    QHBoxLayout *quickSel = new QHBoxLayout();
    QPushButton *allVideoBtn = new QPushButton("▶ VIDEO");
    QPushButton *allAudioBtn = new QPushButton("♪ AUDIO");
    QPushButton *allSubsBtn  = new QPushButton("CC SUBS");
    QPushButton *clearAllBtn = new QPushButton("✕ Limpiar");
    QString qsStyle = "QPushButton { padding:5px; font-size:11px; border-color:#333; }";
    for (auto *b : {allVideoBtn, allAudioBtn, allSubsBtn, clearAllBtn})
        b->setStyleSheet(qsStyle);
    connect(allVideoBtn, &QPushButton::clicked, this, &MainWindow::selectAllVideo);
    connect(allAudioBtn, &QPushButton::clicked, this, &MainWindow::selectAllAudio);
    connect(allSubsBtn,  &QPushButton::clicked, this, &MainWindow::selectAllSubs);
    connect(clearAllBtn, &QPushButton::clicked, this, &MainWindow::clearAllSelections);
    quickSel->addWidget(allVideoBtn);
    quickSel->addWidget(allAudioBtn);
    quickSel->addWidget(allSubsBtn);
    quickSel->addStretch();
    quickSel->addWidget(clearAllBtn);
    rightPanel->addLayout(quickSel);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    streamsContainer = new QWidget();
    streamsLayout = new QVBoxLayout(streamsContainer);
    streamsLayout->setAlignment(Qt::AlignTop);
    scrollArea->setWidget(streamsContainer);
    rightPanel->addWidget(scrollArea, 1);
    mainSplitter->addWidget(rightWidget);

    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 2);
    root->addWidget(mainSplitter, 3);

    // ── Formato de salida ────────────────────────────────────────────────────
    QGroupBox *fmtBox = new QGroupBox("FORMATO DE SALIDA");
    QVBoxLayout *fmtMainLayout = new QVBoxLayout(fmtBox);

    QHBoxLayout *fmtLayout = new QHBoxLayout();
    fmtLayout->setSpacing(16);

    auto makeCombo = [&](const QString &label, const QStringList &items) -> QComboBox* {
        QLabel *lbl = new QLabel(label);
        lbl->setStyleSheet("font-size: 11px; color: #888; font-weight: normal;");
        QComboBox *cb = new QComboBox();
        cb->addItems(items);
        fmtLayout->addWidget(lbl);
        fmtLayout->addWidget(cb);
        return cb;
    };
    audioFormatCombo = makeCombo("Audio:", {"mka", "m4a", "aac", "mp3", "flac", "opus", "wav"});
    fmtLayout->addSpacing(8);
    videoFormatCombo = makeCombo("Video:", {"mkv", "mp4", "avi"});

    videoResCombo = new QComboBox();
    videoResCombo->addItems({"Original", "4K (2160p)", "2K (1440p)", "FHD (1080p)", "HD (720p)", "SD (480p)"});
    videoResCombo->setStyleSheet("font-size: 11px; background-color: #1A1A1A; border: 1px solid #2A2A2A; border-radius: 4px; padding: 4px 8px; color: #D0D0D0;");
    QLabel *resLbl = new QLabel("Res:");
    resLbl->setStyleSheet("font-size: 11px; color: #888; font-weight: normal;");
    fmtLayout->addWidget(resLbl);
    fmtLayout->addWidget(videoResCombo);

    fmtLayout->addSpacing(8);
    subsFormatCombo  = makeCombo("Subs:",  {"srt", "ass", "vtt"});
    fmtLayout->addStretch();

    copyModeCheckBox = new QCheckBox("Copiar pista original sin re-codificar (Rápido y previene errores)");
    copyModeCheckBox->setChecked(true);
    copyModeCheckBox->setStyleSheet("color: #39FF14; font-weight: bold; margin-top: 5px;");

    fmtMainLayout->addLayout(fmtLayout);
    fmtMainLayout->addWidget(copyModeCheckBox);

    root->addWidget(fmtBox);

    connect(copyModeCheckBox, &QCheckBox::toggled, this, [this](bool checked){
        audioFormatCombo->setEnabled(!checked);
        videoFormatCombo->setEnabled(!checked);
        videoResCombo->setEnabled(!checked);
        subsFormatCombo->setEnabled(!checked);
    });
    audioFormatCombo->setEnabled(false);
    videoFormatCombo->setEnabled(false);
    videoResCombo->setEnabled(false);
    subsFormatCombo->setEnabled(false);

    // ── TRADUCCIÓN DE SUBTÍTULOS ──────────────────────────────────────────
    translationGroupBox = new QGroupBox("TRADUCCIÓN DE SUBTÍTULOS");
    QVBoxLayout *transMainLayout = new QVBoxLayout(translationGroupBox);

    QHBoxLayout *transLangLayout = new QHBoxLayout();
    transLangLayout->setSpacing(12);

    QLabel *srcLangLabel = new QLabel("Origen:");
    srcLangLabel->setStyleSheet("font-size: 11px; color: #888; font-weight: normal;");
    srcLangCombo = new QComboBox();
    srcLangCombo->addItem("English", "en");
    srcLangCombo->addItem("Español", "es");
    srcLangCombo->addItem("Français", "fr");
    srcLangCombo->addItem("Português", "pt");
    transLangLayout->addWidget(srcLangLabel);
    transLangLayout->addWidget(srcLangCombo);

    transLangLayout->addSpacing(8);

    QLabel *dstLangLabel = new QLabel("Destino:");
    dstLangLabel->setStyleSheet("font-size: 11px; color: #888; font-weight: normal;");
    dstLangCombo = new QComboBox();
    dstLangCombo->addItem("Español", "es");
    dstLangCombo->addItem("English", "en");
    dstLangCombo->addItem("Français", "fr");
    dstLangCombo->addItem("Português", "pt");
    transLangLayout->addWidget(dstLangLabel);
    transLangLayout->addWidget(dstLangCombo);

    transLangLayout->addSpacing(12);

    translateOnExtractCheck = new QCheckBox("Traducir al extraer");
    translateOnExtractCheck->setToolTip(
        "Si está marcado, al extraer subtítulos se traducirán automáticamente\n"
        "al idioma de destino seleccionado.");
    translateOnExtractCheck->setStyleSheet(
        "QCheckBox { color: #00BFFF; font-weight: bold; font-size: 12px; }"
        "QCheckBox::indicator { width: 18px; height: 18px; border: 2px solid #00BFFF; }"
        "QCheckBox::indicator:checked { background-color: #00BFFF; }");
    transLangLayout->addWidget(translateOnExtractCheck);

    transLangLayout->addStretch();

    manageModelsButton = new QPushButton("Modelos");
    manageModelsButton->setToolTip("Gestionar modelos de traducción");
    connect(manageModelsButton, &QPushButton::clicked, this, &MainWindow::showSettings);
    transLangLayout->addWidget(manageModelsButton);

    transMainLayout->addLayout(transLangLayout);

    // Progress bar for translation
    QHBoxLayout *transProgressLayout = new QHBoxLayout();
    translationProgressBar = new QProgressBar();
    translationProgressBar->setRange(0, 100);
    translationProgressBar->setValue(0);
    translationProgressBar->setFixedHeight(16);
    translationProgressBar->setFormat("%p%");
    translationProgressBar->setVisible(false);

    translationStatusLabel = new QLabel("Selecciona subtítulos y marca \"Traducir al extraer\"");
    translationStatusLabel->setStyleSheet("font-size: 11px; color: #888; font-weight: normal;");

    transProgressLayout->addWidget(translationProgressBar, 1);
    transProgressLayout->addWidget(translationStatusLabel);
    transMainLayout->addLayout(transProgressLayout);
    transMainLayout->addLayout(transProgressLayout);

    root->addWidget(translationGroupBox);

    // ── Directorio de destino ────────────────────────────────────────────────
    QHBoxLayout *outDirLayout = new QHBoxLayout();
    QLabel *destLbl = new QLabel("DESTINO:");
    destLbl->setStyleSheet("font-size: 11px; letter-spacing: 1px;");
    outputLineEdit = new QLineEdit();
    outputLineEdit->setPlaceholderText("Carpeta donde se guardarán los archivos extraídos…");
    QPushButton *browseBtn = new QPushButton(
        style()->standardIcon(QStyle::SP_DirOpenIcon), " …");
    browseBtn->setFixedWidth(68);
    browseBtn->setToolTip("Seleccionar carpeta de destino");
    openFolderButton = new QPushButton(
        style()->standardIcon(QStyle::SP_DirOpenIcon), " Abrir");
    openFolderButton->setToolTip("Abrir carpeta de destino en el explorador");
    openFolderButton->setEnabled(false);
    connect(browseBtn,       &QPushButton::clicked, this, &MainWindow::selectOutputDir);
    connect(openFolderButton,&QPushButton::clicked, this, &MainWindow::openOutputFolder);
    connect(outputLineEdit,  &QLineEdit::textChanged, this,
            [this](const QString &t){ openFolderButton->setEnabled(!t.isEmpty()); });
    outDirLayout->addWidget(destLbl);
    outDirLayout->addWidget(outputLineEdit);
    outDirLayout->addWidget(browseBtn);
    outDirLayout->addWidget(openFolderButton);
    root->addLayout(outDirLayout);

    // ── Progreso ─────────────────────────────────────────────────────────────
    progressBar = new QProgressBar();
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setFixedHeight(18);
    progressBar->setFormat("%p%  —  %v / %m");
    root->addWidget(progressBar);

    // ── Log ──────────────────────────────────────────────────────────────────
    logArea = new QTextEdit();
    logArea->setReadOnly(true);
    logArea->setPlaceholderText("Sistema listo. Añade archivos y selecciona las pistas a extraer.");
    root->addWidget(logArea, 1);

    // ── Botones de acción ────────────────────────────────────────────────────
    QHBoxLayout *actionBtns = new QHBoxLayout();
    cancelButton = new QPushButton(
        style()->standardIcon(QStyle::SP_DialogCancelButton), " Cancelar");
    cancelButton->setObjectName("cancelButton");
    cancelButton->setEnabled(false);
    cancelButton->setToolTip("Cancelar el proceso en curso (Escape)");
    connect(cancelButton, &QPushButton::clicked, this, &MainWindow::cancelExtraction);

    extractButton = new QPushButton(
        style()->standardIcon(QStyle::SP_MediaPlay), "  INICIAR EXTRACCIÓN");
    extractButton->setObjectName("extractButton");
    extractButton->setEnabled(false);
    extractButton->setShortcut(QKeySequence("Ctrl+Return"));
    extractButton->setToolTip("Iniciar la extracción de todas las pistas seleccionadas (Ctrl+Enter)");
    connect(extractButton, &QPushButton::clicked, this, &MainWindow::startExtraction);

    actionBtns->addWidget(cancelButton);
    actionBtns->addWidget(extractButton, 1);
    root->addLayout(actionBtns);

    // ── Barra de estado ──────────────────────────────────────────────────────
    statusBar()->showMessage("Listo");

    setWindowTitle("Elements Extract FFmpeg");
    resize(980, 680);
}

// ─────────────────────────────────────────────────────────────────────────────
// Archivos
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::loadFiles(const QStringList &files)
{
    for (const QString &f : files) {
        if (f.isEmpty()) continue;
        bool exists = false;
        for (int i = 0; i < fileListWidget->count(); ++i) {
            if (fileListWidget->item(i)->data(Qt::UserRole).toString() == f) { exists = true; break; }
        }
        if (!exists) {
            QListWidgetItem *item = new QListWidgetItem(QFileInfo(f).fileName());
            item->setData(Qt::UserRole, f);
            item->setToolTip(f);
            fileListWidget->addItem(item);
            if (outputLineEdit->text().isEmpty())
                outputLineEdit->setText(QFileInfo(f).absolutePath());
        }
    }
    if (fileListWidget->count() > 0 && fileListWidget->currentRow() == -1)
        fileListWidget->setCurrentRow(0);
}

void MainWindow::addFiles()
{
    QStringList files = QFileDialog::getOpenFileNames(
        this, "Añadir Videos", "",
        "Videos (*.mkv *.mp4 *.avi *.mov *.ts *.m2ts *.webm);;Todos los archivos (*.*)");
    loadFiles(files);
}


void MainWindow::removeFiles()
{
    for (QListWidgetItem *item : fileListWidget->selectedItems()) {
        fileSelections.remove(item->data(Qt::UserRole).toString());
        fileDurations.remove(item->data(Qt::UserRole).toString());
        delete item;
    }
    if (fileListWidget->count() == 0) {
        clearStreamsList();
        selectedFileLabel->setText("No hay archivos cargados");
        extractButton->setEnabled(false);
        globalSelectionTypes.clear();
        statusBar()->showMessage("Listo");
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Selección de pistas
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::onFileSelectionChanged()
{
    saveCurrentSelections();
    if (fileListWidget->selectedItems().isEmpty()) {
        clearStreamsList(); lastSelectedFile = ""; return;
    }
    QString currentFile = fileListWidget->selectedItems().first()->data(Qt::UserRole).toString();
    lastSelectedFile = currentFile;
    selectedFileLabel->setText(QFileInfo(currentFile).fileName().toUpper());
    clearStreamsList();
    probeData.clear();
    statusBar()->showMessage("Analizando: " + QFileInfo(currentFile).fileName());

    metaInfoLabel->setText("Analizando…");
    QStringList args;
    args << "-v" << "error"
         << "-show_entries"
         << "stream=index,codec_type,codec_name,width,height:stream_tags=language:format=duration,size"
         << "-of" << "json" << currentFile;
    probeProcess->start("ffprobe", args);
}

void MainWindow::saveCurrentSelections()
{
    if (lastSelectedFile.isEmpty()) return;
    QMap<int, QString> selectedTypes;
    QMap<int, QString> selectedCodecs;
    for (QCheckBox *cb : streamCheckBoxes) {
        if (cb->isChecked()) {
            int idx = cb->property("streamIndex").toInt();
            selectedTypes[idx] = cb->property("streamType").toString();
            selectedCodecs[idx] = cb->property("streamCodec").toString();
        }
    }
    fileSelections[lastSelectedFile] = selectedTypes;
    fileStreamCodecs[lastSelectedFile] = selectedCodecs;

    // Color visual en la lista
    for (int i = 0; i < fileListWidget->count(); ++i) {
        QListWidgetItem *item = fileListWidget->item(i);
        if (item->data(Qt::UserRole).toString() == lastSelectedFile) {
            item->setForeground(QBrush(selectedTypes.isEmpty() && globalSelectionTypes.isEmpty()
                                       ? QColor("#D0D0D0") : QColor("#FF8800")));
            break;
        }
    }
    extractButton->setEnabled(
        fileListWidget->count() > 0 &&
        (!globalSelectionTypes.isEmpty() || !fileSelections.isEmpty()));

    // Enable extract+translate if subtitles are selected
    bool hasSubtitles = globalSelectionTypes.contains("subtitle");
    if (!hasSubtitles) {
        for (auto it = selectedTypes.constBegin(); it != selectedTypes.constEnd(); ++it) {
            if (it.value() == "subtitle") { hasSubtitles = true; break; }
        }
    }
    if (!hasSubtitles) {
        for (const auto &sel : fileSelections) {
            for (auto it = sel.constBegin(); it != sel.constEnd(); ++it) {
                if (it.value() == "subtitle") { hasSubtitles = true; break; }
            }
            if (hasSubtitles) break;
        }
    }
    autoDetectSourceLanguage();
}

void MainWindow::autoDetectSourceLanguage()
{
    // Collect languages from all selected subtitle streams across all files
    QMap<QString, int> langCount;
    for (auto it = fileSelections.constBegin(); it != fileSelections.constEnd(); ++it) {
        const QString &filePath = it.key();
        const QMap<int, QString> &types = it.value();
        const QMap<int, QString> &langs = fileStreamLanguages.value(filePath);
        for (auto tit = types.constBegin(); tit != types.constEnd(); ++tit) {
            if (tit.value() == "subtitle" && langs.contains(tit.key())) {
                QString lang = langs.value(tit.key()).toLower();
                if (!lang.isEmpty()) langCount[lang]++;
            }
        }
    }

    if (langCount.isEmpty()) return;

    // Find the most common subtitle language
    QString detectedLang;
    int maxCount = 0;
    for (auto it = langCount.constBegin(); it != langCount.constEnd(); ++it) {
        if (it.value() > maxCount) {
            maxCount = it.value();
            detectedLang = it.key();
        }
    }

    if (detectedLang.isEmpty()) return;

    // Map common codes to our supported codes
    QMap<QString, QString> codeMap;
    codeMap["eng"] = "en"; codeMap["english"] = "en";
    codeMap["spa"] = "es"; codeMap["spanish"] = "es"; codeMap["cast"] = "es";
    codeMap["fre"] = "fr"; codeMap["fra"] = "fr"; codeMap["french"] = "fr";
    codeMap["por"] = "pt"; codeMap["portuguese"] = "pt";
    codeMap["ger"] = "de"; codeMap["deu"] = "de"; codeMap["german"] = "de";
    codeMap["ita"] = "it"; codeMap["italian"] = "it";
    codeMap["jpn"] = "ja"; codeMap["japanese"] = "ja";
    codeMap["chi"] = "zh"; codeMap["zho"] = "zh"; codeMap["chinese"] = "zh";
    codeMap["kor"] = "ko"; codeMap["korean"] = "ko";
    codeMap["rus"] = "ru"; codeMap["russian"] = "ru";
    codeMap["ara"] = "ar"; codeMap["arabic"] = "ar";
    codeMap["hin"] = "hi"; codeMap["hindi"] = "hi";
    codeMap["dut"] = "nl"; codeMap["nld"] = "nl"; codeMap["dutch"] = "nl";
    codeMap["swe"] = "sv"; codeMap["swedish"] = "sv";
    codeMap["pol"] = "pl"; codeMap["polish"] = "pl";
    codeMap["tur"] = "tr"; codeMap["turkish"] = "tr";

    QString mappedLang = codeMap.value(detectedLang, detectedLang);

    // Check if it's one of our supported source languages
    QStringList supportedSrc = SubtitleTranslator::supportedSourceLanguages();
    if (supportedSrc.contains(mappedLang)) {
        int idx = srcLangCombo->findData(mappedLang);
        if (idx >= 0 && srcLangCombo->currentIndex() != idx) {
            srcLangCombo->setCurrentIndex(idx);
            logArea->append(QString("<font color='#00BFFF'>Idioma origen detectado: %1</font>")
                .arg(SubtitleTranslator::languageName(mappedLang)));
        }

        // Check if translation model exists for this pair
        QString dstLang = dstLangCombo->currentData().toString();
        if (!dstLang.isEmpty() && mappedLang != dstLang) {
            if (!subtitleTranslator->isModelInstalled(mappedLang, dstLang)) {
                // Check for triangular path
                QStringList triPath = subtitleTranslator->findTranslationPath(mappedLang, dstLang);
                if (!triPath.isEmpty() && triPath.size() > 2) {
                    // Triangular translation available — inform user
                    QString pathStr;
                    for (int i = 0; i < triPath.size(); ++i) {
                        if (i > 0) pathStr += " → ";
                        pathStr += SubtitleTranslator::languageName(triPath[i]);
                    }

                    QMessageBox msgBox(this);
                    msgBox.setWindowTitle("Traducción triangular disponible");
                    msgBox.setIcon(QMessageBox::Information);
                    msgBox.setText(
                        QString("No hay modelo directo de <b>%1 → %2</b>.<br><br>"
                                "Pero se usará <b>traducción triangular</b>:<br>%3<br><br>"
                                "Esto puede tardar un poco más, pero funcionará correctamente.")
                            .arg(SubtitleTranslator::languageName(mappedLang))
                            .arg(SubtitleTranslator::languageName(dstLang))
                            .arg(pathStr));
                    msgBox.setInformativeText("¿Qué deseas hacer?");
                    QPushButton *okBtn = msgBox.addButton("Aceptar", QMessageBox::AcceptRole);
                    QPushButton *selectBtn = msgBox.addButton("Cambiar idioma destino", QMessageBox::ActionRole);
                    msgBox.setDefaultButton(okBtn);
                    msgBox.exec();

                    if (msgBox.clickedButton() == selectBtn) {
                        dstLangCombo->setFocus();
                    }
                    // else: continue with triangular translation (okBtn or close)
                } else {
                    // No model at all — recommend download
                    QString modelId = mappedLang + "-" + dstLang + "-tiny";
                    QString modelName = SubtitleTranslator::languageName(mappedLang) + " → " +
                                        SubtitleTranslator::languageName(dstLang);

                    QMessageBox msgBox(this);
                    msgBox.setWindowTitle("Modelo de traducción no encontrado");
                    msgBox.setIcon(QMessageBox::Information);
                    msgBox.setText(
                        QString("No se encontró modelo de traducción para:<br><b>%1</b><br><br>"
                                "Tampoco hay ruta triangular disponible.<br><br>"
                                "Idioma detectado: %2 → %3")
                            .arg(modelName)
                            .arg(SubtitleTranslator::languageName(mappedLang))
                            .arg(SubtitleTranslator::languageName(dstLang)));
                    msgBox.setInformativeText("¿Qué deseas hacer?");
                    QPushButton *downloadBtn = msgBox.addButton("Descargar modelo", QMessageBox::AcceptRole);
                    QPushButton *selectBtn = msgBox.addButton("Cambiar idioma destino", QMessageBox::ActionRole);
                    QPushButton *skipBtn = msgBox.addButton("Continuar sin traducir", QMessageBox::RejectRole);
                    msgBox.setDefaultButton(downloadBtn);
                    msgBox.exec();

                    if (msgBox.clickedButton() == downloadBtn) {
                        showSettings();
                    } else if (msgBox.clickedButton() == selectBtn) {
                        dstLangCombo->setFocus();
                    }
                    // else: continue without translation (skipBtn or close)
                }
            }
        }
    }
}

void MainWindow::onStreamToggled(int) { saveCurrentSelections(); }

void MainWindow::selectAllVideo()
{
    if (globalSelectionTypes.contains("video")) globalSelectionTypes.remove("video");
    else globalSelectionTypes.insert("video");
    updateCheckboxesFromGlobal();
}
void MainWindow::selectAllAudio()
{
    if (globalSelectionTypes.contains("audio")) globalSelectionTypes.remove("audio");
    else globalSelectionTypes.insert("audio");
    updateCheckboxesFromGlobal();
}
void MainWindow::selectAllSubs()
{
    if (globalSelectionTypes.contains("subtitle")) globalSelectionTypes.remove("subtitle");
    else globalSelectionTypes.insert("subtitle");
    updateCheckboxesFromGlobal();
}
void MainWindow::clearAllSelections()
{
    globalSelectionTypes.clear();
    fileSelections.clear();
    for (QCheckBox *cb : streamCheckBoxes) cb->setChecked(false);
    saveCurrentSelections();
}

void MainWindow::updateCheckboxesFromGlobal()
{
    for (QCheckBox *cb : streamCheckBoxes) {
        QString type = cb->property("streamType").toString();
        cb->setChecked(globalSelectionTypes.contains(type));
    }
    saveCurrentSelections();
}

// ─────────────────────────────────────────────────────────────────────────────
// ffprobe
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::readProbeOutput() { probeData.append(probeProcess->readAllStandardOutput()); }

void MainWindow::probeFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    statusBar()->showMessage("Listo");
    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        metaInfoLabel->setText("No se pudo analizar el archivo.");
        logArea->append("<font color='#FF4444'>[ERROR] No se pudo analizar el archivo.</font>"); return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(probeData);
    QJsonObject   rootObj = doc.object();
    QJsonObject   fmt     = rootObj.value("format").toObject();
    QJsonArray    streams = rootObj.value("streams").toArray();

    // Duración y tamaño
    double  dur  = fmt.value("duration").toString().toDouble();
    qint64  size = fmt.value("size").toString().toLongLong();
    if (dur > 0) fileDurations[lastSelectedFile] = (qint64)(dur * 1000);

    // Recopilar info para el panel de metadatos
    QString videoInfo, audioCount, subCount;
    int nAudio = 0, nSubs = 0;

    QMap<int, QString> saved = fileSelections.value(lastSelectedFile);
    QMap<int, QString> langMap;
    for (int i = 0; i < streams.size(); ++i) {
        QJsonObject s = streams.at(i).toObject();
        int     index = s.value("index").toInt();
        QString type  = s.value("codec_type").toString();
        QString codec = s.value("codec_name").toString();
        QString lang  = s.value("tags").toObject().value("language").toString();
        int     w     = s.value("width").toInt();
        int     h     = s.value("height").toInt();

        if (!lang.isEmpty()) langMap[index] = lang;

        if (type == "video" && videoInfo.isEmpty() && w > 0)
            videoInfo = QString("%1x%2 %3").arg(w).arg(h).arg(codec.toUpper());
        else if (type == "video" && videoInfo.isEmpty())
            videoInfo = codec.toUpper();
        if (type == "audio")    ++nAudio;
        if (type == "subtitle") ++nSubs;

        QString label = QString("[%1] %2  ·  %3%4")
                            .arg(index)
                            .arg(type.toUpper())
                            .arg(codec)
                            .arg(lang.isEmpty() ? "" : "  [" + lang + "]");
        if (type == "video" && w > 0)
            label += QString("  %1x%2").arg(w).arg(h);

        QCheckBox *cb = new QCheckBox(label);
        cb->setProperty("streamIndex", index);
        cb->setProperty("streamType",  type);
        cb->setProperty("streamCodec", codec);
        if (saved.contains(index) || globalSelectionTypes.contains(type))
            cb->setChecked(true);
        connect(cb, &QCheckBox::stateChanged, this, &MainWindow::onStreamToggled);
        streamsLayout->addWidget(cb);
        streamCheckBoxes.append(cb);
    }

    fileStreamLanguages[lastSelectedFile] = langMap;

    // Actualizar panel de metadatos
    updateMetadataPanel(dur, size, videoInfo, nAudio, nSubs);

    autoDetectSourceLanguage();
}

// ─────────────────────────────────────────────────────────────────────────────
// Extracción
// ─────────────────────────────────────────────────────────────────────────────
QString MainWindow::extensionForType(const QString &type)
{
    if (type == "audio")    return audioFormatCombo->currentText();
    if (type == "video")    return videoFormatCombo->currentText();
    if (type == "subtitle") return subsFormatCombo->currentText();
    return "mkv";
}

QString MainWindow::extensionForCodec(const QString &codec, const QString &type)
{
    if (codec == "aac") return "m4a";
    if (codec == "ac3" || codec == "mp3" || codec == "flac" || codec == "wav" || codec == "opus") return codec;
    if (codec == "subrip") return "srt";
    if (codec == "ass" || codec == "ssa") return "ass";
    if (codec == "webvtt") return "vtt";
    if (type == "video") return "mkv";
    if (type == "audio") return "mka";
    if (type == "subtitle") return "mks";
    return "mkv";
}

void MainWindow::startExtraction()
{
    saveCurrentSelections();
    batchFiles.clear();
    extractedSubtitleFiles.clear();
    for (int i = 0; i < fileListWidget->count(); ++i)
        batchFiles << fileListWidget->item(i)->data(Qt::UserRole).toString();
    if (batchFiles.isEmpty()) return;

    currentBatchIndex = 0;
    logArea->clear();
    logArea->append("<font color='#FF8800'><b>=== INICIANDO EXTRACCIÓN ===</b></font>");
    extractButton->setEnabled(false);
    cancelButton->setEnabled(true);
    progressBar->setValue(0);
    processNextInBatch();
}

void MainWindow::cancelExtraction()
{
    if (process->state() != QProcess::NotRunning) {
        process->kill();
        logArea->append("<br><font color='#FF4444'><b>[CANCELADO] El proceso fue interrumpido por el usuario.</b></font>");
        statusBar()->showMessage("Cancelado");
    }
    extractButton->setEnabled(true);
    cancelButton->setEnabled(false);
    progressBar->setValue(0);
    currentBatchIndex = -1;
}

void MainWindow::processNextInBatch()
{
    if (currentBatchIndex < 0) return; // cancelado
    if (currentBatchIndex >= batchFiles.size()) {
        logArea->append("<br><font color='#39FF14'><b>=== ✓ PROCESO FINALIZADO ===</b></font>");
        statusBar()->showMessage(QString("✓ Finalizado — %1 archivo(s) procesado(s)")
                                     .arg(batchFiles.size()));
        extractButton->setEnabled(true);
        cancelButton->setEnabled(false);
        progressBar->setValue(100);

        // Auto-translate extracted subtitles if checkbox is checked
        if (translateOnExtractCheck->isChecked() && !extractedSubtitleFiles.isEmpty()) {
            QString srcLang = srcLangCombo->currentData().toString();
            QString dstLang = dstLangCombo->currentData().toString();
            logArea->append(QString("<font color='#888888'>  [DEBUG] translateCheck=ON, srcLang='%1', dstLang='%2'</font>")
                .arg(srcLang, dstLang));
            if (srcLang != dstLang) {
                QStringList existingFiles;
                for (const QString &f : extractedSubtitleFiles)
                    if (QFile::exists(f)) existingFiles.append(f);
                extractedSubtitleFiles = existingFiles;
                logArea->append(QString("<font color='#888888'>  [DEBUG] files to translate: %1</font>")
                    .arg(extractedSubtitleFiles.size()));

                if (!extractedSubtitleFiles.isEmpty()) {
                    logArea->append(QString("<br><font color='#00BFFF'><b>Traduciendo %1 archivo(s) de subtítulos...</b></font>")
                        .arg(extractedSubtitleFiles.size()));
                    translationProgressBar->setVisible(true);
                    translationProgressBar->setValue(0);

                    subtitleTranslator->translateBatch(extractedSubtitleFiles, outputLineEdit->text(), srcLang, dstLang);
                    return;
                }
            } else {
                logArea->append("<font color='#FFCC00'>  [DEBUG] srcLang == dstLang, traducción omitida</font>");
            }
        }

        if (trayIcon->isSystemTrayAvailable()) {
            trayIcon->showMessage("Elements Extract", "El proceso por lotes ha finalizado correctamente.", QSystemTrayIcon::Information, 4000);
        }
        return;
    }

    QString inputPath = batchFiles.at(currentBatchIndex);
    QFileInfo info(inputPath);
    currentFileDuration = fileDurations.value(inputPath, 0);
    progressBar->setValue(0);

    statusBar()->showMessage(QString("Procesando %1/%2: %3")
                                 .arg(currentBatchIndex + 1)
                                 .arg(batchFiles.size())
                                 .arg(info.fileName()));
    logArea->append(QString("<br><font color='#00BFFF'><b>[%1/%2] %3</b></font>")
                        .arg(currentBatchIndex + 1)
                        .arg(batchFiles.size())
                        .arg(info.fileName().toHtmlEscaped()));

    QString outDir  = outputLineEdit->text();
    QString baseName = outDir + "/" + info.baseName();

    QMap<int, QString> selectedTypes = fileSelections.value(inputPath);
    QMap<int, QString> selectedCodecs = fileStreamCodecs.value(inputPath);
    QStringList finalArgs;
    finalArgs << "-i" << inputPath;
    bool anyAction = false;
    bool copyMode = copyModeCheckBox->isChecked();

    if (selectedTypes.isEmpty() && !globalSelectionTypes.isEmpty()) {
        // Selección global por tipo
        for (const QString &type : globalSelectionTypes) {
            QString mapCode = (type == "audio") ? "a" : (type == "video" ? "v" : "s");
            QString ext = copyMode ? (type == "audio" ? "mka" : (type == "video" ? "mkv" : "mks")) : extensionForType(type);
            finalArgs << "-map" << QString("0:%1").arg(mapCode);
            if (copyMode) {
                finalArgs << "-c" << "copy";
            } else if (type == "video" && videoResCombo->currentText() != "Original") {
                QString res = videoResCombo->currentText();
                if (res.contains("2160p")) finalArgs << "-vf" << "scale=-2:2160";
                else if (res.contains("1440p")) finalArgs << "-vf" << "scale=-2:1440";
                else if (res.contains("1080p")) finalArgs << "-vf" << "scale=-2:1080";
                else if (res.contains("720p")) finalArgs << "-vf" << "scale=-2:720";
                else if (res.contains("480p")) finalArgs << "-vf" << "scale=-2:480";
            }
            QString outPath = getUniqueFilePath(baseName + "_all_" + type + "." + ext);
            finalArgs << "-y" << outPath;
            if (type == "subtitle") extractedSubtitleFiles.append(outPath);
            anyAction = true;
        }
    } else {
        // Selección individual por índice
        for (auto it = selectedTypes.constBegin(); it != selectedTypes.constEnd(); ++it) {
            int index = it.key();
            QString type = it.value();
            QString codec = selectedCodecs.value(index);
            QString ext = copyMode ? extensionForCodec(codec, type) : extensionForType(type);
            finalArgs << "-map" << QString("0:%1").arg(index);
            if (copyMode) {
                finalArgs << "-c" << "copy";
            } else if (type == "video" && videoResCombo->currentText() != "Original") {
                QString res = videoResCombo->currentText();
                if (res.contains("2160p")) finalArgs << "-vf" << "scale=-2:2160";
                else if (res.contains("1440p")) finalArgs << "-vf" << "scale=-2:1440";
                else if (res.contains("1080p")) finalArgs << "-vf" << "scale=-2:1080";
                else if (res.contains("720p")) finalArgs << "-vf" << "scale=-2:720";
                else if (res.contains("480p")) finalArgs << "-vf" << "scale=-2:480";
            }
            QString outPath = getUniqueFilePath(baseName + "_track" + QString::number(index) + "." + ext);
            finalArgs << "-y" << outPath;
            if (type == "subtitle") extractedSubtitleFiles.append(outPath);
            anyAction = true;
        }
    }

    if (!anyAction) {
        logArea->append("<font color='#FFCC00'>  › Sin pistas seleccionadas para este archivo, omitido.</font>");
        currentBatchIndex++;
        processNextInBatch();
        return;
    }

    process->start("ffmpeg", finalArgs);
}

void MainWindow::readProcessOutput()
{
    QString out = QString::fromLocal8Bit(process->readAllStandardError());
    if (out.isEmpty()) out = QString::fromLocal8Bit(process->readAllStandardOutput());
    updateProgressFromLine(out);
    // Solo loguear líneas que no sean el progreso (evitar spam)
    if (!out.contains("time=") && !out.trimmed().isEmpty()) {
        QString text = out.trimmed().toHtmlEscaped();
        if (text.contains("Error", Qt::CaseInsensitive) || text.contains("Invalid data found", Qt::CaseInsensitive))
            logArea->append("<font color='#FF4444'>" + text + "</font>");
        else if (text.contains("Warning", Qt::CaseInsensitive))
            logArea->append("<font color='#FFCC00'>" + text + "</font>");
        else
            logArea->append("<font color='#888888'>" + text + "</font>");
    }
}

void MainWindow::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        logArea->append("<font color='#39FF14'>  › OK</font>");
        progressBar->setValue(100);
    } else if (currentBatchIndex >= 0) {
        logArea->append("<font color='#FF4444'>  › Error en este archivo.</font>");
    }
    if (currentBatchIndex >= 0) {
        currentBatchIndex++;
        processNextInBatch();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Progreso real desde la salida de FFmpeg
// ─────────────────────────────────────────────────────────────────────────────
qint64 MainWindow::parseTimestamp(const QString &ts)
{
    // Formato: HH:MM:SS.ss
    QStringList p = ts.split(':');
    if (p.size() != 3) return 0;
    return (qint64)((p[0].toInt() * 3600 + p[1].toInt() * 60 + p[2].toDouble()) * 1000);
}

void MainWindow::updateProgressFromLine(const QString &line)
{
    if (currentFileDuration <= 0) return;
    static QRegularExpression re("time=(\\d+:\\d+:\\d+\\.\\d+)");
    QRegularExpressionMatch m = re.match(line);
    if (m.hasMatch()) {
        qint64 current = parseTimestamp(m.captured(1));
        int pct = (int)(100.0 * current / currentFileDuration);
        progressBar->setValue(qMin(pct, 99));
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Diálogos
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::showAbout()
{
    AboutDialog dlg(this);
    dlg.exec();
}

void MainWindow::showHelp()
{
    HelpDialog dlg(this);
    dlg.exec();
}

// ─────────────────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::selectOutputDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Destino", outputLineEdit->text());
    if (!dir.isEmpty()) outputLineEdit->setText(dir);
}

void MainWindow::openOutputFolder()
{
    QString dir = outputLineEdit->text();
    if (!dir.isEmpty()) QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
}

void MainWindow::clearStreamsList()
{
    qDeleteAll(streamCheckBoxes);
    streamCheckBoxes.clear();
}

QString MainWindow::getUniqueFilePath(const QString &fullPath)
{
    if (!QFile::exists(fullPath)) return fullPath;
    QFileInfo info(fullPath);
    int c = 2;
    while (QFile::exists(info.absolutePath() + "/" + info.baseName()
                         + "_v" + QString::number(c) + "." + info.suffix())) c++;
    return info.absolutePath() + "/" + info.baseName()
           + "_v" + QString::number(c) + "." + info.suffix();
}

// ─────────────────────────────────────────────────────────────────────────────
// Drag & Drop
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QStringList files;
    for (const QUrl &url : event->mimeData()->urls()) {
        QString f = url.toLocalFile();
        if (!f.isEmpty())
            files << f;
    }
    loadFiles(files);
    event->acceptProposedAction();
}
// ─────────────────────────────────────────────────────────────────────────────
// Metadatos del archivo
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::updateMetadataPanel(double durSecs, qint64 sizeBytes,
                                      const QString &videoInfo,
                                      int nAudio, int nSubs)
{
    // Duración formateada
    int h  = (int)(durSecs / 3600);
    int m  = (int)(durSecs / 60) % 60;
    int s  = (int)durSecs % 60;
    QString durStr = QString("%1:%2:%3")
                         .arg(h, 2, 10, QChar('0'))
                         .arg(m, 2, 10, QChar('0'))
                         .arg(s, 2, 10, QChar('0'));

    // Tamaño formateado
    QString sizeStr;
    if      (sizeBytes > 1073741824) sizeStr = QString::number(sizeBytes / 1073741824.0, 'f', 1) + " GB";
    else if (sizeBytes > 1048576)    sizeStr = QString::number(sizeBytes / 1048576.0,    'f', 1) + " MB";
    else                             sizeStr = QString::number(sizeBytes / 1024.0,        'f', 0) + " KB";

    QStringList parts;
    if (durSecs  > 0)        parts << "⏱ " + durStr;
    if (sizeBytes > 0)       parts << "💾 " + sizeStr;
    if (!videoInfo.isEmpty()) parts << "🎬 " + videoInfo;
    if (nAudio > 0)          parts << QString("🎵 %1 audio").arg(nAudio);
    if (nSubs  > 0)          parts << QString("CC %1 subs").arg(nSubs);

    metaInfoLabel->setText(parts.join("   "));
}

// ─────────────────────────────────────────────────────────────────────────────
// Atajos de teclado y persistencia
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete)
        removeFiles();
    else if (event->key() == Qt::Key_Escape && cancelButton->isEnabled())
        cancelExtraction();
    else
        QMainWindow::keyPressEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick || reason == QSystemTrayIcon::Trigger) {
        if (isVisible()) {
            hide();
        } else {
            show();
            raise();
            activateWindow();
        }
    }
}

void MainWindow::loadSettings()
{
    QSettings s("LocalApp", "FFmpegExtractor");
    outputLineEdit->setText(s.value("outputDir", "").toString());
    audioFormatCombo->setCurrentText(s.value("audioFormat", "mka").toString());
    videoFormatCombo->setCurrentText(s.value("videoFormat", "mkv").toString());
    videoResCombo->setCurrentText(s.value("videoRes", "Original").toString());
    subsFormatCombo ->setCurrentText(s.value("subsFormat",  "srt").toString());
    copyModeCheckBox->setChecked(s.value("copyMode", true).toBool());

    // Load translation language settings
    QString srcLang = s.value("srcLang", "en").toString();
    QString dstLang = s.value("dstLang", "es").toString();
    int srcIdx = srcLangCombo->findData(srcLang);
    int dstIdx = dstLangCombo->findData(dstLang);
    if (srcIdx >= 0) srcLangCombo->setCurrentIndex(srcIdx);
    if (dstIdx >= 0) dstLangCombo->setCurrentIndex(dstIdx);

    // Migrate from old geometry format
    if (s.contains("geometry")) {
        s.remove("geometry");
    }

    // Restore window geometry safely
    if (s.contains("windowWidth") && s.contains("windowHeight")) {
        int w = s.value("windowWidth", 980).toInt();
        int h = s.value("windowHeight", 680).toInt();
        int x = s.value("windowX", -1).toInt();
        int y = s.value("windowY", -1).toInt();
        resize(w, h);
        if (x >= 0 && y >= 0) move(x, y);
        if (s.value("windowMaximized", false).toBool()) {
            showMaximized();
        }
    }
}

void MainWindow::saveSettings()
{
    QSettings s("LocalApp", "FFmpegExtractor");
    s.setValue("outputDir",   outputLineEdit->text());
    s.setValue("audioFormat", audioFormatCombo->currentText());
    s.setValue("videoFormat", videoFormatCombo->currentText());
    s.setValue("videoRes",    videoResCombo->currentText());
    s.setValue("subsFormat",  subsFormatCombo->currentText());
    s.setValue("copyMode",    copyModeCheckBox->isChecked());
    s.setValue("srcLang",     srcLangCombo->currentData().toString());
    s.setValue("dstLang",     dstLangCombo->currentData().toString());

    // Save window geometry (use normalGeometry when maximized)
    s.setValue("windowMaximized", isMaximized());
    QRect geo = isMaximized() ? normalGeometry() : geometry();
    s.setValue("windowX", geo.x());
    s.setValue("windowY", geo.y());
    s.setValue("windowWidth", geo.width());
    s.setValue("windowHeight", geo.height());
}

// ─────────────────────────────────────────────────────────────────────────────
// Translation
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::translateExtractedSubtitles(const QString &subtitlePath)
{
    QFileInfo fi(subtitlePath);
    if (!fi.exists()) {
        logArea->append(QString("<font color='#FF4444'>No se encontró el archivo: %1</font>")
            .arg(fi.fileName()));
        return;
    }

    QString srcLang = srcLangCombo->currentData().toString();
    QString dstLang = dstLangCombo->currentData().toString();

    if (srcLang == dstLang) {
        logArea->append("<font color='#FFCC00'>Idioma origen y destino son iguales.</font>");
        return;
    }

    QString outputPath = fi.absolutePath() + "/" + fi.completeBaseName() + "." + dstLang + "." + fi.suffix();

    translationProgressBar->setVisible(true);
    translationProgressBar->setValue(0);
    translationStatusLabel->setText("Traduciendo...");

    logArea->append(QString("<br><font color='#00BFFF'><b>Traduciendo: %1 → %2</b></font>")
        .arg(SubtitleTranslator::languageName(srcLang))
        .arg(SubtitleTranslator::languageName(dstLang)));

    subtitleTranslator->translateFile(subtitlePath, outputPath, srcLang, dstLang);
}

void MainWindow::onTranslationProgress(int current, int total)
{
    if (total > 0) {
        int pct = (current * 100) / total;
        translationProgressBar->setValue(pct);
        translationStatusLabel->setText(QString("Traduciendo: %1 / %2 líneas").arg(current).arg(total));
    }
}

void MainWindow::onFileTranslated(const QString &input, const QString &output)
{
    Q_UNUSED(input);
    translationProgressBar->setValue(100);
    translationStatusLabel->setText("Traducción completada: " + QFileInfo(output).fileName());

    logArea->append(QString("<font color='#39FF14'>  ✓ Traducción guardada: %1</font>")
        .arg(QFileInfo(output).fileName()));
}

void MainWindow::onTranslationError(const QString &message)
{
    translationProgressBar->setVisible(false);
    translationStatusLabel->setText("Error en la traducción");

    logArea->append(QString("<font color='#FF4444'>  ✗ Error de traducción: %1</font>").arg(message));

    QMessageBox::critical(this, "Error de traducción", message);
}

void MainWindow::onTranslationFinished()
{
    translationProgressBar->setVisible(false);
    extractButton->setEnabled(true);

    if (trayIcon->isSystemTrayAvailable()) {
        trayIcon->showMessage("Elements Extract", "La traducción ha finalizado correctamente.",
                              QSystemTrayIcon::Information, 4000);
    }
}

void MainWindow::showModelManager()
{
    QMessageBox info(this);
    info.setWindowTitle("Gestionar Modelos de Traducción");
    info.setIcon(QMessageBox::Information);
    info.setText(
        "<b>Modelos de traducción</b><br><br>"
        "Los modelos se descargan automáticamente cuando se necesita un par de idiomas no disponible.<br><br>"
        "<b>Modelos incluidos:</b><br>"
        "• English → Español (en→es)<br>"
        "• Español → English (es→en)<br>"
        "• English → Français (en→fr)<br>"
        "• Français → English (fr→en)<br>"
        "• English → Português (en→pt)<br>"
        "• Português → English (pt→en)<br><br>"
        "<b>Directorio de modelos:</b><br>" +
        subtitleTranslator->modelDirectory()
    );
    info.exec();
}

void MainWindow::onModelsFetched()
{
    // Models list updated
}

void MainWindow::onModelDownloadProgress(const QString &modelId, qint64 received, qint64 total)
{
    if (total > 0) {
        int pct = (received * 100) / total;
        translationStatusLabel->setText(QString("Descargando modelo %1: %2%")
            .arg(modelId).arg(pct));
    }
}

void MainWindow::onModelDownloaded(const QString &modelId)
{
    translationStatusLabel->setText("Modelo descargado: " + modelId);
    logArea->append(QString("<font color='#39FF14'>  ✓ Modelo descargado: %1</font>").arg(modelId));
}

void MainWindow::onModelDownloadError(const QString &modelId, const QString &message)
{
    translationStatusLabel->setText("Error descargando modelo: " + modelId);
    logArea->append(QString("<font color='#FF4444'>  ✗ Error descargando %1: %2</font>")
        .arg(modelId, message));
}

void MainWindow::changeUILanguage(const QString &langCode)
{
    // TODO: Implement UI language switching with QTranslator
    logArea->append(QString("<font color='#888888'>Idioma de interfaz cambiado a: %1</font>")
        .arg(langCode));
}

// ─────────────────────────────────────────────────────────────────────────────
// Settings
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::showSettings()
{
    SettingsDialog dlg(subtitleTranslator, modelDownloader, this);
    connect(&dlg, &SettingsDialog::settingsChanged, this, &MainWindow::onSettingsChanged);
    dlg.exec();
}

void MainWindow::onSettingsChanged()
{
    // Reload settings after dialog closes
    loadSettings();
    logArea->append("<font color='#888888'>Configuración actualizada</font>");
}
