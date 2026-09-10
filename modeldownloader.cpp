#include "modeldownloader.h"
#include "subtitletranslator.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileInfo>
#include <QDir>
#include <QCryptographicHash>
#include <QProcess>
#include <QDebug>

static const QString DEFAULT_REGISTRY_URL = "https://translatelocally.com/models.json";

ModelDownloader::ModelDownloader(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_registryUrl(DEFAULT_REGISTRY_URL)
    , m_downloadProcess(nullptr)
    , m_currentDownloadExpectedSize(0)
    , m_downloadProgressTimer(new QTimer(this))
{
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &ModelDownloader::onRegistryReply);
    connect(m_downloadProgressTimer, &QTimer::timeout,
            this, &ModelDownloader::onDownloadProgressTimer);
}

ModelDownloader::~ModelDownloader()
{
    cancelDownload();
}

void ModelDownloader::setModelsDirectory(const QString &dir)
{
    m_modelsDir = QDir(dir).absolutePath();
    QDir().mkpath(m_modelsDir);
}

QString ModelDownloader::modelsDirectory() const
{
    return m_modelsDir;
}

void ModelDownloader::setRegistryUrl(const QString &url)
{
    m_registryUrl = url;
}

QString ModelDownloader::registryUrl() const
{
    return m_registryUrl;
}

QList<RemoteModel> ModelDownloader::fetchAvailableModels()
{
    if (m_registryUrl.isEmpty()) {
        emit fetchError("Registry URL not set");
        return QList<RemoteModel>();
    }

    QNetworkRequest request{QUrl(m_registryUrl)};
    m_networkManager->get(request);

    return m_cachedModels;
}

QList<RemoteModel> ModelDownloader::installedModels()
{
    QList<RemoteModel> installed;
    QDir modelsDir(m_modelsDir);

    if (!modelsDir.exists()) return installed;

    QStringList dirs = modelsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &dir : dirs) {
        if (dir.contains('-') && (dir.contains("tiny") || dir.contains("base"))) {
            RemoteModel model;
            model.id = dir;
            model.isInstalled = true;

            QStringList parts = dir.split('-');
            if (parts.size() >= 3) {
                model.sourceLang = parts[0];
                model.targetLang = parts[1];
                model.type = parts[2];
            }

            installed.append(model);
        }
    }

    return installed;
}

bool ModelDownloader::isModelInstalled(const QString &modelId)
{
    return QFile::exists(m_modelsDir + "/" + modelId + "/config.yml") ||
           QFile::exists(m_modelsDir + "/" + modelId + "/config.intgemm8bitalpha.yml");
}

bool ModelDownloader::isModelInstalled(const QString &sourceLang, const QString &targetLang)
{
    return isModelInstalled(modelIdForPair(sourceLang, targetLang, "tiny")) ||
           isModelInstalled(modelIdForPair(sourceLang, targetLang, "base"));
}

void ModelDownloader::downloadModel(const QString &modelId)
{
    RemoteModel model;
    bool found = false;
    for (const RemoteModel &m : m_cachedModels) {
        if (m.id == modelId) { model = m; found = true; break; }
    }
    if (!found) { emit downloadError(modelId, "Model not found in registry"); return; }
    downloadModelWithInfo(model);
}

void ModelDownloader::downloadModelWithInfo(const RemoteModel &model)
{
    if (m_downloadProcess && m_downloadProcess->state() != QProcess::NotRunning) {
        emit downloadError(model.id, "Another download is in progress");
        return;
    }

    if (model.downloadUrl.isEmpty()) {
        emit downloadError(model.id, "No download URL available");
        return;
    }

    m_currentDownloadModelId = model.id;
    m_currentDownloadArchivePath = m_modelsDir + "/" + model.id + ".tar.gz";
    m_currentDownloadExpectedSize = model.sizeBytes;

    QFile::remove(m_currentDownloadArchivePath);

    qInfo() << "Downloading model" << model.id << "from" << model.downloadUrl;

    m_downloadProcess = new QProcess(this);
    connect(m_downloadProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &ModelDownloader::onDownloadProcessFinished);

    m_downloadProcess->start("curl", {
        "-L", "-f", "-s",
        "--connect-timeout", "30",
        "--max-time", "300",
        "-o", m_currentDownloadArchivePath,
        model.downloadUrl
    });

    m_downloadProgressTimer->start(500);
    emit downloadProgress(model.id, 0, model.sizeBytes);
}

void ModelDownloader::cancelDownload()
{
    m_downloadProgressTimer->stop();
    if (m_downloadProcess) {
        if (m_downloadProcess->state() != QProcess::NotRunning) {
            m_downloadProcess->kill();
            m_downloadProcess->waitForFinished(3000);
        }
        m_downloadProcess->deleteLater();
        m_downloadProcess = nullptr;
    }
    if (!m_currentDownloadArchivePath.isEmpty())
        QFile::remove(m_currentDownloadArchivePath);
    m_currentDownloadModelId.clear();
    m_currentDownloadArchivePath.clear();
}

void ModelDownloader::onDownloadProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_downloadProgressTimer->stop();
    QString modelId = m_currentDownloadModelId;
    QString archivePath = m_currentDownloadArchivePath;

    QString stderrOutput;
    if (m_downloadProcess) {
        stderrOutput = QString::fromUtf8(m_downloadProcess->readAllStandardError());
        m_downloadProcess->deleteLater();
        m_downloadProcess = nullptr;
    }

    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        emit downloadError(modelId, QString("curl failed (exit %1): %2").arg(exitCode).arg(stderrOutput.trimmed()));
        QFile::remove(archivePath);
        m_currentDownloadModelId.clear();
        m_currentDownloadArchivePath.clear();
        return;
    }

    QFileInfo fi(archivePath);
    if (!fi.exists() || fi.size() == 0) {
        emit downloadError(modelId, "Downloaded file is empty or missing");
        QFile::remove(archivePath);
        m_currentDownloadModelId.clear();
        m_currentDownloadArchivePath.clear();
        return;
    }

    qInfo() << "Downloaded" << modelId << ":" << fi.size() << "bytes";

    extractModelArchive(archivePath, modelId);
    QFile::remove(archivePath);

    QString extractDir = m_modelsDir + "/" + modelId;
    bool hasConfig = QFile::exists(extractDir + "/config.intgemm8bitalpha.yml") ||
                     QFile::exists(extractDir + "/config.yml") ||
                     QFile::exists(extractDir + "/model_info.json");
    if (!hasConfig) {
        emit downloadError(modelId, "Extraction failed: no config files in " + extractDir);
        m_currentDownloadModelId.clear();
        m_currentDownloadArchivePath.clear();
        return;
    }

    qInfo() << "Model" << modelId << "installed to" << extractDir;
    emit downloadFinished(modelId);
    m_currentDownloadModelId.clear();
    m_currentDownloadArchivePath.clear();
}

void ModelDownloader::onDownloadProgressTimer()
{
    if (m_currentDownloadArchivePath.isEmpty()) return;
    QFileInfo fi(m_currentDownloadArchivePath);
    qint64 currentSize = fi.exists() ? fi.size() : 0;
    emit downloadProgress(m_currentDownloadModelId, currentSize, m_currentDownloadExpectedSize);
}

void ModelDownloader::deleteModel(const QString &modelId)
{
    QString modelPath = m_modelsDir + "/" + modelId;

    if (!QFile::exists(modelPath)) {
        emit deleteError(modelId, "Model directory not found");
        return;
    }

    QDir dir(modelPath);
    if (dir.removeRecursively()) {
        emit deleteFinished(modelId);
    } else {
        emit deleteError(modelId, "Failed to remove model directory");
    }
}

QString ModelDownloader::modelIdForPair(const QString &sourceLang, const QString &targetLang,
                                        const QString &type)
{
    return QString("%1-%2-%3").arg(sourceLang, targetLang, type);
}

void ModelDownloader::onRegistryReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit fetchError("Failed to fetch model registry: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    QJsonObject rootObj = doc.isObject() ? doc.object() : QJsonObject();
    QJsonArray modelsArray = rootObj.value("models").toArray();
    if (modelsArray.isEmpty() && doc.isArray()) {
        modelsArray = doc.array();
    }
    m_cachedModels.clear();

    QStringList supportedSrc = SubtitleTranslator::supportedSourceLanguages();
    QStringList supportedTrg = SubtitleTranslator::supportedTargetLanguages();

    for (const QJsonValue &value : modelsArray) {
        QJsonObject obj = value.toObject();

        RemoteModel model;
        model.id = obj["shortName"].toString();
        model.name = obj["modelName"].toString();
        QJsonObject srcTags = obj["srcTags"].toObject();
        model.sourceLang = srcTags.keys().isEmpty() ? obj["src"].toString().toLower() : srcTags.keys().first().toLower();
        model.targetLang = obj["trgTag"].toString().toLower();
        if (model.targetLang.isEmpty()) model.targetLang = obj["trg"].toString().toLower();
        model.type = obj["type"].toString();
        model.downloadUrl = obj["url"].toString();
        model.checksum = obj["checksum"].toString();
        model.sizeBytes = obj["size"].toVariant().toLongLong();
        model.version = obj["version"].toInt();
        model.isInstalled = isModelInstalled(model.id);

        if (supportedSrc.contains(model.sourceLang) &&
            supportedTrg.contains(model.targetLang)) {
            m_cachedModels.append(model);
        }
    }

    emit modelsFetched(m_cachedModels);
    reply->deleteLater();
}

void ModelDownloader::extractModelArchive(const QString &archivePath, const QString &modelId)
{
    QString extractDir = m_modelsDir + "/" + modelId;

    // Clean up any previous failed extraction
    QDir extractDirObj(extractDir);
    if (extractDirObj.exists()) {
        extractDirObj.removeRecursively();
    }

    QDir().mkpath(extractDir);

    QProcess tar;
    tar.setWorkingDirectory(extractDir);
    tar.start("tar", {"-xzf", archivePath, "--strip-components=1"});

    if (!tar.waitForFinished(60000)) {
        qWarning() << "tar timed out:" << tar.errorString();
        return;
    }

    if (tar.exitCode() != 0) {
        QByteArray errOutput = tar.readAllStandardError();
        qWarning() << "tar failed (exit" << tar.exitCode() << "):" << errOutput;
        return;
    }

    // Verify extraction produced config files
    bool hasConfig = QFile::exists(extractDir + "/config.intgemm8bitalpha.yml") ||
                     QFile::exists(extractDir + "/config.yml") ||
                     QFile::exists(extractDir + "/model_info.json");
    if (!hasConfig) {
        qWarning() << "Extraction succeeded but no config files found in" << extractDir;
        qWarning() << "Contents:" << QDir(extractDir).entryList(QDir::Files);
    }
}

bool ModelDownloader::verifyChecksum(const QString &filePath, const QString &expectedChecksum)
{
    if (expectedChecksum.isEmpty()) return true;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return false;

    QCryptographicHash hash(QCryptographicHash::Sha256);
    if (!hash.addData(&file)) {
        file.close();
        return false;
    }

    file.close();
    QString actualChecksum = hash.result().toHex();
    return actualChecksum == expectedChecksum;
}
