#include "subtitletranslator.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QSettings>
#include <QHash>
#include <algorithm>

namespace {
// El registro remoto mezcla códigos ISO con nombres completos; los modelos
// incluidos usan códigos cortos. Unificar aquí evita que un modelo descargado
// quede "instalado" pero invisible en los selectores de idioma.
const QHash<QString, QString> kLangAliases = {
    {"eng", "en"}, {"english", "en"},
    {"spa", "es"}, {"spanish", "es"}, {"cast", "es"}, {"español", "es"},
    {"fra", "fr"}, {"fre", "fr"}, {"french", "fr"}, {"français", "fr"},
    {"por", "pt"}, {"portuguese", "pt"}, {"português", "pt"},
    {"cze", "cs"}, {"czech", "cs"},
    {"ger", "de"}, {"deu", "de"}, {"german", "de"}, {"deutsch", "de"},
    {"est", "et"}, {"estonian", "et"},
    {"ice", "is"}, {"isl", "is"}, {"icelandic", "is"},
    {"nob", "nb"}, {"norwegian (bokmål)", "nb"}, {"norwegian bokmål", "nb"},
    {"nno", "nn"}, {"norwegian (nynorsk)", "nn"}, {"norwegian nynorsk", "nn"},
    {"bul", "bg"}, {"bulgarian", "bg"},
    {"pol", "pl"}, {"polish", "pl"}, {"polaco", "pl"},
    {"hbs", "hbs"}, {"serbo-croatian", "hbs"},
    {"sl", "sl"}, {"slovene", "sl"}, {"slovenian", "sl"},
    {"mk", "mk"}, {"macedonian", "mk"},
    {"mt", "mt"}, {"maltese", "mt"},
    {"tr", "tr"}, {"turkish", "tr"}, {"turco", "tr"},
    {"sq", "sq"}, {"albanian", "sq"},
    {"ca", "ca"}, {"catalan", "ca"},
    {"el", "el"}, {"greek", "el"},
    {"uk", "uk"}, {"ukrainian", "uk"},
    {"it", "it"}, {"ita", "it"}, {"italian", "it"},
    {"ru", "ru"}, {"rus", "ru"}, {"russian", "ru"},
    {"zh", "zh"}, {"chi", "zh"}, {"zho", "zh"}, {"chinese", "zh"},
    {"ja", "ja"}, {"jpn", "ja"}, {"japanese", "ja"},
    {"ko", "ko"}, {"kor", "ko"}, {"korean", "ko"},
    {"ar", "ar"}, {"ara", "ar"}, {"arabic", "ar"},
    {"hi", "hi"}, {"hin", "hi"}, {"hindi", "hi"},
    {"nl", "nl"}, {"dut", "nl"}, {"nld", "nl"}, {"dutch", "nl"},
    {"sv", "sv"}, {"swe", "sv"}, {"swedish", "sv"}
};

QString normalizeModelLangCode(const QString &raw)
{
    const QString c = raw.trimmed().toLower();
    return kLangAliases.value(c, c);
}

QStringList modelSearchBases()
{
    QStringList bases;
    QSettings settings("LocalApp", "FFmpegExtractor");
    const QString customDir = settings.value("modelsDir").toString().trimmed();
    if (!customDir.isEmpty()) bases << customDir;
    bases << QCoreApplication::applicationDirPath() + "/../models";
    bases << QCoreApplication::applicationDirPath() + "/models";
    bases << QCoreApplication::applicationDirPath() + "/../share/elements-extract/models";
    bases << QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/models";
    bases.removeDuplicates();
    return bases;
}

bool hasBergamotConfig(const QString &dir)
{
    return QFile::exists(dir + "/config.intgemm8bitalpha.yml") ||
           QFile::exists(dir + "/config.yml");
}

// El registro usa a veces códigos distintos de los del selector (p. ej.
// `hbs-eng-tiny` frente a `hbs-en-tiny`). Probar las variantes evita que un
// modelo descargado quede instalado pero inalcanzable para el traductor.
QStringList modelIdVariants(const QString &modelId)
{
    QStringList variants;
    variants << modelId;
    const QStringList parts = modelId.split('-', Qt::SkipEmptyParts);
    if (parts.size() < 3) return variants;

    const QString suffix = QStringList(parts.mid(2)).join('-');
    auto aliasesOf = [](const QString &canonical) {
        QStringList list {canonical};
        for (auto it = kLangAliases.constBegin(); it != kLangAliases.constEnd(); ++it) {
            if (it.value() == canonical && !list.contains(it.key())) list << it.key();
        }
        return list;
    };

    for (const QString &src : aliasesOf(normalizeModelLangCode(parts.at(0)))) {
        for (const QString &trg : aliasesOf(normalizeModelLangCode(parts.at(1)))) {
            const QString candidate = src + "-" + trg + "-" + suffix;
            if (!variants.contains(candidate)) variants << candidate;
        }
    }
    return variants;
}

// Devuelve el directorio que contiene la configuración bergamot de un modelo.
// Acepta el layout plano (models/<id>/config...) y el anidado que dejaban los
// tar.gz antes de usar --strip-components=1 (zip de Windows v1.2.12).
QString locateModelDir(const QString &modelId, const QString &primaryDir)
{
    QStringList bases;
    if (!primaryDir.isEmpty()) bases << primaryDir;
    bases << modelSearchBases();

    for (const QString &variant : modelIdVariants(modelId)) {
        for (const QString &base : bases) {
            if (base.isEmpty()) continue;
            const QString dir = base + "/" + variant;
            if (hasBergamotConfig(dir)) return dir;

            QDir parent(dir);
            if (!parent.exists()) continue;
            for (const QString &sub : parent.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                const QString nested = dir + "/" + sub;
                if (hasBergamotConfig(nested)) return nested;
            }
        }
    }
    return QString();
}

QStringList installedLanguageCodes()
{
    QStringList codes = {"en", "es", "fr", "pt"};
    QSet<QString> seen(codes.begin(), codes.end());

    for (const QString &base : modelSearchBases()) {
        QDir d(base);
        if (!d.exists()) continue;
        for (const QString &entry : d.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            const QString dir = base + "/" + entry;
            bool usable = hasBergamotConfig(dir);
            if (!usable) {
                QDir modelDir(dir);
                for (const QString &sub : modelDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                    if (hasBergamotConfig(dir + "/" + sub)) { usable = true; break; }
                }
            }
            if (!usable) continue;

            const QStringList parts = entry.split('-', Qt::SkipEmptyParts);
            for (int i = 0; i < qMin(2, parts.size()); ++i) {
                const QString code = normalizeModelLangCode(parts.at(i));
                if (!code.isEmpty() && !seen.contains(code)) {
                    seen.insert(code);
                    codes << code;
                }
            }
        }
    }
    std::sort(codes.begin() + 4, codes.end());
    return codes;
}
} // namespace

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
QString SubtitleTranslator::normalizeLanguageCode(const QString &raw)
{
    return normalizeModelLangCode(raw);
}

QStringList SubtitleTranslator::supportedSourceLanguages()
{
    return installedLanguageCodes();
}

QStringList SubtitleTranslator::supportedTargetLanguages()
{
    return installedLanguageCodes();
}

QString SubtitleTranslator::languageName(const QString &code)
{
    static QMap<QString, QString> names = {
        {"en", "English"}, {"es", "Español"}, {"fr", "Français"}, {"pt", "Português"},
        {"cs", "Čeština"}, {"de", "Deutsch"}, {"et", "Eesti"}, {"is", "Íslenska"},
        {"nb", "Norsk bokmål"}, {"nn", "Norsk nynorsk"}, {"bg", "Български"},
        {"pl", "Polski"}, {"hbs", "Srpskohrvatski"}, {"sl", "Slovenščina"},
        {"mk", "Македонски"}, {"mt", "Malti"}, {"tr", "Türkçe"}, {"sq", "Shqip"},
        {"ca", "Català"}, {"el", "Ελληνικά"}, {"uk", "Українська"},
        {"it", "Italiano"}, {"ita", "Italiano"}, {"ru", "Русский"}, {"rus", "Русский"},
        {"zh", "中文"}, {"chi", "中文"}, {"zho", "中文"}, {"ja", "日本語"}, {"jpn", "日本語"},
        {"ko", "한국어"}, {"kor", "한국어"}, {"ar", "العربية"}, {"ara", "العربية"},
        {"hi", "हिन्दी"}, {"hin", "हिन्दी"}, {"nl", "Nederlands"}, {"dut", "Nederlands"},
        {"nld", "Nederlands"}, {"sv", "Svenska"}, {"swe", "Svenska"}
    };
    return names.value(normalizeModelLangCode(code), code);
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
    const QString pairCode = sourceLang + "-" + targetLang;
    for (const QString &size : {QStringLiteral("tiny"), QStringLiteral("base"), QStringLiteral("big")}) {
        if (!locateModelDir(pairCode + "-" + size, m_modelDir).isEmpty()) return true;
    }
    return false;
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
    const QString pairCode = src + "-" + trg;
    const QStringList sizes = {QStringLiteral("tiny"), QStringLiteral("base"), QStringLiteral("big")};

    // Check which model size is active (from settings)
    QSettings s("LocalApp", "FFmpegExtractor");
    const QStringList activeModelsList = s.value("activeModels").toStringList();
    const QSet<QString> activeModels(activeModelsList.begin(), activeModelsList.end());

    // Prefer the active model, then any installed model for this pair.
    for (const QString &size : sizes) {
        const QString modelId = pairCode + "-" + size;
        if (activeModels.contains(modelId)) {
            const QString dir = locateModelDir(modelId, m_modelDir);
            if (!dir.isEmpty()) return dir;
        }
    }
    for (const QString &size : sizes) {
        const QString dir = locateModelDir(pairCode + "-" + size, m_modelDir);
        if (!dir.isEmpty()) return dir;
    }

    // No config found: keep the old fallback so error messages point to the
    // expected location.
    return m_modelDir + "/" + pairCode + "-tiny";
}
