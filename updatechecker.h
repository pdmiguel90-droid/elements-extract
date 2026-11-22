#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QObject>
#include <QString>
#include <QStringList>

class QNetworkAccessManager;
class QNetworkReply;

struct UpdateInfo {
    QString version;      // sin la "v" inicial, p. ej. "1.2.9"
    QString tag;          // tag completo, p. ej. "v1.2.9"
    QString url;          // página del release
    QString notes;        // cuerpo del release
    QStringList assets;   // nombres de los ficheros publicados
};

Q_DECLARE_METATYPE(UpdateInfo)

class UpdateChecker : public QObject
{
    Q_OBJECT

public:
    explicit UpdateChecker(QObject *parent = nullptr);
    ~UpdateChecker() override;

    bool isInFlight() const { return m_inFlight; }
    void check();

signals:
    void updateAvailable(const UpdateInfo &info);
    void upToDate();
    void checkFailed(const QString &error);

private slots:
    void onFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_manager;
    bool m_inFlight = false;
};

#endif // UPDATECHECKER_H
