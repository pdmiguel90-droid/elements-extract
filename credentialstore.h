#ifndef CREDENTIALSTORE_H
#define CREDENTIALSTORE_H

#include <QObject>
#include <QString>

// Identificadores usados por la aplicación en el almacén del sistema.
namespace ElementsExtractCredentials {
inline constexpr const char *Service = "io.github.pdmiguel90droid.ElementsExtract";
inline constexpr const char *ApiKey = "opensub/apiKey";
inline constexpr const char *Username = "opensub/username";
inline constexpr const char *Password = "opensub/password";
} // namespace ElementsExtractCredentials

// Almacén de credenciales del sistema operativo. Nunca utiliza QSettings ni
// ficheros propios cifrados: si no hay un almacén disponible, las operaciones
// fallan y el llamador decide qué hacer (por ejemplo, pedir los datos cada vez).
class CredentialStore : public QObject
{
    Q_OBJECT

public:
    explicit CredentialStore(QObject *parent = nullptr);

    static bool isAvailable();

    bool save(const QString &service, const QString &key, const QString &value,
              QString *error = nullptr) const;
    bool load(const QString &service, const QString &key, QString *value,
              QString *error = nullptr) const;
    bool remove(const QString &service, const QString &key,
                QString *error = nullptr) const;
};

#endif // CREDENTIALSTORE_H
