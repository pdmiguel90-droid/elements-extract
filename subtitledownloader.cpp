#include "subtitledownloader.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QProcess>
#include <QRegularExpression>
#include <QSettings>
#include <QTemporaryDir>
#include <QUrlQuery>
#include <QUuid>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <algorithm>

namespace {

const char *kBaseUrl = "https://api.opensubtitles.com/api/v1/";
const char *kXmlRpcUrl = "https://api.opensubtitles.org/xml-rpc";
const char *kUserAgent = "Elements-Extract/1.2";
// El servicio XML-RPC solo entrega enlaces de descarga reales (/sub/) a los
// clientes que conoce; con otros nombres devuelve un fichero VIP (/subad/).
const char *kXmlRpcAppName = "VLSub 0.11.1";

void setFollowRedirects(QNetworkRequest &request)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    request.setAttribute(QNetworkRequest::RedirectAttributePolicy,
                         QNetworkRequest::FollowRedirectsAttribute);
#else
    // Qt 6.2–6.6: equivalente (https → https se sigue automáticamente).
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);
#endif
}

bool isZipData(const QByteArray &data)
{
    return data.size() >= 4 && data.at(0) == 'P' && data.at(1) == 'K'
           && static_cast<unsigned char>(data.at(2)) == 0x03
           && static_cast<unsigned char>(data.at(3)) == 0x04;
}

bool isGzipData(const QByteArray &data)
{
    return data.size() >= 2 && static_cast<unsigned char>(data.at(0)) == 0x1f
           && static_cast<unsigned char>(data.at(1)) == 0x8b;
}

bool looksLikeRar(const QByteArray &data)
{
    static const QByteArray signature("Rar!\x1a\x07", 7);
    return data.startsWith(signature);
}

QString quotePs(const QString &path)
{
    QString quoted = path;
    return quoted.replace('\'', "''");
}

// Ejecuta un descompresor externo redirigiendo su salida estándar a un fichero.
bool runExtractor(const QString &program, const QStringList &arguments,
                  const QString &outputPath, QString *error, bool captureStdOut = true)
{
    QProcess process;
    if (captureStdOut)
        process.setStandardOutputFile(outputPath);
    process.start(program, arguments);
    if (!process.waitForStarted(5000)) {
        if (error) *error = SubtitleDownloader::tr("No se pudo iniciar %1.").arg(program);
        return false;
    }
    if (!process.waitForFinished(20000)) {
        process.kill();
        process.waitForFinished();
        if (error) *error = SubtitleDownloader::tr("Tiempo de espera agotado al descomprimir con %1.").arg(program);
        return false;
    }
    const QString err = QString::fromLocal8Bit(process.readAllStandardError()).trimmed();
    if (process.exitCode() != 0) {
        if (error) {
            *error = SubtitleDownloader::tr("Error al descomprimir con %1: %2")
                         .arg(program, err.isEmpty() ? QString::number(process.exitCode()) : err);
        }
        return false;
    }
    if (!QFile::exists(outputPath) || QFileInfo(outputPath).size() == 0) {
        if (error) *error = SubtitleDownloader::tr("El archivo comprimido no contiene un subtítulo.");
        return false;
    }
    return true;
}

// Extrae una entrada concreta (puede estar en una subcarpeta del ZIP).
bool extractZipEntry(const QString &archivePath, const QString &entry,
                     const QString &outputPath, QString *error)
{
#ifdef Q_OS_WIN
    const QString script = QStringLiteral(
        "$ErrorActionPreference='Stop'; Add-Type -AssemblyName System.IO.Compression.FileSystem; "
        "$z=[System.IO.Compression.ZipFile]::OpenRead('%1'); "
        "$e=$z.GetEntry('%2'); if ($null -eq $e) { throw 'Entrada no encontrada en el ZIP' }; "
        "$s=$e.Open(); $d=[System.IO.File]::Create('%3'); $s.CopyTo($d); "
        "$d.Dispose(); $s.Dispose(); $z.Dispose();")
        .arg(quotePs(archivePath), quotePs(entry), quotePs(outputPath));
    return runExtractor("powershell", {"-NoProfile", "-NonInteractive", "-Command", script},
                        outputPath, error, false);
#else
    return runExtractor("unzip", {"-p", archivePath, entry}, outputPath, error);
#endif
}

// RAR: se prueba unrar y, si no está, 7z/7za.
bool extractRar(const QString &archivePath, const QString &outputPath, QString *error)
{
    struct Tool { QString program; QStringList arguments; };
    QList<Tool> tools = {
#ifdef Q_OS_WIN
        {"7z",   {"x", "-so", archivePath}},
        {"unrar", {"p", "-inul", archivePath}}
#else
        {"unrar", {"p", "-inul", archivePath}},
        {"7z",    {"x", "-so", archivePath}},
        {"7za",   {"x", "-so", archivePath}}
#endif
    };

    QStringList details;
    for (const Tool &tool : tools) {
        QFile::remove(outputPath);
        QString toolError;
        if (runExtractor(tool.program, tool.arguments, outputPath, &toolError)) {
            qInfo() << "[ONLINE-SUBS] RAR extracted with" << tool.program;
            return true;
        }
        details << toolError;
    }
    if (error) {
        *error = SubtitleDownloader::tr(
            "El subtítulo está comprimido en RAR; instala unrar o 7z, o descarga el subtítulo desde la web de OpenSubtitles.");
        if (!details.isEmpty())
            *error += QStringLiteral(" (") + details.join(QStringLiteral("; ")) + QStringLiteral(")");
    }
    return false;
}

bool extractGzip(const QString &archivePath, const QString &outputPath, QString *error)
{
#ifdef Q_OS_WIN
    const QString script = QStringLiteral(
        "$ErrorActionPreference='Stop'; "
        "$f=[System.IO.File]::OpenRead('%1'); "
        "$g=New-Object System.IO.Compression.GZipStream($f,[System.IO.Compression.CompressionMode]::Decompress); "
        "$d=[System.IO.File]::Create('%2'); $g.CopyTo($d); "
        "$d.Dispose(); $g.Dispose(); $f.Dispose();")
        .arg(quotePs(archivePath), quotePs(outputPath));
    return runExtractor("powershell", {"-NoProfile", "-NonInteractive", "-Command", script},
                        outputPath, error, false);
#else
    return runExtractor("gzip", {"-dc", archivePath}, outputPath, error);
#endif
}

QString toThreeLetterCode(const QString &code)
{
    static const QHash<QString, QString> map = {
        {"es", "spa"}, {"en", "eng"}, {"pt", "por"}, {"fr", "fra"}, {"de", "deu"},
        {"it", "ita"}, {"ca", "cat"}, {"nl", "nld"}, {"pl", "pol"}, {"ru", "rus"},
        {"tr", "tur"}, {"ja", "jpn"}, {"ko", "kor"}, {"zh", "zho"}, {"sv", "swe"},
        {"da", "dan"}, {"no", "nor"}, {"fi", "fin"}, {"cs", "ces"}, {"sk", "slk"},
        {"sl", "slv"}, {"el", "ell"}, {"he", "heb"}, {"ar", "ara"}, {"hi", "hin"},
        {"id", "ind"}, {"th", "tha"}, {"vi", "vie"}, {"ro", "ron"}, {"hu", "hun"},
        {"uk", "ukr"}, {"hr", "hrv"}, {"sr", "srp"}, {"bg", "bul"}, {"et", "est"},
        {"lv", "lav"}, {"lt", "lit"}, {"fa", "fas"}, {"ur", "urd"}, {"bn", "ben"},
        {"ta", "tam"}, {"te", "tel"}, {"ms", "msa"}, {"mk", "mkd"}, {"sq", "sqi"},
        {"is", "isl"}, {"ga", "gle"}, {"mt", "mlt"}, {"cy", "cym"}, {"yi", "yid"}
    };
    const QString lang = code.trimmed().toLower().section('-', 0, 0);
    if (lang.isEmpty() || lang == QLatin1String("all")) return QStringLiteral("all");
    if (lang.size() == 3) return lang;
    if (lang == QLatin1String("pb")) return QStringLiteral("por");
    if (lang == QLatin1String("iw")) return QStringLiteral("heb");
    if (lang == QLatin1String("in")) return QStringLiteral("ind");
    if (lang == QLatin1String("ji")) return QStringLiteral("yid");
    return map.value(lang, lang);
}

QString toTwoLetterCode(const QString &code)
{
    static const QHash<QString, QString> map = {
        {"spa", "es"}, {"eng", "en"}, {"por", "pt"}, {"fra", "fr"}, {"fre", "fr"},
        {"deu", "de"}, {"ger", "de"}, {"ita", "it"}, {"cat", "ca"}, {"nld", "nl"},
        {"dut", "nl"}, {"pol", "pl"}, {"rus", "ru"}, {"tur", "tr"}, {"jpn", "ja"},
        {"kor", "ko"}, {"zho", "zh"}, {"chi", "zh"}, {"swe", "sv"}, {"dan", "da"},
        {"nor", "no"}, {"fin", "fi"}, {"ces", "cs"}, {"cze", "cs"}, {"slk", "sk"},
        {"slv", "sl"}, {"ell", "el"}, {"heb", "he"}, {"ara", "ar"}, {"hin", "hi"},
        {"ind", "id"}, {"tha", "th"}, {"vie", "vi"}, {"ron", "ro"}, {"rum", "ro"},
        {"hun", "hu"}, {"ukr", "uk"}, {"hrv", "hr"}, {"srp", "sr"}, {"bul", "bg"},
        {"est", "et"}, {"lav", "lv"}, {"lit", "lt"}, {"fas", "fa"}, {"per", "fa"},
        {"urd", "ur"}, {"ben", "bn"}, {"tam", "ta"}, {"tel", "te"}, {"msa", "ms"},
        {"mkd", "mk"}, {"sqi", "sq"}, {"isl", "is"}, {"gle", "ga"}, {"mlt", "mt"},
        {"cym", "cy"}, {"yid", "yi"}
    };
    const QString lang = code.trimmed().toLower();
    if (lang.size() != 3) return lang;
    return map.value(lang, lang);
}

bool parseSeasonEpisode(const QString &query, QString *season, QString *episode)
{
    QRegularExpression re(QStringLiteral("[Ss](\\d{1,2})[Ee](\\d{1,3})"));
    QRegularExpressionMatch match = re.match(query);
    if (!match.hasMatch()) {
        re.setPattern(QStringLiteral("(?:^|\\D)(\\d{1,2})x(\\d{1,3})(?:\\D|$)"));
        match = re.match(query);
    }
    if (!match.hasMatch()) return false;
    if (season) *season = match.captured(1);
    if (episode) *episode = match.captured(2);
    return true;
}

bool isVipDecoy(const QByteArray &data)
{
    const QString text = QString::fromUtf8(data.left(16384));
    return text.contains(QStringLiteral("osdb.link/vip"), Qt::CaseInsensitive)
        || text.contains(QStringLiteral("Become OpenSubtitles.org VIP member"),
                         Qt::CaseInsensitive);
}

// Lector genérico de valores XML-RPC. El lector debe estar en el elemento <value>.
QVariant readXmlRpcValue(QXmlStreamReader &xml, bool *ok)
{
    if (ok) *ok = false;
    if (!xml.readNextStartElement()) {
        if (xml.isEndElement() && xml.name() == QLatin1String("value"))
            xml.readNext();
        if (ok) *ok = true;
        return QString();
    }

    const QString type = xml.name().toString();
    QVariant result;
    bool valid = true;

    if (type == QLatin1String("string") || type == QLatin1String("dateTime.iso8601")
        || type == QLatin1String("base64")) {
        result = xml.readElementText();
    } else if (type == QLatin1String("int") || type == QLatin1String("i4")
               || type == QLatin1String("i8")) {
        bool converted = false;
        const qlonglong number = xml.readElementText().toLongLong(&converted);
        valid = converted;
        result = number;
    } else if (type == QLatin1String("double")) {
        bool converted = false;
        const double number = xml.readElementText().toDouble(&converted);
        valid = converted;
        result = number;
    } else if (type == QLatin1String("boolean")) {
        const QString text = xml.readElementText().trimmed();
        result = (text == QLatin1String("1")
                  || text.compare(QLatin1String("true"), Qt::CaseInsensitive) == 0);
    } else if (type == QLatin1String("nil")) {
        xml.skipCurrentElement();
    } else if (type == QLatin1String("array")) {
        QVariantList values;
        while (xml.readNextStartElement()) {
            if (xml.name() == QLatin1String("data")) {
                while (!xml.atEnd()) {
                    if (xml.isStartElement() && xml.name() == QLatin1String("value")) {
                        bool itemOk = false;
                        values.append(readXmlRpcValue(xml, &itemOk));
                        valid = valid && itemOk;
                    } else if (xml.isEndElement() && xml.name() == QLatin1String("data")) {
                        break;
                    } else if (!xml.readNextStartElement()) {
                        break;
                    }
                }
            } else {
                xml.skipCurrentElement();
            }
        }
        result = values;
    } else if (type == QLatin1String("struct")) {
        QVariantMap members;
        while (xml.readNextStartElement()) {
            if (xml.name() == QLatin1String("member")) {
                QString name;
                QVariant value;
                while (!xml.atEnd()) {
                    if (xml.isEndElement() && xml.name() == QLatin1String("member"))
                        break;   // el bucle exterior avanza al siguiente <member>
                    if (!xml.readNextStartElement()) break;
                    if (xml.name() == QLatin1String("name")) {
                        name = xml.readElementText();
                    } else if (xml.name() == QLatin1String("value")) {
                        bool itemOk = false;
                        value = readXmlRpcValue(xml, &itemOk);
                        valid = valid && itemOk;
                    } else {
                        xml.skipCurrentElement();
                    }
                }
                if (!name.isEmpty()) members.insert(name, value);
            } else {
                xml.skipCurrentElement();
            }
        }
        result = members;
    } else {
        xml.skipCurrentElement();
        valid = false;
    }

    while (!xml.atEnd() && !(xml.isEndElement() && xml.name() == QLatin1String("value"))) {
        if (xml.isStartElement()) xml.skipCurrentElement();
        xml.readNext();
    }
    if (xml.isEndElement() && xml.name() == QLatin1String("value"))
        xml.readNext();

    if (ok) *ok = valid;
    return result;
}

QVariant parseXmlRpcResponse(const QByteArray &data, QString *error, bool *isFault)
{
    QXmlStreamReader xml(data);
    QVariant result;
    bool fault = false;
    bool parsed = false;

    while (!xml.atEnd() && !xml.hasError()) {
        if (!xml.readNextStartElement()) break;
        const QString name = xml.name().toString();
        if (name == QLatin1String("methodResponse")) {
            continue;
        }
        if (name == QLatin1String("fault") || name == QLatin1String("params")) {
            fault = (name == QLatin1String("fault"));
            const QString wrapper = fault ? QStringLiteral("value") : QStringLiteral("param");
            while (xml.readNextStartElement()) {
                if (xml.name() != wrapper) {
                    xml.skipCurrentElement();
                    continue;
                }
                if (fault) {
                    bool itemOk = false;
                    result = readXmlRpcValue(xml, &itemOk);
                    parsed = itemOk;
                } else {
                    if (xml.readNextStartElement() && xml.name() == QLatin1String("value")) {
                        bool itemOk = false;
                        result = readXmlRpcValue(xml, &itemOk);
                        parsed = itemOk;
                    } else {
                        xml.skipCurrentElement();
                    }
                }
            }
        } else {
            xml.skipCurrentElement();
        }
    }

    if (isFault) *isFault = fault;
    if (xml.hasError()) {
        if (error)
            *error = SubtitleDownloader::tr("La respuesta XML-RPC de OpenSubtitles no es válida.");
        return QVariant();
    }
    if (!parsed) {
        if (error)
            *error = SubtitleDownloader::tr("La respuesta XML-RPC de OpenSubtitles no contiene valores.");
        return QVariant();
    }
    if (fault) {
        if (error) {
            const QString detail = result.toMap().value(QStringLiteral("faultString")).toString();
            *error = SubtitleDownloader::tr("Error XML-RPC de OpenSubtitles: %1")
                         .arg(detail.isEmpty() ? QStringLiteral("fault") : detail);
        }
        return QVariant();
    }
    return result;
}

void writeXmlRpcStringParam(QXmlStreamWriter &xml, const QString &value)
{
    xml.writeStartElement(QStringLiteral("param"));
    xml.writeStartElement(QStringLiteral("value"));
    xml.writeTextElement(QStringLiteral("string"), value);
    xml.writeEndElement();
    xml.writeEndElement();
}

void writeXmlRpcStringMember(QXmlStreamWriter &xml, const QString &name,
                             const QString &value)
{
    xml.writeStartElement(QStringLiteral("member"));
    xml.writeTextElement(QStringLiteral("name"), name);
    xml.writeStartElement(QStringLiteral("value"));
    xml.writeTextElement(QStringLiteral("string"), value);
    xml.writeEndElement();
    xml.writeEndElement();
}

void writeXmlRpcDoubleMember(QXmlStreamWriter &xml, const QString &name, qint64 value)
{
    xml.writeStartElement(QStringLiteral("member"));
    xml.writeTextElement(QStringLiteral("name"), name);
    xml.writeStartElement(QStringLiteral("value"));
    xml.writeTextElement(QStringLiteral("double"), QString::number(value));
    xml.writeEndElement();
    xml.writeEndElement();
}

} // namespace

SubtitleDownloader::SubtitleDownloader(QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
{
    connect(m_manager, &QNetworkAccessManager::finished,
            this, &SubtitleDownloader::onReplyFinished);

    const QByteArray urlEnv = qgetenv("ELEMENTS_EXTRACT_OS_API_URL");
    m_baseUrl = urlEnv.isEmpty() ? QString::fromUtf8(kBaseUrl)
                                 : QString::fromUtf8(urlEnv).trimmed();
    const QByteArray xmlEnv = qgetenv("ELEMENTS_EXTRACT_OS_XMLRPC_URL");
    m_xmlRpcUrl = xmlEnv.isEmpty() ? QString::fromUtf8(kXmlRpcUrl)
                                   : QString::fromUtf8(xmlEnv).trimmed();

    // MainWindow solo llama a setCredentials(): se relee el backend también ahí
    // para reflejar cambios de Ajustes sin reconstruir el downloader.
    readBackendSetting();

    const QByteArray env = qgetenv("ELEMENTS_EXTRACT_OS_API_KEY");
    if (!env.isEmpty()) {
        m_apiKey = QString::fromUtf8(env).trimmed();
        qInfo() << "[ONLINE-SUBS] API key override from environment";
    }
}

SubtitleDownloader::~SubtitleDownloader()
{
    cancel();
}

void SubtitleDownloader::setCredentials(const QString &apiKey, const QString &user,
                                        const QString &password)
{
    // La variable de entorno tiene prioridad sobre la clave guardada en Ajustes.
    const QByteArray env = qgetenv("ELEMENTS_EXTRACT_OS_API_KEY");
    m_apiKey = env.isEmpty() ? apiKey.trimmed() : QString::fromUtf8(env).trimmed();
    m_user = user.trimmed();
    m_password = password;
    // Un cambio de credenciales invalida la sesión cacheada.
    m_token.clear();
    m_xmlRpcToken.clear();
    readBackendSetting();
    qInfo() << "[ONLINE-SUBS] credentials updated, backend:" << m_backend;
}

void SubtitleDownloader::readBackendSetting()
{
    QSettings settings("LocalApp", "FFmpegExtractor");
    const QString value = settings.value(QStringLiteral("onlineSubs/backend"),
                                         QStringLiteral("xmlrpc")).toString();
    m_backend = (value == QLatin1String("rest")) ? QStringLiteral("rest")
                                                 : QStringLiteral("xmlrpc");
}

bool SubtitleDownloader::backendHasCredentials(const QString &backend) const
{
    if (backend == QLatin1String("xmlrpc"))
        return true;   // la búsqueda anónima está permitida
    return !m_apiKey.isEmpty() && !m_user.isEmpty() && !m_password.isEmpty();
}

bool SubtitleDownloader::hasCredentials() const
{
    return backendHasCredentials(m_backend);
}

void SubtitleDownloader::finishSearchFailure(const QString &error)
{
    if (!m_fallbackAttempted) {
        const QString other = m_backend == QLatin1String("xmlrpc")
                                  ? QStringLiteral("rest") : QStringLiteral("xmlrpc");
        if (backendHasCredentials(other)) {
            m_searchError = error;
            m_fallbackAttempted = true;
            m_backend = other;
            m_hashResults.clear();
            m_pendingSearchStep = Step::None;
            emit statusMessage(other == QLatin1String("xmlrpc")
                                   ? tr("Cambiando al backend XML-RPC…")
                                   : tr("Cambiando al backend API REST…"));
            startSearchByHash();
            return;
        }
    }
    const QString original = m_fallbackAttempted && !m_searchError.isEmpty()
                                 ? m_searchError : error;
    emit statusMessage(original);
    emit searchFinished(m_currentVideoPath, {}, original);
}

QNetworkRequest SubtitleDownloader::makeRequest(const QUrl &url, bool post,
                                                 bool withApiKey) const
{
    QNetworkRequest request(url);
    // La API responde 301 si la URL no es canónica: seguimos redirecciones.
    setFollowRedirects(request);
    if (withApiKey)
        request.setRawHeader("Api-Key", m_apiKey.toUtf8());
    request.setRawHeader("User-Agent", kUserAgent);
    if (post)
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    return request;
}

QNetworkRequest SubtitleDownloader::makeXmlRpcRequest() const
{
    QNetworkRequest request{QUrl(m_xmlRpcUrl)};
    setFollowRedirects(request);
    request.setRawHeader("User-Agent", kUserAgent);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml");
    return request;
}

QByteArray SubtitleDownloader::xmlRpcLoginBody() const
{
    QByteArray body;
    QXmlStreamWriter xml(&body);
    xml.setAutoFormatting(false);
    xml.writeStartDocument();
    xml.writeStartElement(QStringLiteral("methodCall"));
    xml.writeTextElement(QStringLiteral("methodName"), QStringLiteral("LogIn"));
    xml.writeStartElement(QStringLiteral("params"));
    writeXmlRpcStringParam(xml, m_user);
    writeXmlRpcStringParam(xml, m_password);
    writeXmlRpcStringParam(xml, QStringLiteral("en"));
    writeXmlRpcStringParam(xml, QString::fromUtf8(kXmlRpcAppName));
    xml.writeEndElement();
    xml.writeEndElement();
    xml.writeEndDocument();
    return body;
}

QByteArray SubtitleDownloader::xmlRpcSearchBody(bool byHash, const QString &hash,
                                                 qint64 size) const
{
    QByteArray body;
    QXmlStreamWriter xml(&body);
    xml.setAutoFormatting(false);
    xml.writeStartDocument();
    xml.writeStartElement(QStringLiteral("methodCall"));
    xml.writeTextElement(QStringLiteral("methodName"), QStringLiteral("SearchSubtitles"));
    xml.writeStartElement(QStringLiteral("params"));
    writeXmlRpcStringParam(xml, m_xmlRpcToken);
    xml.writeStartElement(QStringLiteral("param"));
    xml.writeStartElement(QStringLiteral("value"));
    xml.writeStartElement(QStringLiteral("array"));
    xml.writeStartElement(QStringLiteral("data"));
    xml.writeStartElement(QStringLiteral("value"));
    xml.writeStartElement(QStringLiteral("struct"));
    writeXmlRpcStringMember(xml, QStringLiteral("sublanguageid"),
                            toThreeLetterCode(m_language));
    if (byHash) {
        writeXmlRpcStringMember(xml, QStringLiteral("moviehash"), hash);
        writeXmlRpcDoubleMember(xml, QStringLiteral("moviebytesize"), size);
    } else {
        writeXmlRpcStringMember(xml, QStringLiteral("query"), m_query);
        QString season, episode;
        if (parseSeasonEpisode(m_query, &season, &episode)) {
            writeXmlRpcStringMember(xml, QStringLiteral("season"),
                                    QString::number(season.toInt()));
            writeXmlRpcStringMember(xml, QStringLiteral("episode"),
                                    QString::number(episode.toInt()));
        }
    }
    xml.writeEndElement();   // struct
    xml.writeEndElement();   // value
    xml.writeEndElement();   // data
    xml.writeEndElement();   // array
    xml.writeEndElement();   // value
    xml.writeEndElement();   // param
    xml.writeEndElement();   // params
    xml.writeEndElement();   // methodCall
    xml.writeEndDocument();
    return body;
}

QString SubtitleDownloader::computeMovieHash(const QString &videoPath, qint64 *fileSize)
{
    QFile file(videoPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "[ONLINE-SUBS] cannot open video for hashing:" << videoPath;
        return QString();
    }
    const qint64 size = file.size();
    if (fileSize) *fileSize = size;
    if (size <= 0) return QString();

    constexpr int blockSize = 64 * 1024;
    QByteArray bytes;
    bytes.reserve(2 * blockSize);
    bytes.append(file.read(blockSize));
    file.seek(qMax<qint64>(0, size - blockSize));
    bytes.append(file.read(blockSize));
    file.close();

    quint64 lo = static_cast<quint64>(size);
    quint64 hi = 0;
    constexpr quint64 modulus = Q_UINT64_C(1) << 32;

    for (int i = 0; i + 8 <= bytes.size(); i += 8) {
        const auto byte = [&bytes](int pos) -> quint64 {
            return static_cast<unsigned char>(bytes.at(pos));
        };
        const quint64 a = byte(i), b = byte(i + 1), c = byte(i + 2), d = byte(i + 3);
        const quint64 e = byte(i + 4), f = byte(i + 5), g = byte(i + 6), h = byte(i + 7);

        lo += a + (b * 256) + (c * 65536) + (d * 16777216);
        hi += e + (f * 256) + (g * 65536) + (h * 16777216);

        if (lo >= modulus) {
            const quint64 overflow = lo / modulus;
            lo -= overflow * modulus;
            hi += overflow;
        }
        if (hi >= modulus)
            hi %= modulus;
    }

    return QStringLiteral("%1%2").arg(hi, 8, 16, QLatin1Char('0'))
                                 .arg(lo, 8, 16, QLatin1Char('0'));
}

QList<OnlineSubtitle> SubtitleDownloader::mergeResults(
    const QList<OnlineSubtitle> &hashResults,
    const QList<OnlineSubtitle> &queryResults)
{
    QList<OnlineSubtitle> merged = hashResults;
    for (const OnlineSubtitle &item : queryResults) {
        const bool duplicate = std::any_of(merged.constBegin(), merged.constEnd(),
            [&item](const OnlineSubtitle &existing) {
                if (!item.fileId.isEmpty() && !existing.fileId.isEmpty())
                    return existing.fileId == item.fileId;
                return existing.language == item.language && existing.release == item.release;
            });
        if (!duplicate) merged.append(item);
    }
    return merged;
}

void SubtitleDownloader::search(const QString &videoPath, const QString &query,
                                const QString &language)
{
    m_cancelled = false;
    m_videoPath = videoPath;
    m_currentVideoPath = videoPath;
    m_query = query.trimmed();
    m_language = language.trimmed();
    m_hashResults.clear();
    m_download = DownloadState();
    m_fallbackAttempted = false;
    m_searchError.clear();
    m_pendingSearchStep = Step::None;
    readBackendSetting();

    if (!hasCredentials()) {
        const QString error = tr("Faltan credenciales de OpenSubtitles para la API REST (clave API, usuario y contraseña). Configúralas en Ajustes → Subtítulos online.");
        emit statusMessage(error);
        emit searchFinished(m_currentVideoPath, {}, error);
        return;
    }
    startSearchByHash();
}

QUrl SubtitleDownloader::restSearchUrl(bool byHash, const QString &hash, qint64 size) const
{
    QUrl url(m_baseUrl + "subtitles");
    QUrlQuery params;
    if (byHash) {
        params.addQueryItem("moviehash", hash);
        params.addQueryItem("moviebytesize", QString::number(size));
    } else {
        params.addQueryItem("query", m_query);
        QString season, episode;
        if (parseSeasonEpisode(m_query, &season, &episode)) {
            params.addQueryItem("type", "episode");
            params.addQueryItem("season_number", season);
            params.addQueryItem("episode_number", episode);
        } else {
            params.addQueryItem("type", "movie");
        }
    }
    if (!m_language.isEmpty() && m_language != "all")
        params.addQueryItem("languages", m_language);
    url.setQuery(params);
    return url;
}

void SubtitleDownloader::startXmlRpcLogin()
{
    m_step = Step::XmlRpcLoginForSearch;
    emit statusMessage(tr("Iniciando sesión en OpenSubtitles (XML-RPC)…"));
    qInfo() << "[ONLINE-SUBS] XML-RPC login";
    m_activeReply = m_manager->post(makeXmlRpcRequest(), xmlRpcLoginBody());
}

void SubtitleDownloader::startSearchByHash()
{
    qint64 size = 0;
    const QString hash = m_videoPath.isEmpty() ? QString() : computeMovieHash(m_videoPath, &size);
    if (hash.isEmpty()) {
        qInfo() << "[ONLINE-SUBS] no hash available for" << m_videoPath;
        startSearchByQuery();
        return;
    }
    if (m_backend == QLatin1String("xmlrpc") && m_xmlRpcToken.isEmpty()) {
        m_pendingSearchStep = Step::SearchByHash;
        startXmlRpcLogin();
        return;
    }

    m_step = Step::SearchByHash;
    emit statusMessage(tr("Buscando subtítulos por hash…"));
    qInfo() << "[ONLINE-SUBS] searching by hash" << hash << size;
    if (m_backend == QLatin1String("xmlrpc"))
        m_activeReply = m_manager->post(makeXmlRpcRequest(), xmlRpcSearchBody(true, hash, size));
    else
        m_activeReply = m_manager->get(makeRequest(restSearchUrl(true, hash, size), false));
}

void SubtitleDownloader::startSearchByQuery()
{
    if (m_query.isEmpty()) {
        const QString error = tr("Introduce un título para buscar subtítulos.");
        emit statusMessage(error);
        emit searchFinished(m_currentVideoPath, {}, error);
        m_step = Step::None;
        return;
    }

    if (m_backend == QLatin1String("xmlrpc") && m_xmlRpcToken.isEmpty()) {
        m_pendingSearchStep = Step::SearchByQuery;
        startXmlRpcLogin();
        return;
    }

    m_step = Step::SearchByQuery;
    emit statusMessage(tr("Buscando subtítulos por título…"));
    qInfo() << "[ONLINE-SUBS] searching by query" << m_query;
    if (m_backend == QLatin1String("xmlrpc"))
        m_activeReply = m_manager->post(makeXmlRpcRequest(),
                                        xmlRpcSearchBody(false, QString(), 0));
    else
        m_activeReply = m_manager->get(makeRequest(restSearchUrl(false, QString(), 0), false));
}

void SubtitleDownloader::download(const OnlineSubtitle &item, const QString &targetPath,
                                  const QString &videoPath)
{
    m_cancelled = false;
    if (!videoPath.isEmpty())
        m_currentVideoPath = videoPath;
    m_download.item = item;
    m_download.targetPath = targetPath;
    m_download.authRetried = false;

    if (!item.downloadUrl.isEmpty()) {
        // XML-RPC: ZipDownloadLink directo, sin login ni petición /download.
        startLinkFetch(QUrl(item.downloadUrl), false);
        return;
    }
    if (!hasCredentials()) {
        const QString error = tr("Faltan credenciales de OpenSubtitles para la API REST (clave API, usuario y contraseña). Configúralas en Ajustes → Subtítulos online.");
        emit statusMessage(error);
        emit downloadFinished(m_currentVideoPath, QString(), error);
        return;
    }
    if (item.fileId.trimmed().isEmpty()) {
        emit downloadFinished(m_currentVideoPath, QString(), tr("El subtítulo seleccionado no tiene identificador de archivo ni enlace de descarga."));
        return;
    }
    if (m_token.isEmpty()) {
        startLogin();
    } else {
        startDownloadRequest();
    }
}

void SubtitleDownloader::startLogin()
{
    if (m_user.isEmpty() || m_password.isEmpty()) {
        const QString error = tr("Faltan el usuario o la contraseña de OpenSubtitles. Configúralos en Ajustes → Subtítulos online.");
        emit statusMessage(error);
        emit downloadFinished(m_currentVideoPath, QString(), error);
        m_step = Step::None;
        return;
    }

    QJsonObject body;
    body.insert("username", m_user);
    body.insert("password", m_password);

    m_step = Step::LoginForDownload;
    emit statusMessage(tr("Iniciando sesión en OpenSubtitles…"));
    qInfo() << "[ONLINE-SUBS] logging in";
    m_activeReply = m_manager->post(makeRequest(QUrl(m_baseUrl + "login"), true),
                                    QJsonDocument(body).toJson(QJsonDocument::Compact));
}

void SubtitleDownloader::startDownloadRequest()
{
    QJsonObject body;
    bool ok = false;
    const qlonglong numericId = m_download.item.fileId.toLongLong(&ok);
    body.insert("file_id", ok ? QJsonValue(numericId) : QJsonValue(m_download.item.fileId));

    QNetworkRequest request = makeRequest(QUrl(m_baseUrl + "download"), true);
    request.setRawHeader("Authorization", "Bearer " + m_token.toUtf8());

    m_step = Step::RequestDownload;
    emit statusMessage(tr("Solicitando el subtítulo…"));
    qInfo() << "[ONLINE-SUBS] requesting download for file_id" << m_download.item.fileId;
    m_activeReply = m_manager->post(request, QJsonDocument(body).toJson(QJsonDocument::Compact));
}

void SubtitleDownloader::startLinkFetch(const QUrl &url, bool withApiKey)
{
    m_step = Step::FetchLink;
    emit statusMessage(tr("Descargando el subtítulo…"));
    qInfo() << "[ONLINE-SUBS] fetching subtitle link";
    m_activeReply = m_manager->get(makeRequest(url, false, withApiKey));
}

void SubtitleDownloader::cancel()
{
    if (m_step == Step::None && !m_activeReply)
        return;
    const Step previous = m_step;
    m_cancelled = true;
    if (m_activeReply) {
        m_activeReply->abort();
        m_activeReply = nullptr;
    }
    m_step = Step::None;
    emit statusMessage(tr("Operación cancelada."));
    if (previous == Step::SearchByHash || previous == Step::SearchByQuery
        || previous == Step::XmlRpcLoginForSearch)
        emit searchFinished(m_currentVideoPath, {}, tr("Búsqueda cancelada."));
    else
        emit downloadFinished(m_currentVideoPath, QString(), tr("Descarga cancelada."));
}

void SubtitleDownloader::onReplyFinished(QNetworkReply *reply)
{
    if (!reply) return;
    reply->deleteLater();
    if (reply != m_activeReply) return;
    m_activeReply = nullptr;

    if (m_cancelled) {
        m_cancelled = false;   // cancel() ya emitió la señal correspondiente
        return;
    }

    const Step step = m_step;
    m_step = Step::None;

    // Qt marca el 401 como error de red; hay que detectarlo aquí para
    // renovar el token antes de caer en la ruta genérica de error.
    const int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (step == Step::RequestDownload && httpStatus == 401 && !m_download.authRetried) {
        m_token.clear();
        m_download.authRetried = true;
        emit statusMessage(tr("Sesión caducada; iniciando sesión de nuevo…"));
        startLogin();
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        const QString error = replyErrorMessage(reply);
        qWarning() << "[ONLINE-SUBS] network error:" << error;
        if (step == Step::SearchByHash) {
            emit statusMessage(tr("Búsqueda por hash no disponible; probando por título…"));
            startSearchByQuery();
        } else if (step == Step::SearchByQuery) {
            if (!m_hashResults.isEmpty()) {
                emit statusMessage(tr("Búsqueda por título no disponible; se muestran los resultados por hash."));
                emit searchFinished(m_currentVideoPath, m_hashResults, QString());
                m_hashResults.clear();
            } else {
                finishSearchFailure(error);
            }
        } else if (step == Step::XmlRpcLoginForSearch) {
            finishSearchFailure(error);
        } else {
            emit downloadFinished(m_currentVideoPath, QString(), error);
        }
        return;
    }

    switch (step) {
    case Step::SearchByHash:
    case Step::SearchByQuery:
        handleSearchReply(reply, step == Step::SearchByHash);
        break;
    case Step::XmlRpcLoginForSearch:
        handleXmlRpcLoginReply(reply);
        break;
    case Step::LoginForDownload:
        handleLoginReply(reply);
        break;
    case Step::RequestDownload:
        handleDownloadRequestReply(reply);
        break;
    case Step::FetchLink:
        handleLinkReply(reply);
        break;
    case Step::None:
        break;
    }
}

QString SubtitleDownloader::replyErrorMessage(QNetworkReply *reply)
{
    const QByteArray body = reply->readAll();
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(body, &parseError);
    QString message;
    if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
        const QJsonObject obj = doc.object();
        message = obj.value("message").toString();
        if (message.isEmpty()) message = obj.value("error").toString();
    }
    if (message.isEmpty())
        message = QString::fromUtf8(body.left(300)).trimmed();
    if (message.isEmpty())
        message = reply->errorString();

    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (status > 0)
        return tr("HTTP %1: %2").arg(status).arg(message);
    return message;
}

QList<OnlineSubtitle> SubtitleDownloader::parseXmlRpcSearchResults(
    const QByteArray &data, QString *error)
{
    bool isFault = false;
    const QVariant response = parseXmlRpcResponse(data, error, &isFault);
    if (isFault || (error && !error->isEmpty())) return {};

    QVariantList rows;
    const QVariantMap wrapper = response.toMap();
    if (!wrapper.isEmpty())
        rows = wrapper.value(QStringLiteral("data")).toList();
    else
        rows = response.toList();

    QList<OnlineSubtitle> results;
    for (const QVariant &row : rows) {
        const QVariantMap fields = row.toMap();
        if (fields.isEmpty()) continue;
        OnlineSubtitle item;
        item.fileId = fields.value(QStringLiteral("IDSubtitleFile")).toString();
        item.release = fields.value(QStringLiteral("MovieReleaseName")).toString().trimmed();
        if (item.release.isEmpty())
            item.release = fields.value(QStringLiteral("SubFileName")).toString().trimmed();
        item.language = toTwoLetterCode(fields.value(QStringLiteral("SubLanguageID")).toString());
        item.year = fields.value(QStringLiteral("Year")).toString();
        if (item.year.isEmpty())
            item.year = fields.value(QStringLiteral("MovieYear")).toString();
        item.downloads = fields.value(QStringLiteral("SubDownloadsCnt")).toInt();
        item.hashMatch = fields.value(QStringLiteral("MatchedBy")).toString()
                             .contains(QStringLiteral("moviehash"), Qt::CaseInsensitive);
        item.downloadUrl = fields.value(QStringLiteral("ZipDownloadLink")).toString();
        results.append(item);
    }
    return results;
}

void SubtitleDownloader::handleXmlRpcLoginReply(QNetworkReply *reply)
{
    QString error;
    bool isFault = false;
    const QVariant response = parseXmlRpcResponse(reply->readAll(), &error, &isFault);
    const QString token = response.toMap().value(QStringLiteral("token")).toString();
    if (token.isEmpty()) {
        if (error.isEmpty())
            error = tr("OpenSubtitles (XML-RPC) no devolvió un token de sesión.");
        finishSearchFailure(error);
        return;
    }
    m_xmlRpcToken = token;   // solo en memoria
    qInfo() << "[ONLINE-SUBS] XML-RPC login successful";
    const Step pending = m_pendingSearchStep;
    m_pendingSearchStep = Step::None;
    if (pending == Step::SearchByQuery)
        startSearchByQuery();
    else
        startSearchByHash();
}

void SubtitleDownloader::handleSearchReply(QNetworkReply *reply, bool wasHashSearch)
{
    const QByteArray body = reply->readAll();
    QList<OnlineSubtitle> results;

    if (m_backend == QLatin1String("xmlrpc")) {
        QString parseError;
        results = parseXmlRpcSearchResults(body, &parseError);
        if (!parseError.isEmpty()) {
            m_hashResults.clear();
            finishSearchFailure(parseError);
            return;
        }
    } else {
        QJsonParseError parseError;
        const QJsonDocument doc = QJsonDocument::fromJson(body, &parseError);
        if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
            m_hashResults.clear();
            finishSearchFailure(tr("La respuesta de OpenSubtitles no es un JSON válido."));
            return;
        }

        const QJsonArray data = doc.object().value("data").toArray();
        for (const QJsonValue &value : data) {
            const QJsonObject attrs = value.toObject().value("attributes").toObject();
            OnlineSubtitle item;
            item.language = attrs.value("language").toString();
            item.release = attrs.value("release").toString();
            if (item.release.isEmpty())
                item.release = attrs.value("feature_details").toObject().value("title").toString();
            item.year = attrs.value("year").toVariant().toString();
            if (item.year.isEmpty())
                item.year = attrs.value("feature_details").toObject()
                                .value("year").toVariant().toString();
            item.downloads = attrs.value("download_count").toInt();
            item.hashMatch = attrs.value("moviehash_match").toBool();
            item.uploader = attrs.value("uploader").toObject().value("name").toString();
            item.url = attrs.value("url").toString();
            const QJsonArray files = attrs.value("files").toArray();
            if (!files.isEmpty())
                item.fileId = files.first().toObject().value("file_id").toVariant().toString();
            results.append(item);
        }
    }

    qInfo() << "[ONLINE-SUBS] search returned" << results.size() << "results";

    // El hash solo encuentra subtítulos subidos para esa Versión concreta: suele
    // devolver únicamente inglés aunque el mismo título tenga otros idiomas.
    // Se completa siempre con la búsqueda por título y se fusionan las dos listas.
    if (wasHashSearch) {
        m_hashResults = results;
        if (!m_query.isEmpty()) {
            emit statusMessage(tr("Buscando también por título…"));
            startSearchByQuery();
            return;
        }
    } else {
        results = mergeResults(m_hashResults, results);
        m_hashResults.clear();
    }

    const QString preferred = m_language.trimmed().toLower();
    std::stable_sort(results.begin(), results.end(),
                     [&preferred](const OnlineSubtitle &a, const OnlineSubtitle &b) {
                         const bool aPreferred = !preferred.isEmpty()
                             && a.language.trimmed().toLower() == preferred;
                         const bool bPreferred = !preferred.isEmpty()
                             && b.language.trimmed().toLower() == preferred;
                         if (aPreferred != bPreferred) return aPreferred > bPreferred;
                         if (a.hashMatch != b.hashMatch) return a.hashMatch > b.hashMatch;
                         return a.downloads > b.downloads;
                     });

    if (results.isEmpty()) {
        finishSearchFailure(tr("No se encontraron subtítulos."));
        return;
    }
    emit searchFinished(m_currentVideoPath, results, QString());
}

void SubtitleDownloader::handleLoginReply(QNetworkReply *reply)
{
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &parseError);
    const QString token = (parseError.error == QJsonParseError::NoError && doc.isObject())
                              ? doc.object().value("token").toString()
                              : QString();
    if (token.isEmpty()) {
        const QString error = tr("OpenSubtitles no devolvió un token de sesión.");
        emit statusMessage(error);
        emit downloadFinished(m_currentVideoPath, QString(), error);
        return;
    }
    m_token = token;   // solo en memoria
    qInfo() << "[ONLINE-SUBS] login successful";
    startDownloadRequest();
}

void SubtitleDownloader::handleDownloadRequestReply(QNetworkReply *reply)
{
    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (status == 401 && !m_download.authRetried) {
        m_token.clear();
        m_download.authRetried = true;
        emit statusMessage(tr("Sesión caducada; iniciando sesión de nuevo…"));
        startLogin();
        return;
    }

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        emit downloadFinished(m_currentVideoPath, QString(), tr("La respuesta de OpenSubtitles no es un JSON válido."));
        return;
    }
    const QJsonObject obj = doc.object();
    const QString link = obj.value("link").toString();
    if (!link.isEmpty()) {
        startLinkFetch(QUrl(link));
        return;
    }
    const QJsonValue content = obj.value("content");
    if (!content.isNull() && content.isString()) {
        QByteArray bytes = content.toString().toUtf8();
        // La API puede devolver el contenido en base64.
        const QByteArray decoded = QByteArray::fromBase64(bytes, QByteArray::AbortOnBase64DecodingErrors);
        if (!decoded.isEmpty()) bytes = decoded;
        QString outPath;
        QString error;
        if (!writeSubtitleBytes(bytes, m_download.targetPath, &outPath, &error)) {
            emit downloadFinished(m_currentVideoPath, QString(), error);
            return;
        }
        emit downloadFinished(m_currentVideoPath, outPath, QString());
        return;
    }
    emit downloadFinished(m_currentVideoPath, QString(), tr("La respuesta de descarga no contiene un enlace ni contenido."));
}

void SubtitleDownloader::handleLinkReply(QNetworkReply *reply)
{
    QString outPath;
    QString error;
    if (!writeSubtitleBytes(reply->readAll(), m_download.targetPath, &outPath, &error)) {
        emit downloadFinished(m_currentVideoPath, QString(), error);
        return;
    }
    qInfo() << "[ONLINE-SUBS] subtitle saved:" << outPath;
    emit statusMessage(tr("Subtítulo descargado: %1").arg(QFileInfo(outPath).fileName()));
    emit downloadFinished(m_currentVideoPath, outPath, QString());
}

QString SubtitleDownloader::detectSubtitleExtension(const QByteArray &data)
{
    const QString head = QString::fromUtf8(data.left(4096));
    if (QRegularExpression(QStringLiteral("^\\s*WEBVTT")).match(head).hasMatch())
        return "vtt";
    if (head.contains(QStringLiteral("[Script Info]"), Qt::CaseInsensitive))
        return "ass";
    if (QRegularExpression(QStringLiteral("\\d{1,2}:\\d{2}:\\d{2}[.,]\\d{1,3}"))
            .match(head).hasMatch())
        return "srt";
    return QString();
}

bool SubtitleDownloader::isRarData(const QByteArray &data)
{
    return looksLikeRar(data);
}

QStringList SubtitleDownloader::listZipEntries(const QString &archivePath, QString *error)
{
    QProcess process;
    QString program;
#ifdef Q_OS_WIN
    program = "powershell";
    const QString script = QStringLiteral(
        "$ErrorActionPreference='Stop'; Add-Type -AssemblyName System.IO.Compression.FileSystem; "
        "$z=[System.IO.Compression.ZipFile]::OpenRead('%1'); "
        "try { $z.Entries | ForEach-Object { $_.FullName } } finally { $z.Dispose() }")
        .arg(quotePs(archivePath));
    process.start(program, {"-NoProfile", "-NonInteractive", "-Command", script});
#else
    program = "unzip";
    process.start(program, {"-Z1", archivePath});
#endif

    if (!process.waitForStarted(5000)) {
        if (error) *error = tr("No se pudo listar el ZIP: no se encontró %1.").arg(program);
        return {};
    }
    if (!process.waitForFinished(20000)) {
        process.kill();
        process.waitForFinished();
        if (error) *error = tr("No se pudo listar el ZIP: tiempo de espera agotado.");
        return {};
    }
    if (process.exitCode() != 0) {
        const QString detail = QString::fromLocal8Bit(process.readAllStandardError()).trimmed();
        if (error) *error = tr("No se pudo listar el ZIP: %1")
                                .arg(detail.isEmpty() ? QString::number(process.exitCode()) : detail);
        return {};
    }

    QStringList entries;
    const QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
    for (const QString &line : output.split('\n')) {
        const QString entry = line.trimmed();
        if (!entry.isEmpty()) entries.append(entry);
    }
    return entries;
}

QStringList SubtitleDownloader::subtitleEntries(const QStringList &entries)
{
    static const QStringList extensions = {"srt", "ass", "ssa", "vtt"};
    QStringList result;
    for (const QString &entry : entries) {
        if (entry.endsWith('/')) continue;
        const QString suffix = QFileInfo(entry).suffix().toLower();
        if (extensions.contains(suffix)) result.append(entry);
    }
    return result;
}

int SubtitleDownloader::entryScore(const QString &entry, const QString &language)
{
    const QString name = entry.toLower();
    int score = 0;

    // (a) El nombre contiene el idioma solicitado (en, pt-BR, eng, English…).
    if (!language.trimmed().isEmpty()) {
        QStringList tokens;
        const QString lang = language.trimmed().toLower();
        const QString base = lang.section('-', 0, 0);
        tokens << lang << base;
        if (base == "en") tokens << "eng" << "english";
        if (base == "es") tokens << "spa" << "spanish";
        if (base == "fr") tokens << "fra" << "fre" << "french";
        if (base == "pt") tokens << "por" << "portuguese" << "ptbr" << "pb";
        if (base == "de") tokens << "deu" << "ger" << "german";
        if (base == "it") tokens << "ita" << "italian";
        tokens.removeDuplicates();

        for (const QString &token : tokens) {
            const QRegularExpression rx(QStringLiteral("(?:^|[^a-z])")
                                        + QRegularExpression::escape(token)
                                        + QStringLiteral("(?:[^a-z]|$)"));
            if (rx.match(name).hasMatch()) { score += 100; break; }
        }
    }

    // (b) SRT primero; (c) el orden original del ZIP desempata (primer archivo).
    if (name.endsWith(QStringLiteral(".srt"))) score += 10;
    else if (name.endsWith(QStringLiteral(".ass"))
             || name.endsWith(QStringLiteral(".ssa"))
             || name.endsWith(QStringLiteral(".vtt"))) score += 5;
    return score;
}

QString SubtitleDownloader::selectArchiveEntry(const QStringList &entries)
{
    if (m_chosenEntry.isEmpty() || !entries.contains(m_chosenEntry)) {
        QList<QPair<int, QString>> scored;
        for (const QString &entry : entries)
            scored.append(qMakePair(entryScore(entry, m_download.item.language), entry));
        std::stable_sort(scored.begin(), scored.end(),
                         [](const QPair<int, QString> &a, const QPair<int, QString> &b) {
                             return a.first > b.first;
                         });

        const int best = scored.isEmpty() ? 0 : scored.first().first;
        QStringList tied;
        for (const QPair<int, QString> &candidate : scored) {
            if (candidate.first == best) tied.append(candidate.second);
        }
        if (tied.size() > 1) {
            m_chosenEntry.clear();
            emit needFileChoice(tied, m_download.item.language);
            if (!m_chosenEntry.isEmpty() && tied.contains(m_chosenEntry)) {
                const QString chosen = m_chosenEntry;
                m_chosenEntry.clear();
                return chosen;
            }
        }
        return scored.isEmpty() ? QString() : scored.first().second;
    }

    const QString chosen = m_chosenEntry;
    m_chosenEntry.clear();
    return chosen;
}

void SubtitleDownloader::setChosenArchiveEntry(const QString &entry)
{
    m_chosenEntry = entry.trimmed();
}

bool SubtitleDownloader::writeSubtitleBytes(const QByteArray &raw, const QString &targetPath,
                                            QString *outPath, QString *error)
{
    m_chosenEntry.clear();
    if (raw.isEmpty()) {
        if (error) *error = tr("El subtítulo descargado está vacío.");
        return false;
    }

    QTemporaryDir temp;
    if (!temp.isValid()) {
        if (error) *error = tr("No se pudo crear un directorio temporal para descomprimir.");
        return false;
    }

    QByteArray data = raw;
    if (isZipData(raw) || isGzipData(raw) || isRarData(raw)) {
        const QString suffix = isZipData(raw) ? "zip" : (isGzipData(raw) ? "gz" : "rar");
        const QString archive = temp.filePath("subtitle." + suffix);
        const QString extracted = temp.filePath("subtitle.out");
        QFile file(archive);
        if (!file.open(QIODevice::WriteOnly) || file.write(raw) != raw.size()) {
            if (error) *error = tr("No se pudo guardar el archivo comprimido temporal.");
            return false;
        }
        file.close();

        bool ok = false;
        if (isZipData(raw)) {
            QString listError;
            const QStringList entries = listZipEntries(archive, &listError);
            if (entries.isEmpty()) {
                if (error) *error = listError.isEmpty()
                            ? tr("El ZIP no contiene archivos.")
                            : listError;
                return false;
            }
            const QStringList candidates = subtitleEntries(entries);
            if (candidates.isEmpty()) {
                if (error) *error = tr("El ZIP no contiene subtítulos de texto (.srt/.ass/.ssa/.vtt).");
                return false;
            }
            const QString entry = selectArchiveEntry(candidates);
            qInfo() << "[ONLINE-SUBS] ZIP entry selected:" << entry;
            emit statusMessage(tr("Extrayendo %1 del ZIP…").arg(entry));
            ok = extractZipEntry(archive, entry, extracted, error);
        } else if (isGzipData(raw)) {
            ok = extractGzip(archive, extracted, error);
        } else {
            emit statusMessage(tr("Extrayendo subtítulo de un archivo RAR…"));
            ok = extractRar(archive, extracted, error);
        }
        if (!ok) return false;
        QFile result(extracted);
        if (!result.open(QIODevice::ReadOnly)) {
            if (error) *error = tr("No se pudo leer el subtítulo descomprimido.");
            return false;
        }
        data = result.readAll();
        result.close();
        if (data.isEmpty()) {
            if (error) *error = tr("El archivo comprimido no contiene un subtítulo.");
            return false;
        }
    }

    QString extension = detectSubtitleExtension(data);
    if (extension.isEmpty()) extension = "srt";

    const QFileInfo info(targetPath);
    const QString path = info.absolutePath() + "/" + info.completeBaseName() + "." + extension;
    if (isVipDecoy(data)) {
        QFile::remove(path);
        qWarning() << "[ONLINE-SUBS] VIP decoy subtitle rejected:" << path;
        if (error) {
            *error = tr("OpenSubtitles devolvió un subtítulo de prueba VIP; inicia sesión con una cuenta con permisos de descarga o usa la API REST.");
        }
        return false;
    }
    QDir().mkpath(info.absolutePath());
    QFile output(path);
    if (!output.open(QIODevice::WriteOnly | QIODevice::Truncate)
        || output.write(data) != data.size()) {
        if (error) *error = tr("No se pudo escribir el subtítulo en %1.").arg(path);
        return false;
    }
    output.close();
    if (outPath) *outPath = path;
    qInfo() << "[ONLINE-SUBS] detected extension ." << extension;
    return true;
}
