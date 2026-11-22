#include "onlinesubsdialog.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFont>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QSettings>
#include <QSignalBlocker>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTimer>
#include <QVBoxLayout>

#include <algorithm>

OnlineSubsDialog::OnlineSubsDialog(SubtitleDownloader *downloader,
                                   const QStringList &videoPaths,
                                   const QString &preferredLanguage,
                                   QWidget *parent)
    : QDialog(parent)
    , m_downloader(downloader)
{
    m_preferredLanguage = preferredLanguage.trimmed().isEmpty()
                              ? QStringLiteral("es")
                              : normalizedLanguage(preferredLanguage);
    for (const QString &path : videoPaths) {
        const QString trimmed = path.trimmed();
        if (trimmed.isEmpty()) continue;
        FileState state;
        state.path = trimmed;
        state.query = cleanQuery(trimmed);
        state.status = tr("Pendiente");
        m_files.append(state);
    }

    if (m_files.size() == 1)
        setWindowTitle(tr("Subtítulos online — %1").arg(QFileInfo(m_files.first().path).fileName()));
    else
        setWindowTitle(tr("Subtítulos online — %1 archivos").arg(m_files.size()));
    setMinimumSize(860, 540);
    resize(960, 640);
    setModal(true);

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setSpacing(8);

    QHBoxLayout *topRow = new QHBoxLayout();
    m_filesLabel = new QLabel();
    if (m_files.size() == 1)
        m_filesLabel->setText(QFileInfo(m_files.first().path).fileName());
    else
        m_filesLabel->setText(tr("%1 archivos seleccionados").arg(m_files.size()));
    m_filesLabel->setProperty("labelColor", "accent");
    topRow->addWidget(m_filesLabel, 1);
    topRow->addWidget(new QLabel(tr("Idioma:")));
    m_languageCombo = new QComboBox();
    QStandardItemModel *languagesModel = new QStandardItemModel(m_languageCombo);
    m_languageCombo->setModel(languagesModel);

    QStandardItem *allItem = new QStandardItem(tr("Todos"));
    allItem->setData(QStringLiteral("all"), Qt::UserRole);
    languagesModel->appendRow(allItem);

    // El idioma preferido va primero (y en negrita) para que destaque.
    QStringList languageCodes = {QStringLiteral("en"), QStringLiteral("es"),
                                 QStringLiteral("fr"), QStringLiteral("pt"),
                                 QStringLiteral("de"), QStringLiteral("it")};
    languageCodes.removeAll(m_preferredLanguage);
    languageCodes.prepend(m_preferredLanguage);
    int preferredIndex = 1;   // justo después de «Todos»
    for (int i = 0; i < languageCodes.size(); ++i) {
        const QString &code = languageCodes.at(i);
        const bool preferred = (code == m_preferredLanguage);
        const QString name = languageDisplayName(code);
        QStandardItem *item = new QStandardItem(
            preferred ? tr("%1 (preferido)").arg(name) : name);
        if (preferred) {
            QFont boldFont = item->font();
            boldFont.setBold(true);
            item->setFont(boldFont);
            preferredIndex = languagesModel->rowCount();
        }
        item->setData(code, Qt::UserRole);
        languagesModel->appendRow(item);
    }
    m_languageCombo->setCurrentIndex(preferredIndex);
    topRow->addWidget(m_languageCombo);
    root->addLayout(topRow);

    // ── Archivos del lote: casilla para marcar, consulta editable por archivo ──
    m_filesTable = new QTableWidget(0, 5, this);
    m_filesTable->setHorizontalHeaderLabels({
        QString(), tr("Archivo"), tr("Consulta"), tr("Estado"), tr("Resultados")
    });
    m_filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_filesTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_filesTable->setEditTriggers(QAbstractItemView::DoubleClicked |
                                  QAbstractItemView::EditKeyPressed);
    m_filesTable->verticalHeader()->setVisible(false);
    m_filesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_filesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_filesTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_filesTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_filesTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    {
        QSignalBlocker blocker(m_filesTable);
        for (int row = 0; row < m_files.size(); ++row) {
            const FileState &state = m_files.at(row);
            m_filesTable->insertRow(row);

            QTableWidgetItem *check = new QTableWidgetItem();
            check->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            check->setCheckState(Qt::Checked);   // marcados por defecto
            m_filesTable->setItem(row, 0, check);

            QTableWidgetItem *file = new QTableWidgetItem(QFileInfo(state.path).fileName());
            file->setFlags(file->flags() & ~Qt::ItemIsEditable);
            file->setData(Qt::UserRole, state.path);
            m_filesTable->setItem(row, 1, file);

            m_filesTable->setItem(row, 2, new QTableWidgetItem(state.query));

            QTableWidgetItem *status = new QTableWidgetItem(state.status);
            status->setFlags(status->flags() & ~Qt::ItemIsEditable);
            m_filesTable->setItem(row, 3, status);

            QTableWidgetItem *count = new QTableWidgetItem(QString::number(state.resultCount));
            count->setFlags(count->flags() & ~Qt::ItemIsEditable);
            m_filesTable->setItem(row, 4, count);
        }
    }
    connect(m_filesTable, &QTableWidget::itemChanged, this,
            [this](QTableWidgetItem *item) {
        if (!item || item->column() != 2) return;
        const int row = item->row();
        if (row >= 0 && row < m_files.size())
            m_files[row].query = item->text();
    });
    root->addWidget(m_filesTable, 1);

    QHBoxLayout *searchRow = new QHBoxLayout();
    searchRow->addStretch(1);
    m_searchSelectedButton = new QPushButton(tr("Buscar en seleccionados"));
    m_searchAllButton = new QPushButton(tr("Buscar en todos"));
    connect(m_searchSelectedButton, &QPushButton::clicked,
            this, &OnlineSubsDialog::searchSelected);
    connect(m_searchAllButton, &QPushButton::clicked,
            this, &OnlineSubsDialog::searchAll);
    searchRow->addWidget(m_searchSelectedButton);
    searchRow->addWidget(m_searchAllButton);
    root->addLayout(searchRow);

    // ── Resultados de todos los vídeos, con casilla por fila ──
    m_resultsTable = new QTableWidget(0, 8, this);
    m_resultsTable->setHorizontalHeaderLabels({
        QString(), tr("Archivo"), tr("Idioma"), tr("Título"), tr("Año"),
        tr("Descargas"), tr("Hash"), tr("Estado")
    });
    m_resultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_resultsTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_resultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_resultsTable->verticalHeader()->setVisible(false);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    for (int col = 4; col < 8; ++col)
        m_resultsTable->horizontalHeader()->setSectionResizeMode(col, QHeaderView::ResizeToContents);
    connect(m_resultsTable, &QTableWidget::itemChanged, this,
            [this](QTableWidgetItem *item) {
        if (!item || item->column() != 0) return;
        const int row = item->row();
        if (row >= 0 && row < m_results.size()) {
            m_results[row].selected = (item->checkState() == Qt::Checked);
            updateDownloadButtons();
        }
    });
    root->addWidget(m_resultsTable, 2);

    m_statusLabel = new QLabel();
    m_statusLabel->setWordWrap(true);
    m_statusLabel->setProperty("labelColor", "muted");
    root->addWidget(m_statusLabel);

    m_triangularHintLabel = new QLabel();
    m_triangularHintLabel->setWordWrap(true);
    m_triangularHintLabel->setProperty("labelColor", "accent");
    m_triangularHintLabel->setVisible(false);
    root->addWidget(m_triangularHintLabel);

    QHBoxLayout *buttons = new QHBoxLayout();
    buttons->addStretch(1);
    m_translateButton = new QPushButton(tr("Descargar y traducir seleccionados"));
    m_downloadButton = new QPushButton(tr("Solo descargar seleccionados"));
    m_closeButton = new QPushButton(tr("Cerrar"));
    buttons->addWidget(m_translateButton);
    buttons->addWidget(m_downloadButton);
    buttons->addWidget(m_closeButton);
    root->addLayout(buttons);

    connect(m_translateButton, &QPushButton::clicked,
            this, &OnlineSubsDialog::downloadAndTranslate);
    connect(m_downloadButton, &QPushButton::clicked,
            this, &OnlineSubsDialog::downloadOnly);
    connect(m_closeButton, &QPushButton::clicked, this, [this]() { reject(); });
    connect(m_languageCombo, &QComboBox::currentIndexChanged,
            this, &OnlineSubsDialog::clearTriangularHint);

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

    if (!m_downloader || !m_downloader->hasCredentials())
        m_statusLabel->setText(tr("Configura la clave API, el usuario y la contraseña en Ajustes → Subtítulos online."));
    setBusy(false);   // la búsqueda siempre es manual; sin credenciales queda deshabilitada
}

void OnlineSubsDialog::reject()
{
    if (m_busy && m_downloader) m_downloader->cancel();
    QDialog::reject();
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

QString OnlineSubsDialog::downloadDirectory(const QString &videoPath) const
{
    QSettings settings("LocalApp", "FFmpegExtractor");
    QString dir = settings.value("outputDir").toString().trimmed();
    if (dir.isEmpty())
        dir = QFileInfo(videoPath).absolutePath();
    return QDir(dir).absoluteFilePath(QStringLiteral("Descargados"));
}

int OnlineSubsDialog::fileRowFor(const QString &videoPath) const
{
    for (int i = 0; i < m_files.size(); ++i) {
        if (m_files.at(i).path == videoPath) return i;
    }
    return -1;
}

QString OnlineSubsDialog::languageFor(const ResultState &result) const
{
    return result.sub.language.isEmpty() ? QStringLiteral("en") : result.sub.language;
}

QString OnlineSubsDialog::normalizedLanguage(const QString &language)
{
    QString code = language.trimmed().toLower();
    if (code.size() > 2) code = code.left(2);
    if (code.isEmpty()) code = QStringLiteral("en");   // coherente con languageFor()
    return code;
}

QString OnlineSubsDialog::languageDisplayName(const QString &code)
{
    if (code == QStringLiteral("en")) return tr("Inglés");
    if (code == QStringLiteral("es")) return tr("Español");
    if (code == QStringLiteral("fr")) return tr("Francés");
    if (code == QStringLiteral("pt")) return tr("Portugués");
    if (code == QStringLiteral("de")) return tr("Alemán");
    if (code == QStringLiteral("it")) return tr("Italiano");
    return code.toUpper();   // idioma añadido dinámicamente: mostrar su código
}

bool OnlineSubsDialog::fileHasPreferredResults(const QString &videoPath) const
{
    for (const ResultState &result : m_results) {
        if (result.videoPath != videoPath) continue;
        if (normalizedLanguage(result.sub.language) == m_preferredLanguage)
            return true;
    }
    return false;
}

int OnlineSubsDialog::bestOtherLanguageRow(const QString &videoPath) const
{
    int bestRow = -1;
    for (int i = 0; i < m_results.size(); ++i) {
        const ResultState &result = m_results.at(i);
        if (result.videoPath != videoPath) continue;
        if (normalizedLanguage(result.sub.language) == m_preferredLanguage) continue;
        if (bestRow < 0 || result.sub.downloads > m_results.at(bestRow).sub.downloads)
            bestRow = i;
    }
    return bestRow;
}

void OnlineSubsDialog::setFileStatus(const QString &videoPath, const QString &status,
                                     int resultCount)
{
    const int row = fileRowFor(videoPath);
    if (row < 0) return;
    m_files[row].status = status;
    if (resultCount >= 0) m_files[row].resultCount = resultCount;

    QSignalBlocker blocker(m_filesTable);
    if (QTableWidgetItem *item = m_filesTable->item(row, 3))
        item->setText(status);
    if (resultCount >= 0) {
        if (QTableWidgetItem *item = m_filesTable->item(row, 4))
            item->setText(QString::number(resultCount));
    }
}

void OnlineSubsDialog::setResultStatus(int row, const QString &status)
{
    if (row < 0 || row >= m_results.size()) return;
    m_results[row].status = status;
    QSignalBlocker blocker(m_resultsTable);
    if (QTableWidgetItem *item = m_resultsTable->item(row, 7))
        item->setText(status);
}

void OnlineSubsDialog::updateDownloadButtons() const
{
    bool anySelected = false;
    for (const ResultState &result : m_results) {
        if (result.selected) { anySelected = true; break; }
    }
    const bool enabled = !m_busy && anySelected;
    m_translateButton->setEnabled(enabled);
    m_downloadButton->setEnabled(enabled);
}

void OnlineSubsDialog::setBusy(bool busy)
{
    m_busy = busy;
    const bool ready = m_downloader && m_downloader->hasCredentials() && !m_files.isEmpty();
    m_searchSelectedButton->setEnabled(!busy && ready);
    m_searchAllButton->setEnabled(!busy && ready);
    updateDownloadButtons();
}

void OnlineSubsDialog::searchSelected()
{
    startSearch(true);
}

void OnlineSubsDialog::searchAll()
{
    startSearch(false);
}

void OnlineSubsDialog::startSearch(bool selectedOnly)
{
    if (!m_downloader || m_busy) return;

    clearTriangularHint();
    m_searchQueue.clear();
    for (int row = 0; row < m_files.size(); ++row) {
        if (selectedOnly) {
            QTableWidgetItem *check = m_filesTable->item(row, 0);
            if (!check || check->checkState() != Qt::Checked) continue;
        }
        const FileState &state = m_files.at(row);
        if (state.query.trimmed().isEmpty()) {
            setFileStatus(state.path, tr("Introduce un título para buscar subtítulos."), 0);
            continue;
        }
        removeResultsFor(state.path);
        setFileStatus(state.path, tr("Buscando subtítulos…"), 0);
        m_searchQueue.append(state.path);
    }

    if (m_searchQueue.isEmpty()) {
        m_statusLabel->setText(tr("No hay archivos marcados."));
        return;
    }

    m_nextSearch = 0;
    m_statusLabel->setText(tr("Buscando subtítulos…"));
    setBusy(true);
    startNextSearch();
}

void OnlineSubsDialog::startNextSearch()
{
    if (m_nextSearch >= m_searchQueue.size()) {
        finishSearch();
        return;
    }

    m_currentSearchPath = m_searchQueue.at(m_nextSearch);
    const int row = fileRowFor(m_currentSearchPath);
    const QString query = (row >= 0) ? m_files.at(row).query : QString();
    m_downloader->search(m_currentSearchPath, query,
                         m_languageCombo->currentData().toString());
}

void OnlineSubsDialog::onSearchFinished(const QString &videoPath,
                                        const QList<OnlineSubtitle> &results,
                                        const QString &error)
{
    if (videoPath != m_currentSearchPath || m_searchQueue.isEmpty()) return;

    if (!error.isEmpty()) {
        setFileStatus(videoPath, tr("Error: %1").arg(error), 0);
    } else if (results.isEmpty()) {
        setFileStatus(videoPath, tr("No se encontraron subtítulos."), 0);
    } else {
        addResultsFor(videoPath, results);
    }

    ++m_nextSearch;
    // El downloader solo admite una petición activa: encadenamos la siguiente.
    QTimer::singleShot(0, this, &OnlineSubsDialog::startNextSearch);
}

void OnlineSubsDialog::finishSearch()
{
    m_searchQueue.clear();
    m_currentSearchPath.clear();
    setBusy(false);
    m_statusLabel->setText(tr("Búsqueda finalizada: %1 resultados.").arg(m_results.size()));

    // Solo avisamos de archivos que tienen resultados, pero ninguno en el idioma preferido.
    QStringList affectedFiles;
    for (const FileState &file : m_files) {
        bool hasResults = false;
        for (const ResultState &result : m_results) {
            if (result.videoPath != file.path) continue;
            hasResults = true;
            if (normalizedLanguage(result.sub.language) == m_preferredLanguage) break;
        }
        if (hasResults && !fileHasPreferredResults(file.path))
            affectedFiles.append(file.path);
    }
    updateTriangularHint(affectedFiles);
}

QString OnlineSubsDialog::exampleOtherLanguage() const
{
    int bestRow = -1;
    for (int i = 0; i < m_results.size(); ++i) {
        const ResultState &result = m_results.at(i);
        if (!m_triangularFiles.contains(result.videoPath)) continue;
        if (normalizedLanguage(result.sub.language) == m_preferredLanguage) continue;
        if (bestRow < 0 || result.sub.downloads > m_results.at(bestRow).sub.downloads)
            bestRow = i;
    }
    return languageDisplayName(
        bestRow >= 0 ? normalizedLanguage(m_results.at(bestRow).sub.language)
                     : QStringLiteral("en"));
}

void OnlineSubsDialog::updateTriangularHint(const QStringList &affectedFiles)
{
    m_triangularFiles = affectedFiles;
    if (m_triangularFiles.isEmpty()) {
        clearTriangularHint();
        return;
    }

    m_triangularHintLabel->setText(
        tr("No hay subtítulos en %1 para uno o más archivos. Puedes descargar el subtítulo disponible (p. ej. %2) y traducirlo a %1 mediante traducción triangular.")
            .arg(languageDisplayName(m_preferredLanguage), exampleOtherLanguage())
        + QStringLiteral(" ")
        + tr("Afecta a %1 archivo(s).").arg(m_triangularFiles.size()));
    m_triangularHintLabel->setVisible(true);
}

void OnlineSubsDialog::clearTriangularHint()
{
    m_triangularFiles.clear();
    m_triangularHintLabel->clear();
    m_triangularHintLabel->setVisible(false);
}

void OnlineSubsDialog::ensureTriangularSelection(const QStringList &files)
{
    QSignalBlocker blocker(m_resultsTable);
    for (const QString &videoPath : files) {
        bool anySelected = false;
        for (const ResultState &result : m_results) {
            if (result.videoPath == videoPath && result.selected) {
                anySelected = true;
                break;
            }
        }
        if (anySelected) continue;   // respetar lo que el usuario haya marcado

        const int row = bestOtherLanguageRow(videoPath);
        if (row < 0) continue;
        m_results[row].selected = true;
        if (QTableWidgetItem *check = m_resultsTable->item(row, 0))
            check->setCheckState(Qt::Checked);
    }
    updateDownloadButtons();
}

void OnlineSubsDialog::removeResultsFor(const QString &videoPath)
{
    QSignalBlocker blocker(m_resultsTable);
    for (int i = m_results.size() - 1; i >= 0; --i) {
        if (m_results.at(i).videoPath == videoPath) {
            m_results.removeAt(i);
            m_resultsTable->removeRow(i);
        }
    }
    updateDownloadButtons();
}

void OnlineSubsDialog::addResultsFor(const QString &videoPath,
                                     const QList<OnlineSubtitle> &results)
{
    removeResultsFor(videoPath);
    const QString fileName = QFileInfo(videoPath).fileName();

    QList<OnlineSubtitle> sortedResults = results;
    std::stable_sort(sortedResults.begin(), sortedResults.end(),
                     [this](const OnlineSubtitle &a, const OnlineSubtitle &b) {
        const QString langA = normalizedLanguage(a.language);
        const QString langB = normalizedLanguage(b.language);
        const bool preferredA = (langA == m_preferredLanguage);
        const bool preferredB = (langB == m_preferredLanguage);
        if (preferredA != preferredB) return preferredA;
        if (a.downloads != b.downloads) return a.downloads > b.downloads;
        return langA < langB;
    });

    QSignalBlocker blocker(m_resultsTable);
    bool firstForThisVideo = true;
    for (const OnlineSubtitle &sub : sortedResults) {
        ResultState state;
        state.videoPath = videoPath;
        state.sub = sub;
        // Solo el primer resultado de cada vídeo viene marcado: descargar 50
        // subtítulos por archivo por defecto llenaría la cola y agotaría el
        // límite de peticiones de OpenSubtitles.
        state.selected = firstForThisVideo;
        firstForThisVideo = false;
        state.status = tr("Pendiente");
        m_results.append(state);

        const int row = m_resultsTable->rowCount();
        m_resultsTable->insertRow(row);

        QTableWidgetItem *check = new QTableWidgetItem();
        check->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        check->setCheckState(state.selected ? Qt::Checked : Qt::Unchecked);
        m_resultsTable->setItem(row, 0, check);

        const QString title = sub.release.isEmpty() ? sub.uploader : sub.release;
        m_resultsTable->setItem(row, 1, new QTableWidgetItem(fileName));
        m_resultsTable->setItem(row, 2, new QTableWidgetItem(sub.language.toUpper()));
        m_resultsTable->setItem(row, 3, new QTableWidgetItem(title));
        m_resultsTable->setItem(row, 4, new QTableWidgetItem(sub.year));
        m_resultsTable->setItem(row, 5, new QTableWidgetItem(QString::number(sub.downloads)));
        m_resultsTable->setItem(row, 6, new QTableWidgetItem(sub.hashMatch
                                                                 ? QStringLiteral("✓")
                                                                 : QStringLiteral("—")));
        m_resultsTable->setItem(row, 7, new QTableWidgetItem(state.status));
    }

    setFileStatus(videoPath, tr("%1 resultados").arg(results.size()), results.size());
    updateDownloadButtons();
}

void OnlineSubsDialog::downloadAndTranslate()
{
    if (m_busy) return;

    if (!m_triangularFiles.isEmpty()) {
        const QString message = tr(
            "No hay subtítulos en %1 para uno o más archivos. Se descargará el mejor "
            "subtítulo disponible (p. ej. %2) y se traducirá a %1 mediante traducción "
            "triangular. ¿Continuar?")
                .arg(languageDisplayName(m_preferredLanguage), exampleOtherLanguage());
        if (QMessageBox::question(this, tr("Traducción triangular"), message,
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::Yes) != QMessageBox::Yes)
            return;
        ensureTriangularSelection(m_triangularFiles);
    }

    startDownload(true);
}

void OnlineSubsDialog::downloadOnly()
{
    startDownload(false);
}

void OnlineSubsDialog::startDownload(bool translate)
{
    if (!m_downloader || m_busy) return;

    m_downloadQueue.clear();
    for (int row = 0; row < m_results.size(); ++row) {
        if (m_results.at(row).selected) m_downloadQueue.append(row);
    }
    if (m_downloadQueue.isEmpty()) {
        m_statusLabel->setText(tr("Marca al menos un resultado."));
        return;
    }

    m_pendingTranslate = translate;
    m_downloadedCount = 0;
    m_failedCount = 0;
    m_nextDownload = 0;
    m_statusLabel->setText(tr("Descargando el subtítulo…"));
    setBusy(true);
    startNextDownload();
}

void OnlineSubsDialog::startNextDownload()
{
    if (m_nextDownload >= m_downloadQueue.size()) {
        finishDownload();
        return;
    }

    const int row = m_downloadQueue.at(m_nextDownload);
    if (row < 0 || row >= m_results.size()) {
        ++m_nextDownload;
        QTimer::singleShot(0, this, &OnlineSubsDialog::startNextDownload);
        return;
    }

    const ResultState result = m_results.at(row);
    const QString lang = languageFor(result);
    const QString dir = downloadDirectory(result.videoPath);
    QDir().mkpath(dir);
    const QString base = cleanQuery(result.videoPath);
    const QString fileName = QStringLiteral("%1.%2.srt")
                                 .arg(base.isEmpty() ? QStringLiteral("subtitle") : base,
                                      lang);
    const QString target = QDir(dir).absoluteFilePath(fileName);

    setResultStatus(row, tr("Descargando el subtítulo…"));
    qInfo() << "[ONLINE-SUBS] download requested" << result.sub.fileId << "->" << target;
    m_downloader->download(result.sub, target, result.videoPath);
}

void OnlineSubsDialog::onDownloadFinished(const QString &videoPath,
                                          const QString &filePath,
                                          const QString &error)
{
    if (m_downloadQueue.isEmpty() || m_nextDownload >= m_downloadQueue.size()) return;
    const int row = m_downloadQueue.at(m_nextDownload);
    if (row < 0 || row >= m_results.size()) return;

    const ResultState result = m_results.at(row);
    const QString downloadedFor = videoPath.isEmpty() ? result.videoPath : videoPath;
    if (!error.isEmpty()) {
        ++m_failedCount;
        setResultStatus(row, tr("Error: %1").arg(error));
        setFileStatus(downloadedFor, tr("Error: %1").arg(error));
    } else {
        ++m_downloadedCount;
        setResultStatus(row, tr("Descarga completada: %1").arg(QFileInfo(filePath).fileName()));
        emit subtitleReady(filePath, downloadedFor, languageFor(result), m_pendingTranslate);
    }

    ++m_nextDownload;
    QTimer::singleShot(0, this, &OnlineSubsDialog::startNextDownload);
}

void OnlineSubsDialog::finishDownload()
{
    m_downloadQueue.clear();
    setBusy(false);
    m_statusLabel->setText(tr("%1 descargados, %2 fallidos")
                               .arg(m_downloadedCount).arg(m_failedCount));
}
