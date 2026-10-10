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

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Acerca de Elements Extract FFmpeg"));
    setFixedSize(560, 480);
    setModal(true);

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setSpacing(10);
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
    grid->setHorizontalSpacing(16);
    grid->setVerticalSpacing(6);
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
    addRow(1, tr("Contacto"), "pdmiguel90-droid@users.noreply.github.com");
    addRow(2, tr("Repositorio"), "github.com/pdmiguel90-droid/elements-extract", true,
           "https://github.com/pdmiguel90-droid/elements-extract");
    addRow(3, tr("Licencia"), "GNU General Public License v3.0");

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
        tr("Qt 5\nInterfaz"),
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