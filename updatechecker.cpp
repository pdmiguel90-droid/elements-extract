#include "updatechecker.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QTimer>

namespace {

const char *kReleasesUrl =
    "https://api.github.com/repos/pdmiguel90-droid/elements-extract/releases/latest";

// Los tags pueden ser "v1.2.9", "1.2.9" o "release-1.2.9"; solo importan
// los componentes numéricos para comparar con la versión compilada.
QString normalizeVersion(const QString &tag)
{
    QRegularExpression rx(QStringLiteral("(\\d+)\\.(\\d+)(?:\\.(\\d+))?"));
    const auto m = rx.match(tag);
    if (!m.hasMatch()) return QString();
    return QStringLiteral("%1.%2.%3")
        .arg(m.captured(1), m.captured(2), m.captured(3).isEmpty() ? QStringLiteral("0") : m.captured(3));
}

bool isNewerVersion(const QString &remote, const QString &current)
{
    const auto parts = [](const QString &v) {
        const QStringList p = v.split(QLatin1Char('.'));
        return QList<int>{p.value(0).toInt(), p.value(1).toInt(), p.value(2).toInt()};
    };
    const QList<int> r = parts(remote), c = parts(current);
    for (int i = 0; i < 3; ++i) {
        if (r[i] != c[i]) return r[i] > c[i];
    }
    return false;
}

} // namespace

UpdateChecker::UpdateChecker(QObject *parent)
    : QObject(parent), m_manager(new QNetworkAccessManager(this))
{
    qRegisterMetaType<UpdateInfo>("UpdateInfo");
    connect(m_manager, &QNetworkAccessManager::finished,
            this, &UpdateChecker::onFinished);
}

UpdateChecker::~UpdateChecker() = default;

void UpdateChecker::check()
{
    if (m_inFlight) return;
    m_inFlight = true;

    // Variable interna de prueba: permite apuntar a un JSON local sin tocar
    // la URL pública del Release más reciente.
    const QByteArray overrideUrl = qgetenv("ELEMENTS_EXTRACT_RELEASES_URL");
    const QUrl url = overrideUrl.isEmpty()
                         ? QUrl(QString::fromLatin1(kReleasesUrl))
                         : QUrl(QString::fromUtf8(overrideUrl));
    QNetworkRequest request{url};
    request.setRawHeader("User-Agent", "Elements-Extract/1.2");
    request.setRawHeader("Accept", "application/vnd.github+json");
    // API válida en todo Qt 6 (6.2–6.9).
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    // Sin conexión no debe bloquear ni siquiera el arranque: 10 s de margen.
    request.setTransferTimeout(10000);
#endif
    m_manager->get(request);
}

void UpdateChecker::onFinished(QNetworkReply *reply)
{
    m_inFlight = false;
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        emit checkFailed(reply->errorString());
        return;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    if (!doc.isObject()) {
        emit checkFailed(tr("La respuesta de GitHub no es válida."));
        return;
    }

    const QJsonObject obj = doc.object();
    const QString tag = obj.value(QStringLiteral("tag_name")).toString();
    const QString version = normalizeVersion(tag);
    if (version.isEmpty()) {
        emit checkFailed(tr("La versión publicada no se pudo interpretar."));
        return;
    }

    if (!isNewerVersion(version, QStringLiteral(ELEMENTS_EXTRACT_VERSION))) {
        emit upToDate();
        return;
    }

    UpdateInfo info;
    info.tag = tag;
    info.version = version;
    info.url = obj.value(QStringLiteral("html_url")).toString();
    info.notes = obj.value(QStringLiteral("body")).toString().trimmed();
    if (info.url.isEmpty())
        info.url = QStringLiteral("https://github.com/pdmiguel90-droid/elements-extract/releases");

    const QJsonArray assets = obj.value(QStringLiteral("assets")).toArray();
    for (const QJsonValue &a : assets) {
        const QString name = a.toObject().value(QStringLiteral("name")).toString();
        if (!name.isEmpty()) info.assets << name;
    }

    emit updateAvailable(info);
}
