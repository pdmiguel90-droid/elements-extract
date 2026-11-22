#include "onlinesubsdialog.h"

#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QSettings>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTimer>
#include <QVBoxLayout>

OnlineSubsDialog::OnlineSubsDialog(SubtitleDownloader *downloader,
                                   const QString &videoPath,
                                   QWidget *parent)
    : QDialog(parent)
    , m_downloader(downloader)
    , m_videoPath(videoPath)
{
    setWindowTitle(tr("Subtítulos online — %1").arg(QFileInfo(videoPath).fileName()));
    setMinimumSize(680, 460);
    resize(760, 540);
    setModal(true);

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setSpacing(8);

    QFormLayout *form = new QFormLayout();
    QLabel *videoLabel = new QLabel(QFileInfo(videoPath).fileName());
    videoLabel->setProperty("labelColor", "muted");
    form->addRow(tr("Vídeo:"), videoLabel);

    m_queryEdit = new QLineEdit(cleanQuery(videoPath));
    m_queryEdit->setPlaceholderText(tr("Título de la película o serie…"));
    form->addRow(tr("Consulta:"), m_queryEdit);

    m_languageCombo = new QComboBox();
    m_languageCombo->addItem(tr("Todos"), "all");
    m_languageCombo->addItem(tr("Inglés"), "en");
    m_languageCombo->addItem(tr("Español"), "es");
    m_languageCombo->addItem(tr("Francés"), "fr");
    m_languageCombo->addItem(tr("Portugués"), "pt");
    m_languageCombo->addItem(tr("Alemán"), "de");
    m_languageCombo->addItem(tr("Italiano"), "it");
    form->addRow(tr("Idioma:"), m_languageCombo);

    QHBoxLayout *searchRow = new QHBoxLayout();
    searchRow->addStretch(1);
    m_searchButton = new QPushButton(tr("Buscar"));
    connect(m_searchButton, &QPushButton::clicked, this, &OnlineSubsDialog::doSearch);
    searchRow->addWidget(m_searchButton);
    form->addRow(searchRow);
    root->addLayout(form);

    m_resultsTable = new QTableWidget(0, 5, this);
    m_resultsTable->setHorizontalHeaderLabels({
        tr("Idioma"), tr("Título"), tr("Año"), tr("Descargas"), tr("Hash")
    });
    m_resultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_resultsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_resultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_resultsTable->verticalHeader()->setVisible(false);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    root->addWidget(m_resultsTable, 1);

    m_statusLabel = new QLabel();
    m_statusLabel->setWordWrap(true);
    m_statusLabel->setProperty("labelColor", "muted");
    root->addWidget(m_statusLabel);

    QHBoxLayout *buttons = new QHBoxLayout();
    buttons->addStretch(1);
    m_translateButton = new QPushButton(tr("Descargar y traducir"));
    m_downloadButton = new QPushButton(tr("Solo descargar"));
    m_cancelButton = new QPushButton(tr("Cancelar"));
    m_translateButton->setEnabled(false);
    m_downloadButton->setEnabled(false);
    buttons->addWidget(m_translateButton);
    buttons->addWidget(m_downloadButton);
    buttons->addWidget(m_cancelButton);
    root->addLayout(buttons);

    connect(m_translateButton, &QPushButton::clicked,
            this, &OnlineSubsDialog::downloadAndTranslate);
    connect(m_downloadButton, &QPushButton::clicked,
            this, &OnlineSubsDialog::downloadOnly);
    connect(m_cancelButton, &QPushButton::clicked, this, [this]() {
        if (m_downloader) m_downloader->cancel();
        reject();
    });

    if (m_downloader) {
        connect(m_downloader, &SubtitleDownloader::searchFinished,
                this, &OnlineSubsDialog::onSearchFinished);
        connect(m_downloader, &SubtitleDownloader::downloadFinished,
                this, &OnlineSubsDialog::onDownloadFinished);
        connect(m_downloader, &SubtitleDownloader::statusMessage,
                this, [this](const QString &message) { m_statusLabel->setText(message); });
        connect(m_downloader, &SubtitleDownloader::needFileChoice,
                this, [this](const QStringList &files, const QString &language) {
            if (files.isEmpty()) return;
            const QString lang = language.isEmpty() ? tr("idioma desconocido") : language;
            bool accepted = false;
            const QString chosen = QInputDialog::getItem(
                this, tr("Elegir subtítulo"),
                tr("El archivo contiene varios subtítulos (%1). Elige cuál usar:").arg(lang),
                files, 0, false, &accepted);
            m_downloader->setChosenArchiveEntry(accepted ? chosen : files.first());
        });
    }

    if (!m_downloader || !m_downloader->hasCredentials()) {
        m_statusLabel->setText(tr("Configura la clave API, el usuario y la contraseña en Ajustes → Subtítulos online."));
        m_searchButton->setEnabled(false);
    } else {
        // Búsqueda automática por hash del vídeo al abrir el diálogo.
        QTimer::singleShot(0, this, &OnlineSubsDialog::doSearch);
    }
}

QString OnlineSubsDialog::cleanQuery(const QString &fileName) const
{
    QString name = QFileInfo(fileName).completeBaseName();
    name.replace('_', ' ').replace('.', ' ');
    name.remove(QRegularExpression(QStringLiteral("[\\[\\(][^\\]\\)]*[\\]\\)]")));
    name.remove(QRegularExpression(
        QStringLiteral("(?i)\\b(1080p|720p|2160p|4k|480p|10bit|8bit|x264|x265|h264|h265|"
                       "web[- ]?dl|webpdl|web[- ]?rip|bluray|bdrip|brrip|hdtv|dvdrip|"
                       "aac|ac3|eac3|ddp5|dd5|truehd|flac|extended|proper|repack|internal|"
                       "limited|remux|hdr|amzn|nf|dsnp|hmax|atvp|hulu)\\b")));
    name.replace(QRegularExpression(QStringLiteral("\\s+")), QStringLiteral(" "));
    name = name.trimmed();
    name.remove(QRegularExpression(QStringLiteral("^[-–—\\s]+|[-–—\\s]+$")));
    return name.trimmed();
}

QString OnlineSubsDialog::downloadDirectory() const
{
    QSettings settings("LocalApp", "FFmpegExtractor");
    QString dir = settings.value("outputDir").toString().trimmed();
    if (dir.isEmpty())
        dir = QFileInfo(m_videoPath).absolutePath();
    return QDir(dir).absoluteFilePath(QStringLiteral("Descargados"));
}

void OnlineSubsDialog::doSearch()
{
    if (!m_downloader) return;
    if (m_queryEdit->text().trimmed().isEmpty()) {
        m_statusLabel->setText(tr("Introduce un título para buscar subtítulos."));
        return;
    }
    m_results.clear();
    m_resultsTable->setRowCount(0);
    m_translateButton->setEnabled(false);
    m_downloadButton->setEnabled(false);
    m_searchButton->setEnabled(false);
    m_statusLabel->setText(tr("Buscando subtítulos…"));
    m_downloader->search(m_videoPath, m_queryEdit->text(),
                         m_languageCombo->currentData().toString());
}

void OnlineSubsDialog::onSearchFinished(const QList<OnlineSubtitle> &results,
                                        const QString &error)
{
    m_searchButton->setEnabled(true);
    m_results = results;
    m_resultsTable->setRowCount(results.size());

    for (int row = 0; row < results.size(); ++row) {
        const OnlineSubtitle &item = results.at(row);
        m_resultsTable->setItem(row, 0, new QTableWidgetItem(item.language.toUpper()));
        const QString title = item.release.isEmpty() ? item.uploader : item.release;
        m_resultsTable->setItem(row, 1, new QTableWidgetItem(title));
        m_resultsTable->setItem(row, 2, new QTableWidgetItem(item.year));
        m_resultsTable->setItem(row, 3, new QTableWidgetItem(QString::number(item.downloads)));
        m_resultsTable->setItem(row, 4, new QTableWidgetItem(item.hashMatch ? QStringLiteral("✓")
                                                                           : QStringLiteral("—")));
        m_resultsTable->item(row, 0)->setData(Qt::UserRole, row);
    }

    const bool haveResults = !results.isEmpty();
    m_translateButton->setEnabled(haveResults);
    m_downloadButton->setEnabled(haveResults);
    if (!error.isEmpty()) {
        m_statusLabel->setText(tr("Error: %1").arg(error));
    } else if (haveResults) {
        m_statusLabel->setText(tr("%1 subtítulos encontrados.").arg(results.size()));
        if (m_resultsTable->rowCount() > 0) m_resultsTable->selectRow(0);
    } else {
        m_statusLabel->setText(tr("No se encontraron subtítulos."));
    }
}

void OnlineSubsDialog::startDownload(bool translate)
{
    const int row = m_resultsTable->currentRow();
    if (row < 0 || row >= m_results.size()) {
        m_statusLabel->setText(tr("Selecciona un resultado."));
        return;
    }

    const OnlineSubtitle &item = m_results.at(row);
    m_pendingLanguage = item.language.isEmpty() ? QStringLiteral("en") : item.language;
    m_pendingTranslate = translate;

    const QString dir = downloadDirectory();
    QDir().mkpath(dir);
    const QString base = cleanQuery(m_videoPath);
    const QString fileName = QStringLiteral("%1.%2.srt")
                                 .arg(base.isEmpty() ? QStringLiteral("subtitle") : base,
                                      m_pendingLanguage);
    const QString target = QDir(dir).absoluteFilePath(fileName);

    m_translateButton->setEnabled(false);
    m_downloadButton->setEnabled(false);
    m_statusLabel->setText(tr("Descargando el subtítulo…"));
    qInfo() << "[ONLINE-SUBS] download requested" << item.fileId << "->" << target;
    m_downloader->download(item, target);
}

void OnlineSubsDialog::downloadAndTranslate()
{
    startDownload(true);
}

void OnlineSubsDialog::downloadOnly()
{
    startDownload(false);
}

void OnlineSubsDialog::onDownloadFinished(const QString &filePath, const QString &error)
{
    m_translateButton->setEnabled(!m_results.isEmpty());
    m_downloadButton->setEnabled(!m_results.isEmpty());
    if (!error.isEmpty()) {
        m_statusLabel->setText(tr("Error: %1").arg(error));
        return;
    }
    m_statusLabel->setText(tr("Descarga completada: %1").arg(QFileInfo(filePath).fileName()));
    emit subtitleReady(filePath, m_videoPath, m_pendingLanguage, m_pendingTranslate);
    accept();
}
