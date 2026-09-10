#include "settingsdialog.h"
#include "subtitletranslator.h"
#include "modeldownloader.h"

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

static const QString REGISTRY_URL = "https://translatelocally.com/models.json";

SettingsDialog::SettingsDialog(SubtitleTranslator *translator,
                               ModelDownloader *downloader,
                               QWidget *parent)
    : QDialog(parent)
    , m_translator(translator)
    , m_downloader(downloader)
    , m_netManager(new QNetworkAccessManager(this))
{
    setWindowTitle("Ajustes — Elements Extract");
    setMinimumSize(750, 550);
    resize(800, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(12, 12, 12, 12);

    QTabWidget *tabs = new QTabWidget();
    tabs->setStyleSheet(R"(
        QTabWidget::pane { border: 1px solid #2A2A2A; background-color: #121212; }
        QTabBar::tab { background-color: #1A1A1A; color: #D0D0D0;
                       padding: 8px 16px; border: 1px solid #2A2A2A;
                       border-bottom: none; margin-right: 2px; }
        QTabBar::tab:selected { background-color: #121212; color: #FF8800;
                                border-bottom: 2px solid #FF8800; }
        QTabBar::tab:hover { background-color: #2A2A2A; }
    )");

    // ════════════════════════════════════════════════════════════════════════
    // TAB 1: GENERAL
    // ════════════════════════════════════════════════════════════════════════
    QWidget *tabGeneral = new QWidget();
    QVBoxLayout *genLay = new QVBoxLayout(tabGeneral);

    // Model directory
    QGroupBox *dirBox = new QGroupBox("Directorio de Modelos");
    QVBoxLayout *dirLay = new QVBoxLayout(dirBox);
    QHBoxLayout *dirRow = new QHBoxLayout();
    m_modelsDirEdit = new QLineEdit(m_translator->modelDirectory());
    m_modelsDirEdit->setReadOnly(true);
    QPushButton *browseBtn = new QPushButton("Examinar...");
    connect(browseBtn, &QPushButton::clicked, this, &SettingsDialog::onBrowseModelsDir);
    dirRow->addWidget(m_modelsDirEdit, 1);
    dirRow->addWidget(browseBtn);
    dirLay->addLayout(dirRow);
    genLay->addWidget(dirBox);

    // Languages
    QGroupBox *langBox = new QGroupBox("Idiomas por Defecto");
    QGridLayout *langGrid = new QGridLayout(langBox);
    QLabel *srcLbl = new QLabel("Origen:"); srcLbl->setStyleSheet("font-weight:normal;color:#888;");
    m_defaultSrcLangCombo = new QComboBox();
    m_defaultSrcLangCombo->addItem("English", "en");
    m_defaultSrcLangCombo->addItem("Español", "es");
    m_defaultSrcLangCombo->addItem("Français", "fr");
    m_defaultSrcLangCombo->addItem("Português", "pt");
    QLabel *dstLbl = new QLabel("Destino:"); dstLbl->setStyleSheet("font-weight:normal;color:#888;");
    m_defaultDstLangCombo = new QComboBox();
    m_defaultDstLangCombo->addItem("Español", "es");
    m_defaultDstLangCombo->addItem("English", "en");
    m_defaultDstLangCombo->addItem("Français", "fr");
    m_defaultDstLangCombo->addItem("Português", "pt");
    langGrid->addWidget(srcLbl, 0, 0); langGrid->addWidget(m_defaultSrcLangCombo, 0, 1);
    langGrid->addWidget(dstLbl, 0, 2); langGrid->addWidget(m_defaultDstLangCombo, 0, 3);
    genLay->addWidget(langBox);

    // Options
    QGroupBox *optBox = new QGroupBox("Opciones");
    QVBoxLayout *optLay = new QVBoxLayout(optBox);
    m_autoTranslateCheck = new QCheckBox("Traducir subtítulos automáticamente al extraer");
    m_autoTranslateCheck->setStyleSheet("font-weight:normal;");
    optLay->addWidget(m_autoTranslateCheck);
    genLay->addWidget(optBox);

    genLay->addStretch(1);
    tabs->addTab(tabGeneral, "General");

    // ════════════════════════════════════════════════════════════════════════
    // TAB 2: MODELOS
    // ════════════════════════════════════════════════════════════════════════
    QWidget *tabModelos = new QWidget();
    QVBoxLayout *modLay = new QVBoxLayout(tabModelos);

    // Status row
    QHBoxLayout *statusRow = new QHBoxLayout();
    m_statusLabel = new QLabel("Modelos instalados");
    m_statusLabel->setStyleSheet("font-weight:normal;color:#FF8800;");
    QPushButton *fetchBtn = new QPushButton("Buscar online");
    fetchBtn->setToolTip("Buscar modelos disponibles en translatelocally.com");
    connect(fetchBtn, &QPushButton::clicked, this, &SettingsDialog::onFetchOnlineModels);
    statusRow->addWidget(m_statusLabel, 1);
    statusRow->addWidget(fetchBtn);
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
    QLabel *leftTitle = new QLabel("INSTALADOS — doble clic para activar/desactivar");
    leftTitle->setStyleSheet("font-size:11px;color:#39FF14;font-weight:bold;");
    leftLay->addWidget(leftTitle);

    m_localListWidget = new QListWidget();
    m_localListWidget->setStyleSheet(R"(
        QListWidget{background:#1A1A1A;border:1px solid #2A2A2A;border-radius:4px;padding:4px;color:#D0D0D0;}
        QListWidget::item{padding:6px 8px;border-bottom:1px solid #1A1A1A;}
        QListWidget::item:selected{background:#1A3A1A;color:#39FF14;}
        QListWidget::item:hover{background:#222;}
    )");
    connect(m_localListWidget, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        onActivateModel(item->data(Qt::UserRole).toString());
    });
    leftLay->addWidget(m_localListWidget, 1);

    QHBoxLayout *delRow = new QHBoxLayout();
    QPushButton *delBtn = new QPushButton("Eliminar");
    connect(delBtn, &QPushButton::clicked, this, [this]() {
        QListWidgetItem *item = m_localListWidget->currentItem();
        if (!item) return;
        if (QMessageBox::question(this, "Eliminar", "¿Eliminar " + item->text().split('\n').first() + "?")
            == QMessageBox::Yes)
            onDeleteModel(item->data(Qt::UserRole).toString());
    });
    delRow->addStretch(); delRow->addWidget(delBtn);
    leftLay->addLayout(delRow);
    splitter->addWidget(leftW);

    // Right: Online models
    QWidget *rightW = new QWidget();
    QVBoxLayout *rightLay = new QVBoxLayout(rightW);
    rightLay->setContentsMargins(0,0,0,0);
    QLabel *rightTitle = new QLabel("ONLINE — doble clic para descargar");
    rightTitle->setStyleSheet("font-size:11px;color:#FF8800;font-weight:bold;");
    rightLay->addWidget(rightTitle);

    m_onlineListWidget = new QListWidget();
    m_onlineListWidget->setStyleSheet(R"(
        QListWidget{background:#1A1A1A;border:1px solid #2A2A2A;border-radius:4px;padding:4px;color:#D0D0D0;}
        QListWidget::item{padding:6px 8px;border-bottom:1px solid #1A1A1A;}
        QListWidget::item:selected{background:#3A2A00;color:#FF8800;}
        QListWidget::item:hover{background:#222;}
    )");
    connect(m_onlineListWidget, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        onDownloadModel(item->data(Qt::UserRole).toString());
    });
    rightLay->addWidget(m_onlineListWidget, 1);
    splitter->addWidget(rightW);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    modLay->addWidget(splitter, 1);

    // Info
    QLabel *infoLbl = new QLabel(
        "<b>tiny</b> ~15-25MB rápido · <b>base</b> ~50MB mejor calidad<br>"
        "Doble clic = activar/desactivar (solo activos se usan para traducir)");
    infoLbl->setStyleSheet("color:#888;font-size:11px;font-weight:normal;");
    infoLbl->setWordWrap(true);
    modLay->addWidget(infoLbl);

    tabs->addTab(tabModelos, "Modelos de Traducción");

    mainLayout->addWidget(tabs, 1);

    // Close button
    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->addStretch(1);
    QPushButton *closeBtn = new QPushButton("Cerrar");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    btnRow->addWidget(closeBtn);
    mainLayout->addLayout(btnRow);

    // Theme
    setStyleSheet(R"(
        QDialog{background:#121212;}
        QLabel{color:#FF8800;font-weight:bold;}
        QGroupBox{border:1px solid #2A2A2A;border-radius:5px;margin-top:10px;padding-top:14px;color:#FF8800;font-weight:bold;font-size:11px;}
        QGroupBox::title{subcontrol-origin:margin;left:10px;}
        QLineEdit{background:#1A1A1A;border:1px solid #2A2A2A;border-radius:4px;padding:5px;color:#D0D0D0;}
        QLineEdit:focus{border:1px solid #FF8800;}
        QComboBox{background:#1A1A1A;border:1px solid #2A2A2A;border-radius:4px;padding:4px 8px;color:#D0D0D0;}
        QComboBox:hover{border-color:#FF8800;}
        QComboBox QAbstractItemView{background:#1A1A1A;border:1px solid #FF8800;color:#D0D0D0;}
        QPushButton{background:#1A1A1A;border:1px solid #39FF14;border-radius:5px;padding:7px 14px;color:#39FF14;font-weight:bold;}
        QPushButton:hover{background:#39FF14;color:#000;}
        QCheckBox{padding:4px;color:#D0D0D0;font-weight:normal;}
        QCheckBox::indicator{width:16px;height:16px;background:#1A1A1A;border:1px solid #39FF14;}
        QCheckBox::indicator:checked{background:#39FF14;}
        QProgressBar{border:1px solid #2A2A2A;border-radius:4px;background:#1A1A1A;text-align:center;color:#D0D0D0;}
        QProgressBar::chunk{background:#FF8800;border-radius:3px;}
    )");

    // Load settings
    QSettings s("LocalApp", "FFmpegExtractor");
    int si = m_defaultSrcLangCombo->findData(s.value("srcLang","en").toString());
    int di = m_defaultDstLangCombo->findData(s.value("dstLang","es").toString());
    if (si >= 0) m_defaultSrcLangCombo->setCurrentIndex(si);
    if (di >= 0) m_defaultDstLangCombo->setCurrentIndex(di);
    m_autoTranslateCheck->setChecked(s.value("autoTranslate",false).toBool());

    if (m_downloader) {
        connect(m_downloader, &ModelDownloader::downloadProgress, this, &SettingsDialog::onModelDownloadProgress);
        connect(m_downloader, &ModelDownloader::downloadFinished, this, &SettingsDialog::onModelDownloaded);
        connect(m_downloader, &ModelDownloader::downloadError, this, &SettingsDialog::onModelDownloadError);
    }

    refreshLocalModels();

    // Auto-fetch online models on dialog open
    QTimer::singleShot(100, this, &SettingsDialog::onFetchOnlineModels);
}

// ── Actions ──────────────────────────────────────────────────────────────
void SettingsDialog::onBrowseModelsDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Directorio de modelos", m_modelsDirEdit->text());
    if (!dir.isEmpty()) {
        m_modelsDirEdit->setText(dir);
        m_translator->setModelDirectory(dir);
        refreshLocalModels();
        emit settingsChanged();
    }
}

void SettingsDialog::onFetchOnlineModels()
{
    m_statusLabel->setText("Buscando...");
    m_onlineListWidget->clear();
    // Also fetch in downloader so it has the URLs for download
    if (m_downloader) {
        connect(m_downloader, &ModelDownloader::modelsFetched, this, [this](const QList<RemoteModel> &models) {
            // Update installed state after fetch
            for (RemoteModel &m : m_onlineModels) {
                m.isInstalled = m_installedIds.contains(m.id);
            }
            refreshOnlineModels();
        }, Qt::UniqueConnection);
        m_downloader->fetchAvailableModels();
    }
    QNetworkRequest request{QUrl(REGISTRY_URL)};
    QNetworkReply *reply = m_netManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onOnlineModelsReply(reply);
    });
}

void SettingsDialog::onOnlineModelsReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        m_statusLabel->setText("Error: " + reply->errorString());
        reply->deleteLater();
        return;
    }
    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) { m_statusLabel->setText("Respuesta inválida"); return; }

    QJsonArray arr = doc.object().value("models").toArray();
    m_onlineModels.clear();

    for (const QJsonValue &v : arr) {
        QJsonObject o = v.toObject();
        RemoteModel m;
        m.id = o["shortName"].toString();
        m.name = o["modelName"].toString();
        // Extract language code from srcTags (e.g., {"en":"English"})
        QJsonObject srcTags = o["srcTags"].toObject();
        m.sourceLang = srcTags.keys().isEmpty() ? QString() : srcTags.keys().first().toLower();
        m.targetLang = o["trgTag"].toString().toLower();
        m.type = o["type"].toString();
        m.downloadUrl = o["url"].toString();
        m.checksum = o["checksum"].toString();
        m.sizeBytes = o["size"].toVariant().toLongLong();
        m.version = o["version"].toInt();
        // Show all models (no language filter)
        m.isInstalled = m_installedIds.contains(m.id);
        m_onlineModels.append(m);
    }
    refreshOnlineModels();
    m_statusLabel->setText(QString("%1 modelos online").arg(m_onlineModels.size()));
}

void SettingsDialog::onDownloadModel(const QString &modelId)
{
    if (!m_downloader) {
        QMessageBox::information(this, "No disponible", "Gestor de modelos no disponible.");
        return;
    }

    // Find model info in our online list
    RemoteModel modelInfo;
    bool found = false;
    for (const RemoteModel &m : m_onlineModels) {
        if (m.id == modelId) { modelInfo = m; found = true; break; }
    }

    if (!found) {
        QMessageBox::warning(this, "Error", "Modelo no encontrado en la lista online.");
        return;
    }

    m_downloadProgress->setVisible(true);
    m_downloadProgress->setValue(0);
    m_statusLabel->setText("Descargando: " + modelId);
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
        m_downloadProgress->setValue(static_cast<int>((received * 100) / total));
        m_statusLabel->setText(QString("Descargando %1: %2%").arg(modelId).arg((received*100)/total));
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
    m_statusLabel->setText("Descargado y activado: " + modelId);
    emit settingsChanged();
}

void SettingsDialog::onModelDownloadError(const QString &modelId, const QString &message)
{
    m_downloadProgress->setVisible(false);
    m_statusLabel->setText("Error: " + modelId);
    QMessageBox::warning(this, "Error", "No se pudo descargar " + modelId + ":\n" + message);
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

            // Check for any bergamot config file
            bool hasConfig = QFile::exists(full + "/config.intgemm8bitalpha.yml") ||
                             QFile::exists(full + "/config.yml") ||
                             QFile::exists(full + "/model_info.json");
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
    m_statusLabel->setText(QString("%1 instalados, %2 activos").arg(n).arg(na));
}

void SettingsDialog::refreshOnlineModels()
{
    m_onlineListWidget->clear();

    // Show available translation paths header
    QStringList sources = SubtitleTranslator::supportedSourceLanguages();
    QStringList targets = SubtitleTranslator::supportedTargetLanguages();
    QStringList headerLines;
    headerLines << "<b>Modelos disponibles:</b>";
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
                headerLines << QString("  <span style='color:#FF5555'>✗</span> %1 → %2 <span style='color:#888'>(no disponible)</span>")
                    .arg(srcName, trgName);
            }
        }
    }
    QLabel *headerLabel = new QLabel(headerLines.join("<br>"));
    headerLabel->setStyleSheet("color:#AAA;font-size:11px;padding:8px;background:#1A1A1A;border-radius:4px;");
    headerLabel->setWordWrap(true);
    m_onlineListWidget->addItem(new QListWidgetItem());
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
        QString inst = m.isInstalled ? "  (ya instalado)" : "";
        QString szStr = m.sizeBytes > 0 ? formatSize(m.sizeBytes) : "";
        QString txt = QString("%1\n   %2 → %3  ·  %4 %5%6")
            .arg(m.id)
            .arg(SubtitleTranslator::languageName(m.sourceLang))
            .arg(SubtitleTranslator::languageName(m.targetLang))
            .arg(sz).arg(szStr).arg(inst);
        QListWidgetItem *item = new QListWidgetItem(txt);
        item->setData(Qt::UserRole, m.id);
        if (m.isInstalled) item->setForeground(QBrush(QColor("#666")));
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
