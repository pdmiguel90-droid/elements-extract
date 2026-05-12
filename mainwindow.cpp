#include "mainwindow.h"
#include "aboutdialog.h"

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
#include <QPixmap>
#include <QPainter>
#include <QStyle>

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

    // Icono de ventana generado programáticamente
    QPixmap ico(64, 64);
    ico.fill(Qt::transparent);
    QPainter p(&ico);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(QColor("#1A1A1A"));
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(2, 2, 60, 60, 10, 10);
    p.setPen(QColor("#FF8800"));
    p.setFont(QFont("Arial", 30, QFont::Bold));
    p.drawText(QRect(0, 0, 64, 64), Qt::AlignCenter, "F");
    p.end();
    setWindowIcon(QIcon(ico));

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
    root->setSpacing(8);
    root->setContentsMargins(12, 10, 12, 10);

    // Título
    QLabel *titleLabel = new QLabel("ELEMENTS EXTRACT FFMPEG");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 20px; letter-spacing: 3px; color: #FF8800; margin-bottom: 6px;");
    root->addWidget(titleLabel);

    // ── Panel central (izquierda + derecha) ──────────────────────────────────
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(10);

    // Panel izquierdo: lista de archivos
    QVBoxLayout *leftPanel = new QVBoxLayout();
    QLabel *lbLote = new QLabel("LOTE DE ARCHIVOS");
    lbLote->setStyleSheet("font-size: 11px; letter-spacing: 1px;");
    leftPanel->addWidget(lbLote);

    fileListWidget = new QListWidget();
    fileListWidget->setDragDropMode(QAbstractItemView::InternalMove);
    connect(fileListWidget, &QListWidget::itemSelectionChanged,
            this, &MainWindow::onFileSelectionChanged);
    leftPanel->addWidget(fileListWidget);

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
    contentLayout->addLayout(leftPanel, 1);

    // Panel derecho: pistas del archivo
    QVBoxLayout *rightPanel = new QVBoxLayout();
    selectedFileLabel = new QLabel("Selecciona un archivo para ver sus pistas");
    selectedFileLabel->setStyleSheet("color: #39FF14; font-weight: bold;"
                                     "border-bottom: 1px solid #2A2A2A; padding-bottom: 4px;");
    rightPanel->addWidget(selectedFileLabel);

    // Panel de metadatos del archivo seleccionado
    metaInfoLabel = new QLabel("");
    metaInfoLabel->setStyleSheet(
        "color: #888888; font-size: 11px; font-weight: normal; padding: 2px 0;");
    metaInfoLabel->setWordWrap(true);
    rightPanel->addWidget(metaInfoLabel);

    // Botones de selección rápida
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
    rightPanel->addWidget(scrollArea);
    contentLayout->addLayout(rightPanel, 2);
    root->addLayout(contentLayout);

    // ── Formato de salida ────────────────────────────────────────────────────
    QGroupBox *fmtBox = new QGroupBox("FORMATO DE SALIDA");
    QHBoxLayout *fmtLayout = new QHBoxLayout(fmtBox);
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
    fmtLayout->addSpacing(8);
    subsFormatCombo  = makeCombo("Subs:",  {"srt", "ass", "vtt"});
    fmtLayout->addStretch();
    root->addWidget(fmtBox);

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
    logArea->setFixedHeight(110);
    logArea->setPlaceholderText("Sistema listo. Añade archivos y selecciona las pistas a extraer.");
    root->addWidget(logArea);

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
void MainWindow::addFiles()
{
    QStringList files = QFileDialog::getOpenFileNames(
        this, "Añadir Videos", "",
        "Videos (*.mkv *.mp4 *.avi *.mov *.ts *.m2ts *.webm);;Todos los archivos (*.*)");
    for (const QString &f : files) {
        bool exists = false;
        for (int i = 0; i < fileListWidget->count(); ++i)
            if (fileListWidget->item(i)->data(Qt::UserRole).toString() == f) { exists = true; break; }
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
    QMap<int, QString> selected;
    for (QCheckBox *cb : streamCheckBoxes)
        if (cb->isChecked())
            selected[cb->property("streamIndex").toInt()] = cb->property("streamType").toString();
    fileSelections[lastSelectedFile] = selected;

    // Color visual en la lista
    for (int i = 0; i < fileListWidget->count(); ++i) {
        QListWidgetItem *item = fileListWidget->item(i);
        if (item->data(Qt::UserRole).toString() == lastSelectedFile) {
            item->setForeground(QBrush(selected.isEmpty() && globalSelectionTypes.isEmpty()
                                       ? QColor("#D0D0D0") : QColor("#FF8800")));
            break;
        }
    }
    extractButton->setEnabled(
        fileListWidget->count() > 0 &&
        (!globalSelectionTypes.isEmpty() || !fileSelections.isEmpty()));
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
        logArea->append("[ERROR] No se pudo analizar el archivo."); return;
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
    for (int i = 0; i < streams.size(); ++i) {
        QJsonObject s = streams.at(i).toObject();
        int     index = s.value("index").toInt();
        QString type  = s.value("codec_type").toString();
        QString codec = s.value("codec_name").toString();
        QString lang  = s.value("tags").toObject().value("language").toString();
        int     w     = s.value("width").toInt();
        int     h     = s.value("height").toInt();

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
        if (saved.contains(index) || globalSelectionTypes.contains(type))
            cb->setChecked(true);
        connect(cb, &QCheckBox::stateChanged, this, &MainWindow::onStreamToggled);
        streamsLayout->addWidget(cb);
        streamCheckBoxes.append(cb);
    }

    // Actualizar panel de metadatos
    updateMetadataPanel(dur, size, videoInfo, nAudio, nSubs);
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

void MainWindow::startExtraction()
{
    saveCurrentSelections();
    batchFiles.clear();
    for (int i = 0; i < fileListWidget->count(); ++i)
        batchFiles << fileListWidget->item(i)->data(Qt::UserRole).toString();
    if (batchFiles.isEmpty()) return;

    currentBatchIndex = 0;
    logArea->clear();
    logArea->append("=== INICIANDO EXTRACCIÓN ===");
    extractButton->setEnabled(false);
    cancelButton->setEnabled(true);
    progressBar->setValue(0);
    processNextInBatch();
}

void MainWindow::cancelExtraction()
{
    if (process->state() != QProcess::NotRunning) {
        process->kill();
        logArea->append("\n[CANCELADO] El proceso fue interrumpido por el usuario.");
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
        logArea->append("\n=== ✓ PROCESO FINALIZADO ===");
        statusBar()->showMessage(QString("✓ Finalizado — %1 archivo(s) procesado(s)")
                                     .arg(batchFiles.size()));
        extractButton->setEnabled(true);
        cancelButton->setEnabled(false);
        progressBar->setValue(100);
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
    logArea->append(QString("\n[%1/%2] %3")
                        .arg(currentBatchIndex + 1)
                        .arg(batchFiles.size())
                        .arg(info.fileName()));

    QString outDir  = outputLineEdit->text();
    QString baseName = outDir + "/" + info.baseName();

    QMap<int, QString> selectedMap = fileSelections.value(inputPath);
    QStringList finalArgs;
    finalArgs << "-i" << inputPath;
    bool anyAction = false;

    if (selectedMap.isEmpty() && !globalSelectionTypes.isEmpty()) {
        // Selección global por tipo
        for (const QString &type : globalSelectionTypes) {
            QString mapCode = (type == "audio") ? "a" : (type == "video" ? "v" : "s");
            QString ext     = extensionForType(type);
            finalArgs << "-map" << QString("0:%1").arg(mapCode) << "-c" << "copy";
            finalArgs << "-y"   << getUniqueFilePath(baseName + "_all_" + type + "." + ext);
            anyAction = true;
        }
    } else {
        // Selección individual por índice
        for (auto it = selectedMap.constBegin(); it != selectedMap.constEnd(); ++it) {
            int     index = it.key();
            QString type  = it.value();
            QString ext   = extensionForType(type);
            finalArgs << "-map" << QString("0:%1").arg(index) << "-c" << "copy";
            finalArgs << "-y"   << getUniqueFilePath(baseName + "_track" + QString::number(index) + "." + ext);
            anyAction = true;
        }
    }

    if (!anyAction) {
        logArea->append("  › Sin pistas seleccionadas para este archivo, omitido.");
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
    if (!out.contains("time=") && !out.trimmed().isEmpty())
        logArea->append(out.trimmed());
}

void MainWindow::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        logArea->append("  › OK");
        progressBar->setValue(100);
    } else if (currentBatchIndex >= 0) {
        logArea->append("  › Error en este archivo.");
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
    QMessageBox help(this);
    help.setWindowTitle("Manual de uso — Elements Extract FFmpeg");
    help.setIcon(QMessageBox::Information);
    help.setText(
        "<b>¿Cómo usar la aplicación?</b><br><br>"
        "<b>1. Añadir archivos</b><br>"
        "Haz clic en <i>+ Añadir</i> o arrastra archivos directamente a la lista.<br><br>"
        "<b>2. Seleccionar pistas</b><br>"
        "Haz clic en un archivo para ver sus pistas. Marca las que quieras extraer.<br>"
        "Usa los botones <i>VIDEO / AUDIO / SUBS</i> para selección global por tipo.<br><br>"
        "<b>3. Elegir formato de salida</b><br>"
        "Usa los combos de la sección <i>Formato de Salida</i> para elegir el contenedor.<br><br>"
        "<b>4. Destino</b><br>"
        "Elige la carpeta donde se guardarán los archivos extraídos.<br><br>"
        "<b>5. Extraer</b><br>"
        "Pulsa <i>⚙ INICIAR EXTRACCIÓN</i>. Puedes cancelar en cualquier momento.<br><br>"
        "<b>Atajos de teclado:</b><br>"
        "Ctrl+O → Añadir archivos<br>"
        "F1 → Este manual<br>"
        "Ctrl+Q → Salir"
    );
    help.exec();
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
    for (const QUrl &url : event->mimeData()->urls()) {
        QString f = url.toLocalFile();
        if (f.isEmpty()) continue;
        bool exists = false;
        for (int i = 0; i < fileListWidget->count(); ++i)
            if (fileListWidget->item(i)->data(Qt::UserRole).toString() == f) { exists = true; break; }
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

void MainWindow::loadSettings()
{
    QSettings s("LocalApp", "FFmpegExtractor");
    outputLineEdit->setText(s.value("outputDir", "").toString());
    audioFormatCombo->setCurrentText(s.value("audioFormat", "mka").toString());
    videoFormatCombo->setCurrentText(s.value("videoFormat", "mkv").toString());
    subsFormatCombo ->setCurrentText(s.value("subsFormat",  "srt").toString());
    if (s.contains("geometry")) restoreGeometry(s.value("geometry").toByteArray());
}

void MainWindow::saveSettings()
{
    QSettings s("LocalApp", "FFmpegExtractor");
    s.setValue("outputDir",   outputLineEdit->text());
    s.setValue("audioFormat", audioFormatCombo->currentText());
    s.setValue("videoFormat", videoFormatCombo->currentText());
    s.setValue("subsFormat",  subsFormatCombo->currentText());
    s.setValue("geometry",    saveGeometry());
}
