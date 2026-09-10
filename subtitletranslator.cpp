#include "subtitletranslator.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QSettings>

// ─────────────────────────────────────────────────────────────────────────────
// Construction / Destruction
// ─────────────────────────────────────────────────────────────────────────────
SubtitleTranslator::SubtitleTranslator(QObject *parent)
    : QObject(parent)
    , m_process(nullptr)
    , m_cancelled(false)
    , m_batchIndex(0)
    , m_currentLineTotal(0)
    , m_blocking(false)
{
    m_bergamotBinary = findBergamotBinary();
}

SubtitleTranslator::~SubtitleTranslator()
{
    cancel();
}

// ─────────────────────────────────────────────────────────────────────────────
// Configuration
// ─────────────────────────────────────────────────────────────────────────────
void SubtitleTranslator::setModelDirectory(const QString &dir)
{
    m_modelDir = dir;
    QDir().mkpath(dir);
}

QString SubtitleTranslator::modelDirectory() const
{
    return m_modelDir;
}

void SubtitleTranslator::setBergamotBinary(const QString &path)
{
    m_bergamotBinary = path;
}

QString SubtitleTranslator::bergamotBinary() const
{
    return m_bergamotBinary;
}

// ─────────────────────────────────────────────────────────────────────────────
// Available languages
// ─────────────────────────────────────────────────────────────────────────────
QStringList SubtitleTranslator::supportedSourceLanguages()
{
    return {"en", "es", "fr", "pt"};
}

QStringList SubtitleTranslator::supportedTargetLanguages()
{
    return {"en", "es", "fr", "pt"};
}

QString SubtitleTranslator::languageName(const QString &code)
{
    static QMap<QString, QString> names = {
        {"en", "English"},
        {"es", "Español"},
        {"fr", "Français"},
        {"pt", "Português"}
    };
    return names.value(code, code);
}

// ─────────────────────────────────────────────────────────────────────────────
// Model management
// ─────────────────────────────────────────────────────────────────────────────
QList<LanguagePair> SubtitleTranslator::availableModels()
{
    QList<LanguagePair> models;
    QStringList sources = supportedSourceLanguages();
    QStringList targets = supportedTargetLanguages();

    for (const QString &src : sources) {
        for (const QString &trg : targets) {
            if (src == trg) continue;

            LanguagePair pair;
            pair.sourceCode = src;
            pair.targetCode = trg;
            pair.modelCode = modelPathForPair(src, trg);
            pair.displayName = QString("%1 → %2")
                .arg(languageName(src))
                .arg(languageName(trg));
            pair.isInstalled = isModelInstalled(src, trg);
            pair.sizeBytes = 0;
            models.append(pair);
        }
    }
    return models;
}

QList<LanguagePair> SubtitleTranslator::installedModels()
{
    QList<LanguagePair> all = availableModels();
    QList<LanguagePair> installed;
    for (const LanguagePair &pair : all) {
        if (pair.isInstalled) installed.append(pair);
    }
    return installed;
}

bool SubtitleTranslator::isModelInstalled(const QString &sourceLang, const QString &targetLang)
{
    QString modelPath = modelPathForPair(sourceLang, targetLang);
    // Check for model config file (bergamot format)
    return QFile::exists(modelPath + "/config.intgemm8bitalpha.yml") ||
           QFile::exists(modelPath + "/config.yml");
}

QStringList SubtitleTranslator::findTranslationPath(const QString &sourceLang, const QString &targetLang)
{
    // Direct model available
    if (isModelInstalled(sourceLang, targetLang)) {
        return {sourceLang, targetLang};
    }

    // Try triangular translation through an intermediate language
    QStringList intermediates = supportedSourceLanguages();
    intermediates.removeAll(sourceLang);
    intermediates.removeAll(targetLang);

    for (const QString &mid : intermediates) {
        if (isModelInstalled(sourceLang, mid) && isModelInstalled(mid, targetLang)) {
            qDebug() << "[TRANSLATE] Found triangular path:" << sourceLang << "→" << mid << "→" << targetLang;
            return {sourceLang, mid, targetLang};
        }
    }

    // No path found
    return {};
}

// ─────────────────────────────────────────────────────────────────────────────
// Translation
// ─────────────────────────────────────────────────────────────────────────────
bool SubtitleTranslator::translateFile(const QString &inputPath,
                                       const QString &outputPath,
                                       const QString &sourceLang,
                                       const QString &targetLang)
{
    qDebug() << "[TRANSLATE] translateFile:" << inputPath << "→" << outputPath << " (" << sourceLang << "→" << targetLang << ")";

    // Parse input file
    QList<SubtitleCue> cues = SubtitleParser::parseFile(inputPath);
    if (cues.isEmpty()) {
        qDebug() << "[TRANSLATE] FAIL: no cues parsed from" << inputPath;
        emit translationError("No se pudieron leer los subtítulos del archivo: " + inputPath);
        return false;
    }
    qDebug() << "[TRANSLATE] Parsed" << cues.size() << "cues";

    // Check if bergamot binary exists
    qDebug() << "[TRANSLATE] Bergamot binary:" << m_bergamotBinary << "exists:" << QFile::exists(m_bergamotBinary);
    if (m_bergamotBinary.isEmpty() || !QFile::exists(m_bergamotBinary)) {
        qDebug() << "[TRANSLATE] FAIL: bergamot binary not found";
        emit translationError("No se encontró el motor de traducción (bergamot).\n"
                              "Asegúrese de que está instalado correctamente.\n"
                              "Buscado en: " + m_bergamotBinary);
        return false;
    }

    // Check translation path (direct or triangular)
    QStringList path = findTranslationPath(sourceLang, targetLang);
    if (path.isEmpty()) {
        qDebug() << "[TRANSLATE] FAIL: no translation path found for" << sourceLang << "→" << targetLang;
        emit translationError(QString("No se encontró un modelo de traducción para %1→%2.\n"
                                      "Ni modelo directo ni ruta triangular disponible.\n"
                                      "Modelos instalados disponibles para: en, es, fr, pt.")
                                  .arg(languageName(sourceLang))
                                  .arg(languageName(targetLang)));
        return false;
    }

    if (path.size() > 2) {
        qDebug() << "[TRANSLATE] Using triangular path:" << path;
        emit triangularTranslationUsed(path);
    }

    // Direct translation (single hop) — async (batch mode compatible)
    if (path.size() == 2) {
        m_blocking = false;
        return translateFileDirect(inputPath, outputPath, sourceLang, targetLang);
    }

    // Triangular translation (multi-hop): write intermediate files to temp, chain translations
    QFileInfo outInfo(outputPath);
    QString tempDir = outInfo.absolutePath();
    QString baseName = outInfo.completeBaseName();

    QString currentInput = inputPath;
    for (int i = 0; i < path.size() - 1; ++i) {
        QString src = path[i];
        QString trg = path[i + 1];
        bool isLastHop = (i == path.size() - 2);

        QString intermediateOutput;
        if (isLastHop) {
            intermediateOutput = outputPath;
        } else {
            intermediateOutput = tempDir + "/" + baseName + "." + trg + ".tmp.srt";
        }

        qDebug() << "[TRANSLATE] Hop" << i + 1 << ":" << src << "→" << trg
                 << "  input:" << currentInput << "  output:" << intermediateOutput;

        // Each hop must be blocking — wait for bergamot to finish before next hop
        m_blocking = true;
        bool ok = translateFileDirect(currentInput, intermediateOutput, src, trg);
        if (!ok) {
            qDebug() << "[TRANSLATE] FAIL: hop" << i + 1 << "failed (" << src << "→" << trg << ")";
            // Clean up temp file if it was created
            if (!isLastHop && QFile::exists(intermediateOutput)) {
                QFile::remove(intermediateOutput);
            }
            m_blocking = false;
            return false;
        }

        // Clean up intermediate temp file from previous hop (not the input for next hop)
        if (i > 0 && QFile::exists(currentInput)) {
            QFile::remove(currentInput);
        }
        currentInput = intermediateOutput;
    }

    m_blocking = false;

    // Notify completion (batch flow needs this to advance)
    emit fileTranslated(inputPath, outputPath);
    return true;
}

bool SubtitleTranslator::translateFileDirect(const QString &inputPath,
                                             const QString &outputPath,
                                             const QString &sourceLang,
                                             const QString &targetLang)
{
    qDebug() << "[TRANSLATE] translateFileDirect:" << inputPath << "→" << outputPath;

    // Parse input file
    QList<SubtitleCue> cues = SubtitleParser::parseFile(inputPath);
    if (cues.isEmpty()) {
        qDebug() << "[TRANSLATE] FAIL: no cues parsed from" << inputPath;
        emit translationError("No se pudieron leer los subtítulos del archivo: " + inputPath);
        return false;
    }
    qDebug() << "[TRANSLATE] Parsed" << cues.size() << "cues";

    // Check if bergamot binary exists
    qDebug() << "[TRANSLATE] Bergamot binary:" << m_bergamotBinary << "exists:" << QFile::exists(m_bergamotBinary);
    if (m_bergamotBinary.isEmpty() || !QFile::exists(m_bergamotBinary)) {
        qDebug() << "[TRANSLATE] FAIL: bergamot binary not found";
        emit translationError("No se encontró el motor de traducción (bergamot).\n"
                              "Asegúrese de que está instalado correctamente.\n"
                              "Buscado en: " + m_bergamotBinary);
        return false;
    }

    // Check if model is installed
    QString modelPath = modelPathForPair(sourceLang, targetLang);
    bool modelOk = isModelInstalled(sourceLang, targetLang);
    qDebug() << "[TRANSLATE] Model path:" << modelPath << "installed:" << modelOk;
    if (!modelOk) {
        qDebug() << "[TRANSLATE] FAIL: model not installed";
        emit translationError(QString("El modelo de traducción %1→%2 no está instalado.\n"
                                      "Directorio buscado: %3")
                                  .arg(languageName(sourceLang))
                                  .arg(languageName(targetLang))
                                  .arg(modelPath));
        return false;
    }

    // Store state for translation
    m_currentInputPath = inputPath;
    m_currentOutputPath = outputPath;
    m_currentCues = cues;
    m_translatableIndices.clear();
    m_currentLineTotal = cues.size();
    m_batchSourceLang = sourceLang;
    m_batchTargetLang = targetLang;

    // Translate entire file content at once
    translateFileContent();

    // In blocking mode, wait for the process to finish.
    // Disconnect the auto-handler so we can process output ourselves.
    if (m_blocking && m_process) {
        disconnect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                   this, &SubtitleTranslator::onProcessFinished);
        m_process->waitForFinished(600000);  // 10 min timeout

        // Manually process the output
        if (m_process && m_process->state() == QProcess::NotRunning) {
            int exitCode = m_process->exitCode();
            QProcess::ExitStatus exitStatus = m_process->exitStatus();
            QString output = QString::fromLocal8Bit(m_process->readAllStandardOutput());
            QStringList translatedLines = output.split('\n', Qt::SkipEmptyParts);

            qDebug() << "[TRANSLATE] blocking hop finished: exitCode=" << exitCode
                     << "output lines:" << translatedLines.size()
                     << "cues:" << m_currentCues.size();

            // Assign translations back to cues
            for (int i = 0; i < m_translatableIndices.size() && i < translatedLines.size(); ++i) {
                int cueIdx = m_translatableIndices[i];
                QString translated = translatedLines[i].trimmed();
                if (!translated.isEmpty()) {
                    m_currentCues[cueIdx].text = translated;
                }
            }

            // Write output file
            if (exitStatus == QProcess::NormalExit && exitCode == 0) {
                SubtitleParser::writeSrt(m_currentOutputPath, m_currentCues);
                qDebug() << "[TRANSLATE] Blocking hop file written:" << m_currentOutputPath;
            } else {
                QString stderrOutput = QString::fromLocal8Bit(m_process->readAllStandardError());
                qWarning() << "Bergamot error (blocking hop):" << stderrOutput;
                // Write file with original text on error
                SubtitleParser::writeSrt(m_currentOutputPath, m_currentCues);
            }

            m_process->deleteLater();
            m_process = nullptr;
        }
    }

    return true;
}

void SubtitleTranslator::translateFileContent()
{
    if (m_cancelled) {
        emit translationError("Traducción cancelada por el usuario");
        return;
    }

    // Build multi-line input: only non-empty lines go to bergamot
    // Each cue must be exactly one line — join multi-line cue text with space
    QStringList inputLines;
    m_translatableIndices.clear();
    for (int i = 0; i < m_currentCues.size(); ++i) {
        QString text = m_currentCues[i].text.trimmed().replace('\n', ' ');
        if (!text.isEmpty()) {
            inputLines.append(text);
            m_translatableIndices.append(i);
        }
    }

    if (m_translatableIndices.isEmpty()) {
        // No translatable text — just copy file
        if (SubtitleParser::writeSrt(m_currentOutputPath, m_currentCues)) {
            emit fileTranslated(m_currentInputPath, m_currentOutputPath);
        } else {
            emit translationError("Error al escribir el archivo de salida: " + m_currentOutputPath);
        }
        return;
    }

    // Build bergamot command
    QString modelDir = modelPathForPair(m_batchSourceLang, m_batchTargetLang);
    QString configPath = modelDir + "/config.intgemm8bitalpha.yml";
    if (!QFile::exists(configPath))
        configPath = modelDir + "/config.yml";

    QStringList args;
    args << "--model-config-paths" << configPath
         << "--cpu-threads" << "2";

    qDebug() << "[TRANSLATE] translateFileContent:" << m_currentInputPath
             << "lines:" << m_currentCues.size()
             << "translatable:" << m_translatableIndices.size();

    // Create process
    if (m_process) {
        m_process->deleteLater();
    }
    m_process = new QProcess(this);
    m_process->setWorkingDirectory(modelDir);

    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &SubtitleTranslator::onProcessFinished);
    connect(m_process, &QProcess::readyReadStandardOutput,
            this, &SubtitleTranslator::onProcessReadyRead);
    connect(m_process, &QProcess::errorOccurred,
            this, &SubtitleTranslator::onProcessError);

    m_process->start(m_bergamotBinary, args);

    if (m_process->waitForStarted(5000)) {
        // Send all lines at once, separated by newlines
        QByteArray input = inputLines.join("\n").toUtf8();
        m_process->write(input);
        m_process->closeWriteChannel();
    } else {
        emit translationError("No se pudo iniciar el motor de traducción");
        m_process->deleteLater();
        m_process = nullptr;
    }
}

void SubtitleTranslator::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        QString stderrOutput = QString::fromLocal8Bit(m_process->readAllStandardError());
        qWarning() << "Bergamot error:" << stderrOutput;
        // Keep original text on error — write file as-is
    }

    // Read translated text from stdout (multi-line)
    QString output = QString::fromLocal8Bit(m_process->readAllStandardOutput());
    QStringList translatedLines = output.split('\n', Qt::SkipEmptyParts);

    qDebug() << "[TRANSLATE] onProcessFinished: exitCode=" << exitCode
             << "output lines:" << translatedLines.size()
             << "cues:" << m_currentCues.size();

    // Assign translations back to cues using stored indices
    for (int i = 0; i < m_translatableIndices.size() && i < translatedLines.size(); ++i) {
        int cueIdx = m_translatableIndices[i];
        QString translated = translatedLines[i].trimmed();
        if (!translated.isEmpty()) {
            qDebug() << "[TRANSLATE] line" << cueIdx << ":"
                     << m_currentCues[cueIdx].text.trimmed().left(40) << "->"
                     << translated.left(40);
            m_currentCues[cueIdx].text = translated;
        }
    }

    // Write output file
    if (SubtitleParser::writeSrt(m_currentOutputPath, m_currentCues)) {
        qDebug() << "[TRANSLATE] File written:" << m_currentOutputPath;
        emit fileTranslated(m_currentInputPath, m_currentOutputPath);
    } else {
        emit translationError("Error al escribir el archivo de salida: " + m_currentOutputPath);
    }

    // Clean up process
    m_process->deleteLater();
    m_process = nullptr;
}

void SubtitleTranslator::onProcessReadyRead()
{
    // Process output is handled in onProcessFinished
}

void SubtitleTranslator::onProcessError(QProcess::ProcessError procError)
{
    QString errorMsg;
    switch (procError) {
        case QProcess::FailedToStart:
            errorMsg = "No se pudo iniciar el motor de traducción";
            break;
        case QProcess::Crashed:
            errorMsg = "El motor de traducción terminó inesperadamente";
            break;
        case QProcess::Timedout:
            errorMsg = "El motor de traducción excedió el tiempo límite";
            break;
        default:
            errorMsg = "Error desconocido del motor de traducción";
    }
    emit translationError(errorMsg);
}

void SubtitleTranslator::translateBatch(const QStringList &inputPaths,
                                        const QString &outputDir,
                                        const QString &sourceLang,
                                        const QString &targetLang)
{
    m_batchQueue.clear();
    m_batchIndex = 0;
    m_batchSourceLang = sourceLang;
    m_batchTargetLang = targetLang;
    m_cancelled = false;

    qDebug() << "[TRANSLATE] translateBatch:" << inputPaths.size() << "files," << sourceLang << "→" << targetLang;

    for (const QString &inputPath : inputPaths) {
        QFileInfo info(inputPath);
        QString baseName = info.completeBaseName();

        // Remove language code suffix if present
        if (baseName.contains('.')) {
            int lastDot = baseName.lastIndexOf('.');
            QString possibleLang = baseName.mid(lastDot + 1);
            if (supportedSourceLanguages().contains(possibleLang) ||
                supportedTargetLanguages().contains(possibleLang)) {
                baseName = baseName.left(lastDot);
            }
        }

        QString outputPath = outputDir + "/" + baseName + "." + targetLang + ".srt";
        m_batchQueue.append({inputPath, outputPath});
    }

    processNextInBatch();
}

void SubtitleTranslator::processNextInBatch()
{
    if (m_cancelled || m_batchIndex >= m_batchQueue.size()) {
        qDebug() << "[TRANSLATE] batchFinished, processed" << m_batchIndex << "of" << m_batchQueue.size();
        emit batchFinished();
        return;
    }

    auto [inputPath, outputPath] = m_batchQueue[m_batchIndex];
    qDebug() << "[TRANSLATE] processNextInBatch [" << m_batchIndex << "/" << m_batchQueue.size() << "]" << inputPath << "→" << outputPath;

    connect(this, &SubtitleTranslator::fileTranslated,
            this, [this](const QString &, const QString &) {
                qDebug() << "[TRANSLATE] fileTranslated signal, advancing to" << m_batchIndex + 1;
                m_batchIndex++;
                processNextInBatch();
            }, Qt::UniqueConnection);

    connect(this, &SubtitleTranslator::translationError,
            this, [this](const QString &) {
                m_batchIndex++;
                processNextInBatch();
            }, Qt::UniqueConnection);

    translateFile(inputPath, outputPath, m_batchSourceLang, m_batchTargetLang);
}

void SubtitleTranslator::cancel()
{
    m_cancelled = true;
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->kill();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Private helpers
// ─────────────────────────────────────────────────────────────────────────────
QString SubtitleTranslator::findBergamotBinary()
{
    QStringList searchPaths;

    // Development paths
    searchPaths << QCoreApplication::applicationDirPath() + "/bergamot";
    searchPaths << QCoreApplication::applicationDirPath() + "/../bin/bergamot";
    searchPaths << QCoreApplication::applicationDirPath() + "/../third_party/bergamot/build/app/bergamot";

    // Installed paths
    searchPaths << QCoreApplication::applicationDirPath() + "/../share/elements-extract/bin/bergamot";
    searchPaths << "/usr/share/elements-extract/bin/bergamot";

    // User config directory
    searchPaths << QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/bin/bergamot";

    // System PATH
    searchPaths << "bergamot";

    for (const QString &path : searchPaths) {
        if (QFile::exists(path)) return path;
    }

    // Try to find in PATH
    QProcess which;
    which.start("which", {"bergamot"});
    if (which.waitForFinished(3000) && which.exitCode() == 0) {
        return QString::fromLocal8Bit(which.readAllStandardOutput()).trimmed();
    }

    return QString();
}

QString SubtitleTranslator::modelPathForPair(const QString &src, const QString &trg)
{
    QString pairCode = src + "-" + trg;

    // Check which model size is active (from settings)
    QSettings s("LocalApp", "FFmpegExtractor");
    QStringList activeModelsList = s.value("activeModels").toStringList();
    QSet<QString> activeModels(activeModelsList.begin(), activeModelsList.end());

    // Try active models first
    QStringList sizes = {"tiny", "base", "big"};
    for (const QString &size : sizes) {
        QString modelId = pairCode + "-" + size;
        if (activeModels.contains(modelId)) {
            // Find this model in the filesystem
            QStringList searchPaths;
            searchPaths << m_modelDir;
            searchPaths << QCoreApplication::applicationDirPath() + "/../models";
            searchPaths << QCoreApplication::applicationDirPath() + "/models";
            searchPaths << QCoreApplication::applicationDirPath() + "/../share/elements-extract/models";

            for (const QString &basePath : searchPaths) {
                QString fullPath = basePath + "/" + modelId;
                if (QDir(fullPath).exists()) return fullPath;
            }
        }
    }

    // Fallback: find any installed model for this pair
    for (const QString &size : sizes) {
        QString modelDir = m_modelDir + "/" + pairCode + "-" + size;
        if (QDir(modelDir).exists()) return modelDir;
    }

    // Check development paths
    QStringList devPaths;
    devPaths << QCoreApplication::applicationDirPath() + "/../models";
    devPaths << QCoreApplication::applicationDirPath() + "/models";
    devPaths << QCoreApplication::applicationDirPath() + "/../share/elements-extract/models";

    for (const QString &basePath : devPaths) {
        for (const QString &size : sizes) {
            QString devDir = basePath + "/" + pairCode + "-" + size;
            if (QDir(devDir).exists()) return devDir;
        }
    }

    // Scan for any matching directory
    QDir modelRoot(m_modelDir);
    QStringList dirs = modelRoot.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &dir : dirs) {
        if (dir.startsWith(pairCode)) {
            return m_modelDir + "/" + dir;
        }
    }

    return m_modelDir + "/" + pairCode + "-tiny";
}
