#include "settingsdialog.h"
#include "subtitletranslator.h"
#include "modeldownloader.h"
#include "theme.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
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
#include <QHash>
#include <algorithm>

// Modelos tiny incluidos en la app (instalados por defecto en todos los builds).
static const QStringList kBundledModelIds = {
    QStringLiteral("en-es-tiny"), QStringLiteral("es-en-tiny"),
    QStringLiteral("en-fr-tiny"), QStringLiteral("fr-en-tiny"),
    QStringLiteral("en-pt-tiny"), QStringLiteral("pt-en-tiny")
};

static QString bundledModelSize(const QString &id)
{
    static const QHash<QString, QString> sizes = {
        {"en-es-tiny", "~21 MB"}, {"es-en-tiny", "~21 MB"},
        {"en-fr-tiny", "~25 MB"}, {"fr-en-tiny", "~26 MB"},
        {"en-pt-tiny", "~24 MB"}, {"pt-en-tiny", "~25 MB"}
    };
    return sizes.value(id, "~20 MB");
}

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
    m_dstLbl = new QLabel(tr("Destino:"));
    m_dstLbl->setProperty("labelColor", "muted");
    m_defaultDstLangCombo = new QComboBox();
    refreshLanguageCombos();
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

    // Buscador común para ambas listas
    QHBoxLayout *searchRow = new QHBoxLayout();
    QLabel *searchLbl = new QLabel(tr("Buscar:"));
    searchLbl->setObjectName("modelSearchLabel");
    searchLbl->setProperty("labelColor", "muted");
    m_searchEdit = new QLineEdit();
    m_searchEdit->setObjectName("modelSearchEdit");
    m_searchEdit->setPlaceholderText(tr("ID, idioma, nombre o tamaño…"));
    m_searchEdit->setClearButtonEnabled(true);
    connect(m_searchEdit, &QLineEdit::textChanged, this, [this]() {
        refreshLocalModels();
        refreshOnlineModels();
    });
    searchRow->addWidget(searchLbl);
    searchRow->addWidget(m_searchEdit, 1);
    modLay->addLayout(searchRow);

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
    m_leftTitle = new QLabel(tr("INSTALADOS — incluidos y descargados"));
    m_leftTitle->setProperty("labelColor", "green");
    leftLay->addWidget(m_leftTitle);

    m_localListWidget = new QListWidget();
    m_localListWidget->setObjectName("localModelList");
    connect(m_localListWidget, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        if (!item->data(Qt::UserRole + 1).toBool())
            onActivateModel(item->data(Qt::UserRole).toString());
    });
    leftLay->addWidget(m_localListWidget, 1);

    QHBoxLayout *delRow = new QHBoxLayout();
    m_deleteBtn = new QPushButton(QIcon(":/icons/delete.svg"), tr("Eliminar"));
    m_deleteBtn->setEnabled(false);
    connect(m_localListWidget, &QListWidget::itemSelectionChanged, this, [this]() {
        QListWidgetItem *item = m_localListWidget->currentItem();
        const bool bundled = item && kBundledModelIds.contains(item->data(Qt::UserRole).toString());
        m_deleteBtn->setEnabled(item && !bundled);
        m_deleteBtn->setToolTip(bundled
            ? tr("Los modelos incluidos no se pueden eliminar")
            : tr("Elimina el modelo descargado seleccionado"));
    });
    connect(m_deleteBtn, &QPushButton::clicked, this, [this]() {
        QListWidgetItem *item = m_localListWidget->currentItem();
        if (!item) return;
        const QString id = item->data(Qt::UserRole).toString();
        if (kBundledModelIds.contains(id)) {
            QMessageBox::information(this, tr("Modelo incluido"),
                                     tr("Este modelo viene con la aplicación y no se puede eliminar."));
            return;
        }
        if (QMessageBox::question(this, tr("Eliminar"), tr("¿Eliminar ") + id + "?")
            == QMessageBox::Yes)
            onDeleteModel(id);
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
        tr("<b>Incluidos</b>: 6 modelos tiny listos para usar y no eliminables · <b>Activo</b> = preferido si hay varias opciones<br>"
        "Doble clic = activar/desactivar o descargar · El buscador filtra ambas listas"));
    m_infoLbl->setProperty("labelColor", "muted");
    m_infoLbl->setWordWrap(true);
    modLay->addWidget(m_infoLbl);

    tabs->addTab(tabModelos, tr("Modelos de Traducción"));

    // ════════════════════════════════════════════════════════════════════════
    // TAB 3: SUBTÍTULOS ONLINE
    // ════════════════════════════════════════════════════════════════════════
    QWidget *tabOnlineSubs = new QWidget();
    QVBoxLayout *onlineLay = new QVBoxLayout(tabOnlineSubs);

    m_onlineSubsBox = new QGroupBox(tr("Credenciales de OpenSubtitles"));
    QFormLayout *onlineForm = new QFormLayout(m_onlineSubsBox);
    m_backendLbl = new QLabel(tr("Backend:"));
    m_backendCombo = new QComboBox();
    m_backendCombo->addItem(tr("XML-RPC (enlaces directos, tipo VLC)"), QStringLiteral("xmlrpc"));
    m_backendCombo->addItem(tr("API REST (oficial)"), QStringLiteral("rest"));
    onlineForm->addRow(m_backendLbl, m_backendCombo);
    m_apiKeyLbl = new QLabel(tr("Clave API:"));
    m_onlineUserLbl = new QLabel(tr("Usuario:"));
    m_onlinePasswordLbl = new QLabel(tr("Contraseña:"));
    m_apiKeyEdit = new QLineEdit();
    m_onlineUserEdit = new QLineEdit();
    m_onlinePasswordEdit = new QLineEdit();
    m_onlinePasswordEdit->setEchoMode(QLineEdit::Password);
    onlineForm->addRow(m_apiKeyLbl, m_apiKeyEdit);
    onlineForm->addRow(m_onlineUserLbl, m_onlineUserEdit);
    onlineForm->addRow(m_onlinePasswordLbl, m_onlinePasswordEdit);
    onlineLay->addWidget(m_onlineSubsBox);

    m_onlineSubsInfoLbl = new QLabel();
    m_onlineSubsInfoLbl->setOpenExternalLinks(true);
    m_onlineSubsInfoLbl->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_onlineSubsInfoLbl->setWordWrap(true);
    m_onlineSubsInfoLbl->setProperty("labelColor", "muted");
    updateOnlineSubsInfoLabel();
    onlineLay->addWidget(m_onlineSubsInfoLbl);
    onlineLay->addStretch(1);

    tabs->addTab(tabOnlineSubs, tr("Subtítulos online"));

    mainLayout->addWidget(tabs, 1);

    // Close button
    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->addStretch(1);
    m_closeBtn = new QPushButton(QIcon(":/icons/check.svg"), tr("Cerrar"));
    connect(m_closeBtn, &QPushButton::clicked, this, &SettingsDialog::saveOnlineSubsSettings);
    btnRow->addWidget(m_closeBtn);
    mainLayout->addLayout(btnRow);


    // Load settings: credenciales solo desde el almacén seguro del sistema.
    QSettings s("LocalApp", "FFmpegExtractor");

    QString apiKey, onlineUser, onlinePassword;
    m_credentialStore.load(ElementsExtractCredentials::Service,
                           ElementsExtractCredentials::ApiKey, &apiKey);
    m_credentialStore.load(ElementsExtractCredentials::Service,
                           ElementsExtractCredentials::Username, &onlineUser);
    m_credentialStore.load(ElementsExtractCredentials::Service,
                           ElementsExtractCredentials::Password, &onlinePassword);
    m_apiKeyEdit->setText(apiKey);
    m_onlineUserEdit->setText(onlineUser);
    m_onlinePasswordEdit->setText(onlinePassword);

    const QString backend = s.value(QStringLiteral("onlineSubs/backend"),
                                    QStringLiteral("xmlrpc")).toString();
    const int backendIndex = m_backendCombo->findData(backend);
    m_backendCombo->setCurrentIndex(backendIndex >= 0 ? backendIndex : 0);
    connect(m_backendCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) {
        QSettings qs("LocalApp", "FFmpegExtractor");
        qs.setValue(QStringLiteral("onlineSubs/backend"),
                    m_backendCombo->currentData().toString());
        emit settingsChanged();
    });

    // Migración: purge de los antiguos valores en texto plano.
    s.remove("onlineSubs/apiKey");
    s.remove("onlineSubs/username");
    s.remove("onlineSubs/password");
    connect(m_apiKeyEdit, &QLineEdit::editingFinished,
            this, &SettingsDialog::saveOnlineSubsSettings);
    connect(m_onlineUserEdit, &QLineEdit::editingFinished,
            this, &SettingsDialog::saveOnlineSubsSettings);
    connect(m_onlinePasswordEdit, &QLineEdit::editingFinished,
            this, &SettingsDialog::saveOnlineSubsSettings);

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
            refreshLocalModels();
            refreshOnlineModels();
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
        m_tabs->setTabText(2, tr("Subtítulos online"));
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
    if (QLabel *l = findChild<QLabel *>("modelSearchLabel")) l->setText(tr("Buscar:"));
    if (m_searchEdit) m_searchEdit->setPlaceholderText(tr("ID, idioma, nombre o tamaño…"));
    if (m_leftTitle)    m_leftTitle->setText(tr("INSTALADOS — incluidos y descargados"));
    if (m_rightTitle)   m_rightTitle->setText(tr("ONLINE — doble clic para descargar"));
    if (m_deleteBtn)    m_deleteBtn->setText(tr("Eliminar"));
    if (m_infoLbl)
        m_infoLbl->setText(
            tr("<b>Incluidos</b>: 6 modelos tiny listos para usar y no eliminables · <b>Activo</b> = preferido si hay varias opciones<br>"
            "Doble clic = activar/desactivar o descargar · El buscador filtra ambas listas"));
    if (m_onlineSubsBox) m_onlineSubsBox->setTitle(tr("Credenciales de OpenSubtitles"));
    if (m_backendLbl)    m_backendLbl->setText(tr("Backend:"));
    if (m_backendCombo) {
        const QString current = m_backendCombo->currentData().toString();
        m_backendCombo->blockSignals(true);
        m_backendCombo->clear();
        m_backendCombo->addItem(tr("XML-RPC (enlaces directos, tipo VLC)"), QStringLiteral("xmlrpc"));
        m_backendCombo->addItem(tr("API REST (oficial)"), QStringLiteral("rest"));
        const int index = m_backendCombo->findData(current);
        m_backendCombo->setCurrentIndex(index >= 0 ? index : 0);
        m_backendCombo->blockSignals(false);
    }
    if (m_apiKeyLbl)     m_apiKeyLbl->setText(tr("Clave API:"));
    if (m_onlineUserLbl) m_onlineUserLbl->setText(tr("Usuario:"));
    if (m_onlinePasswordLbl) m_onlinePasswordLbl->setText(tr("Contraseña:"));
    updateOnlineSubsInfoLabel();
    if (m_closeBtn)     m_closeBtn->setText(tr("Cerrar"));

    refreshLocalModels();
    refreshOnlineModels();
}

void SettingsDialog::updateOnlineSubsInfoLabel(const QString &saveError)
{
    if (!m_onlineSubsInfoLbl)
        return;

    const QString base = tr(
        "La búsqueda de subtítulos en <a href=\"https://www.opensubtitles.com\">OpenSubtitles.com</a> "
        "es opcional: necesitas una cuenta y tu propia clave API.")
        + QStringLiteral("<br>")
        + tr("XML-RPC permite buscar sin credenciales, pero sin una cuenta con permisos de descarga "
             "los enlaces devuelven un subtítulo de prueba VIP.");
    const QString status = !saveError.isEmpty()
        ? tr("No se pudieron guardar las credenciales en el almacén seguro: %1")
              .arg(saveError.toHtmlEscaped())
        : (CredentialStore::isAvailable()
               ? tr("Las credenciales se guardan en el almacén seguro del sistema.")
               : tr("El almacén seguro del sistema no está disponible. Las credenciales no se guardarán: "
                    "tendrás que introducirlas cada vez."));
    m_onlineSubsInfoLbl->setText(base + QStringLiteral("<br><br>") + status);
}

void SettingsDialog::saveOnlineSubsSettings()
{
    m_credentialError.clear();
    if (CredentialStore::isAvailable()) {
        const QString apiKey = m_apiKeyEdit->text().trimmed();
        const QString username = m_onlineUserEdit->text().trimmed();
        const QString password = m_onlinePasswordEdit->text();
        QString error;
        bool ok = m_credentialStore.save(ElementsExtractCredentials::Service,
                                         ElementsExtractCredentials::ApiKey, apiKey, &error);
        ok = m_credentialStore.save(ElementsExtractCredentials::Service,
                                    ElementsExtractCredentials::Username, username, &error) && ok;
        ok = m_credentialStore.save(ElementsExtractCredentials::Service,
                                    ElementsExtractCredentials::Password, password, &error) && ok;
        if (!ok)
            m_credentialError = error;
    }

    // Nunca se escriben credenciales en QSettings; se eliminan las claves
    // heredadas de versiones anteriores aunque el almacén no esté disponible.
    QSettings s("LocalApp", "FFmpegExtractor");
    s.setValue(QStringLiteral("onlineSubs/backend"),
               m_backendCombo->currentData().toString());
    s.remove("onlineSubs/apiKey");
    s.remove("onlineSubs/username");
    s.remove("onlineSubs/password");

    updateOnlineSubsInfoLabel(m_credentialError);
    emit settingsChanged();
    if (sender() == m_closeBtn)
        accept();
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
        refreshLanguageCombos();
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

    if (kBundledModelIds.contains(modelId)) {
        QMessageBox::information(this, tr("Modelo incluido"),
                                 tr("Este modelo ya viene instalado con la aplicación; no hace falta descargarlo."));
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
    if (kBundledModelIds.contains(modelId)) {
        QMessageBox::information(this, tr("Modelo incluido"),
                                 tr("Este modelo viene con la aplicación y no se puede eliminar."));
        return;
    }

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

    refreshLanguageCombos();
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

    // Refresh language selectors first: the new model may add a language
    refreshLanguageCombos();
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
void SettingsDialog::refreshLanguageCombos()
{
    if (!m_defaultSrcLangCombo || !m_defaultDstLangCombo) return;

    QSettings s("LocalApp", "FFmpegExtractor");
    const QString prevSrc = m_defaultSrcLangCombo->count()
        ? m_defaultSrcLangCombo->currentData().toString()
        : s.value("srcLang", "en").toString();
    const QString prevDst = m_defaultDstLangCombo->count()
        ? m_defaultDstLangCombo->currentData().toString()
        : s.value("dstLang", "es").toString();

    const QStringList languages = SubtitleTranslator::supportedSourceLanguages();
    m_defaultSrcLangCombo->blockSignals(true);
    m_defaultDstLangCombo->blockSignals(true);
    m_defaultSrcLangCombo->clear();
    m_defaultDstLangCombo->clear();
    for (const QString &code : languages) {
        const QString name = SubtitleTranslator::languageName(code);
        m_defaultSrcLangCombo->addItem(name, code);
        m_defaultDstLangCombo->addItem(name, code);
    }

    int idx = m_defaultSrcLangCombo->findData(prevSrc);
    m_defaultSrcLangCombo->setCurrentIndex(idx >= 0 ? idx : 0);
    idx = m_defaultDstLangCombo->findData(prevDst);
    if (idx < 0) idx = m_defaultDstLangCombo->findData(QStringLiteral("es"));
    m_defaultDstLangCombo->setCurrentIndex(idx >= 0 ? idx : 0);
    m_defaultSrcLangCombo->blockSignals(false);
    m_defaultDstLangCombo->blockSignals(false);
}

void SettingsDialog::refreshLocalModels()
{
    m_localListWidget->clear();
    m_installedIds.clear();

    QSettings s("LocalApp", "FFmpegExtractor");
    const QStringList activeList = s.value("activeModels").toStringList();

    QStringList searchDirs;
    searchDirs << m_translator->modelDirectory();
    searchDirs << QCoreApplication::applicationDirPath() + "/../models";
    searchDirs << QCoreApplication::applicationDirPath() + "/models";
    searchDirs << QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/models";

    QMap<QString, QString> found;
    for (const QString &baseDir : searchDirs) {
        QDir d(baseDir);
        if (!d.exists()) continue;
        for (const QString &entry : d.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            if (found.contains(entry)) continue;
            const QString full = d.absolutePath() + "/" + entry;
            bool hasConfig = QFile::exists(full + "/config.intgemm8bitalpha.yml") ||
                             QFile::exists(full + "/config.yml");
            if (!hasConfig) {
                QDir modelDir(full);
                for (const QString &sub : modelDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                    const QString nested = full + "/" + sub;
                    if (QFile::exists(nested + "/config.intgemm8bitalpha.yml") ||
                        QFile::exists(nested + "/config.yml")) { hasConfig = true; break; }
                }
            }
            if (!hasConfig) continue;
            found.insert(entry, full);
            m_installedIds.insert(entry);
        }
    }

    const QString term = m_searchEdit ? m_searchEdit->text().trimmed().toLower() : QString();
    auto remoteOf = [this](const QString &id) -> const RemoteModel * {
        for (const RemoteModel &m : m_onlineModels)
            if (m.id == id) return &m;
        return nullptr;
    };
    auto matches = [&](const QString &id, const RemoteModel *r) {
        if (term.isEmpty()) return true;
        const QString src = id.section('-', 0, 0);
        const QString trg = id.section('-', 1, 1);
        QStringList haystack { id, src, trg,
                               SubtitleTranslator::languageName(src),
                               SubtitleTranslator::languageName(trg) };
        if (r) haystack << r->name << r->type << QString::number(r->sizeBytes);
        for (const QString &h : haystack)
            if (h.toLower().contains(term)) return true;
        return false;
    };
    auto addSection = [&](const QString &title) {
        QListWidgetItem *sec = new QListWidgetItem(title);
        sec->setFlags(sec->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
        sec->setForeground(QBrush(QColor("#FF8800")));
        m_localListWidget->addItem(sec);
    };
    auto addItem = [&](const QString &id, bool bundled) {
        const RemoteModel *r = remoteOf(id);
        if (!matches(id, r)) return;

        const QString src = id.section('-', 0, 0);
        const QString trg = id.section('-', 1, 1);
        const QString sizeText = id.contains(QStringLiteral("-base")) ? tr("★ base") : tr("⚡ tiny");
        const bool active = activeList.contains(id);
        const QString name = (r && !r->name.isEmpty()) ? r->name : tr("Modelo Bergamot");
        const QString size = bundled ? bundledModelSize(id)
                                     : (r && r->sizeBytes > 0 ? formatSize(r->sizeBytes) : tr("tamaño desconocido"));
        const QString version = (r && r->version > 0) ? tr(" · v%1").arg(r->version) : QString();
        const QString origin = bundled ? tr("incluido · no eliminable") : tr("descargado");

        const QString label = QString("%1\n   %2 → %3  ·  %4 · %5%6\n   %7 · %8%9")
            .arg(id)
            .arg(SubtitleTranslator::languageName(src),
                 SubtitleTranslator::languageName(trg),
                 sizeText, origin)
            .arg(active ? QString("  ✓ ") + tr("activo") : QString())
            .arg(name, size, version);
        QListWidgetItem *item = new QListWidgetItem(label);
        item->setData(Qt::UserRole, id);
        item->setData(Qt::UserRole + 1, bundled);
        item->setToolTip(tr("Ruta: %1").arg(found.value(id)));
        if (active) item->setForeground(QBrush(QColor("#39FF14")));
        else if (bundled) item->setForeground(QBrush(QColor("#A8D8A0")));
        m_localListWidget->addItem(item);
    };

    // Apartado 1: modelos incluidos (no eliminables). Apartado 2: descargados.
    int bundledShown = 0, downloadedShown = 0;
    for (const QString &id : kBundledModelIds) {
        if (found.contains(id) && matches(id, remoteOf(id))) ++bundledShown;
    }
    for (const QString &id : found.keys()) {
        if (!kBundledModelIds.contains(id) && matches(id, remoteOf(id))) ++downloadedShown;
    }
    if (bundledShown) {
        addSection(tr("INCLUIDOS (NO ELIMINABLES)"));
        for (const QString &id : kBundledModelIds)
            if (found.contains(id)) addItem(id, true);
    }
    if (downloadedShown) {
        addSection(tr("DESCARGADOS"));
        for (const QString &id : found.keys())
            if (!kBundledModelIds.contains(id)) addItem(id, false);
    }

    m_statusLabel->setText(tr("%1 incluidos · %2 descargados · %3 activos · %4 online")
                               .arg(bundledShown).arg(downloadedShown)
                               .arg(activeList.size()).arg(m_onlineModels.size()));
    m_deleteBtn->setEnabled(false);
}

void SettingsDialog::refreshOnlineModels()
{
    m_onlineListWidget->clear();

    const QStringList supported = SubtitleTranslator::supportedSourceLanguages();
    int direct = 0, triangular = 0;
    for (const QString &src : supported) {
        for (const QString &trg : supported) {
            if (src == trg) continue;
            const QStringList path = m_translator->findTranslationPath(src, trg);
            if (path.size() == 2) ++direct;
            else if (path.size() > 2) ++triangular;
        }
    }

    QListWidgetItem *headerItem = new QListWidgetItem();
    headerItem->setFlags(headerItem->flags() & ~Qt::ItemIsSelectable);
    m_onlineListWidget->addItem(headerItem);
    QWidget *headerW = new QWidget();
    QVBoxLayout *hdrLay = new QVBoxLayout(headerW);
    hdrLay->setContentsMargins(8, 8, 8, 8);
    QLabel *hdrLabel = new QLabel(
        tr("<b>Modelos online</b><br>"
           "%1 disponibles · %2 rutas directas · %3 rutas triangulares<br>"
           "Todos se pueden descargar; al instalar un par, su idioma aparece en los selectores.")
            .arg(m_onlineModels.size()).arg(direct).arg(triangular));
    hdrLabel->setStyleSheet("color:#AAA;font-size:11px;");
    hdrLabel->setWordWrap(true);
    hdrLay->addWidget(hdrLabel);
    headerItem->setSizeHint(headerW->sizeHint());
    m_onlineListWidget->setItemWidget(headerItem, headerW);

    QListWidgetItem *sepItem = new QListWidgetItem("─────────────────────────────");
    sepItem->setFlags(sepItem->flags() & ~Qt::ItemIsSelectable);
    sepItem->setForeground(QBrush(QColor("#444")));
    m_onlineListWidget->addItem(sepItem);

    const QString term = m_searchEdit ? m_searchEdit->text().trimmed().toLower() : QString();
    QList<RemoteModel> visible;
    for (const RemoteModel &m : m_onlineModels) {
        if (term.isEmpty()) { visible.append(m); continue; }
        const QString srcName = SubtitleTranslator::languageName(m.sourceLang);
        const QString trgName = SubtitleTranslator::languageName(m.targetLang);
        const QStringList haystack { m.id, m.name, m.sourceLang, m.targetLang,
                                     srcName, trgName, m.type,
                                     QString::number(m.sizeBytes) };
        bool hit = false;
        for (const QString &h : haystack) {
            if (h.toLower().contains(term)) { hit = true; break; }
        }
        if (hit) visible.append(m);
    }

    std::sort(visible.begin(), visible.end(), [](const RemoteModel &a, const RemoteModel &b) {
        const QString as = SubtitleTranslator::languageName(a.sourceLang);
        const QString bs = SubtitleTranslator::languageName(b.sourceLang);
        if (as != bs) return as < bs;
        const QString at = SubtitleTranslator::languageName(a.targetLang);
        const QString bt = SubtitleTranslator::languageName(b.targetLang);
        if (at != bt) return at < bt;
        if (a.type != b.type) return a.type < b.type;
        return a.id < b.id;
    });

    for (const RemoteModel &m : visible) {
        const QString type = m.type == QStringLiteral("base") ? tr("★ base") : tr("⚡ tiny");
        QString inst;
        if (m.isInstalled) {
            inst = kBundledModelIds.contains(m.id) ? tr(" · incluido") : tr(" · ya instalado");
        }
        const bool canUpdate = m.isInstalled && m_downloader
                               && m_downloader->isModelUpdateAvailable(m);
        const QString upd = canUpdate ? tr("  ↻ actualizar") : QString();
        const QString szStr = m.sizeBytes > 0
            ? formatSize(m.sizeBytes)
            : (m.type == QStringLiteral("base") ? tr("~50 MB") : tr("~20 MB"));
        const QString version = m.version > 0 ? tr("v%1").arg(m.version) : QString();
        const QString name = m.name.isEmpty() ? tr("Modelo Bergamot") : m.name;

        const QString txt = QString("%1\n   %2 → %3  ·  %4 · %5%6%7\n   %8 · %9")
            .arg(m.id)
            .arg(SubtitleTranslator::languageName(m.sourceLang),
                 SubtitleTranslator::languageName(m.targetLang),
                 type, szStr, inst, upd)
            .arg(name, version);
        QListWidgetItem *item = new QListWidgetItem(txt);
        item->setData(Qt::UserRole, m.id);
        item->setToolTip(tr("Idiomas: %1 → %2%3\nNombre: %4\nTamaño: %5\nTipo: %6")
            .arg(m.sourceLang, m.targetLang,
                 version.isEmpty() ? QString() : QString(" · " + version),
                 name, szStr, type));
        if (canUpdate) item->setForeground(QBrush(QColor("#FFD700")));
        else if (m.isInstalled) item->setForeground(QBrush(QColor("#666")));
        m_onlineListWidget->addItem(item);
    }
}

QString SettingsDialog::formatSize(qint64 bytes)
{
    if (bytes > 1073741824) return QString::number(bytes/1073741824.0,'f',1) + " GB";
    if (bytes > 1048576) return QString::number(bytes/1048576.0,'f',1) + " MB";
    return QString::number(bytes/1024.0,'f',0) + " KB";
}
