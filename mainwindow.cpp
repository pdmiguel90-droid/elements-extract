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
#include <QTranslator>
#include <QLocale>
#include <QScreen>
#include <QGuiApplication>
#include <algorithm>

// ─────────────────────────────────────────────────────────────────────────────
// Constructor / Destructor
// ─────────────────────────────────────────────────────────────────────────────
static QString normalizeLangCode(const QString &raw)
{
    const QString c = raw.trimmed().toLower();
    static const QMap<QString, QString> map = {
        {"en", "en"}, {"eng", "en"}, {"english", "en"},
        {"es", "es"}, {"spa", "es"}, {"spanish", "es"}, {"cast", "es"},
        {"fr", "fr"}, {"fra", "fr"}, {"fre", "fr"}, {"french", "fr"},
        {"pt", "pt"}, {"por", "pt"}, {"portuguese", "pt"},
        {"de", "de"}, {"ger", "de"}, {"deu", "de"}, {"german", "de"},
        {"it", "it"}, {"ita", "it"}, {"italian", "it"},
        {"ja", "ja"}, {"jpn", "ja"}, {"japanese", "ja"},
        {"zh", "zh"}, {"chi", "zh"}, {"zho", "zh"}, {"chinese", "zh"},
        {"ko", "ko"}, {"kor", "ko"}, {"korean", "ko"},
        {"ru", "ru"}, {"rus", "ru"}, {"russian", "ru"},
        {"ar", "ar"}, {"ara", "ar"}, {"arabic", "ar"},
        {"hi", "hi"}, {"hin", "hi"}, {"hindi", "hi"},
        {"nl", "nl"}, {"dut", "nl"}, {"nld", "nl"}, {"dutch", "nl"},
        {"sv", "sv"}, {"swe", "sv"}, {"swedish", "sv"},
        {"pl", "pl"}, {"pol", "pl"}, {"polish", "pl"},
        {"tr", "tr"}, {"tur", "tr"}, {"turkish", "tr"}
    };
    return map.value(c, c);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentBatchIndex(-1), currentFileDuration(0),
      m_muxIndex(0), m_muxActive(false)
{
    setupUi();
    setupMenuBar();
    setAcceptDrops(true);
    loadSettings();

    // Initialize translation engine
    subtitleTranslator = new SubtitleTranslator(this);
    // Check development path first, then installed path. Los modelos
    // descargados necesitan un directorio ESCRIBIBLE (/usr/share o /app no
    // lo son para .deb/Flatpak): si el instalado no es escribible se usa el
    // de datos del usuario. Los incluidos se siguen encontrando por las
    // rutas de reserva de modelPathForPair().
    QString devModelsPath = QCoreApplication::applicationDirPath() + "/../models";
    QString installedModelsPath = QCoreApplication::applicationDirPath() + "/../share/elements-extract/models";
    QString modelsPath;
    if (QDir(devModelsPath).exists()) {
        modelsPath = devModelsPath;
    } else if (QDir(installedModelsPath).exists()
               && QFileInfo(installedModelsPath).isWritable()) {
        modelsPath = installedModelsPath;
    } else {
        modelsPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/models";
    }
    subtitleTranslator->setModelDirectory(modelsPath);

    // bergamot binary found automatically by findBergamotBinary()

    // Initialize model downloader — use SAME directory as translator
    modelDownloader = new ModelDownloader(this);
    modelDownloader->setModelsDirectory(modelsPath);
    // Calentar la caché del registro online (async): permite avisar qué
    // modelos existen para descargar cuando falte uno al traducir.
    modelDownloader->fetchAvailableModels();

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
        logArea->append(QString("<font color='#FFD700'>" + tr("↻ Traducción triangular: %1") + "</font>").arg(pathStr));
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

    QAction *restoreAction = trayMenu->addAction(tr("Mostrar / Ocultar ventana"));
    connect(restoreAction, &QAction::triggered, this, [this]() {
        if (isVisible() && !isMinimized()) hide();
        else { showNormal(); activateWindow(); }
    });

    QAction *cancelAction = trayMenu->addAction(tr("Cancelar proceso actual"));
    connect(cancelAction, &QAction::triggered, this, &MainWindow::cancelExtraction);

    trayMenu->addSeparator();

    QAction *quitAction = trayMenu->addAction(tr("Salir"));
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    trayIcon->setContextMenu(trayMenu);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);
    trayIcon->show();

    process = new QProcess(this);
    connect(process, &QProcess::readyReadStandardOutput, this, &MainWindow::readProcessOutput);
    connect(process, &QProcess::readyReadStandardError,  this, &MainWindow::readProcessOutput);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::processFinished);
    connect(process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError e) {
        if (e == QProcess::FailedToStart) {
            logArea->append(tr("<font color='#FF4444'>  › No se pudo iniciar ffmpeg. ¿Está instalado?.</font>"));
            if (m_muxActive) {
                m_muxIndex++;
                processNextMux();
            } else if (currentBatchIndex >= 0) {
                currentBatchIndex++;
                processNextInBatch();
            }
        }
    });

    probeProcess = new QProcess(this);
    connect(probeProcess, &QProcess::readyReadStandardOutput, this, &MainWindow::readProbeOutput);
    connect(probeProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::probeFinished);
}

MainWindow::~MainWindow() {}

// ─────────────────────────────────────────────────────────────────────────────
// Menú
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::setupMenuBar()
{
    menuArchivo = menuBar()->addMenu(tr("Archivo"));
    actAnadir = menuArchivo->addAction(
        QIcon(":/icons/add.svg"), tr("Añadir archivos…"));
    actAnadir->setShortcut(QKeySequence("Ctrl+O"));
    connect(actAnadir, &QAction::triggered, this, &MainWindow::addFiles);
    menuArchivo->addSeparator();
    actSalir = menuArchivo->addAction(
        QIcon(":/icons/close.svg"), tr("Salir"));
    actSalir->setShortcut(QKeySequence("Ctrl+Q"));
    connect(actSalir, &QAction::triggered, this, &QWidget::close);

    menuAjustes = menuBar()->addMenu(tr("Ajustes"));
    actSettings = menuAjustes->addAction(
        QIcon(":/icons/settings.svg"), tr("Configuración y Modelos…"));
    actSettings->setShortcut(QKeySequence("Ctrl+,"));
    connect(actSettings, &QAction::triggered, this, &MainWindow::showSettings);

    menuAyuda = menuBar()->addMenu(tr("Ayuda"));
    actAyuda = menuAyuda->addAction(
        QIcon(":/icons/help.svg"), tr("Manual de uso"));
    actAyuda->setShortcut(Qt::Key_F1);
    connect(actAyuda, &QAction::triggered, this, &MainWindow::showHelp);
    menuAyuda->addSeparator();
    actAcerca = menuAyuda->addAction(
        QIcon(":/icons/check.svg"), tr("Acerca de…"));
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

    // Título (compacto)
    QLabel *titleLabel = new QLabel("ELEMENTS EXTRACT FFMPEG");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setProperty("labelColor", "title");
    root->addWidget(titleLabel);

    // ══════════ Splitter principal: archivos+pistas  │  configuración ──────
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal);
    mainSplitter->setChildrenCollapsible(false);

    // ── Panel izquierdo: lote + pistas ──
    QWidget *leftWidget = new QWidget();
    QVBoxLayout *leftPanel = new QVBoxLayout(leftWidget);
    leftPanel->setContentsMargins(0, 0, 0, 0);
    leftPanel->setSpacing(4);

    loteLabel = new QLabel(tr("LOTE DE ARCHIVOS"));
    loteLabel->setProperty("labelColor", "muted");
    leftPanel->addWidget(loteLabel);

    fileListWidget = new QListWidget();
    fileListWidget->setDragDropMode(QAbstractItemView::InternalMove);
    connect(fileListWidget, &QListWidget::itemSelectionChanged,
            this, &MainWindow::onFileSelectionChanged);
    leftPanel->addWidget(fileListWidget, 3);

    QHBoxLayout *fileBtns = new QHBoxLayout();
    fileBtns->setSpacing(6);
    addButton = new QPushButton(QIcon(":/icons/add.svg"), tr("Añadir"));
    removeButton = new QPushButton(QIcon(":/icons/remove.svg"), tr("Quitar"));
    addButton->setToolTip(tr("Añadir archivos de video (Ctrl+O)"));
    removeButton->setToolTip(tr("Quitar archivos seleccionados (Del)"));
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addFiles);
    connect(removeButton, &QPushButton::clicked, this, &MainWindow::removeFiles);
    fileBtns->addWidget(addButton);
    fileBtns->addWidget(removeButton);
    fileBtns->addStretch();
    leftPanel->addLayout(fileBtns);

    pistasLabel = new QLabel(tr("PISTAS"));
    pistasLabel->setProperty("labelColor", "muted");
    leftPanel->addWidget(pistasLabel);

    selectedFileLabel = new QLabel(tr("Selecciona un archivo para ver sus pistas"));
    selectedFileLabel->setProperty("labelColor", "accent");
    leftPanel->addWidget(selectedFileLabel);

    metaInfoLabel = new QLabel("");
    metaInfoLabel->setProperty("labelColor", "muted");
    metaInfoLabel->setWordWrap(true);
    leftPanel->addWidget(metaInfoLabel);

    QHBoxLayout *quickSel = new QHBoxLayout();
    quickSel->setSpacing(4);
    videoAllButton = new QPushButton(QIcon(":/icons/video.svg"), tr("VIDEO"));
    audioAllButton = new QPushButton(QIcon(":/icons/audio.svg"), tr("AUDIO"));
    subsAllButton  = new QPushButton(QIcon(":/icons/subtitle.svg"), tr("SUBS"));
    clearAllButton = new QPushButton(QIcon(":/icons/clear.svg"), tr("Limpiar"));
    for (auto *b : {videoAllButton, audioAllButton, subsAllButton, clearAllButton}) {
        b->setObjectName("quickSelButton");
        b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        b->setFixedHeight(24);
    }
    connect(videoAllButton, &QPushButton::clicked, this, &MainWindow::selectAllVideo);
    connect(audioAllButton, &QPushButton::clicked, this, &MainWindow::selectAllAudio);
    connect(subsAllButton,  &QPushButton::clicked, this, &MainWindow::selectAllSubs);
    connect(clearAllButton, &QPushButton::clicked, this, &MainWindow::clearAllSelections);
    quickSel->addWidget(videoAllButton);
    quickSel->addWidget(audioAllButton);
    quickSel->addWidget(subsAllButton);
    quickSel->addWidget(clearAllButton);
    leftPanel->addLayout(quickSel);

    QHBoxLayout *subFilterRow = new QHBoxLayout();
    subFilterRow->setSpacing(4);
    subFilterLabel = new QLabel(tr("Solo subs:"));
    subFilterLabel->setProperty("labelColor", "muted");
    subFilterCombo = new QComboBox();
    subFilterCombo->addItem(tr("Todos los idiomas"), "");
    subFilterCombo->setToolTip(
        tr("Selecciona los subtítulos de un idioma concreto en TODOS los archivos del lote.\n"
           "Mientras esté activo, la extracción y la traducción trabajarán solo con ese idioma."));
    connect(subFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onSubLangFilterChanged);
    subFilterRow->addWidget(subFilterLabel);
    subFilterRow->addWidget(subFilterCombo, 1);
    leftPanel->addLayout(subFilterRow);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    streamsContainer = new QWidget();
    streamsLayout = new QVBoxLayout(streamsContainer);
    streamsLayout->setContentsMargins(0, 0, 0, 0);
    streamsLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    scrollArea->setWidget(streamsContainer);
    leftPanel->addWidget(scrollArea, 2);
    mainSplitter->addWidget(leftWidget);

    // ── Panel derecho: configuración ──
    QWidget *configWidget = new QWidget();
    QVBoxLayout *configPanel = new QVBoxLayout(configWidget);
    configPanel->setContentsMargins(0, 0, 0, 0);
    configPanel->setSpacing(6);

    // ▶ FORMATO DE SALIDA (grilla 2x2)
    formatGroupBox = new QGroupBox(tr("FORMATO DE SALIDA"));
    QVBoxLayout *fmtMainLayout = new QVBoxLayout(formatGroupBox);

    auto makeFormatCombo = [&](const QStringList &items) -> QComboBox* {
        QComboBox *cb = new QComboBox();
        cb->addItems(items);
        return cb;
    };
    audioFormatCombo = makeFormatCombo({"mka", "m4a", "aac", "mp3", "flac", "opus", "wav"});
    videoFormatCombo = makeFormatCombo({"mkv", "mp4", "avi"});
    subsFormatCombo  = makeFormatCombo({"srt", "ass", "vtt"});
    videoResCombo    = makeFormatCombo({tr("Original"), "4K (2160p)", "2K (1440p)", "FHD (1080p)", "HD (720p)", "SD (480p)"});

    auto fmtLabel = [&](const QString &text) -> QLabel* {
        QLabel *lbl = new QLabel(text);
        lbl->setProperty("labelColor", "muted");
        return lbl;
    };

    QGridLayout *fmtGrid = new QGridLayout();
    fmtGrid->setHorizontalSpacing(10);
    fmtGrid->setVerticalSpacing(4);
    audioFmtLabel = fmtLabel(tr("Audio:"));
    videoFmtLabel = fmtLabel(tr("Video:"));
    resFmtLabel   = fmtLabel(tr("Res:"));
    subsFmtLabel  = fmtLabel(tr("Subs:"));
    fmtGrid->addWidget(audioFmtLabel, 0, 0); fmtGrid->addWidget(audioFormatCombo, 0, 1);
    fmtGrid->addWidget(videoFmtLabel, 0, 2); fmtGrid->addWidget(videoFormatCombo, 0, 3);
    fmtGrid->addWidget(resFmtLabel,   1, 0); fmtGrid->addWidget(videoResCombo,    1, 1);
    fmtGrid->addWidget(subsFmtLabel,  1, 2); fmtGrid->addWidget(subsFormatCombo,  1, 3);
    fmtGrid->setColumnStretch(1, 1);
    fmtGrid->setColumnStretch(3, 1);
    fmtMainLayout->addLayout(fmtGrid);

    copyModeCheckBox = new QCheckBox(tr("Copiar pista original sin re-codificar (Rápido y previene errores)"));
    copyModeCheckBox->setChecked(true);
    fmtMainLayout->addWidget(copyModeCheckBox);
    configPanel->addWidget(formatGroupBox);

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

    // ▶ TRADUCCIÓN DE SUBTÍTULOS
    translationGroupBox = new QGroupBox(tr("TRADUCCIÓN DE SUBTÍTULOS"));
    QVBoxLayout *transMainLayout = new QVBoxLayout(translationGroupBox);

    QHBoxLayout *transLangLayout = new QHBoxLayout();
    transLangLayout->setSpacing(8);
    srcPromptLabel = fmtLabel(tr("Origen:"));
    srcLangCombo = new QComboBox();
    srcLangCombo->addItem("English", "en");
    srcLangCombo->addItem("Español", "es");
    srcLangCombo->addItem("Français", "fr");
    srcLangCombo->addItem("Português", "pt");
    dstPromptLabel = fmtLabel(tr("Destino:"));
    dstLangCombo = new QComboBox();
    dstLangCombo->addItem("Español", "es");
    dstLangCombo->addItem("English", "en");
    dstLangCombo->addItem("Français", "fr");
    dstLangCombo->addItem("Português", "pt");
    transLangLayout->addWidget(srcPromptLabel);
    transLangLayout->addWidget(srcLangCombo, 1);
    transLangLayout->addWidget(dstPromptLabel);
    transLangLayout->addWidget(dstLangCombo, 1);
    transMainLayout->addLayout(transLangLayout);

    translateOnExtractCheck = new QCheckBox(tr("Traducir al extraer"));
    translateOnExtractCheck->setToolTip(
        tr("Si está marcado, al extraer subtítulos se traducirán automáticamente\n"
        "al idioma de destino seleccionado."));
    connect(translateOnExtractCheck, &QCheckBox::toggled, this, [this](bool checked) {
        QSettings s("LocalApp", "FFmpegExtractor");
        s.setValue("autoTranslate", checked);
    });
    transMainLayout->addWidget(translateOnExtractCheck);

    muxOnExtractCheck = new QCheckBox(tr("Incrustar la traducción (remux)"));
    muxOnExtractCheck->setToolTip(
        tr("Tras traducir, genera un archivo .mkv/.mp4 con el vídeo y el audio originales\n"
        "y la pista de subtítulos traducida incrustada, sin re-codificar (rápido y sin pérdida)."));
    transMainLayout->addWidget(muxOnExtractCheck);

    QHBoxLayout *transBtns = new QHBoxLayout();
    transBtns->setSpacing(6);
    manageModelsButton = new QPushButton(QIcon(":/icons/model.svg"), tr("Modelos"));
    manageModelsButton->setToolTip(tr("Gestionar modelos de traducción"));
    connect(manageModelsButton, &QPushButton::clicked, this, &MainWindow::showSettings);
    translateFilesButton = new QPushButton(QIcon(":/icons/translate.svg"), tr("Traducir subtítulos"));
    translateFilesButton->setToolTip(
        tr("Seleccionar subtítulos ya existentes en disco y traducirlos en lote"));
    connect(translateFilesButton, &QPushButton::clicked, this, &MainWindow::translateExistingFiles);
    transBtns->addWidget(manageModelsButton);
    transBtns->addWidget(translateFilesButton, 1);
    transMainLayout->addLayout(transBtns);

    QHBoxLayout *transProgressLayout = new QHBoxLayout();
    translationProgressBar = new QProgressBar();
    translationProgressBar->setRange(0, 100);
    translationProgressBar->setValue(0);
    translationProgressBar->setFixedHeight(14);
    translationProgressBar->setFormat("%p%");
    translationProgressBar->setVisible(false);
    translationStatusLabel = new QLabel(tr("Selecciona subtítulos y marca \"Traducir al extraer\""));
    translationStatusLabel->setProperty("labelColor", "muted");
    transProgressLayout->addWidget(translationProgressBar, 1);
    transProgressLayout->addWidget(translationStatusLabel);
    transMainLayout->addLayout(transProgressLayout);

    configPanel->addWidget(translationGroupBox);

    // ▶ DESTINO
    destGroupBox = new QGroupBox(tr("DESTINO"));
    QVBoxLayout *destLay = new QVBoxLayout(destGroupBox);
    outputLineEdit = new QLineEdit();
    outputLineEdit->setPlaceholderText(tr("Carpeta donde se guardarán los archivos extraídos…"));
    destLay->addWidget(outputLineEdit);
    QHBoxLayout *destBtns = new QHBoxLayout();
    destBtns->setSpacing(6);
    browseButton = new QPushButton(QIcon(":/icons/folder.svg"), tr("Examinar…"));
    browseButton->setToolTip(tr("Seleccionar carpeta de destino"));
    openFolderButton = new QPushButton(QIcon(":/icons/folder-open.svg"), tr("Abrir"));
    openFolderButton->setToolTip(tr("Abrir carpeta de destino en el explorador"));
    openFolderButton->setEnabled(false);
    connect(browseButton,        &QPushButton::clicked, this, &MainWindow::selectOutputDir);
    connect(openFolderButton, &QPushButton::clicked, this, &MainWindow::openOutputFolder);
    connect(outputLineEdit,   &QLineEdit::textChanged, this,
            [this](const QString &t){ openFolderButton->setEnabled(!t.isEmpty()); });
    destBtns->addWidget(browseButton);
    destBtns->addWidget(openFolderButton, 1);
    destLay->addLayout(destBtns);
    configPanel->addWidget(destGroupBox);

    configPanel->addStretch(1);

    // ── Acciones ──
    QHBoxLayout *actionBtns = new QHBoxLayout();
    actionBtns->setSpacing(6);
    cancelButton = new QPushButton(QIcon(":/icons/cancel.svg"), tr("Cancelar"));
    cancelButton->setObjectName("cancelButton");
    cancelButton->setEnabled(false);
    cancelButton->setToolTip(tr("Cancelar el proceso en curso (Escape)"));
    connect(cancelButton, &QPushButton::clicked, this, &MainWindow::cancelExtraction);

    extractButton = new QPushButton(QIcon(":/icons/extract.svg"), tr("INICIAR EXTRACCIÓN"));
    extractButton->setObjectName("extractButton");
    extractButton->setEnabled(false);
    extractButton->setShortcut(QKeySequence("Ctrl+Return"));
    extractButton->setToolTip(tr("Iniciar la extracción de todas las pistas seleccionadas (Ctrl+Enter)"));
    connect(extractButton, &QPushButton::clicked, this, &MainWindow::startExtraction);

    actionBtns->addWidget(cancelButton);
    actionBtns->addWidget(extractButton, 1);
    configPanel->addLayout(actionBtns);

    // Columna de configuración dentro de un QScrollArea: su contenido no debería
    // fijar el tamaño mínimo de la ventana (eso impedía maximizar en pantallas bajas).
    QScrollArea *configScroller = new QScrollArea();
    configScroller->setWidgetResizable(true);
    configScroller->setFrameShape(QFrame::NoFrame);
    configScroller->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    configScroller->setWidget(configWidget);
    configScroller->setMinimumWidth(340);
    configScroller->setMaximumWidth(460);

    mainSplitter->addWidget(configScroller);
    mainSplitter->setStretchFactor(0, 3);
    mainSplitter->setStretchFactor(1, 0);
    root->addWidget(mainSplitter, 3);

    // ── Barra de progreso fina + toggle del log ──
    QHBoxLayout *progressRow = new QHBoxLayout();
    progressRow->setSpacing(6);
    progressBar = new QProgressBar();
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setFixedHeight(14);
    progressBar->setFormat("%p%  —  %v / %m");
    progressRow->addWidget(progressBar, 1);
    QPushButton *logToggle = new QPushButton(tr("LOG ▸"));
    logToggle->setObjectName("quickSelButton");
    logToggle->setToolTip(tr("Mostrar / ocultar registro de actividad"));
    logToggleButton = logToggle;
    connect(logToggle, &QPushButton::clicked, this, [this, logToggle]() {
        const bool show = !logArea->isVisible();
        logArea->setVisible(show);
        logToggle->setText(show ? tr("LOG ▾") : tr("LOG ▸"));
    });
    progressRow->addWidget(logToggle);
    root->addLayout(progressRow);

    // ── Log (colapsable) ──
    logArea = new QTextEdit();
    logArea->setReadOnly(true);
    logArea->setMinimumHeight(50);
    logArea->setVisible(false);
    logArea->setPlaceholderText(tr("Sistema listo. Añade archivos y selecciona las pistas a extraer."));
    root->addWidget(logArea, 1);

    statusBar()->showMessage(tr("Listo"));

    setWindowTitle(tr("Elements Extract FFmpeg"));
    resize(1100, 700);
}

void MainWindow::showLogArea()
{
    if (!logArea) return;
    logArea->setVisible(true);
    if (logToggleButton) logToggleButton->setText(tr("LOG ▾"));
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
        this, tr("Añadir Videos"), "",
        tr("Videos (*.mkv *.mp4 *.avi *.mov *.ts *.m2ts *.webm);;Todos los archivos (*.*)"));
    loadFiles(files);
}


void MainWindow::removeFiles()
{
    for (QListWidgetItem *item : fileListWidget->selectedItems()) {
        fileSelections.remove(item->data(Qt::UserRole).toString());
        fileStreamTypes.remove(item->data(Qt::UserRole).toString());
        fileStreamCodecs.remove(item->data(Qt::UserRole).toString());
        fileStreamLanguages.remove(item->data(Qt::UserRole).toString());
        fileDurations.remove(item->data(Qt::UserRole).toString());
        delete item;
    }
    refreshSubLangFilter();
    if (fileListWidget->count() == 0) {
        clearStreamsList();
        selectedFileLabel->setText(tr("No hay archivos cargados"));
        extractButton->setEnabled(false);
        globalSelectionTypes.clear();
        statusBar()->showMessage(tr("Listo"));
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Selección de pistas
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::onFileSelectionChanged()
{
    saveCurrentSelections();
    if (fileListWidget->selectedItems().isEmpty()) {
        clearStreamsList(); lastSelectedFile = ""; m_probedFile = ""; return;
    }
    QString currentFile = fileListWidget->selectedItems().first()->data(Qt::UserRole).toString();
    lastSelectedFile = currentFile;
    selectedFileLabel->setText(QFileInfo(currentFile).fileName().toUpper());
    clearStreamsList();
    probeData.clear();

    // Cancela un análisis en curso: sus resultados se descartarían al llegar (C1)
    if (probeProcess->state() != QProcess::NotRunning) {
        probeProcess->kill();
        probeProcess->waitForFinished(1000);
    }
    m_probedFile = currentFile;

    statusBar()->showMessage(tr("Analizando: ") + QFileInfo(currentFile).fileName());

    metaInfoLabel->setText(tr("Analizando…"));
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
    bool translateEnabled = translateOnExtractCheck && translateOnExtractCheck->isChecked();

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

    QString mappedLang = normalizeLangCode(detectedLang);

    // Check if it's one of our supported source languages
    QStringList supportedSrc = SubtitleTranslator::supportedSourceLanguages();
    if (supportedSrc.contains(mappedLang)) {
        int idx = srcLangCombo->findData(mappedLang);
        if (idx >= 0 && srcLangCombo->currentIndex() != idx) {
            srcLangCombo->setCurrentIndex(idx);
            logArea->append(QString("<font color='#00BFFF'>" + tr("Idioma origen detectado: %1") + "</font>")
                .arg(SubtitleTranslator::languageName(mappedLang)));
        }

        // Check if translation model exists for this pair — only if translate-on-extract is enabled
        QString dstLang = dstLangCombo->currentData().toString();
        if (translateEnabled && !dstLang.isEmpty() && mappedLang != dstLang) {
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
                    msgBox.setWindowTitle(tr("Traducción triangular disponible"));
                    msgBox.setIcon(QMessageBox::Information);
                    msgBox.setText(
                        tr("No hay modelo directo de <b>%1 → %2</b>.<br><br>"
                           "Pero se usará <b>traducción triangular</b>:<br>%3<br><br>"
                           "Esto puede tardar un poco más, pero funcionará correctamente.")
                            .arg(SubtitleTranslator::languageName(mappedLang))
                            .arg(SubtitleTranslator::languageName(dstLang))
                            .arg(pathStr));
                    msgBox.setInformativeText(tr("¿Qué deseas hacer?"));
                    QPushButton *okBtn = msgBox.addButton(tr("Aceptar"), QMessageBox::AcceptRole);
                    QPushButton *selectBtn = msgBox.addButton(tr("Cambiar idioma destino"), QMessageBox::ActionRole);
                    msgBox.setDefaultButton(okBtn);
                    msgBox.exec();

                    if (msgBox.clickedButton() == selectBtn) {
                        dstLangCombo->setFocus();
                    }
                    // else: continue with triangular translation (okBtn or close)
                } else {
                    // Sin modelo instalado: ¿existe para descargar o no existe?
                    QString modelName = SubtitleTranslator::languageName(mappedLang) + " → " +
                                        SubtitleTranslator::languageName(dstLang);
                    const QStringList downloadable = modelDownloader
                        ? modelDownloader->registryPathForPair(mappedLang, dstLang)
                        : QStringList();
                    const bool checkable = !modelDownloader || modelDownloader->isRegistryLoaded();

                    QMessageBox msgBox(this);
                    msgBox.setIcon(QMessageBox::Information);
                    if (!downloadable.isEmpty()) {
                        msgBox.setWindowTitle(tr("Modelo de traducción no instalado"));
                        msgBox.setText(
                            tr("No hay modelo instalado para:<br><b>%1</b><br><br>"
                               "Pero existe para descargar: <b>%2</b><br><br>"
                               "Idioma detectado: %3 → %4")
                                .arg(modelName, downloadable.join(" → "))
                                .arg(SubtitleTranslator::languageName(mappedLang))
                                .arg(SubtitleTranslator::languageName(dstLang)));
                    } else if (!checkable) {
                        msgBox.setWindowTitle(tr("Modelo de traducción no encontrado"));
                        msgBox.setText(
                            tr("No hay modelo instalado para:<br><b>%1</b><br><br>"
                               "No se pudo comprobar el catálogo online (sin conexión o aún cargando).<br><br>"
                               "Idioma detectado: %2 → %3")
                                .arg(modelName)
                                .arg(SubtitleTranslator::languageName(mappedLang))
                                .arg(SubtitleTranslator::languageName(dstLang)));
                    } else {
                        msgBox.setWindowTitle(tr("Modelo de traducción no disponible"));
                        msgBox.setText(
                            tr("No existe modelo de traducción para:<br><b>%1</b><br><br>"
                               "Tampoco hay ruta triangular ni descarga disponible "
                               "(el ruso, por ejemplo, no tiene modelo publicado).<br><br>"
                               "Idioma detectado: %2 → %3")
                                .arg(modelName)
                                .arg(SubtitleTranslator::languageName(mappedLang))
                                .arg(SubtitleTranslator::languageName(dstLang)));
                    }
                    msgBox.setInformativeText(tr("¿Qué deseas hacer?"));
                    QPushButton *downloadBtn = nullptr;
                    if (!downloadable.isEmpty() || !checkable)
                        downloadBtn = msgBox.addButton(tr("Descargar modelo"), QMessageBox::AcceptRole);
                    QPushButton *selectBtn = msgBox.addButton(tr("Cambiar idioma destino"), QMessageBox::ActionRole);
                    QPushButton *skipBtn = msgBox.addButton(tr("Continuar sin traducir"), QMessageBox::RejectRole);
                    msgBox.setDefaultButton(downloadBtn ? downloadBtn : skipBtn);
                    msgBox.exec();

                    if (downloadBtn && msgBox.clickedButton() == downloadBtn) {
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

void MainWindow::onStreamClicked(bool checked)
{
    // Solo pistas de subtítulo cubiertas por el filtro activo: recordar la
    // decisión manual para que applySubtitleFilterToFile no la revierta.
    if (m_subLangFilter.isEmpty() || lastSelectedFile.isEmpty()) return;
    QCheckBox *cb = qobject_cast<QCheckBox *>(sender());
    if (!cb || cb->property("streamType").toString() != "subtitle") return;
    const int idx = cb->property("streamIndex").toInt();
    const QString lang = normalizeLangCode(
        fileStreamLanguages.value(lastSelectedFile).value(idx));
    if (lang != m_subLangFilter) return;
    QSet<int> &set = m_filterDeselected[lastSelectedFile];
    if (checked) set.remove(idx);
    else set.insert(idx);
    if (set.isEmpty()) m_filterDeselected.remove(lastSelectedFile);
}

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
    if (!m_subLangFilter.isEmpty()) {
        logArea->append(tr("<font color='#FFCC00'>El filtro \"Solo subs\" está activo. "
                           "Ponlo en \"Todos los idiomas\" para la selección global de subs.</font>"));
        return;
    }
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
    statusBar()->showMessage(tr("Listo"));
    // Resultado obsoleto: el usuario ya seleccionó otro archivo o vació la lista (C1/C2)
    if (lastSelectedFile.isEmpty() || m_probedFile != lastSelectedFile) {
        m_probedFile.clear();
        return;
    }
    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        metaInfoLabel->setText(tr("No se pudo analizar el archivo."));
        logArea->append(tr("<font color='#FF4444'>[ERROR] No se pudo analizar el archivo.</font>")); return;
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
    QMap<int, QString> typeMap;
    QMap<int, QString> codecMap;
    QMap<int, QString> titleMap;
    for (int i = 0; i < streams.size(); ++i) {
        QJsonObject s = streams.at(i).toObject();
        int     index = s.value("index").toInt();
        QString type  = s.value("codec_type").toString();
        QString codec = s.value("codec_name").toString();
        QString lang  = s.value("tags").toObject().value("language").toString();
        QString title = s.value("tags").toObject().value("title").toString();
        int     w     = s.value("width").toInt();
        int     h     = s.value("height").toInt();

        typeMap[index] = type;
        codecMap[index] = codec;
        if (!lang.isEmpty()) langMap[index] = lang;
        if (!title.isEmpty()) titleMap[index] = title;

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
        // clicked solo lo emite la interacción del usuario (setChecked no):
        // sirve para recordar desmarcados manuales frente al filtro.
        connect(cb, &QCheckBox::clicked, this, &MainWindow::onStreamClicked);
        streamsLayout->addWidget(cb);
        streamCheckBoxes.append(cb);
    }

    fileStreamLanguages[lastSelectedFile] = langMap;
    fileStreamTypes[lastSelectedFile] = typeMap;
    fileStreamTitles[lastSelectedFile] = titleMap;
    if (!fileStreamCodecs.contains(lastSelectedFile))
        fileStreamCodecs[lastSelectedFile] = codecMap;

    // Si hay un filtro por idioma activo, aplicarlo y reflejarlo en los checkboxes
    if (!m_subLangFilter.isEmpty()) {
        applySubtitleFilterToFile(lastSelectedFile);
        const QMap<int, QString> &sel = fileSelections.value(lastSelectedFile);
        for (QCheckBox *cb : streamCheckBoxes)
            cb->setChecked(sel.contains(cb->property("streamIndex").toInt()));
    }
    refreshSubLangFilter();

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
    int filterSkipped = 0;
    if (!m_subLangFilter.isEmpty()) {
        globalSelectionTypes.remove("subtitle");
        const int n = fileListWidget->count();
        for (int i = 0; i < n; ++i)
            filterSkipped += applySubtitleFilterToFile(fileListWidget->item(i)->data(Qt::UserRole).toString());
    }
    batchFiles.clear();
    m_extractedSubs.clear();
    m_muxQueue.clear();
    m_muxIndex = 0;
    m_muxActive = false;
    for (int i = 0; i < fileListWidget->count(); ++i)
        batchFiles << fileListWidget->item(i)->data(Qt::UserRole).toString();
    if (batchFiles.isEmpty()) return;

    currentBatchIndex = 0;
    showLogArea();
    logArea->clear();
    logArea->append(tr("<font color='#FF8800'><b>=== INICIANDO EXTRACCIÓN ===</b></font>"));
    if (filterSkipped > 0)
        logArea->append(QString("<font color='#888888'>  › " + tr("%1 pista(s) desmarcadas a mano, excluidas aunque coincidan con el filtro.") + "</font>")
            .arg(filterSkipped));
    extractButton->setEnabled(false);
    cancelButton->setEnabled(true);
    progressBar->setValue(0);
    processNextInBatch();
}

void MainWindow::cancelExtraction()
{
    if (subtitleTranslator && subtitleTranslator->isBatchActive()) {
        subtitleTranslator->cancel();
        logArea->append(tr("<br><font color='#FF4444'><b>[CANCELADO] La traducción fue interrumpida por el usuario.</b></font>"));
        statusBar()->showMessage(tr("Cancelado"));
        translationProgressBar->setVisible(false);
        extractButton->setEnabled(true);
        cancelButton->setEnabled(false);
        return;
    }
    if (m_muxActive) {
        if (process->state() != QProcess::NotRunning) {
            process->kill();
            logArea->append(tr("<br><font color='#FF4444'><b>[CANCELADO] El remux fue interrumpido por el usuario.</b></font>"));
        }
        m_muxActive = false;
        m_muxQueue.clear();
        extractButton->setEnabled(true);
        cancelButton->setEnabled(false);
        progressBar->setValue(0);
        return;
    }
    if (process->state() != QProcess::NotRunning) {
        process->kill();
        logArea->append(tr("<br><font color='#FF4444'><b>[CANCELADO] El proceso fue interrumpido por el usuario.</b></font>"));
        statusBar()->showMessage(tr("Cancelado"));
        // El ffmpeg abortado deja ficheros a medio escribir (a veces de 0
        // bytes): podarlos para no arrastrarlos a la siguiente ejecución.
        if (currentBatchIndex >= 0 && currentBatchIndex < batchFiles.size())
            pruneEmptyExtractions(batchFiles.at(currentBatchIndex));
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
        logArea->append(tr("<br><font color='#39FF14'><b>=== ✓ PROCESO FINALIZADO ===</b></font>"));
        statusBar()->showMessage(tr("✓ Finalizado — %1 archivo(s) procesado(s)")
                                     .arg(batchFiles.size()));
        extractButton->setEnabled(true);
        cancelButton->setEnabled(false);
        progressBar->setValue(100);

        // Auto-translate extracted subtitles if checkbox is checked
        if (translateOnExtractCheck->isChecked() && !m_extractedSubs.isEmpty()) {
            QString dstLang = dstLangCombo->currentData().toString();
            QString srcLang;
            if (!m_subLangFilter.isEmpty()) {
                srcLang = m_subLangFilter;
                logArea->append(QString("<font color='#888888'>  [DEBUG] idioma origen fijado por el filtro: %1</font>")
                    .arg(SubtitleTranslator::languageName(srcLang)));
            } else {
                srcLang = srcLangCombo->currentData().toString();
            }
            logArea->append(QString("<font color='#888888'>  [DEBUG] translateCheck=ON, srcLang='%1', dstLang='%2'</font>")
                .arg(srcLang, dstLang));
            if (srcLang != dstLang) {
                QStringList triPath = checkInstalledTranslationPath(srcLang, dstLang);
                QStringList subPaths;
                int skippedOtherLang = 0, unknownLang = 0;
                if (!triPath.isEmpty()) {
                    for (const SubExtractInfo &se : m_extractedSubs) {
                        if (!QFile::exists(se.subPath)) continue;
                        // No traducir como <srcLang> un contenido extraído en otro
                        // idioma: el motor lo traduciría como si fuera <srcLang>.
                        if (!se.subLang.isEmpty() && se.subLang != srcLang) {
                            ++skippedOtherLang;
                            continue;
                        }
                        if (se.subLang.isEmpty()) ++unknownLang;
                        subPaths.append(se.subPath);
                    }
                }
                if (skippedOtherLang > 0)
                    logArea->append(QString("<font color='#FFCC00'>  › " + tr("%1 archivo(s) omitidos: no están en %2.") + "</font>")
                        .arg(skippedOtherLang)
                        .arg(SubtitleTranslator::languageName(srcLang)));
                if (unknownLang > 0)
                    logArea->append(QString("<font color='#FFCC00'>  › " + tr("%1 archivo(s) sin idioma detectado: se traducen como %2; verifica el resultado.") + "</font>")
                        .arg(unknownLang)
                        .arg(SubtitleTranslator::languageName(srcLang)));
                logArea->append(QString("<font color='#888888'>  [DEBUG] files to translate: %1</font>")
                    .arg(subPaths.size()));

                if (!subPaths.isEmpty()) {
                    // Bloquear re-entrada mientras se traduce (A6)
                    extractButton->setEnabled(false);
                    cancelButton->setEnabled(true);
                    logArea->append(QString("<br><font color='#00BFFF'><b>" + tr("Traduciendo %1 archivo(s) de subtítulos...") + "</b></font>")
                        .arg(subPaths.size()));
                    translationProgressBar->setVisible(true);
                    translationProgressBar->setValue(0);

                    subtitleTranslator->translateBatch(subPaths, ensureOutputSubdir(tr("Traducidos")), srcLang, dstLang);
                    return;
                }
            } else {
                logArea->append("<font color='#FFCC00'>  [DEBUG] srcLang == dstLang, traducción omitida</font>");
            }
        }

        // Sin traducción (o ya hecha): pasar al remux si se pidió
        if (muxOnExtractCheck->isChecked() && !m_muxQueue.isEmpty()) {
            startMuxQueue();
            return;
        }
        if (muxOnExtractCheck->isChecked() && m_muxQueue.isEmpty())
            logArea->append(QString("<font color='#FFCC00'>  › " + tr("Remux omitido: no hay subtítulos traducidos para incrustar.") + "</font>"));

        if (trayIcon->isSystemTrayAvailable()) {
            trayIcon->showMessage("Elements Extract", tr("El proceso por lotes ha finalizado correctamente."), QSystemTrayIcon::Information, 4000);
        }
        return;
    }

    QString inputPath = batchFiles.at(currentBatchIndex);
    QFileInfo info(inputPath);
    currentFileDuration = fileDurations.value(inputPath, 0);
    progressBar->setValue(0);

    statusBar()->showMessage(tr("Procesando %1/%2: %3")
                                 .arg(currentBatchIndex + 1)
                                 .arg(batchFiles.size())
                                 .arg(info.fileName()));
    logArea->append(tr("<br><font color='#00BFFF'><b>[%1/%2] %3</b></font>")
                        .arg(currentBatchIndex + 1)
                        .arg(batchFiles.size())
                        .arg(info.fileName().toHtmlEscaped()));

    bool copyMode = copyModeCheckBox->isChecked();
    QString outDir = ensureOutputSubdir(copyMode ? tr("Extraídos") : tr("Convertidos"));
    if (outDir.isEmpty()) {
        logArea->append(tr("<font color='#FF4444'>  › No se ha especificado carpeta de destino, omitido.</font>"));
        currentBatchIndex++;
        processNextInBatch();
        return;
    }
    QString baseName = outDir + "/" + info.completeBaseName();

    // Determinista: evita colisiones tanto con archivos en disco como con rutas
    // ya reclamadas en esta misma invocación de ffmpeg.
    QSet<QString> usedPaths;
    auto claimUnique = [this, &usedPaths](const QString &name) -> QString {
        QString p = getUniqueFilePath(name);
        int c = 2;
        while (usedPaths.contains(p)) {
            QFileInfo fi(name);
            // completeBaseName (solo recorta la última extensión): conserva
            // el idioma ("X.eng_v2.srt", no el críptico "X_v2.srt").
            p = getUniqueFilePath(fi.absolutePath() + "/" + fi.completeBaseName()
                                   + "_v" + QString::number(c) + "." + fi.suffix());
            ++c;
        }
        usedPaths.insert(p);
        return p;
    };

    auto subtitleExtForCodec = [](const QString &codec) -> QString {
        if (codec == "subrip" || codec == "webvtt") return "srt";
        if (codec == "ass" || codec == "ssa" || codec == "mov_text") return "ass";
        return "mks";  // imagen u otros contenedores
    };
    auto subtitleCodecIsText = [](const QString &codec) -> bool {
        return codec == "subrip" || codec == "ass" || codec == "ssa"
            || codec == "webvtt" || codec == "mov_text";
    };

    QMap<int, QString> selectedTypes = fileSelections.value(inputPath);
    QMap<int, QString> selectedCodecs = fileStreamCodecs.value(inputPath);
    QMap<int, QString> strTypes = fileStreamTypes.value(inputPath);
    QMap<int, QString> strLangs = fileStreamLanguages.value(inputPath);
    QMap<int, QString> strTitles = fileStreamTitles.value(inputPath);
    QStringList finalArgs;
    finalArgs << "-i" << inputPath;
    bool anyAction = false;

    if (selectedTypes.isEmpty() && !globalSelectionTypes.isEmpty()) {
        // Selección global por tipo
        for (const QString &type : globalSelectionTypes) {
            QString mapCode = (type == "audio") ? "a" : (type == "video" ? "v" : "s");

            // Para subtítulos globales: si hay más de una pista, se necesita un
            // contenedor multi-stream (.mks); si solo hay una, usar su formato.
            QString ext;
            if (type == "subtitle") {
                QStringList subCodecs;
                for (auto it = strTypes.constBegin(); it != strTypes.constEnd(); ++it)
                    if (it.value() == "subtitle")
                        subCodecs << selectedCodecs.value(it.key());
                if (subCodecs.size() == 1 && copyMode)
                    ext = subtitleExtForCodec(subCodecs.first());
                else if (subCodecs.size() == 1)
                    ext = extensionForType("subtitle");
                else
                    ext = "mks";
            } else {
                ext = copyMode ? (type == "audio" ? "mka" : "mkv") : extensionForType(type);
            }

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

            QString outName;
            if (type == "subtitle") {
                QStringList subLangs;
                for (auto it = strTypes.constBegin(); it != strTypes.constEnd(); ++it) {
                    if (it.value() == "subtitle") {
                        QString l = strLangs.value(it.key());
                        if (!l.isEmpty() && !subLangs.contains(l)) subLangs << l;
                    }
                }
                if (subLangs.size() == 1)
                    outName = baseName + "." + subLangs.first() + "." + ext;
                else
                    outName = baseName + "_all_subtitle." + ext;
            } else {
                outName = baseName + "." + ext;
            }
            QString outPath = claimUnique(outName);
            finalArgs << "-y" << outPath;
            if (type == "subtitle") {
                // Solo se puede traducir/incrustar un sub de texto único
                int textSubCount = 0;
                QString recLang, recTitle;
                for (auto it = strTypes.constBegin(); it != strTypes.constEnd(); ++it) {
                    if (it.value() == "subtitle" && subtitleCodecIsText(selectedCodecs.value(it.key()))) {
                        ++textSubCount;
                        recLang = strLangs.value(it.key());
                        recTitle = strTitles.value(it.key());
                    }
                }
                if (textSubCount == 1 && !normalizeLangCode(recLang).isEmpty())
                    m_extractedSubs.append({inputPath, outPath, normalizeLangCode(recLang), recTitle});
                else
                    logArea->append(tr("<font color='#FFCC00'>  › Subtítulo omitido para traducción (varias pistas de texto o formato de imagen): %1</font>").arg(outPath));
            }
            anyAction = true;
        }
    } else {
        // Selección individual por índice
        for (auto it = selectedTypes.constBegin(); it != selectedTypes.constEnd(); ++it) {
            int index = it.key();
            QString type = it.value();
            QString codec = selectedCodecs.value(index);
            QString ext = copyMode ? extensionForCodec(codec, type) : extensionForType(type);
            bool translatableSub = (type == "subtitle") && subtitleCodecIsText(codec);

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
            QString outName;
            if (type == "subtitle") {
                QString lang = strLangs.value(index);
                outName = lang.isEmpty()
                          ? baseName + "_track" + QString::number(index) + "." + ext
                          : baseName + "." + lang + "." + ext;
            } else {
                outName = baseName + "." + ext;
            }
            QString outPath = claimUnique(outName);
            finalArgs << "-y" << outPath;
            if (translatableSub) {
                QString normLang = normalizeLangCode(strLangs.value(index));
                m_extractedSubs.append({inputPath, outPath,
                                       (normLang.isEmpty() || normLang == "und") ? QString() : normLang,
                                       strTitles.value(index)});
            }
            anyAction = true;
        }
    }

    if (!anyAction) {
        logArea->append(tr("<font color='#FFCC00'>  › Sin pistas seleccionadas para este archivo, omitido.</font>"));
        currentBatchIndex++;
        processNextInBatch();
        return;
    }

    if (process->state() != QProcess::NotRunning) return;  // ffmpeg aún en marcha: no relanzar
    logArea->append("<font color='#888888'>  $ " + QString("ffmpeg %1").arg(finalArgs.join(" ")).toHtmlEscaped() + "</font>");
    process->start(ffmpegExecutable(), finalArgs);
}

void MainWindow::readProcessOutput()
{
    QString out = QString::fromLocal8Bit(process->readAllStandardError());
    if (out.isEmpty()) out = QString::fromLocal8Bit(process->readAllStandardOutput());
    updateProgressFromLine(out);
    if (m_muxActive && !out.trimmed().isEmpty()) {
        m_muxTail.append(out.trimmed());
        while (m_muxTail.size() > 30) m_muxTail.removeFirst();
    }
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
    if (m_muxActive) {
        if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            logArea->append(tr("<font color='#39FF14'>  › OK</font>"));
            m_muxOk++;
            cleanupMuxIntermediates(m_muxQueue[m_muxIndex]);
        } else {
            const MuxJobInfo &job = m_muxQueue[m_muxIndex];
            QStringList hits;
            for (int i = m_muxTail.size() - 1; i >= 0 && hits.size() < 3; --i) {
                const QString l = m_muxTail[i].trimmed();
                if (l.isEmpty()) continue;
                if (l.contains("Error", Qt::CaseInsensitive) || l.contains("muxer", Qt::CaseInsensitive)
                    || l.contains("stream", Qt::CaseInsensitive) || l.contains("does not", Qt::CaseInsensitive)
                    || l.contains("not found", Qt::CaseInsensitive))
                    hits.prepend(l);
            }
            if (hits.isEmpty() && !m_muxTail.isEmpty())
                hits << m_muxTail.last().trimmed();
            const QString reason = hits.isEmpty() ? tr("error de ffmpeg") : hits.join(" ");
            m_muxFailed++;
            m_muxReasons.append(QFileInfo(job.videoInput).fileName() + " → " + reason);
            logArea->append(tr("<font color='#FF4444'>  › Error al incrustar, omitido.</font>"));
            for (const QString &h : hits)
                logArea->append("<font color='#FF8844'>     " + h.toHtmlEscaped() + "</font>");
        }
        progressBar->setValue(100);
        m_muxIndex++;
        processNextMux();
        return;
    }
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        logArea->append(tr("<font color='#39FF14'>  › OK</font>"));
        progressBar->setValue(100);
    } else if (currentBatchIndex >= 0) {
        logArea->append(tr("<font color='#FF4444'>  › Error en este archivo.</font>"));
    }
    if (currentBatchIndex >= 0 && currentBatchIndex < batchFiles.size()) {
        // Podar salidas vacías o inexistentes de este archivo: ffmpeg crea el
        // fichero al arrancar aunque luego falle (o se cancele), y un .srt de
        // 0 bytes solo produciría un confuso "No se pudieron leer..." al traducir.
        pruneEmptyExtractions(batchFiles.at(currentBatchIndex));
        currentBatchIndex++;
        processNextInBatch();
    }
}

// Tras un remux correcto, borra los .srt intermedios (extraídos y
// traducidos) si el usuario lo pidió en Ajustes. Solo en éxito: si el remux
// falla se conservan para diagnosticar.
void MainWindow::cleanupMuxIntermediates(const MuxJobInfo &job)
{
    QSettings s("LocalApp", "FFmpegExtractor");
    if (!s.value("cleanupAfterMux", false).toBool()) return;
    QStringList removed;
    for (const MuxSubItem &ms : job.subs) {
        for (const QString &path : {ms.translatedSub, ms.sourceSub}) {
            if (!path.isEmpty() && QFile::exists(path) && QFile::remove(path))
                removed << QFileInfo(path).fileName();
        }
    }
    // Las entradas de m_extractedSubs de este vídeo ya no apuntan a nada
    for (int i = m_extractedSubs.size() - 1; i >= 0; --i) {
        if (m_extractedSubs.at(i).videoInput != job.videoInput) continue;
        if (!QFile::exists(m_extractedSubs.at(i).subPath))
            m_extractedSubs.removeAt(i);
    }
    if (!removed.isEmpty())
        logArea->append(QString("<font color='#888888'>  › " + tr("Intermedios eliminados: %1") + "</font>")
                            .arg(removed.join(", ")));
}

// Elimina de m_extractedSubs (y del disco) las salidas vacías/inexistentes
// del vídeo indicado. Devuelve cuántas podó.
int MainWindow::pruneEmptyExtractions(const QString &videoInput)
{
    int pruned = 0;
    for (int i = m_extractedSubs.size() - 1; i >= 0; --i) {
        if (m_extractedSubs.at(i).videoInput != videoInput) continue;
        QFileInfo fi(m_extractedSubs.at(i).subPath);
        if (!fi.exists() || fi.size() == 0) {
            logArea->append(QString("<font color='#FFCC00'>  › " + tr("Extracción vacía o fallida, excluida de la traducción: %1") + "</font>")
                .arg(fi.fileName()));
            if (fi.exists()) QFile::remove(m_extractedSubs.at(i).subPath);
            m_extractedSubs.removeAt(i);
            ++pruned;
        }
    }
    return pruned;
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
    QString dir = QFileDialog::getExistingDirectory(this, tr("Destino"), outputLineEdit->text());
    if (!dir.isEmpty()) outputLineEdit->setText(dir);
}

void MainWindow::openOutputFolder()
{
    QString dir = outputLineEdit->text();
    if (dir.isEmpty()) return;
    QString appDir = dir + "/Elements Extract";
    if (QDir(appDir).exists()) dir = appDir;
    QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
}

void MainWindow::clearStreamsList()
{
    qDeleteAll(streamCheckBoxes);
    streamCheckBoxes.clear();
}

QString MainWindow::ffmpegExecutable() const
{
    QString bin = QCoreApplication::applicationDirPath()
                  + QDir::separator() + QStringLiteral("ffmpeg");
#ifdef Q_OS_WIN
    bin += QStringLiteral(".exe");
#endif
    return QFileInfo::exists(bin) ? bin : QStringLiteral("ffmpeg");
}

// ─────────────────────────────────────────────────────────────────────────────
// Filtro de subtítulos por idioma
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::refreshSubLangFilter()
{
    if (!subFilterCombo) return;
    m_detectedSubLangs.clear();
    QString prev = subFilterCombo->currentData().toString();
    const int n = fileListWidget ? fileListWidget->count() : 0;
    for (int i = 0; i < n; ++i) {
        const QString path = fileListWidget->item(i)->data(Qt::UserRole).toString();
        const QMap<int, QString> &types = fileStreamTypes.value(path);
        const QMap<int, QString> &langs = fileStreamLanguages.value(path);
        for (auto it = types.constBegin(); it != types.constEnd(); ++it) {
            if (it.value() == "subtitle") {
                const QString norm = normalizeLangCode(langs.value(it.key()));
                if (!norm.isEmpty() && norm != "und" && !m_detectedSubLangs.contains(norm))
                    m_detectedSubLangs.append(norm);
            }
        }
    }
    std::sort(m_detectedSubLangs.begin(), m_detectedSubLangs.end());
    subFilterCombo->blockSignals(true);
    subFilterCombo->clear();
    subFilterCombo->addItem(tr("Todos los idiomas"), "");
    for (const QString &lang : m_detectedSubLangs)
        subFilterCombo->addItem(SubtitleTranslator::languageName(lang), lang);
    int idx = subFilterCombo->findData(prev);
    subFilterCombo->setCurrentIndex(idx >= 0 ? idx : 0);
    subFilterCombo->blockSignals(false);
    if (idx < 0) m_subLangFilter.clear();
}

int MainWindow::applySubtitleFilterToFile(const QString &path)
{
    if (m_subLangFilter.isEmpty()) return 0;
    QMap<int, QString> sel = fileSelections.value(path);
    const QMap<int, QString> &types = fileStreamTypes.value(path);
    const QMap<int, QString> &langs = fileStreamLanguages.value(path);
    const QMap<int, QString> &codecs = fileStreamCodecs.value(path);
    const QSet<int> deselected = m_filterDeselected.value(path);
    int skipped = 0;
    for (auto it = types.constBegin(); it != types.constEnd(); ++it) {
        if (it.value() != "subtitle") continue;
        int idx = it.key();
        if (normalizeLangCode(langs.value(idx)) == m_subLangFilter) {
            if (deselected.contains(idx)) {
                // Desmarcado a mano: se respeta aunque coincida con el filtro
                sel.remove(idx);
                ++skipped;
                continue;
            }
            sel[idx] = "subtitle";
            fileStreamCodecs[path][idx] = codecs.value(idx);
        } else {
            sel.remove(idx);
        }
    }
    fileSelections[path] = sel;
    return skipped;
}

void MainWindow::onSubLangFilterChanged(int)
{
    m_subLangFilter = subFilterCombo->currentData().toString();
    // Cambiar de filtro reinicia las exclusiones manuales (nueva intención)
    m_filterDeselected.clear();
    if (m_subLangFilter.isEmpty()) return;  // "Todos": se mantiene la selección manual

    // Un filtro activo toma el control: desactiva la selección global de SUBS
    globalSelectionTypes.remove("subtitle");
    const int n = fileListWidget ? fileListWidget->count() : 0;
    for (int i = 0; i < n; ++i)
        applySubtitleFilterToFile(fileListWidget->item(i)->data(Qt::UserRole).toString());

    // Refrescar visualmente los checkboxes del archivo visible
    if (!lastSelectedFile.isEmpty()) {
        const QMap<int, QString> &sel = fileSelections.value(lastSelectedFile);
        int subCount = 0;
        for (QCheckBox *cb : streamCheckBoxes) {
            const bool on = sel.contains(cb->property("streamIndex").toInt());
            cb->setChecked(on);
            if (on && cb->property("streamType").toString() == "subtitle")
                ++subCount;
        }
        for (int i = 0; i < fileListWidget->count(); ++i) {
            QListWidgetItem *item = fileListWidget->item(i);
            if (item->data(Qt::UserRole).toString() == lastSelectedFile) {
                item->setForeground(QBrush(sel.isEmpty() ? QColor("#D0D0D0") : QColor("#FF8800")));
                break;
            }
        }
        if (subCount > 1)
            logArea->append(QString("<font color='#FFCC00'>  › " + tr("El filtro coincide con %1 pistas en %2: desmarca las que no necesites.") + "</font>")
                .arg(subCount)
                .arg(QFileInfo(lastSelectedFile).fileName().toHtmlEscaped()));
    }
    statusBar()->showMessage(tr("Filtro: solo subtítulos en ") + SubtitleTranslator::languageName(m_subLangFilter));
}

// ─────────────────────────────────────────────────────────────────────────────
// Remux: incrustar la traducción en el vídeo
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::startMuxQueue()
{
    if (m_muxQueue.isEmpty()) return;
    m_muxActive  = true;
    m_muxIndex   = 0;
    m_muxOk      = 0;
    m_muxFailed  = 0;
    m_muxReasons.clear();
    m_muxTail.clear();
    extractButton->setEnabled(false);
    cancelButton->setEnabled(true);
    showLogArea();
    logArea->append(tr("<br><font color='#FF8800'><b>=== INCRUSTANDO SUBTÍTULOS (REMUX) ===</b></font>"));
    processNextMux();
}

// Códecs que pueden copiarse (stream copy) a contenedor MP4
static bool isMp4SafeAudio(const QString &c)
{
    if (c.startsWith("pcm_")) return false;
    static const QSet<QString> bad = {"vorbis", "flac", "dts", "dtshd", "truehd",
                                      "tta", "wmav1", "wmav2", "wmalossless"};
    return !bad.contains(c);
}

static bool isMp4SafeVideo(const QString &c)
{
    static const QSet<QString> good = {"h264", "hevc", "mpeg4", "mpeg2video",
                                       "mpeg1video", "vp8", "vp9", "av1", "mjpeg", "prores"};
    return good.contains(c);
}

void MainWindow::processNextMux()
{
    if (m_muxIndex >= m_muxQueue.size()) {
        m_muxActive = false;
        logArea->append(tr("<br><font color='#39FF14'><b>=== ✓ PROCESO FINALIZADO ===</b></font>"));
        if (m_muxFailed > 0) {
            logArea->append(tr("<font color='#FFCC00'><b>Incrustados: %1 · Omitidos/error: %2</b></font>")
                                .arg(m_muxOk).arg(m_muxFailed));
            for (const QString &r : m_muxReasons)
                logArea->append(tr("<font color='#FF8844'>  › %1</font>").arg(r.toHtmlEscaped()));
        }
        statusBar()->showMessage(tr("✓ Finalizado — %1 incrustados, %2 omitidos")
                                     .arg(m_muxOk).arg(m_muxFailed));
        extractButton->setEnabled(true);
        cancelButton->setEnabled(false);
        progressBar->setValue(100);
        if (trayIcon->isSystemTrayAvailable()) {
            QString msg = m_muxFailed > 0
                ? tr("Subtítulos incrustados: %1 correctos, %2 con problemas.").arg(m_muxOk).arg(m_muxFailed)
                : tr("Subtítulos incrustados correctamente.");
            trayIcon->showMessage("Elements Extract", msg, QSystemTrayIcon::Information, 4000);
        }
        return;
    }

    const MuxJobInfo &job = m_muxQueue[m_muxIndex];
    QFileInfo vi(job.videoInput);
    // Si alguna traducción falta, se incrusta el resto avisando (no se pierde
    // todo el vídeo por un sub).
    QList<MuxSubItem> validSubs;
    for (const MuxSubItem &ms : job.subs) {
        if (QFile::exists(ms.translatedSub)) {
            validSubs.append(ms);
        } else {
            logArea->append(tr("<font color='#FFCC00'>  › No se encontró la traducción, se omite esa pista: %1</font>")
                                .arg(QFileInfo(ms.translatedSub).fileName().toHtmlEscaped()));
        }
    }
    QString reason;
    if (validSubs.isEmpty()) {
        reason = tr("no se encontró la traducción");
    } else if (outputLineEdit->text().trimmed().isEmpty()) {
        reason = tr("no hay carpeta de destino configurada");
    } else {
        const QMap<int, QString> &types = fileStreamTypes.value(job.videoInput);
        bool hasVideo = types.isEmpty();
        for (auto it = types.constBegin(); it != types.constEnd(); ++it)
            if (it.value() == "video") { hasVideo = true; break; }
        if (!hasVideo)
            reason = tr("sin pista de vídeo: no se puede incrustar");
    }

    if (!reason.isEmpty()) {
        m_muxFailed++;
        m_muxReasons.append(vi.fileName() + " → " + reason);
        logArea->append(tr("<font color='#FF8844'>  › Omitido: %1 (%2)</font>")
                            .arg(vi.fileName().toHtmlEscaped(), reason.toHtmlEscaped()));
        m_muxIndex++;
        processNextMux();
        return;
    }

    QString outDir = ensureOutputSubdir(tr("Remuxados"));
    m_muxTail.clear();  // buffer de líneas para diagnosticar fallos

    bool toMp4 = vi.suffix().toLower() == "mp4";
    if (toMp4 && !fileStreamTypes.value(job.videoInput).isEmpty()) {
        // Compatibilidad stream-copy a MP4: si algún códec no es copiable, bajar a MKV
        const QMap<int, QString> &typeMap = fileStreamTypes.value(job.videoInput);
        const QMap<int, QString> &codecs  = fileStreamCodecs.value(job.videoInput);
        QString switched;
        for (auto it = typeMap.constBegin(); it != typeMap.constEnd(); ++it) {
            const QString codec = codecs.value(it.key());
            if (it.value() == "audio" && !isMp4SafeAudio(codec))
                switched = tr("audio %1").arg(codec);
            else if (it.value() == "video" && !isMp4SafeVideo(codec))
                switched = tr("vídeo %1").arg(codec);
            if (!switched.isEmpty()) break;
        }
        if (!switched.isEmpty()) {
            toMp4 = false;
            logArea->append(tr("<font color='#FFCC00'>  › %1 no es copiable a MP4 (%2) → se usará MKV</font>")
                                .arg(switched, vi.fileName().toHtmlEscaped()));
        }
    }

    // El contenedor MP4 necesita el código ISO-639-2 de 3 letras; Matroska admite el de 2
    static const QMap<QString, QString> three = {
        {"en", "eng"}, {"es", "spa"}, {"fr", "fre"}, {"pt", "por"},
        {"de", "deu"}, {"it", "ita"}, {"ja", "jpn"}, {"zh", "zho"},
        {"ko", "kor"}, {"ru", "rus"}, {"ar", "ara"}, {"hi", "hin"},
        {"nl", "nld"}, {"sv", "swe"}, {"pl", "pol"}, {"tr", "tur"}
    };

    QString container = toMp4 ? "mp4" : "mkv";
    QString subCodec  = toMp4 ? "mov_text" : "srt";
    QStringList muxLangs;
    for (const MuxSubItem &ms : validSubs)
        if (!ms.langCode.isEmpty() && !muxLangs.contains(ms.langCode))
            muxLangs << ms.langCode;
    if (muxLangs.isEmpty()) muxLangs << "und";
    QString outPath = getUniqueFilePath(outDir + "/" + vi.completeBaseName()
                                        + "." + muxLangs.join("+") + "." + container);

    currentFileDuration = fileDurations.value(job.videoInput, 0);
    progressBar->setValue(0);
    statusBar()->showMessage(tr("Incrustando %1/%2: %3")
                                 .arg(m_muxIndex + 1)
                                 .arg(m_muxQueue.size())
                                 .arg(vi.fileName()));
    logArea->append(tr("<br><font color='#00BFFF'><b>[%1/%2] %3</b></font>")
                        .arg(m_muxIndex + 1)
                        .arg(m_muxQueue.size())
                        .arg(vi.fileName().toHtmlEscaped()));
    logArea->append(tr("  › Incrustando %1 pista(s) → %2")
                        .arg(validSubs.size())
                        .arg(outPath));
    for (const MuxSubItem &ms : validSubs)
        logArea->append(QString("<font color='#888888'>  › · %1%2</font>")
                            .arg(QFileInfo(ms.translatedSub).fileName().toHtmlEscaped(),
                                 ms.title.trimmed().isEmpty() ? QString()
                                 : QString(" (%1)").arg(ms.title.trimmed().toHtmlEscaped())));

    QStringList args;
    args << "-i" << vi.absoluteFilePath();
    for (const MuxSubItem &ms : validSubs)
        args << "-i" << ms.translatedSub;
    args << "-map" << "0:v"
         << "-map" << "0:a?";
    for (int k = 0; k < validSubs.size(); ++k)
        args << "-map" << QString("%1:0").arg(k + 1);
    args << "-c" << "copy"
         << "-c:s" << subCodec;
    for (int k = 0; k < validSubs.size(); ++k) {
        QString metadataLang = validSubs[k].langCode;
        if (toMp4) metadataLang = three.value(metadataLang, metadataLang);
        args << QString("-metadata:s:s:%1").arg(k) << ("language=" + metadataLang);
        if (!validSubs[k].title.trimmed().isEmpty())
            args << QString("-metadata:s:s:%1").arg(k) << ("title=" + validSubs[k].title.trimmed());
    }
    args << "-y" << outPath;
    logArea->append("<font color='#888888'>  $ " + QString("ffmpeg %1").arg(args.join(" ")).toHtmlEscaped() + "</font>");
    process->start(ffmpegExecutable(), args);
}

QString MainWindow::getUniqueFilePath(const QString &fullPath)
{
    if (!QFile::exists(fullPath)) return fullPath;
    QFileInfo info(fullPath);
    int c = 2;
    // completeBaseName (solo recorta la última extensión): conserva el idioma
    // ("X.eng_v2.srt", no el críptico "X_v2.srt" que producía baseName).
    while (QFile::exists(info.absolutePath() + "/" + info.completeBaseName()
                         + "_v" + QString::number(c) + "." + info.suffix())) c++;
    return info.absolutePath() + "/" + info.completeBaseName()
            + "_v" + QString::number(c) + "." + info.suffix();
}

QString MainWindow::ensureOutputSubdir(const QString &subName)
{
    QString dest = outputLineEdit->text().trimmed();
    if (dest.isEmpty()) return QString();
    QString path = dest + "/Elements Extract/" + subName;
    QDir().mkpath(path);
    return path;
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
    if (nAudio > 0)          parts << tr("🎵 %1 audio").arg(nAudio);
    if (nSubs  > 0)          parts << tr("CC %1 subs").arg(nSubs);

    metaInfoLabel->setText(parts.join("   "));
}

// ─────────────────────────────────────────────────────────────────────────────
// Atajos de teclado y persistencia
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete && fileListWidget->hasFocus())
        removeFiles();
    else if (event->key() == Qt::Key_Escape && cancelButton->isEnabled())
        cancelExtraction();
    else
        QMainWindow::keyPressEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Detener procesos en curso para un cierre limpio (A8)
    if (process && process->state() != QProcess::NotRunning) process->kill();
    if (probeProcess && probeProcess->state() != QProcess::NotRunning) probeProcess->kill();
    if (subtitleTranslator && subtitleTranslator->isBatchActive()) subtitleTranslator->cancel();
    saveSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::retranslateUi()
{
    setWindowTitle(tr("Elements Extract FFmpeg"));
    statusBar()->showMessage(tr("Listo"));

    // ── Menú ──
    if (menuArchivo) menuArchivo->setTitle(tr("Archivo"));
    if (menuAjustes) menuAjustes->setTitle(tr("Ajustes"));
    if (menuAyuda)   menuAyuda->setTitle(tr("Ayuda"));
    if (actAnadir)   actAnadir->setText(tr("Añadir archivos…"));
    if (actSalir)    actSalir->setText(tr("Salir"));
    if (actSettings) actSettings->setText(tr("Configuración y Modelos…"));
    if (actAyuda)    actAyuda->setText(tr("Manual de uso"));
    if (actAcerca)   actAcerca->setText(tr("Acerca de…"));

    // ── Panel izquierdo ──
    if (loteLabel)   loteLabel->setText(tr("LOTE DE ARCHIVOS"));
    if (pistasLabel) pistasLabel->setText(tr("PISTAS"));
    if (addButton) {
        addButton->setText(tr("Añadir"));
        addButton->setToolTip(tr("Añadir archivos de video (Ctrl+O)"));
    }
    if (removeButton) {
        removeButton->setText(tr("Quitar"));
        removeButton->setToolTip(tr("Quitar archivos seleccionados (Del)"));
    }
    if (selectedFileLabel && fileListWidget && fileListWidget->count() == 0)
        selectedFileLabel->setText(tr("Selecciona un archivo para ver sus pistas"));
    if (videoAllButton) videoAllButton->setText(tr("VIDEO"));
    if (audioAllButton) audioAllButton->setText(tr("AUDIO"));
    if (subsAllButton)  subsAllButton->setText(tr("SUBS"));
    if (clearAllButton) clearAllButton->setText(tr("Limpiar"));
    if (subFilterLabel) subFilterLabel->setText(tr("Solo subs:"));
    if (subFilterCombo) {
        subFilterCombo->setToolTip(
            tr("Selecciona los subtítulos de un idioma concreto en TODOS los archivos del lote.\n"
               "Mientras esté activo, la extracción y la traducción trabajarán solo con ese idioma."));
        refreshSubLangFilter();
    }

    // ── Formato de salida ──
    if (formatGroupBox) formatGroupBox->setTitle(tr("FORMATO DE SALIDA"));
    if (audioFmtLabel)  audioFmtLabel->setText(tr("Audio:"));
    if (videoFmtLabel)  videoFmtLabel->setText(tr("Video:"));
    if (resFmtLabel)    resFmtLabel->setText(tr("Res:"));
    if (subsFmtLabel)   subsFmtLabel->setText(tr("Subs:"));
    if (copyModeCheckBox)
        copyModeCheckBox->setText(tr("Copiar pista original sin re-codificar (Rápido y previene errores)"));
    if (videoResCombo) {
        int cur = videoResCombo->currentIndex();
        videoResCombo->clear();
        videoResCombo->addItems({tr("Original"), "4K (2160p)", "2K (1440p)",
                                 "FHD (1080p)", "HD (720p)", "SD (480p)"});
        if (cur >= 0) videoResCombo->setCurrentIndex(qBound(0, cur, videoResCombo->count() - 1));
    }

    // ── Traducción ──
    if (translationGroupBox) translationGroupBox->setTitle(tr("TRADUCCIÓN DE SUBTÍTULOS"));
    if (srcPromptLabel) srcPromptLabel->setText(tr("Origen:"));
    if (dstPromptLabel) dstPromptLabel->setText(tr("Destino:"));
    if (translateOnExtractCheck) {
        translateOnExtractCheck->setText(tr("Traducir al extraer"));
        translateOnExtractCheck->setToolTip(
            tr("Si está marcado, al extraer subtítulos se traducirán automáticamente\n"
            "al idioma de destino seleccionado."));
    }
    if (muxOnExtractCheck) {
        muxOnExtractCheck->setText(tr("Incrustar la traducción (remux)"));
        muxOnExtractCheck->setToolTip(
            tr("Tras traducir, genera un archivo .mkv/.mp4 con el vídeo y el audio originales\n"
            "y la pista de subtítulos traducida incrustada, sin re-codificar (rápido y sin pérdida)."));
    }
    if (manageModelsButton) {
        manageModelsButton->setText(tr("Modelos"));
        manageModelsButton->setToolTip(tr("Gestionar modelos de traducción"));
    }
    if (translateFilesButton) {
        translateFilesButton->setText(tr("Traducir subtítulos"));
        translateFilesButton->setToolTip(
            tr("Seleccionar subtítulos ya existentes en disco y traducirlos en lote"));
    }
    if (translationProgressBar && !translationProgressBar->isVisible() && translationStatusLabel)
        translationStatusLabel->setText(tr("Selecciona subtítulos y marca \"Traducir al extraer\""));

    // ── Destino ──
    if (destGroupBox) destGroupBox->setTitle(tr("DESTINO"));
    if (outputLineEdit)
        outputLineEdit->setPlaceholderText(tr("Carpeta donde se guardarán los archivos extraídos…"));
    if (browseButton) {
        browseButton->setText(tr("Examinar…"));
        browseButton->setToolTip(tr("Seleccionar carpeta de destino"));
    }
    if (openFolderButton) {
        openFolderButton->setText(tr("Abrir"));
        openFolderButton->setToolTip(tr("Abrir carpeta de destino en el explorador"));
    }

    // ── Acciones ──
    if (cancelButton) {
        cancelButton->setText(tr("Cancelar"));
        cancelButton->setToolTip(tr("Cancelar el proceso en curso (Escape)"));
    }
    if (extractButton) {
        extractButton->setText(tr("INICIAR EXTRACCIÓN"));
        extractButton->setToolTip(tr("Iniciar la extracción de todas las pistas seleccionadas (Ctrl+Enter)"));
    }
    if (logToggleButton) {
        logToggleButton->setText(logArea && logArea->isVisible() ? tr("LOG ▾") : tr("LOG ▸"));
        logToggleButton->setToolTip(tr("Mostrar / ocultar registro de actividad"));
    }
    if (logArea)
        logArea->setPlaceholderText(tr("Sistema listo. Añade archivos y selecciona las pistas a extraer."));
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

    // Load auto-translate setting
    translateOnExtractCheck->setChecked(s.value("autoTranslate", false).toBool());

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

        // Acotar a la pantalla: si la geometría "normal" es casi de pantalla completa
        // (p. ej. guardada estando maximizada), al restaurarla no se distingue del
        // estado maximizado y el botón de maximizar queda inoperante.
        const QRect wa = QGuiApplication::primaryScreen()->availableGeometry();
        if (w > wa.width() - 60) w = wa.width() - 60;
        if (h > wa.height() - 60) h = wa.height() - 60;
        if (w < 640) w = 640;
        if (h < 480) h = 480;

        resize(w, h);
        if (x >= 0 && y >= 0) move(x, y);
        if (s.value("windowMaximized", false).toBool()) {
            setWindowState(windowState() | Qt::WindowMaximized);
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
    s.setValue("autoTranslate", translateOnExtractCheck->isChecked());

    // Save window geometry (use normalGeometry when maximized)
    s.setValue("windowMaximized", isMaximized());
    QRect geo = isMaximized() ? normalGeometry() : geometry();
    // Evitar persistir un "normal" a pantalla completa (rompe el botón de maximizar)
    const QRect wa = QGuiApplication::primaryScreen()->availableGeometry();
    if (geo.width() > wa.width() - 60) geo.setWidth(wa.width() - 60);
    if (geo.height() > wa.height() - 60) geo.setHeight(wa.height() - 60);
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
        logArea->append(QString("<font color='#FF4444'>" + tr("No se encontró el archivo: %1") + "</font>")
            .arg(fi.fileName()));
        return;
    }

    QString srcLang = srcLangCombo->currentData().toString();
    QString dstLang = dstLangCombo->currentData().toString();

    if (srcLang == dstLang) {
        logArea->append(tr("<font color='#FFCC00'>Idioma origen y destino son iguales.</font>"));
        return;
    }

    QString outputPath = fi.absolutePath() + "/" + fi.completeBaseName() + "." + dstLang + "." + fi.suffix();

    translationProgressBar->setVisible(true);
    translationProgressBar->setValue(0);
    translationStatusLabel->setText(tr("Traduciendo..."));

    logArea->append(QString("<br><font color='#00BFFF'><b>" + tr("Traduciendo: %1 → %2") + "</b></font>")
        .arg(SubtitleTranslator::languageName(srcLang))
        .arg(SubtitleTranslator::languageName(dstLang)));

    subtitleTranslator->translateFile(subtitlePath, outputPath, srcLang, dstLang);
}

QStringList MainWindow::checkInstalledTranslationPath(const QString &srcLang, const QString &dstLang)
{
    // Sin ruta instalada (ni directa ni triangular) cada archivo fallaría:
    // avisarlo una sola vez con un mensaje accionable en lugar de un error
    // por archivo, cotejando con el catálogo online.
    QStringList triPath = subtitleTranslator->findTranslationPath(srcLang, dstLang);
    if (!triPath.isEmpty()) return triPath;

    logArea->append(QString("<font color='#FFCC00'><b>  › " + tr("Sin traducción %1 → %2: no hay modelo instalado (ni directo ni triangular).") + "</b></font>")
        .arg(SubtitleTranslator::languageName(srcLang))
        .arg(SubtitleTranslator::languageName(dstLang)));
    const QStringList downloadable = modelDownloader
        ? modelDownloader->registryPathForPair(srcLang, dstLang)
        : QStringList();
    if (!downloadable.isEmpty()) {
        logArea->append(QString("<font color='#FFCC00'>  › " + tr("Pero sí existe para descargar: %1. Ábrelo en Ajustes → Configuración y Modelos… → pestaña Modelos de Traducción → ONLINE (doble clic para descargar).") + "</font>")
            .arg(downloadable.join(" → ")));
    } else if (modelDownloader && !modelDownloader->isRegistryLoaded()) {
        logArea->append(tr("  › No se pudo comprobar el catálogo online (sin conexión o aún cargando). "
                          "Con internet, abre Ajustes → Configuración y Modelos… → ONLINE para ver si existe."));
    } else {
        logArea->append(tr("  › Tampoco existe en el catálogo online (el ruso, por ejemplo, no tiene modelo publicado). "
                          "Solo se pueden traducir subtítulos en English, Español, Français o Português: "
                          "cambia el filtro \"Solo subs\" o el idioma origen."));
    }
    return {};
}

void MainWindow::translateExistingFiles()
{
    if (!subtitleTranslator) return;

    QString srcLang = srcLangCombo->currentData().toString();
    QString dstLang = dstLangCombo->currentData().toString();
    if (srcLang.isEmpty() || dstLang.isEmpty() || srcLang == dstLang) {
        QMessageBox::warning(this, tr("Traducción"),
            tr("Selecciona idiomas de origen y destino distintos antes de traducir."));
        return;
    }

    QStringList files = QFileDialog::getOpenFileNames(
        this, tr("Seleccionar subtítulos para traducir"), "",
        tr("Subtítulos (*.srt *.ass *.ssa *.vtt);;Todos los archivos (*.*)"));
    if (files.isEmpty()) return;

    QString outDir = ensureOutputSubdir(tr("Traducidos"));
    if (outDir.isEmpty()) {
        QMessageBox::warning(this, tr("Traducción"),
            tr("Selecciona primero una carpeta de destino."));
        return;
    }

    showLogArea();
    logArea->append(tr("<br><font color='#FF8800'><b>=== TRADUCCIÓN POR LOTES ===</b></font>"));
    logArea->append(QString("<font color='#00BFFF'>" + tr("Traduciendo %1 archivo(s): %2 → %3") + "</font>")
        .arg(files.size())
        .arg(SubtitleTranslator::languageName(srcLang))
        .arg(SubtitleTranslator::languageName(dstLang)));

    // Mismo pre-chequeo que al extraer: sin modelo, guidance y salir limpio
    if (checkInstalledTranslationPath(srcLang, dstLang).isEmpty()) {
        translationProgressBar->setVisible(false);
        translationStatusLabel->setText(tr("Sin modelo de traducción"));
        return;
    }

    translationProgressBar->setVisible(true);
    translationProgressBar->setValue(0);
    translationStatusLabel->setText(tr("Traduciendo en lote..."));
    extractButton->setEnabled(false);
    cancelButton->setEnabled(true);

    subtitleTranslator->translateBatch(files, outDir, srcLang, dstLang);
}

void MainWindow::onTranslationProgress(int current, int total)
{
    if (total > 0) {
        int pct = (current * 100) / total;
        translationProgressBar->setValue(pct);
        translationStatusLabel->setText(
            tr("Traduciendo archivo %1 / %2…").arg(current).arg(total));
    }
}

void MainWindow::onFileTranslated(const QString &input, const QString &output)
{
    // Asociar la traducción con su vídeo de origen para el remux posterior.
    // Los subs del mismo vídeo se agrupan en UN solo trabajo: una única
    // salida con todas las pistas incrustadas (no un vídeo por sub).
    if (muxOnExtractCheck && muxOnExtractCheck->isChecked()) {
        for (const SubExtractInfo &se : m_extractedSubs) {
            if (se.subPath == input) {
                MuxSubItem item;
                item.translatedSub = output;
                item.sourceSub = se.subPath;
                item.langCode = dstLangCombo->currentData().toString();
                item.title = se.subTitle;
                int ji = -1;
                for (int i = 0; i < m_muxQueue.size(); ++i) {
                    if (m_muxQueue[i].videoInput == se.videoInput) { ji = i; break; }
                }
                if (ji < 0) {
                    MuxJobInfo job;
                    job.videoInput = se.videoInput;
                    job.subs.append(item);
                    m_muxQueue.append(job);
                } else {
                    m_muxQueue[ji].subs.append(item);
                }
                break;
            }
        }
    }

    if (!subtitleTranslator->isBatchActive())
        translationProgressBar->setValue(100);
    translationStatusLabel->setText(tr("Traducción completada: ") + QFileInfo(output).fileName());

    logArea->append(QString("<font color='#39FF14'>  ✓ " + tr("Traducción guardada: %1") + "</font>")
        .arg(QFileInfo(output).fileName()));
}

void MainWindow::onTranslationError(const QString &message)
{
    translationProgressBar->setVisible(false);
    translationStatusLabel->setText(tr("Error en la traducción"));

    logArea->append(QString("<font color='#FF4444'>  ✗ " + tr("Error de traducción: %1") + "</font>").arg(message));

    // During a batch a modal dialog would interrupt the whole run — just log and continue.
    if (!subtitleTranslator->isBatchActive())
        QMessageBox::critical(this, tr("Error de traducción"), message);
}

void MainWindow::onTranslationFinished()
{
    translationProgressBar->setValue(100);
    translationProgressBar->setVisible(false);
    translationStatusLabel->setText(tr("Traducción completada"));

    // Si el usuario pidió incrustar, pasar a la fase de remux
    if (muxOnExtractCheck && muxOnExtractCheck->isChecked() && !m_muxQueue.isEmpty()) {
        startMuxQueue();
        return;
    }
    if (muxOnExtractCheck && muxOnExtractCheck->isChecked() && m_muxQueue.isEmpty())
        logArea->append(QString("<font color='#FFCC00'>  › " + tr("Remux omitido: no hay subtítulos traducidos para incrustar.") + "</font>"));

    extractButton->setEnabled(true);
    cancelButton->setEnabled(false);

    if (trayIcon->isSystemTrayAvailable()) {
        trayIcon->showMessage("Elements Extract", tr("La traducción ha finalizado correctamente."),
                              QSystemTrayIcon::Information, 4000);
    }
}

void MainWindow::showModelManager()
{
    QMessageBox info(this);
    info.setWindowTitle(tr("Gestionar Modelos de Traducción"));
    info.setIcon(QMessageBox::Information);
    info.setText(
        tr("<b>Modelos de traducción</b><br><br>"
           "Si te falta un modelo para un par de idiomas, descárgalo desde Ajustes → Configuración y Modelos… → pestaña Modelos de Traducción → ONLINE (doble clic). "
           "La app te avisará en el registro si el modelo que necesitas existe para descargar.<br><br>"
           "<b>Modelos incluidos:</b><br>"
           "• English → Español (en→es)<br>"
           "• Español → English (es→en)<br>"
           "• English → Français (en→fr)<br>"
           "• Français → English (fr→en)<br>"
           "• English → Português (en→pt)<br>"
           "• Português → English (pt→en)<br><br>"
           "<b>Directorio de modelos:</b><br>") +
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
        translationStatusLabel->setText(tr("Descargando modelo %1: %2%")
            .arg(modelId).arg(pct));
    }
}

void MainWindow::onModelDownloaded(const QString &modelId)
{
    translationStatusLabel->setText(tr("Modelo descargado: ") + modelId);
    logArea->append(QString("<font color='#39FF14'>  ✓ " + tr("Modelo descargado: %1") + "</font>").arg(modelId));
}

void MainWindow::onModelDownloadError(const QString &modelId, const QString &message)
{
    translationStatusLabel->setText(tr("Error descargando modelo: ") + modelId);
    logArea->append(QString("<font color='#FF4444'>  ✗ " + tr("Error descargando %1: %2") + "</font>")
        .arg(modelId, message));
}

void MainWindow::changeUILanguage(const QString &langCode)
{
    // Save preference
    QSettings s("LocalApp", "FFmpegExtractor");
    s.setValue("language", langCode);

    // Reload translations
    static QTranslator *translator = nullptr;
    if (translator) {
        qApp->removeTranslator(translator);
        delete translator;
        translator = nullptr;
    }

    translator = new QTranslator(this);
    QString actualLang = langCode;
    if (actualLang == "system") {
        actualLang = QLocale::system().name();
    }

    bool loaded = translator->load("elements_extract_" + actualLang, ":/translations");
    if (!loaded) {
        loaded = translator->load("elements_extract_" + actualLang,
                                  QCoreApplication::applicationDirPath() + "/../share/elements-extract/translations");
    }
    if (!loaded) {
        QString shortLang = actualLang.left(2);
        loaded = translator->load("elements_extract_" + shortLang, ":/translations");
        if (!loaded) {
            loaded = translator->load("elements_extract_" + shortLang,
                                      QCoreApplication::applicationDirPath() + "/../share/elements-extract/translations");
        }
    }

    if (loaded) {
        qApp->installTranslator(translator);
        // Re-translate the UI by rebuilding it
        logArea->append(QString("<font color='#00BFFF'>%1</font>")
            .arg(tr("Idioma de interfaz cambiado a: %1").arg(langCode)));
    } else {
        logArea->append(QString("<font color='#FF8800'>%1</font>")
            .arg(tr("No se pudo cargar la traducción para: %1").arg(langCode)));
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Settings
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::showSettings()
{
    SettingsDialog dlg(subtitleTranslator, modelDownloader, this);
    connect(&dlg, &SettingsDialog::settingsChanged, this, &MainWindow::onSettingsChanged);
    connect(&dlg, &SettingsDialog::uiLanguageChanged, this, &MainWindow::changeUILanguage);
    dlg.exec();
}

void MainWindow::onSettingsChanged()
{
    // Reload settings after dialog closes
    loadSettings();
    logArea->append(tr("<font color='#888888'>Configuración actualizada</font>"));
}
