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
    , m_batchActive(false)
    , m_currentLineTotal(0)
    , m_blocking(false)
{
    m_bergamotBinary = findBergamotBinary();

    // Advance the batch once, after the current signal emission has fully unwound.
    // QueuedConnection avoids re-entrant recursion during blocking (triangular) hops.
    connect(this, &SubtitleTranslator::fileTranslated,
            this, &SubtitleTranslator::onBatchFileDone, Qt::QueuedConnection);
    connect(this, &SubtitleTranslator::translationError,
            this, &SubtitleTranslator::onBatchFileFailed, Qt::QueuedConnection);
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
        {"pt", "Português"},
        // Solo para mensajes legibles: estos idiomas no tienen modelo local
        {"de", "Deutsch"}, {"it", "Italiano"}, {"ru", "Ruso"},
        {"zh", "Chino"}, {"ja", "Japonés"}, {"ko", "Coreano"},
        {"ar", "Árabe"}, {"hi", "Hindi"}, {"nl", "Holandés"},
        {"sv", "Sueco"}, {"pl", "Polaco"}, {"tr", "Turco"}
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
        emit translationError(tr("No se pudieron leer los subtítulos del archivo: ") + inputPath);
        return false;
    }
    qDebug() << "[TRANSLATE] Parsed" << cues.size() << "cues";

    // Check if bergamot binary exists
    qDebug() << "[TRANSLATE] Bergamot binary:" << m_bergamotBinary << "exists:" << QFile::exists(m_bergamotBinary);
    if (m_bergamotBinary.isEmpty() || !QFile::exists(m_bergamotBinary)) {
        qDebug() << "[TRANSLATE] FAIL: bergamot binary not found";
        emit translationError(tr("No se encontró el motor de traducción (bergamot).\n"
                              "Asegúrese de que está instalado correctamente.\n"
                              "Buscado en: ") + m_bergamotBinary);
        return false;
    }

    // Check translation path (direct or triangular)
    QStringList path = findTranslationPath(sourceLang, targetLang);
    if (path.isEmpty()) {
        qDebug() << "[TRANSLATE] FAIL: no translation path found for" << sourceLang << "→" << targetLang;
        emit translationError(tr("No se encontró un modelo de traducción para %1→%2.\n"
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
        emit translationError(tr("No se pudieron leer los subtítulos del archivo: ") + inputPath);
        return false;
    }
    qDebug() << "[TRANSLATE] Parsed" << cues.size() << "cues";

    // Check if bergamot binary exists
    qDebug() << "[TRANSLATE] Bergamot binary:" << m_bergamotBinary << "exists:" << QFile::exists(m_bergamotBinary);
    if (m_bergamotBinary.isEmpty() || !QFile::exists(m_bergamotBinary)) {
        qDebug() << "[TRANSLATE] FAIL: bergamot binary not found";
        emit translationError(tr("No se encontró el motor de traducción (bergamot).\n"
                              "Asegúrese de que está instalado correctamente.\n"
                              "Buscado en: ") + m_bergamotBinary);
        return false;
    }

    // Check if model is installed
    QString modelPath = modelPathForPair(sourceLang, targetLang);
    bool modelOk = isModelInstalled(sourceLang, targetLang);
    qDebug() << "[TRANSLATE] Model path:" << modelPath << "installed:" << modelOk;
    if (!modelOk) {
        qDebug() << "[TRANSLATE] FAIL: model not installed";
        emit translationError(tr("El modelo de traducción %1→%2 no está instalado.\n"
                                       "Directorio buscado: %3")
                                  .arg(languageName(sourceLang))
                                  .arg(languageName(targetLang))
                                  .arg(modelPath));
        return false;
    }

    m_currentInputPath = inputPath;
    m_currentOutputPath = outputPath;
    m_currentSourceLang = sourceLang;
    m_currentTargetLang = targetLang;
    m_currentCues = cues;
    m_translatableIndices.clear();
    m_currentLineTotal = cues.size();

    // Build multi-line input: only non-empty lines go to bergamot.
    // Each cue must be exactly one line — join multi-line cue text with space.
    QStringList inputLines;
    for (int i = 0; i < cues.size(); ++i) {
        QString text = cues[i].text.trimmed().replace('\n', ' ');
        if (!text.isEmpty()) {
            inputLines.append(text);
            m_translatableIndices.append(i);
        }
    }

    if (m_translatableIndices.isEmpty()) {
        // No translatable text — just copy file
        if (SubtitleParser::writeSrt(outputPath, cues)) {
            return true;
        }
        emit translationError(tr("Error al escribir el archivo de salida: ") + outputPath);
        return false;
    }

    // Build bergamot command for THIS pair (never touches the batch languages)
    QString configPath = modelPath + "/config.intgemm8bitalpha.yml";
    if (!QFile::exists(configPath))
        configPath = modelPath + "/config.yml";

    QStringList args;
    args << "--model-config-paths" << configPath
         << "--cpu-threads" << "2";

    qDebug() << "[TRANSLATE] translateFileContent:" << inputPath
             << "lines:" << cues.size()
             << "translatable:" << m_translatableIndices.size();

    // Blocking mode (triangular hops): use a local QProcess so the member
    // process and its async slots can never interfere.
    if (m_blocking) {
        if (m_cancelled) return false;
        QProcess proc;
        proc.setWorkingDirectory(modelPath);
        proc.start(m_bergamotBinary, args);
        if (!proc.waitForStarted(5000)) {
            emit translationError(tr("No se pudo iniciar el motor de traducción"));
            return false;
        }
        proc.write(inputLines.join("\n").toUtf8() + "\n");
        proc.closeWriteChannel();
        if (!proc.waitForFinished(600000)) {  // 10 min timeout
            proc.kill();
            proc.waitForFinished(1000);
            emit translationError(tr("Tiempo de espera agotado del motor de traducción"));
            return false;
        }
        if (proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
            QString err = QString::fromLocal8Bit(proc.readAllStandardError());
            qWarning() << "Bergamot error (blocking hop):" << err;
            emit translationError(tr("Error del motor de traducción (código %1).")
                                      .arg(proc.exitCode()) +
                                  (err.trimmed().isEmpty() ? QString()
                                  : QString("\n%1").arg(err.trimmed().left(200))));
            return false;
        }
        QString output = QString::fromLocal8Bit(proc.readAllStandardOutput());
        QStringList translatedLines = output.split('\n');
        int n = qMin(m_translatableIndices.size(), translatedLines.size());
        for (int i = 0; i < n; ++i) {
            QString translated = translatedLines[i].trimmed();
            if (!translated.isEmpty())
                m_currentCues[m_translatableIndices[i]].text = translated;
        }
        if (SubtitleParser::writeSrt(outputPath, m_currentCues)) {
            return true;
        }
        emit translationError(tr("Error al escribir el archivo de salida: ") + outputPath);
        return false;
    }

    // Async mode (direct translation / batch): drive completion via signals.
    resetProcess();
    m_process = new QProcess(this);
    m_process->setWorkingDirectory(modelPath);

    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &SubtitleTranslator::onProcessFinished);
    connect(m_process, &QProcess::readyReadStandardOutput,
            this, &SubtitleTranslator::onProcessReadyRead);
    connect(m_process, &QProcess::errorOccurred,
            this, &SubtitleTranslator::onProcessError);

    m_process->start(m_bergamotBinary, args);

    if (m_process->waitForStarted(5000)) {
        m_process->write(inputLines.join("\n").toUtf8() + "\n");
        m_process->closeWriteChannel();
        return true;
    }

    emit translationError(tr("No se pudo iniciar el motor de traducción"));
    resetProcess();
    return false;
}

void SubtitleTranslator::resetProcess()
{
    if (!m_process) return;
    disconnect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
               this, &SubtitleTranslator::onProcessFinished);
    disconnect(m_process, &QProcess::readyReadStandardOutput,
               this, &SubtitleTranslator::onProcessReadyRead);
    disconnect(m_process, &QProcess::errorOccurred,
               this, &SubtitleTranslator::onProcessError);
    if (m_process->state() != QProcess::NotRunning)
        m_process->kill();
    m_process->deleteLater();
    m_process = nullptr;
}

void SubtitleTranslator::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QProcess *p = qobject_cast<QProcess*>(sender());
    if (!p || p != m_process) return;  // ignore signals from a stale/replaced process

    bool ok = (exitStatus == QProcess::NormalExit && exitCode == 0);
    QString output, stderrOutput;
    if (p->state() == QProcess::NotRunning) {
        output       = QString::fromLocal8Bit(p->readAllStandardOutput());
        stderrOutput = QString::fromLocal8Bit(p->readAllStandardError());
    }

    if (!ok) {
        qWarning() << "Bergamot error:" << stderrOutput;
        resetProcess();
        emit translationError(tr("Error del motor de traducción (código %1).")
                                  .arg(exitCode) +
                              (stderrOutput.trimmed().isEmpty() ? QString()
                              : QString("\n%1").arg(stderrOutput.trimmed().left(200))));
        return;
    }

    QStringList translatedLines = output.split('\n');
    int n = qMin(m_translatableIndices.size(), translatedLines.size());
    for (int i = 0; i < n; ++i) {
        QString translated = translatedLines[i].trimmed();
        if (!translated.isEmpty())
            m_currentCues[m_translatableIndices[i]].text = translated;
    }
    if (translatedLines.size() < m_translatableIndices.size())
        qWarning() << "Bergamot returned fewer lines than requested:" << translatedLines.size()
                   << "<" << m_translatableIndices.size();

    resetProcess();
    if (SubtitleParser::writeSrt(m_currentOutputPath, m_currentCues)) {
        qDebug() << "[TRANSLATE] File written:" << m_currentOutputPath;
        emit fileTranslated(m_currentInputPath, m_currentOutputPath);
    } else {
        emit translationError(tr("Error al escribir el archivo de salida: ") + m_currentOutputPath);
    }
}

void SubtitleTranslator::onProcessReadyRead()
{
    // Process output is handled in onProcessFinished
}

void SubtitleTranslator::onProcessError(QProcess::ProcessError procError)
{
    QProcess *p = qobject_cast<QProcess*>(sender());
    if (!p || p != m_process) return;  // ignore stale processes

    QString errorMsg;
    switch (procError) {
        case QProcess::FailedToStart:
            errorMsg = tr("No se pudo iniciar el motor de traducción");
            break;
        case QProcess::Crashed:
            errorMsg = tr("El motor de traducción terminó inesperadamente");
            break;
        case QProcess::Timedout:
            errorMsg = tr("El motor de traducción excedió el tiempo límite");
            break;
        default:
            errorMsg = tr("Error desconocido del motor de traducción");
    }
    resetProcess();
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
    m_batchActive = true;

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
        m_batchActive = false;
        emit batchFinished();
        return;
    }

    auto [inputPath, outputPath] = m_batchQueue[m_batchIndex];
    qDebug() << "[TRANSLATE] processNextInBatch [" << m_batchIndex << "/" << m_batchQueue.size() << "]" << inputPath << "→" << outputPath;

    emit progress(m_batchIndex + 1, m_batchQueue.size());
    translateFile(inputPath, outputPath, m_batchSourceLang, m_batchTargetLang);
}

void SubtitleTranslator::onBatchFileDone()
{
    if (!m_batchActive) return;
    qDebug() << "[TRANSLATE] fileTranslated, advancing to" << m_batchIndex + 1;
    m_batchIndex++;
    processNextInBatch();
}

void SubtitleTranslator::onBatchFileFailed()
{
    if (!m_batchActive) return;
    qDebug() << "[TRANSLATE] translationError, skipping to" << m_batchIndex + 1;
    m_batchIndex++;
    processNextInBatch();
}

void SubtitleTranslator::cancel()
{
    m_cancelled = true;
    m_batchActive = false;
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

#ifdef Q_OS_WIN
    const QString exe = QStringLiteral(".exe");
#else
    const QString exe = QString();
#endif

    // Development paths
    searchPaths << QCoreApplication::applicationDirPath() + "/bergamot" + exe;
    searchPaths << QCoreApplication::applicationDirPath() + "/../bin/bergamot" + exe;
    searchPaths << QCoreApplication::applicationDirPath() + "/../third_party/bergamot/build/app/bergamot" + exe;

    // Installed paths
    searchPaths << QCoreApplication::applicationDirPath() + "/../share/elements-extract/bin/bergamot" + exe;
    searchPaths << "/usr/share/elements-extract/bin/bergamot" + exe;

    // User config directory
    searchPaths << QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/bin/bergamot" + exe;

    // System PATH
    searchPaths << "bergamot" + exe;

    for (const QString &path : searchPaths) {
        if (QFile::exists(path)) return path;
    }

    // Try to find in PATH (portable: equivale a `where` en Windows)
    return QStandardPaths::findExecutable("bergamot" + exe);
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
