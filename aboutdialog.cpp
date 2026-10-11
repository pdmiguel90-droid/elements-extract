#include "aboutdialog.h"
#include "theme.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QDesktopServices>
#include <QUrl>
#include <QColor>
#include <QGuiApplication>
#include <QClipboard>
#include <QTimer>

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Acerca de Elements Extract FFmpeg"));
    setFixedSize(600, 630);
    setModal(true);

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setSpacing(8);
    root->setContentsMargins(32, 24, 32, 18);

    // ── Logo ────────────────────────────────────────────────────────────────
    QLabel *iconLabel = new QLabel();
    iconLabel->setObjectName("appLogo");
    QPixmap pix(":/ffmpeg-extractor.svg");
    iconLabel->setPixmap(pix.scaled(76, 76, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    iconLabel->setAlignment(Qt::AlignCenter);
    root->addWidget(iconLabel);

    // ── Título ──────────────────────────────────────────────────────────────
    QLabel *title = new QLabel("ELEMENTS EXTRACT FFMPEG");
    title->setObjectName("appTitle");
    title->setAlignment(Qt::AlignCenter);
    root->addWidget(title);

    QLabel *ver = new QLabel(tr("Versión 1.2.0  ·  2026"));
    ver->setObjectName("appVersion");
    ver->setAlignment(Qt::AlignCenter);
    root->addWidget(ver);

    auto makeSep = [&]() {
        QFrame *f = new QFrame();
        f->setObjectName("sep");
        f->setFrameShape(QFrame::HLine);
        f->setFixedHeight(1);
        return f;
    };
    root->addWidget(makeSep());

    // ── Descripción ─────────────────────────────────────────────────────────
    QLabel *desc = new QLabel(
        tr("Extractor gráfico de pistas de audio, vídeo y subtítulos.\n"
        "Procesamiento por lotes con selección individual por archivo\n"
        "y traducción automática de subtítulos sin conexión."));
    desc->setAlignment(Qt::AlignCenter);
    desc->setWordWrap(true);
    root->addWidget(desc);

    root->addWidget(makeSep());

    // ── Datos del autor ─────────────────────────────────────────────────────
    QGridLayout *grid = new QGridLayout();
    grid->setHorizontalSpacing(20);
    grid->setVerticalSpacing(12);
    grid->setColumnMinimumWidth(1, 340);
    grid->setColumnStretch(1, 1);

    auto addRow = [&](int row, const QString &key, const QString &value,
                      bool link = false, const QString &url = QString()) {
        QLabel *k = new QLabel(key + ":"); k->setObjectName("key");
        k->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        QLabel *v;
        if (link) {
            const QString linkColor =
                ThemeManager::instance()->effectiveTheme() == ThemeManager::Light
                    ? "#D9700A" : "#FFA040";
            v = new QLabel(QString("<a style=\"color:%1; text-decoration:none;\" href=\"%2\">%3</a>")
                               .arg(linkColor, url, value.toHtmlEscaped()));
            v->setTextFormat(Qt::RichText);
            v->setTextInteractionFlags(Qt::TextBrowserInteraction);
            v->setOpenExternalLinks(true);
            v->setCursor(Qt::PointingHandCursor);
            v->setProperty("link", true);
        } else {
            v = new QLabel(value);
        }
        v->setObjectName("value");
        v->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        grid->addWidget(k, row, 0);
        grid->addWidget(v, row, 1);
    };

    addRow(0, tr("Desarrollado por"), "pdmiguel90-droid");
    addRow(1, tr("Repositorio"), "github.com/pdmiguel90-droid/elements-extract", true,
           "https://github.com/pdmiguel90-droid/elements-extract");
    addRow(2, tr("Licencia"), "GNU General Public License v3.0");

    root->addLayout(grid);
    root->addWidget(makeSep());

    // ── Tecnologías ─────────────────────────────────────────────────────────
    QLabel *techTitle = new QLabel(tr("TECNOLOGÍAS"));
    techTitle->setObjectName("sectionTitle");
    techTitle->setAlignment(Qt::AlignCenter);
    root->addWidget(techTitle);

    QHBoxLayout *techRow = new QHBoxLayout();
    techRow->setSpacing(8);
    const QStringList techs = {
        tr("Qt %1\nInterfaz").arg(qVersion()),
        tr("FFmpeg\nExtracción"),
        tr("bergamot\nTraducción"),
        tr("C++17\nLenguaje")
    };
    for (const QString &t : techs) {
        QLabel *lbl = new QLabel(t);
        lbl->setObjectName("tech");
        lbl->setAlignment(Qt::AlignCenter);
        techRow->addWidget(lbl);
    }
    root->addLayout(techRow);

    root->addWidget(makeSep());

    QLabel *donarTitle = new QLabel(tr("DONAR"));
    donarTitle->setObjectName("sectionTitle");
    donarTitle->setAlignment(Qt::AlignCenter);
    root->addWidget(donarTitle);

    QGridLayout *don = new QGridLayout();
    don->setHorizontalSpacing(20);
    don->setVerticalSpacing(12);
    don->setColumnStretch(1, 1);

    const QString linkColor =
        ThemeManager::instance()->effectiveTheme() == ThemeManager::Light
            ? "#D9700A" : "#FFA040";

    QLabel *ppKey = new QLabel(tr("Donar con PayPal") + ":");
    ppKey->setObjectName("key");
    ppKey->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QLabel *ppVal = new QLabel(
        QString("<a style=\"color:%1; text-decoration:none;\" href=\"%2\">%3</a>")
            .arg(linkColor,
                 "https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=pdmiguel90@gmail.com&currency_code=BRL&item_name=Elements%20Extract",
                 "pdmiguel90@gmail.com"));
    ppVal->setTextFormat(Qt::RichText);
    ppVal->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ppVal->setOpenExternalLinks(true);
    ppVal->setCursor(Qt::PointingHandCursor);
    ppVal->setObjectName("value");
    ppVal->setProperty("link", true);
    ppVal->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    don->addWidget(ppKey, 0, 0);
    don->addWidget(ppVal, 0, 1);

    QLabel *pixLabel = new QLabel(tr("Donar con PIX") + ":");
    pixLabel->setObjectName("key");
    pixLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QLabel *pixKey = new QLabel(
        "2bd297c6-9332-4df1-abf9-a84bba4858fb");
    pixKey->setObjectName("value");
    QPushButton *copyPix = new QPushButton(tr("Copiar"));
    copyPix->setCursor(Qt::PointingHandCursor);
    connect(copyPix, &QPushButton::clicked, this, [this, copyPix, pixKey]() {
        QGuiApplication::clipboard()->setText(pixKey->text());
        copyPix->setText(tr("¡Copiada!"));
        QTimer::singleShot(1600, copyPix, [copyPix]() {
            copyPix->setText(tr("Copiar"));
        });
    });
    QWidget *pixCell = new QWidget(this);
    QHBoxLayout *pixLay = new QHBoxLayout(pixCell);
    pixLay->setContentsMargins(0, 0, 0, 0);
    pixLay->setSpacing(10);
    pixLay->addWidget(pixKey);
    pixLay->addWidget(copyPix);
    pixLay->addStretch();
    don->addWidget(pixLabel, 1, 0);
    don->addWidget(pixCell, 1, 1);

    root->addLayout(don);

    root->addStretch();

    // ── Botón cerrar ─────────────────────────────────────────────────────────
    QPushButton *closeBtn = new QPushButton(QIcon(":/icons/check.svg"), tr("Cerrar"));
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    root->addWidget(closeBtn, 0, Qt::AlignCenter);

    QLabel *copy = new QLabel(tr("© 2026 pdmiguel90-droid  —  GPL-3.0"));
    copy->setObjectName("copy");
    copy->setAlignment(Qt::AlignCenter);
    root->addWidget(copy);
}