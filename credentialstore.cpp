#include "credentialstore.h"

#include <QCoreApplication>
#include <QEventLoop>
#include <QTimer>

#ifdef ELEMENTS_EXTRACT_HAVE_SECRET_SERVICE
#  include <QtDBus/QtDBus>

namespace {

// Estructura Secret del estándar org.freedesktop.Secret: (oayays)
// sesión, parámetros, valor, tipo de contenido.
struct ElementsExtractSecret
{
    QDBusObjectPath session;
    QByteArray parameters;
    QByteArray value;
    QString contentType;
};

QDBusArgument &operator<<(QDBusArgument &arg, const ElementsExtractSecret &secret)
{
    arg.beginStructure();
    arg << secret.session << secret.parameters << secret.value << secret.contentType;
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, ElementsExtractSecret &secret)
{
    arg.beginStructure();
    arg >> secret.session >> secret.parameters >> secret.value >> secret.contentType;
    arg.endStructure();
    return arg;
}

} // namespace

Q_DECLARE_METATYPE(ElementsExtractSecret)

// Receptores de la señal Completed de un Prompt de Secret Service.
class SecretPromptWatcher : public QObject
{
    Q_OBJECT
public:
    explicit SecretPromptWatcher(QObject *parent = nullptr) : QObject(parent) {}

    bool completed = false;
    bool dismissed = true;
    QEventLoop *loop = nullptr;

public slots:
    void onCompleted(bool d, QDBusVariant /*result*/)
    {
        dismissed = d;
        completed = true;
        if (loop)
            loop->quit();
    }
};

namespace {

constexpr const char *kSecretService = "org.freedesktop.secrets";
constexpr const char *kSecretRoot = "/org/freedesktop/secrets";
constexpr const char *kDefaultCollection = "/org/freedesktop/secrets/aliases/default";
constexpr const char *kServiceInterface = "org.freedesktop.Secret.Service";
constexpr const char *kCollectionInterface = "org.freedesktop.Secret.Collection";
constexpr const char *kItemInterface = "org.freedesktop.Secret.Item";
constexpr const char *kPromptInterface = "org.freedesktop.Secret.Prompt";
constexpr const char *kPropertiesInterface = "org.freedesktop.DBus.Properties";
constexpr const char *kApplicationAttribute = "ElementsExtract";

void setError(QString *error, const QString &message)
{
    if (error)
        *error = message;
}

QString replyError(const QDBusMessage &reply, const QString &action)
{
    const QString detail = reply.errorMessage().isEmpty()
        ? QStringLiteral("error DBus desconocido")
        : reply.errorMessage();
    return QStringLiteral("%1: %2").arg(action, detail);
}

void registerSecretType()
{
    static const bool registered = []() {
        qDBusRegisterMetaType<ElementsExtractSecret>();
        qDBusRegisterMetaType<QMap<QString, QString>>();       // a{ss}
        qDBusRegisterMetaType<QList<QDBusObjectPath>>();       // ao
        return true;
    }();
    Q_UNUSED(registered);
}

bool isNoReplyPath(const QDBusObjectPath &path)
{
    return path.path().isEmpty() || path.path() == QStringLiteral("/");
}

// Qt6 entrega los tipos compuestos (arrays, mapas, structs) como QDBusArgument.
template <typename T>
T fromDBusVariant(const QVariant &variant)
{
    if (variant.canConvert<QDBusArgument>()) {
        const QDBusArgument arg = variant.value<QDBusArgument>();
        T result;
        arg >> result;
        return result;
    }
    return variant.value<T>();
}

QMap<QString, QString> itemAttributes(const QString &service, const QString &key)
{
    QMap<QString, QString> attributes;
    attributes.insert(QStringLiteral("application"), QLatin1String(kApplicationAttribute));
    attributes.insert(QStringLiteral("service"), service);
    attributes.insert(QStringLiteral("key"), key);
    return attributes;
}

// Espera a que el usuario responda a un prompt de desbloqueo/creación.
bool waitForPrompt(const QDBusObjectPath &promptPath, bool *dismissed, QString *error)
{
    if (isNoReplyPath(promptPath))
        return true; // no hay nada que esperar

    QDBusConnection bus = QDBusConnection::sessionBus();
    QEventLoop loop;
    SecretPromptWatcher watcher;
    watcher.loop = &loop;
    bus.connect(QLatin1String(kSecretService), promptPath.path(),
                QLatin1String(kPromptInterface), QStringLiteral("Completed"),
                &watcher, SLOT(onCompleted(bool,QDBusVariant)));

    QTimer::singleShot(15000, &loop, [&loop]() { loop.quit(); });

    QDBusInterface prompt(QLatin1String(kSecretService), promptPath.path(),
                          QLatin1String(kPromptInterface), bus);
    const QDBusMessage reply = prompt.call(QStringLiteral("Prompt"), QString());
    if (reply.type() == QDBusMessage::ErrorMessage) {
        bus.disconnect(QLatin1String(kSecretService), promptPath.path(),
                       QLatin1String(kPromptInterface), QStringLiteral("Completed"),
                       &watcher, SLOT(onCompleted(bool,QDBusVariant)));
        setError(error, replyError(reply, QStringLiteral("No se pudo mostrar el diálogo del almacén secreto")));
        return false;
    }

    if (!watcher.completed)
        loop.exec();
    bus.disconnect(QLatin1String(kSecretService), promptPath.path(),
                   QLatin1String(kPromptInterface), QStringLiteral("Completed"),
                   &watcher, SLOT(onCompleted(bool,QDBusVariant)));

    if (dismissed)
        *dismissed = watcher.dismissed;
    if (!watcher.completed) {
        setError(error, QStringLiteral("El almacén secreto no respondió al desbloqueo (tiempo de espera agotado)"));
        return false;
    }
    if (watcher.dismissed) {
        setError(error, QStringLiteral("El usuario canceló el desbloqueo del almacén secreto"));
        return false;
    }
    return true;
}

// Abre una sesión "plain" para poder leer/escribir secretos.
bool openSession(QDBusObjectPath *session, QString *error)
{
    registerSecretType();
    QDBusInterface service(QLatin1String(kSecretService), QLatin1String(kSecretRoot),
                           QLatin1String(kServiceInterface), QDBusConnection::sessionBus());
    if (!service.isValid()) {
        setError(error, QStringLiteral("Servicio Secret Service (org.freedesktop.secrets) no disponible"));
        return false;
    }
    // El segundo parámetro es 'v': debe enviarse como QDBusVariant (un
    // QVariant normal se marshalla como el tipo que contiene, no como 'v').
    // El algoritmo "plain" exige una cadena vacía como entrada.
    const QDBusMessage reply = service.call(
        QStringLiteral("OpenSession"), QStringLiteral("plain"),
        QVariant::fromValue(QDBusVariant(QVariant(QString()))));
    if (reply.type() == QDBusMessage::ErrorMessage) {
        setError(error, replyError(reply, QStringLiteral("No se pudo abrir la sesión del almacén secreto")));
        return false;
    }
    const QList<QVariant> args = reply.arguments();
    if (args.size() < 2) {
        setError(error, QStringLiteral("Respuesta inesperada al abrir la sesión del almacén secreto"));
        return false;
    }
    const QDBusObjectPath path = args.at(1).value<QDBusObjectPath>();
    if (isNoReplyPath(path)) {
        setError(error, QStringLiteral("El almacén secreto devolvió una sesión inválida"));
        return false;
    }
    if (session)
        *session = path;
    return true;
}

bool collectionIsLocked(bool *locked, QString *error)
{
    QDBusInterface props(QLatin1String(kSecretService), QLatin1String(kDefaultCollection),
                         QLatin1String(kPropertiesInterface), QDBusConnection::sessionBus());
    const QDBusMessage reply = props.call(QStringLiteral("Get"),
                                          QLatin1String(kCollectionInterface),
                                          QStringLiteral("Locked"));
    if (reply.type() == QDBusMessage::ErrorMessage) {
        setError(error, replyError(reply, QStringLiteral("No se pudo acceder a la colección secreta predeterminada")));
        return false;
    }
    if (reply.arguments().isEmpty()) {
        setError(error, QStringLiteral("Respuesta inesperada al consultar la colección secreta"));
        return false;
    }
    if (locked)
        *locked = reply.arguments().first().toBool();
    return true;
}

bool unlockPaths(const QList<QDBusObjectPath> &paths, QString *error)
{
    if (paths.isEmpty())
        return true;
    QDBusInterface service(QLatin1String(kSecretService), QLatin1String(kSecretRoot),
                           QLatin1String(kServiceInterface), QDBusConnection::sessionBus());
    const QDBusMessage reply = service.call(QStringLiteral("Unlock"),
                                            QVariant::fromValue(paths));
    if (reply.type() == QDBusMessage::ErrorMessage) {
        setError(error, replyError(reply, QStringLiteral("No se pudo desbloquear el almacén secreto")));
        return false;
    }
    const QList<QVariant> args = reply.arguments();
    if (args.size() < 2) {
        setError(error, QStringLiteral("Respuesta inesperada al desbloquear el almacén secreto"));
        return false;
    }
    const QDBusObjectPath prompt = args.at(1).value<QDBusObjectPath>();
    if (!isNoReplyPath(prompt)) {
        bool dismissed = false;
        if (!waitForPrompt(prompt, &dismissed, error))
            return false;
    }
    return true;
}

bool ensureCollectionUnlocked(QString *error)
{
    bool locked = false;
    if (!collectionIsLocked(&locked, error))
        return false;
    if (locked && !unlockPaths({QDBusObjectPath(QLatin1String(kDefaultCollection))}, error))
        return false;
    return true;
}

// Busca los elementos con nuestros atributos y desbloquea los que estén bloqueados.
bool findItems(const QString &service, const QString &key,
               QList<QDBusObjectPath> *items, QString *error)
{
    QDBusInterface iface(QLatin1String(kSecretService), QLatin1String(kSecretRoot),
                         QLatin1String(kServiceInterface), QDBusConnection::sessionBus());
    const QDBusMessage reply = iface.call(QStringLiteral("SearchItems"),
                                          QVariant::fromValue(itemAttributes(service, key)));
    if (reply.type() == QDBusMessage::ErrorMessage) {
        setError(error, replyError(reply, QStringLiteral("No se pudo buscar en el almacén secreto")));
        return false;
    }
    const QList<QVariant> args = reply.arguments();
    if (args.size() < 2) {
        setError(error, QStringLiteral("Respuesta inesperada al buscar en el almacén secreto"));
        return false;
    }
    const QList<QDBusObjectPath> unlocked =
        fromDBusVariant<QList<QDBusObjectPath>>(args.at(0));
    const QList<QDBusObjectPath> locked =
        fromDBusVariant<QList<QDBusObjectPath>>(args.at(1));
    if (!unlockPaths(locked, error))
        return false;
    if (items)
        *items = unlocked + locked;
    return true;
}

// Elimina los elementos encontrados. En la API Secret Service el método de
// borrado pertenece a org.freedesktop.Secret.Item (no a Collection).
bool deleteItems(const QList<QDBusObjectPath> &items, QString *error)
{
    for (const QDBusObjectPath &item : items) {
        QDBusInterface itemIface(QLatin1String(kSecretService), item.path(),
                                 QLatin1String(kItemInterface), QDBusConnection::sessionBus());
        const QDBusMessage reply = itemIface.call(QStringLiteral("Delete"));
        if (reply.type() == QDBusMessage::ErrorMessage) {
            setError(error, replyError(reply, QStringLiteral("No se pudo borrar una credencial del almacén secreto")));
            return false;
        }
        if (!reply.arguments().isEmpty()) {
            const QDBusObjectPath prompt = reply.arguments().first().value<QDBusObjectPath>();
            bool dismissed = false;
            if (!waitForPrompt(prompt, &dismissed, error))
                return false;
        }
    }
    return true;
}

} // namespace

CredentialStore::CredentialStore(QObject *parent) : QObject(parent)
{
    registerSecretType();
}

bool CredentialStore::isAvailable()
{
    const QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.isConnected() || !bus.interface())
        return false;
    const QDBusReply<bool> registered =
        bus.interface()->isServiceRegistered(QLatin1String(kSecretService));
    return registered.isValid() && registered.value();
}

bool CredentialStore::save(const QString &service, const QString &key,
                           const QString &value, QString *error) const
{
    if (!isAvailable()) {
        setError(error, QStringLiteral("Almacén seguro del sistema no disponible (Secret Service)"));
        return false;
    }
    if (service.isEmpty() || key.isEmpty()) {
        setError(error, QStringLiteral("Servicio y clave de la credencial no pueden estar vacíos"));
        return false;
    }
    QString localError;
    if (!ensureCollectionUnlocked(&localError)) {
        setError(error, localError);
        return false;
    }
    QDBusObjectPath session;
    if (!openSession(&session, &localError)) {
        setError(error, localError);
        return false;
    }
    QList<QDBusObjectPath> existing;
    if (!findItems(service, key, &existing, &localError)) {
        setError(error, localError);
        return false;
    }
    // CreateItem(replace=true) no es respetado por todos los backend; borrar
    // primero garantiza que no queden duplicados con la contraseña antigua.
    if (!deleteItems(existing, &localError)) {
        setError(error, localError);
        return false;
    }

    QVariantMap properties;
    properties.insert(QStringLiteral("org.freedesktop.Secret.Item.Label"),
                      QStringLiteral("ElementsExtract: %1/%2").arg(service, key));
    properties.insert(QStringLiteral("org.freedesktop.Secret.Item.Attributes"),
                      QVariant::fromValue(itemAttributes(service, key)));

    const QByteArray data = value.toUtf8();
    ElementsExtractSecret secret{session, QByteArray(), data,
                                 QStringLiteral("text/plain; charset=utf-8")};

    QDBusInterface collection(QLatin1String(kSecretService), QLatin1String(kDefaultCollection),
                              QLatin1String(kCollectionInterface), QDBusConnection::sessionBus());
    const QDBusMessage reply = collection.call(
        QStringLiteral("CreateItem"), QVariant::fromValue(properties),
        QVariant::fromValue(secret), true);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        setError(error, replyError(reply, QStringLiteral("No se pudo guardar la credencial en el almacén secreto")));
        return false;
    }
    const QList<QVariant> args = reply.arguments();
    if (args.size() < 2) {
        setError(error, QStringLiteral("Respuesta inesperada al guardar en el almacén secreto"));
        return false;
    }
    const QDBusObjectPath prompt = args.at(1).value<QDBusObjectPath>();
    if (!isNoReplyPath(prompt)) {
        bool dismissed = false;
        if (!waitForPrompt(prompt, &dismissed, &localError)) {
            setError(error, localError);
            return false;
        }
    }
    return true;
}

bool CredentialStore::load(const QString &service, const QString &key,
                           QString *value, QString *error) const
{
    if (!value) {
        setError(error, QStringLiteral("No se indicó dónde devolver la credencial"));
        return false;
    }
    value->clear();
    if (!isAvailable()) {
        setError(error, QStringLiteral("Almacén seguro del sistema no disponible (Secret Service)"));
        return false;
    }
    QString localError;
    if (!ensureCollectionUnlocked(&localError)) {
        setError(error, localError);
        return false;
    }
    QDBusObjectPath session;
    if (!openSession(&session, &localError)) {
        setError(error, localError);
        return false;
    }
    QList<QDBusObjectPath> items;
    if (!findItems(service, key, &items, &localError)) {
        setError(error, localError);
        return false;
    }
    if (items.isEmpty()) {
        setError(error, QStringLiteral("No hay ninguna credencial guardada para %1/%2").arg(service, key));
        return false;
    }

    QDBusInterface iface(QLatin1String(kSecretService), QLatin1String(kSecretRoot),
                         QLatin1String(kServiceInterface), QDBusConnection::sessionBus());
    const QDBusMessage reply = iface.call(QStringLiteral("GetSecrets"),
                                          QVariant::fromValue(items),
                                          QVariant::fromValue(session));
    if (reply.type() == QDBusMessage::ErrorMessage) {
        setError(error, replyError(reply, QStringLiteral("No se pudieron leer las credenciales del almacén secreto")));
        return false;
    }
    if (reply.arguments().isEmpty()) {
        setError(error, QStringLiteral("Respuesta inesperada al leer del almacén secreto"));
        return false;
    }

    // a{o(oayays)}: se recorre a mano porque QDBusObjectPath no es clave de QMap.
    // Importante: beginMap() sobre un QDBusArgument no-const sería la
    // sobrecarga de *escritura*; se fuerza la de lectura con una referencia
    // const para recorrer el mapa devuelto por GetSecrets.
    const QDBusArgument arg = reply.arguments().first().value<QDBusArgument>();
    bool found = false;
    arg.beginMap();
    while (!arg.atEnd()) {
        QDBusObjectPath itemPath;
        ElementsExtractSecret secret;
        arg.beginMapEntry();
        arg >> itemPath >> secret;
        arg.endMapEntry();
        if (!found) {
            *value = QString::fromUtf8(secret.value);
            found = true;
        }
    }
    arg.endMap();
    if (!found) {
        setError(error, QStringLiteral("El almacén secreto no devolvió la credencial solicitada"));
        return false;
    }
    return true;
}

bool CredentialStore::remove(const QString &service, const QString &key, QString *error) const
{
    if (!isAvailable()) {
        setError(error, QStringLiteral("Almacén seguro del sistema no disponible (Secret Service)"));
        return false;
    }
    QString localError;
    if (!ensureCollectionUnlocked(&localError)) {
        setError(error, localError);
        return false;
    }
    QList<QDBusObjectPath> items;
    if (!findItems(service, key, &items, &localError)) {
        setError(error, localError);
        return false;
    }
    if (items.isEmpty()) {
        setError(error, QStringLiteral("No hay ninguna credencial que borrar para %1/%2").arg(service, key));
        return false;
    }
    if (!deleteItems(items, &localError)) {
        setError(error, localError);
        return false;
    }
    return true;
}

#include "credentialstore.moc"

#elif defined(Q_OS_WIN)

#include <windows.h>
#include <wincred.h>

namespace {

QString targetName(const QString &service, const QString &key)
{
    return QStringLiteral("ElementsExtract/%1/%2").arg(service, key);
}

void setError(QString *error, const QString &message)
{
    if (error)
        *error = message;
}

} // namespace

CredentialStore::CredentialStore(QObject *parent) : QObject(parent) {}

bool CredentialStore::isAvailable()
{
    // Credential Manager forma parte de Windows; advapi32 está enlazado.
    return true;
}

bool CredentialStore::save(const QString &service, const QString &key,
                           const QString &value, QString *error) const
{
    const QString target = targetName(service, key);
    const QString localValue = value; // mantiene vivo el búfer de utf16()
    CREDENTIALW cred = {};
    cred.Type = CRED_TYPE_GENERIC;
    cred.TargetName = const_cast<wchar_t *>(reinterpret_cast<const wchar_t *>(target.utf16()));
    cred.Comment = const_cast<wchar_t *>(L"ElementsExtract credential");
    cred.CredentialBlobSize = static_cast<DWORD>(localValue.size() * sizeof(ushort));
    cred.CredentialBlob = reinterpret_cast<BYTE *>(const_cast<ushort *>(localValue.utf16()));
    cred.Persist = CRED_PERSIST_LOCAL_MACHINE;

    if (!CredWriteW(&cred, 0)) {
        setError(error, QStringLiteral("CredWriteW falló (%1) al guardar la credencial").arg(GetLastError()));
        return false;
    }
    return true;
}

bool CredentialStore::load(const QString &service, const QString &key,
                           QString *value, QString *error) const
{
    if (!value) {
        setError(error, QStringLiteral("No se indicó dónde devolver la credencial"));
        return false;
    }
    value->clear();
    const QString target = targetName(service, key);
    CREDENTIALW *cred = nullptr;
    if (!CredReadW(reinterpret_cast<const wchar_t *>(target.utf16()), CRED_TYPE_GENERIC, 0, &cred)) {
        setError(error, QStringLiteral("CredReadW falló (%1) al leer la credencial").arg(GetLastError()));
        return false;
    }
    if (cred->CredentialBlob && cred->CredentialBlobSize >= sizeof(ushort)) {
        *value = QString::fromUtf16(reinterpret_cast<const ushort *>(cred->CredentialBlob),
                                    static_cast<int>(cred->CredentialBlobSize / sizeof(ushort)));
    }
    CredFree(cred);
    return true;
}

bool CredentialStore::remove(const QString &service, const QString &key, QString *error) const
{
    const QString target = targetName(service, key);
    if (!CredDeleteW(reinterpret_cast<const wchar_t *>(target.utf16()), CRED_TYPE_GENERIC, 0)) {
        setError(error, QStringLiteral("CredDeleteW falló (%1) al borrar la credencial").arg(GetLastError()));
        return false;
    }
    return true;
}

#elif defined(Q_OS_MACOS)

#include <CoreFoundation/CoreFoundation.h>
#include <Security/Security.h>

namespace {

void setError(QString *error, const QString &message)
{
    if (error)
        *error = message;
}

CFStringRef toCFString(const QString &text)
{
    const QByteArray utf8 = text.toUtf8();
    return CFStringCreateWithBytes(kCFAllocatorDefault,
                                   reinterpret_cast<const UInt8 *>(utf8.constData()),
                                   static_cast<CFIndex>(utf8.size()),
                                   kCFStringEncodingUTF8, false);
}

CFMutableDictionaryRef baseQuery(const QString &service, const QString &key)
{
    CFMutableDictionaryRef query = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                                             &kCFTypeDictionaryKeyCallBacks,
                                                             &kCFTypeDictionaryValueCallBacks);
    CFStringRef serviceName = toCFString(QStringLiteral("ElementsExtract/%1").arg(service));
    CFStringRef account = toCFString(key);
    CFDictionarySetValue(query, kSecClass, kSecClassGenericPassword);
    CFDictionarySetValue(query, kSecAttrService, serviceName);
    CFDictionarySetValue(query, kSecAttrAccount, account);
    CFRelease(serviceName);
    CFRelease(account);
    return query;
}

QString osStatusError(OSStatus status, const QString &action)
{
    return QStringLiteral("%1 (OSStatus %2)").arg(action).arg(static_cast<int>(status));
}

} // namespace

CredentialStore::CredentialStore(QObject *parent) : QObject(parent) {}

bool CredentialStore::isAvailable()
{
    // Security.framework está enlazado en macOS.
    return true;
}

bool CredentialStore::save(const QString &service, const QString &key,
                           const QString &value, QString *error) const
{
    CFMutableDictionaryRef query = baseQuery(service, key);
    const QByteArray utf8 = value.toUtf8();
    CFDataRef data = CFDataCreate(kCFAllocatorDefault,
                                  reinterpret_cast<const UInt8 *>(utf8.constData()),
                                  static_cast<CFIndex>(utf8.size()));
    CFDictionarySetValue(query, kSecValueData, data);
    CFDictionarySetValue(query, kSecAttrAccessible,
                         kSecAttrAccessibleWhenUnlockedThisDeviceOnly);

    OSStatus status = SecItemDelete(query);
    if (status != noErr && status != errSecItemNotFound) {
        CFRelease(data);
        CFRelease(query);
        setError(error, osStatusError(status, QStringLiteral("No se pudo borrar la credencial anterior del LLavero")));
        return false;
    }
    CFDictionaryRemoveValue(query, kSecValueData);
    CFDictionaryRemoveValue(query, kSecAttrAccessible);
    status = SecItemAdd(query, nullptr);
    CFRelease(data);
    CFRelease(query);
    if (status != noErr) {
        setError(error, osStatusError(status, QStringLiteral("SecItemAdd falló al guardar en el LLavero")));
        return false;
    }
    return true;
}

bool CredentialStore::load(const QString &service, const QString &key,
                           QString *value, QString *error) const
{
    if (!value) {
        setError(error, QStringLiteral("No se indicó dónde devolver la credencial"));
        return false;
    }
    value->clear();
    CFMutableDictionaryRef query = baseQuery(service, key);
    CFDictionarySetValue(query, kSecReturnData, kCFBooleanTrue);
    CFDictionarySetValue(query, kSecMatchLimit, kSecMatchLimitOne);

    CFTypeRef item = nullptr;
    const OSStatus status = SecItemCopyMatching(query, &item);
    CFRelease(query);
    if (status == errSecItemNotFound) {
        setError(error, QStringLiteral("No hay ninguna credencial guardada en el LLavero"));
        return false;
    }
    if (status != noErr) {
        setError(error, osStatusError(status, QStringLiteral("SecItemCopyMatching falló al leer del LLavero")));
        return false;
    }
    if (item) {
        const CFDataRef data = static_cast<CFDataRef>(item);
        *value = QString::fromUtf8(reinterpret_cast<const char *>(CFDataGetBytePtr(data)),
                                   static_cast<qsizetype>(CFDataGetLength(data)));
        CFRelease(item);
    }
    return true;
}

bool CredentialStore::remove(const QString &service, const QString &key, QString *error) const
{
    CFMutableDictionaryRef query = baseQuery(service, key);
    const OSStatus status = SecItemDelete(query);
    CFRelease(query);
    if (status != noErr && status != errSecItemNotFound) {
        setError(error, osStatusError(status, QStringLiteral("SecItemDelete falló al borrar del LLavero")));
        return false;
    }
    if (status == errSecItemNotFound) {
        setError(error, QStringLiteral("No hay ninguna credencial que borrar del LLavero"));
        return false;
    }
    return true;
}

#else

CredentialStore::CredentialStore(QObject *parent) : QObject(parent) {}

bool CredentialStore::isAvailable()
{
    return false;
}

bool CredentialStore::save(const QString &, const QString &, const QString &, QString *error) const
{
    if (error)
        *error = QStringLiteral("No hay ningún almacén seguro de credenciales disponible en esta plataforma");
    return false;
}

bool CredentialStore::load(const QString &, const QString &, QString *, QString *error) const
{
    if (error)
        *error = QStringLiteral("No hay ningún almacén seguro de credenciales disponible en esta plataforma");
    return false;
}

bool CredentialStore::remove(const QString &, const QString &, QString *error) const
{
    if (error)
        *error = QStringLiteral("No hay ningún almacén seguro de credenciales disponible en esta plataforma");
    return false;
}

#endif
