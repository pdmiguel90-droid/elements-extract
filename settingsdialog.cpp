#include "settingsdialog.h"
#include "subtitletranslator.h"
#include "modeldownloader.h"
#include "theme.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QListWidget>
#include <QProgressBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>

SettingsDialog::SettingsDialog(SubtitleTranslator *translator,
                               ModelDownloader *downloader,
                               QWidget *parent)
    : QDialog(parent)
    , m_translator(translator)
    , m_downloader(downloader)
{
    setWindowTitle(tr("Ajustes — Elements Extract"));
    setMinimumSize(750, 550);
    resize(800, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(12, 12, 12, 12);

    QTabWidget *tabs = new QTabWidget();
    m_tabs = tabs;

    // ════════════════════════════════════════════════════════════════════════
    // TAB 1: GENERAL
    // ════════════════════════════════════════════════════════════════════════
    QWidget *tabGeneral = new QWidget();
    QVBoxLayout *genLay = new QVBoxLayout(tabGeneral);

    // Model directory
    m_dirBox = new QGroupBox(tr("Directorio de Modelos"));
    QVBoxLayout *dirLay = new QVBoxLayout(m_dirBox);
    QHBoxLayout *dirRow = new QHBoxLayout();
    m_modelsDirEdit = new QLineEdit(m_translator->modelDirectory());
    m_modelsDirEdit->setReadOnly(true);
    m_browseBtn = new QPushButton(QIcon(":/icons/folder.svg"), tr("Examinar..."));
    connect(m_browseBtn, &QPushButton::clicked, this, &SettingsDialog::onBrowseModelsDir);
    dirRow->addWidget(m_modelsDirEdit, 1);
    dirRow->addWidget(m_browseBtn);
    dirLay->addLayout(dirRow);
    genLay->addWidget(m_dirBox);

    // Languages
    m_langBox = new QGroupBox(tr("Idiomas por Defecto"));
    QGridLayout *langGrid = new QGridLayout(m_langBox);
    m_srcLbl = new QLabel(tr("Origen:"));
    m_srcLbl->setProperty("labelColor", "muted");
    m_defaultSrcLangCombo = new QComboBox();
    m_defaultSrcLangCombo->addItem("English", "en");
    m_defaultSrcLangCombo->addItem("Español", "es");
    m_defaultSrcLangCombo->addItem("Français", "fr");
    m_defaultSrcLangCombo->addItem("Português", "pt");
    m_dstLbl = new QLabel(tr("Destino:"));
    m_dstLbl->setProperty("labelColor", "muted");
    m_defaultDstLangCombo = new QComboBox();
    m_defaultDstLangCombo->addItem("Español", "es");
    m_defaultDstLangCombo->addItem("English", "en");
    m_defaultDstLangCombo->addItem("Français", "fr");
    m_defaultDstLangCombo->addItem("Português", "pt");
    langGrid->addWidget(m_srcLbl, 0, 0); langGrid->addWidget(m_defaultSrcLangCombo, 0, 1);
    langGrid->addWidget(m_dstLbl, 0, 2); langGrid->addWidget(m_defaultDstLangCombo, 0, 3);
    genLay->addWidget(m_langBox);

    // Options
    m_optBox = new QGroupBox(tr("Opciones"));
    QVBoxLayout *optLay = new QVBoxLayout(m_optBox);
    m_autoTranslateCheck = new QCheckBox(tr("Traducir subtítulos automáticamente al extraer"));
    connect(m_autoTranslateCheck, &QCheckBox::toggled, this, [this](bool checked) {
        QSettings s("LocalApp", "FFmpegExtractor");
        s.setValue("autoTranslate", checked);
        emit settingsChanged();
    });
    optLay->addWidget(m_autoTranslateCheck);
    m_cleanupAfterMuxCheck = new QCheckBox(tr("Eliminar los .srt intermedios tras incrustarlos (remux)"));
    m_cleanupAfterMuxCheck->setToolTip(tr("Borra los subtítulos extraídos y traducidos de cada vídeo cuando su remux termina correctamente. Sin remux no se borra nada."));
    connect(m_cleanupAfterMuxCheck, &QCheckBox::toggled, this, [this](bool checked) {
        QSettings s("LocalApp", "FFmpegExtractor");
        s.setValue("cleanupAfterMux", checked);
        emit settingsChanged();
    });
    optLay->addWidget(m_cleanupAfterMuxCheck);
    genLay->addWidget(m_optBox);

    // Appearance (Theme + Language)
    m_appearanceBox = new QGroupBox(tr("Apariencia"));
    QGridLayout *appGrid = new QGridLayout(m_appearanceBox);

    m_themeLbl = new QLabel(tr("Tema:"));
    m_themeLbl->setProperty("labelColor", "muted");
    m_themeCombo = new QComboBox();
    m_themeCombo->addItem(tr("Automático (detectar del sistema)"), "system");
    m_themeCombo->addItem(tr("Oscuro"), "dark");
    m_themeCombo->addItem(tr("Claro"), "light");
    connect(m_themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        QString theme = m_themeCombo->currentData().toString();
        ThemeManager::Theme t = ThemeManager::themeFromName(theme);
        ThemeManager::instance()->setTheme(t);
    });

    m_langLbl = new QLabel(tr("Idioma de interfaz:"));
    m_langLbl->setProperty("labelColor", "muted");
    m_uiLangCombo = new QComboBox();
    m_uiLangCombo->addItem(tr("Automático (detectar del sistema)"), "system");
    m_uiLangCombo->addItem("Español", "es");
    m_uiLangCombo->addItem("English", "en");
    m_uiLangCombo->addItem("Português", "pt");
    connect(m_uiLangCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        QString lang = m_uiLangCombo->currentData().toString();
        emit uiLanguageChanged(lang);
    });

    appGrid->addWidget(m_themeLbl, 0, 0); appGrid->addWidget(m_themeCombo, 0, 1);
    appGrid->addWidget(m_langLbl, 0, 2); appGrid->addWidget(m_uiLangCombo, 0, 3);
    genLay->addWidget(m_appearanceBox);

    genLay->addStretch(1);
    tabs->addTab(tabGeneral, tr("General"));

    // ════════════════════════════════════════════════════════════════════════
    // TAB 2: MODELOS
    // ════════════════════════════════════════════════════════════════════════
    QWidget *tabModelos = new QWidget();
    QVBoxLayout *modLay = new QVBoxLayout(tabModelos);

    // Status row
    QHBoxLayout *statusRow = new QHBoxLayout();
    m_statusLabel = new QLabel(tr("Modelos instalados"));
    m_statusLabel->setProperty("labelColor", "normal");
    m_fetchBtn = new QPushButton(QIcon(":/icons/refresh.svg"), tr("Buscar online"));
    m_fetchBtn->setToolTip(tr("Buscar modelos disponibles en translatelocally.com"));
    connect(m_fetchBtn, &QPushButton::clicked, this, &SettingsDialog::onFetchOnlineModels);
    statusRow->addWidget(m_statusLabel, 1);
    statusRow->addWidget(m_fetchBtn);
    modLay->addLayout(statusRow);

    // Progress
    m_downloadProgress = new QProgressBar();
    m_downloadProgress->setRange(0, 100);
    m_downloadProgress->setFixedHeight(16);
    m_downloadProgress->setVisible(false);
    modLay->addWidget(m_downloadProgress);

    // Splitter
    QSplitter *splitter = new QSplitter(Qt::Horizontal);

    // Left: Local models
    QWidget *leftW = new QWidget();
    QVBoxLayout *leftLay = new QVBoxLayout(leftW);
    leftLay->setContentsMargins(0,0,0,0);
    m_leftTitle = new QLabel(tr("INSTALADOS — doble clic para activar/desactivar"));
    m_leftTitle->setProperty("labelColor", "green");
    leftLay->addWidget(m_leftTitle);

    m_localListWidget = new QListWidget();
    m_localListWidget->setObjectName("localModelList");
    connect(m_localListWidget, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        onActivateModel(item->data(Qt::UserRole).toString());
    });
    leftLay->addWidget(m_localListWidget, 1);

    QHBoxLayout *delRow = new QHBoxLayout();
    m_deleteBtn = new QPushButton(QIcon(":/icons/delete.svg"), tr("Eliminar"));
    connect(m_deleteBtn, &QPushButton::clicked, this, [this]() {
        QListWidgetItem *item = m_localListWidget->currentItem();
        if (!item) return;
        if (QMessageBox::question(this, tr("Eliminar"), tr("¿Eliminar ") + item->text().split('\n').first() + "?")
            == QMessageBox::Yes)
            onDeleteModel(item->data(Qt::UserRole).toString());
    });
    delRow->addStretch(); delRow->addWidget(m_deleteBtn);
    leftLay->addLayout(delRow);
    splitter->addWidget(leftW);

    // Right: Online models
    QWidget *rightW = new QWidget();
    QVBoxLayout *rightLay = new QVBoxLayout(rightW);
    rightLay->setContentsMargins(0,0,0,0);
    m_rightTitle = new QLabel(tr("ONLINE — doble clic para descargar"));
    m_rightTitle->setProperty("labelColor", "normal");
    rightLay->addWidget(m_rightTitle);

    m_onlineListWidget = new QListWidget();
    m_onlineListWidget->setObjectName("onlineModelList");
    connect(m_onlineListWidget, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        onDownloadModel(item->data(Qt::UserRole).toString());
    });
    rightLay->addWidget(m_onlineListWidget, 1);
    splitter->addWidget(rightW);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    modLay->addWidget(splitter, 1);

    // Info
    m_infoLbl = new QLabel(
        tr("<b>tiny</b> ~15-25MB rápido · <b>base</b> ~50MB mejor calidad<br>"
        "Doble clic = activar/desactivar (solo activos se usan para traducir)"));
    m_infoLbl->setProperty("labelColor", "muted");
    m_infoLbl->setWordWrap(true);
    modLay->addWidget(m_infoLbl);

    tabs->addTab(tabModelos, tr("Modelos de Traducción"));

    mainLayout->addWidget(tabs, 1);

    // Close button
    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->addStretch(1);
    m_closeBtn = new QPushButton(QIcon(":/icons/check.svg"), tr("Cerrar"));
    connect(m_closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    btnRow->addWidget(m_closeBtn);
    mainLayout->addLayout(btnRow);


    // Load settings
    QSettings s("LocalApp", "FFmpegExtractor");

    // F2/B3: directorio de modelos persistido
    QString modelsDir = s.value("modelsDir").toString();
    if (!modelsDir.isEmpty()) {
        m_modelsDirEdit->setText(modelsDir);
        m_translator->setModelDirectory(modelsDir);
        if (m_downloader) m_downloader->setModelsDirectory(modelsDir);
    }

    m_defaultSrcLangCombo->blockSignals(true);
    m_defaultDstLangCombo->blockSignals(true);
    int si = m_defaultSrcLangCombo->findData(s.value("srcLang","en").toString());
    int di = m_defaultDstLangCombo->findData(s.value("dstLang","es").toString());
    if (si >= 0) m_defaultSrcLangCombo->setCurrentIndex(si);
    if (di >= 0) m_defaultDstLangCombo->setCurrentIndex(di);
    m_defaultSrcLangCombo->blockSignals(false);
    m_defaultDstLangCombo->blockSignals(false);
    m_autoTranslateCheck->setChecked(s.value("autoTranslate",false).toBool());
    m_cleanupAfterMuxCheck->setChecked(s.value("cleanupAfterMux",false).toBool());

    // F1: persistir idiomas por defecto al cambiarlos
    connect(m_defaultSrcLangCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
        QSettings qs("LocalApp", "FFmpegExtractor");
        qs.setValue("srcLang", m_defaultSrcLangCombo->currentData().toString());
        emit settingsChanged();
    });
    connect(m_defaultDstLangCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
        QSettings qs("LocalApp", "FFmpegExtractor");
        qs.setValue("dstLang", m_defaultDstLangCombo->currentData().toString());
        emit settingsChanged();
    });

    // Load theme setting
    QString themeName = s.value("theme", "system").toString();
    int ti = m_themeCombo->findData(themeName);
    if (ti >= 0) m_themeCombo->setCurrentIndex(ti);

    // Load language setting
    QString langName = s.value("language", "system").toString();
    int li = m_uiLangCombo->findData(langName);
    if (li >= 0) m_uiLangCombo->setCurrentIndex(li);

    if (m_downloader) {
        connect(m_downloader, &ModelDownloader::downloadProgress, this, &SettingsDialog::onModelDownloadProgress);
        connect(m_downloader, &ModelDownloader::downloadFinished, this, &SettingsDialog::onModelDownloaded);
        connect(m_downloader, &ModelDownloader::downloadError, this, &SettingsDialog::onModelDownloadError);
        // B5/F5: una sola fuente de verdades para la lista online (el downloader)
        connect(m_downloader, &ModelDownloader::modelsFetched, this, [this](const QList<RemoteModel> &models) {
            m_onlineModels = models;
            refreshOnlineModels();
            m_statusLabel->setText(tr("%1 modelos online").arg(m_onlineModels.size()));
        });
    }

    refreshLocalModels();

    // Auto-fetch online models on dialog open
    QTimer::singleShot(100, this, &SettingsDialog::onFetchOnlineModels);
}

// ── Actions ──────────────────────────────────────────────────────────────
void SettingsDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QDialog::changeEvent(event);
}

void SettingsDialog::retranslateUi()
{
    setWindowTitle(tr("Ajustes — Elements Extract"));
    if (m_tabs) {
        m_tabs->setTabText(0, tr("General"));
        m_tabs->setTabText(1, tr("Modelos de Traducción"));
    }
    if (m_dirBox)       m_dirBox->setTitle(tr("Directorio de Modelos"));
    if (m_browseBtn)    m_browseBtn->setText(tr("Examinar..."));
    if (m_langBox)      m_langBox->setTitle(tr("Idiomas por Defecto"));
    if (m_srcLbl)       m_srcLbl->setText(tr("Origen:"));
    if (m_dstLbl)       m_dstLbl->setText(tr("Destino:"));
    if (m_optBox)       m_optBox->setTitle(tr("Opciones"));
    if (m_autoTranslateCheck)
        m_autoTranslateCheck->setText(tr("Traducir subtítulos automáticamente al extraer"));
    if (m_cleanupAfterMuxCheck) {
        m_cleanupAfterMuxCheck->setText(tr("Eliminar los .srt intermedios tras incrustarlos (remux)"));
        m_cleanupAfterMuxCheck->setToolTip(tr("Borra los subtítulos extraídos y traducidos de cada vídeo cuando su remux termina correctamente. Sin remux no se borra nada."));
    }
    if (m_appearanceBox) m_appearanceBox->setTitle(tr("Apariencia"));
    if (m_themeLbl)     m_themeLbl->setText(tr("Tema:"));
    if (m_langLbl)      m_langLbl->setText(tr("Idioma de interfaz:"));
    if (m_fetchBtn) {
        m_fetchBtn->setText(tr("Buscar online"));
        m_fetchBtn->setToolTip(tr("Buscar modelos disponibles en translatelocally.com"));
    }
    if (m_leftTitle)    m_leftTitle->setText(tr("INSTALADOS — doble clic para activar/desactivar"));
    if (m_rightTitle)   m_rightTitle->setText(tr("ONLINE — doble clic para descargar"));
    if (m_deleteBtn)    m_deleteBtn->setText(tr("Eliminar"));
    if (m_infoLbl)
        m_infoLbl->setText(
            tr("<b>tiny</b> ~15-25MB rápido · <b>base</b> ~50MB mejor calidad<br>"
            "Doble clic = activar/desactivar (solo activos se usan para traducir)"));
    if (m_closeBtn)     m_closeBtn->setText(tr("Cerrar"));

    refreshLocalModels();
    refreshOnlineModels();
}

void SettingsDialog::onBrowseModelsDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Directorio de modelos"), m_modelsDirEdit->text());
    if (!dir.isEmpty()) {
        m_modelsDirEdit->setText(dir);
        QSettings s("LocalApp", "FFmpegExtractor");
        s.setValue("modelsDir", dir);                          // F2: persistir
        m_translator->setModelDirectory(dir);
        if (m_downloader) m_downloader->setModelsDirectory(dir); // F3: sincronizar downloader
        refreshLocalModels();
        emit settingsChanged();
    }
}

void SettingsDialog::onFetchOnlineModels()
{
    m_statusLabel->setText(tr("Buscando..."));
    m_onlineListWidget->clear();
    if (m_downloader) m_downloader->fetchAvailableModels();
}

void SettingsDialog::onDownloadModel(const QString &modelId)
{
    if (!m_downloader) {
        QMessageBox::information(this, tr("No disponible"), tr("Gestor de modelos no disponible."));
        return;
    }

    // Find model info in our online list
    RemoteModel modelInfo;
    bool found = false;
    for (const RemoteModel &m : m_onlineModels) {
        if (m.id == modelId) { modelInfo = m; found = true; break; }
    }

    if (!found) {
        QMessageBox::warning(this, tr("Error"), tr("Modelo no encontrado en la lista online."));
        return;
    }

    // B12: evitar re-descargar modelos ya instalados sin confirmación
    if (modelInfo.isInstalled) {
        QMessageBox::StandardButton ret = QMessageBox::question(
            this, tr("Volver a descargar"),
            tr("El modelo '%1' ya está instalado.\n¿Descargarlo de nuevo? (reemplaza la copia local)").arg(modelId));
        if (ret != QMessageBox::Yes) return;
    }

    m_downloadProgress->setRange(0, 100);
    m_downloadProgress->setVisible(true);
    m_downloadProgress->setValue(0);
    m_statusLabel->setText(tr("Descargando: ") + modelId);
    m_downloader->downloadModelWithInfo(modelInfo);
}

void SettingsDialog::onDeleteModel(const QString &modelId)
{
    // Search in multiple directories
    QStringList searchDirs;
    searchDirs << m_translator->modelDirectory();
    searchDirs << QCoreApplication::applicationDirPath() + "/../models";
    searchDirs << QCoreApplication::applicationDirPath() + "/models";
    searchDirs << QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/models";

    for (const QString &baseDir : searchDirs) {
        QString path = baseDir + "/" + modelId;
        if (QDir(path).exists()) {
            QDir(path).removeRecursively();
            break;
        }
    }

    // Remove from active list
    QSettings s("LocalApp", "FFmpegExtractor");
    QStringList activeList = s.value("activeModels").toStringList();
    activeList.removeAll(modelId);
    s.setValue("activeModels", activeList);

    refreshLocalModels();
    // Update installed status in online models
    for (RemoteModel &m : m_onlineModels) {
        m.isInstalled = m_installedIds.contains(m.id);
    }
    refreshOnlineModels();
    emit settingsChanged();
}

void SettingsDialog::onActivateModel(const QString &modelId)
{
    QSettings s("LocalApp", "FFmpegExtractor");
    QStringList list = s.value("activeModels").toStringList();
    if (list.contains(modelId))
        list.removeAll(modelId);
    else
        list.append(modelId);
    s.setValue("activeModels", list);
    refreshLocalModels();
    emit settingsChanged();
}

void SettingsDialog::onModelDownloadProgress(const QString &modelId, qint64 received, qint64 total)
{
    if (total > 0) {
        m_downloadProgress->setRange(0, 100);
        m_downloadProgress->setValue(static_cast<int>((received * 100) / total));
        m_statusLabel->setText(tr("Descargando %1: %2%").arg(modelId).arg((received*100)/total));
    } else {
        // El registro no publica el tamaño: barra indeterminada (B1)
        m_downloadProgress->setRange(0, 0);
        m_statusLabel->setText(tr("Descargando %1...").arg(modelId));
    }
}

void SettingsDialog::onModelDownloaded(const QString &modelId)
{
    m_downloadProgress->setVisible(false);
    // Auto-activate
    QSettings s("LocalApp", "FFmpegExtractor");
    QStringList list = s.value("activeModels").toStringList();
    if (!list.contains(modelId)) list.append(modelId);
    s.setValue("activeModels", list);

    // Refresh local list first (updates m_installedIds)
    refreshLocalModels();

    // Update installed status in online models list
    for (RemoteModel &m : m_onlineModels) {
        m.isInstalled = m_installedIds.contains(m.id);
    }
    refreshOnlineModels();
    m_statusLabel->setText(tr("Descargado y activado: ") + modelId);
    emit settingsChanged();
}

void SettingsDialog::onModelDownloadError(const QString &modelId, const QString &message)
{
    m_downloadProgress->setVisible(false);
    m_statusLabel->setText(tr("Error: ") + modelId);
    QMessageBox::warning(this, tr("Error"), tr("No se pudo descargar ") + modelId + ":\n" + message);
}

// ── Refresh lists ────────────────────────────────────────────────────────
void SettingsDialog::refreshLocalModels()
{
    m_localListWidget->clear();
    m_installedIds.clear();

    QSettings s("LocalApp", "FFmpegExtractor");
    QStringList activeList = s.value("activeModels").toStringList();

    // Scan all possible model directories
    QStringList searchDirs;
    searchDirs << m_translator->modelDirectory();
    searchDirs << QCoreApplication::applicationDirPath() + "/../models";
    searchDirs << QCoreApplication::applicationDirPath() + "/models";
    searchDirs << QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/models";

    for (const QString &baseDir : searchDirs) {
        QDir d(baseDir);
        if (!d.exists()) continue;
        for (const QString &entry : d.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            if (m_installedIds.contains(entry)) continue;
            QString full = d.absolutePath() + "/" + entry;

            // B4: un modelo instalado requiere la config bergamot (model_info.json no basta)
            bool hasConfig = QFile::exists(full + "/config.intgemm8bitalpha.yml") ||
                             QFile::exists(full + "/config.yml");
            if (!hasConfig) continue;

            m_installedIds.insert(entry);

            // Try to parse language pair from directory name (e.g., "en-es-tiny")
            QStringList parts = entry.split('-');
            QString displayName = entry;
            QString src, trg, sz;
            if (parts.size() >= 3) {
                src = parts[0];
                trg = parts[1];
                sz = parts[2];
                displayName = QString("%1\n   %2 → %3  ·  %4")
                    .arg(entry)
                    .arg(SubtitleTranslator::languageName(src))
                    .arg(SubtitleTranslator::languageName(trg))
                    .arg(sz == "tiny" ? "⚡ tiny" : "★ base");
            } else if (parts.size() == 2) {
                src = parts[0];
                trg = parts[1];
                displayName = QString("%1\n   %2 → %3")
                    .arg(entry)
                    .arg(SubtitleTranslator::languageName(src))
                    .arg(SubtitleTranslator::languageName(trg));
            }

            bool active = activeList.contains(entry);
            if (active) displayName += "  ✓ ACTIVO";

            QListWidgetItem *item = new QListWidgetItem(displayName);
            item->setData(Qt::UserRole, entry);
            if (active) item->setForeground(QBrush(QColor("#39FF14")));
            m_localListWidget->addItem(item);
        }
    }

    int n = m_localListWidget->count();
    int na = activeList.size();
    m_statusLabel->setText(tr("%1 instalados, %2 activos").arg(n).arg(na));
}

void SettingsDialog::refreshOnlineModels()
{
    m_onlineListWidget->clear();

    // Show available translation paths header
    QStringList sources = SubtitleTranslator::supportedSourceLanguages();
    QStringList targets = SubtitleTranslator::supportedTargetLanguages();
    QStringList headerLines;
    headerLines << tr("<b>Modelos disponibles:</b>");
    for (const QString &src : sources) {
        for (const QString &trg : targets) {
            if (src == trg) continue;
            QStringList path = m_translator->findTranslationPath(src, trg);
            QString srcName = SubtitleTranslator::languageName(src);
            QString trgName = SubtitleTranslator::languageName(trg);
            if (path.size() == 2) {
                headerLines << QString("  <span style='color:#66BB6A'>✓</span> %1 → %2").arg(srcName, trgName);
            } else if (path.size() > 2) {
                QString pathStr;
                for (int i = 0; i < path.size(); ++i) {
                    if (i > 0) pathStr += " → ";
                    pathStr += SubtitleTranslator::languageName(path[i]);
                }
                headerLines << QString("  <span style='color:#FFD700'>↻</span> %1 → %2 <span style='color:#888'>(via %3)</span>")
                    .arg(srcName, trgName, SubtitleTranslator::languageName(path[1]));
            } else {
                headerLines << QString("  <span style='color:#FF5555'>✗</span> %1 → %2 <span style='color:#888'>") + tr("(no disponible)") + QString("</span>")
                    .arg(srcName, trgName);
            }
        }
    }
    // Leak: eliminar QLabel huérfano y la fila vacía (item 0)
    QListWidgetItem *headerItem = new QListWidgetItem();
    headerItem->setFlags(headerItem->flags() & ~Qt::ItemIsSelectable);
    m_onlineListWidget->addItem(headerItem);

    // Separator
    QListWidgetItem *sepItem = new QListWidgetItem("─────────────────────────────");
    sepItem->setFlags(sepItem->flags() & ~Qt::ItemIsSelectable);
    sepItem->setForeground(QBrush(QColor("#444")));
    m_onlineListWidget->addItem(sepItem);

    // Add models from registry
    for (const RemoteModel &m : m_onlineModels) {
        QString sz = (m.type == "tiny") ? "⚡ tiny" : "★ base";
        QString inst = m.isInstalled ? "  " + tr("(ya instalado)") : "";
        QString szStr = m.sizeBytes > 0 ? formatSize(m.sizeBytes) : "";
        bool canUpdate = m.isInstalled && m_downloader && m_downloader->isModelUpdateAvailable(m);
        QString upd = canUpdate ? "  " + tr("↻ actualizar") : "";
        QString txt = QString("%1\n   %2 → %3  ·  %4 %5%6%7")
            .arg(m.id)
            .arg(SubtitleTranslator::languageName(m.sourceLang))
            .arg(SubtitleTranslator::languageName(m.targetLang))
            .arg(sz).arg(szStr).arg(inst).arg(upd);
        QListWidgetItem *item = new QListWidgetItem(txt);
        item->setData(Qt::UserRole, m.id);
        if (m.isInstalled && !canUpdate) item->setForeground(QBrush(QColor("#666")));
        if (canUpdate) item->setForeground(QBrush(QColor("#FFD700")));
        m_onlineListWidget->addItem(item);
    }

    // Set the header widget on the first item
    if (m_onlineListWidget->count() > 0) {
        // Use a custom widget for the header
        QWidget *headerW = new QWidget();
        QVBoxLayout *hdrLay = new QVBoxLayout(headerW);
        hdrLay->setContentsMargins(8, 8, 8, 8);
        QLabel *hdrLabel = new QLabel(headerLines.join("<br>"));
        hdrLabel->setStyleSheet("color:#AAA;font-size:11px;");
        hdrLabel->setWordWrap(true);
        hdrLay->addWidget(hdrLabel);
        QListWidgetItem *firstItem = m_onlineListWidget->item(0);
        firstItem->setSizeHint(headerW->sizeHint());
        m_onlineListWidget->setItemWidget(firstItem, headerW);
    }
}

QString SettingsDialog::formatSize(qint64 bytes)
{
    if (bytes > 1073741824) return QString::number(bytes/1073741824.0,'f',1) + " GB";
    if (bytes > 1048576) return QString::number(bytes/1048576.0,'f',1) + " MB";
    return QString::number(bytes/1024.0,'f',0) + " KB";
}
