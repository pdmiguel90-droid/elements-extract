#include "aboutdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QDesktopServices>
#include <QUrl>

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Acerca de Elements Extract FFmpeg");
    setFixedSize(500, 420);
    setModal(true);

    setStyleSheet(R"(
        QDialog  { background-color: #121212; }
        QWidget  { background-color: #121212; color: #CCCCCC;
                   font-family: 'Segoe UI', sans-serif; font-size: 13px; }
        QLabel#appTitle  { font-size: 18px; font-weight: bold;
                           color: #FF8800; letter-spacing: 3px; }
        QLabel#appVersion { font-size: 11px; color: #666666; }
        QLabel#sectionTitle { color: #FF8800; font-weight: bold;
                              font-size: 11px; letter-spacing: 1px; }
        QLabel#key   { color: #888888; }
        QLabel#value { color: #39FF14; }
        QLabel#tech  { color: #CCCCCC; font-size: 11px;
                       background-color: #1E1E1E;
                       border: 1px solid #2A2A2A; border-radius: 4px;
                       padding: 6px 10px; }
        QFrame#sep   { background-color: #2A2A2A; }
        QPushButton  { background-color: #1E1E1E; border: 1px solid #39FF14;
                       border-radius: 5px; padding: 7px 24px;
                       color: #39FF14; font-weight: bold; }
        QPushButton:hover { background-color: #39FF14; color: #000; }
        QLabel#copy { color: #444444; font-size: 11px; }
    )");

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setSpacing(10);
    root->setContentsMargins(30, 22, 30, 18);

    // ── Título ──────────────────────────────────────────────────────────────
    QLabel *title = new QLabel("ELEMENTS EXTRACT FFMPEG");
    title->setObjectName("appTitle");
    title->setAlignment(Qt::AlignCenter);
    root->addWidget(title);

    QLabel *ver = new QLabel("Versión 1.0.0  ·  2025");
    ver->setObjectName("appVersion");
    ver->setAlignment(Qt::AlignCenter);
    root->addWidget(ver);

    auto makeSep = [&]() {
        QFrame *f = new QFrame(); f->setObjectName("sep");
        f->setFrameShape(QFrame::HLine); f->setFixedHeight(1);
        return f;
    };
    root->addWidget(makeSep());

    // ── Descripción ─────────────────────────────────────────────────────────
    QLabel *desc = new QLabel(
        "Extractor gráfico de pistas de audio, video y subtítulos.\n"
        "Procesamiento en lote con selección individual por archivo.");
    desc->setAlignment(Qt::AlignCenter);
    desc->setWordWrap(true);
    root->addWidget(desc);

    root->addWidget(makeSep());

    // ── Datos del autor ──────────────────────────────────────────────────────
    QGridLayout *grid = new QGridLayout();
    grid->setHorizontalSpacing(16);
    grid->setVerticalSpacing(5);
    grid->setColumnStretch(1, 1);

    auto addRow = [&](int row, const QString &key, const QString &value,
                      const QUrl &link = QUrl()) {
        QLabel *k = new QLabel(key + ":"); k->setObjectName("key");
        k->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        QLabel *v = new QLabel(value);   v->setObjectName("value");
        if (link.isValid()) {
            v->setCursor(Qt::PointingHandCursor);
            v->setToolTip("Abrir en navegador");
            // click via mouse-press handled through installEventFilter if needed;
            // for simplicity show the URL as text
        }
        grid->addWidget(k, row, 0);
        grid->addWidget(v, row, 1);
    };

    addRow(0, "Desarrollado por", "[Tu Nombre]");
    addRow(1, "Contacto",         "tu@email.com");
    addRow(2, "Repositorio",      "github.com/tuusuario/ffmpeg-extractor",
               QUrl("https://github.com/tuusuario/ffmpeg-extractor"));
    addRow(3, "Licencia",         "GNU General Public License v3.0");

    root->addLayout(grid);
    root->addWidget(makeSep());

    // ── Tecnologías ──────────────────────────────────────────────────────────
    QLabel *techTitle = new QLabel("TECNOLOGÍAS UTILIZADAS");
    techTitle->setObjectName("sectionTitle");
    techTitle->setAlignment(Qt::AlignCenter);
    root->addWidget(techTitle);

    QHBoxLayout *techRow = new QHBoxLayout();
    techRow->setSpacing(8);
    const QStringList techs = {
        "Qt 5\nInterfaz gráfica",
        "FFmpeg\nExtracción",
        "ffprobe\nAnálisis",
        "C++17\nLenguaje"
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
    QPushButton *closeBtn = new QPushButton("Cerrar");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    root->addWidget(closeBtn, 0, Qt::AlignCenter);

    QLabel *copy = new QLabel("© 2025 [Tu Nombre]  —  GPL-3.0");
    copy->setObjectName("copy");
    copy->setAlignment(Qt::AlignCenter);
    root->addWidget(copy);
}
