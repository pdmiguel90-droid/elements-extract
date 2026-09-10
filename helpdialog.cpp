#include "helpdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QFrame>

HelpDialog::HelpDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Ayuda — Elements Extract");
    setMinimumSize(650, 520);
    resize(700, 560);
    setModal(true);

    setStyleSheet(R"(
        QDialog { background-color: #121212; }
        QWidget { background-color: #121212; color: #CCCCCC;
                  font-family: 'Segoe UI', sans-serif; font-size: 13px; }
        QTabWidget::pane { border: 1px solid #2A2A2A; background-color: #121212; }
        QTabBar::tab { background-color: #1A1A1A; color: #D0D0D0;
                       padding: 8px 18px; border: 1px solid #2A2A2A;
                       border-bottom: none; margin-right: 2px; }
        QTabBar::tab:selected { background-color: #121212; color: #FF8800;
                                border-bottom: 2px solid #FF8800; }
        QTabBar::tab:hover { background-color: #2A2A2A; }
        QTextBrowser { background-color: #1A1A1A; color: #CCCCCC;
                       border: 1px solid #2A2A2A; border-radius: 4px;
                       padding: 12px; font-size: 13px; }
        QTextBrowser a { color: #39FF14; }
        QTextBrowser b { color: #FF8800; }
        QTextBrowser h2 { color: #FF8800; font-size: 15px; margin-top: 8px; }
        QTextBrowser h3 { color: #39FF14; font-size: 13px; }
        QPushButton { background-color: #1E1E1E; border: 1px solid #39FF14;
                       border-radius: 5px; padding: 7px 24px;
                       color: #39FF14; font-weight: bold; }
        QPushButton:hover { background-color: #39FF14; color: #000; }
    )");

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setSpacing(10);
    root->setContentsMargins(16, 12, 16, 12);

    QTabWidget *tabs = new QTabWidget();

    // ════════════════════════════════════════════════════════════════════
    // TAB 1: Extracción
    // ════════════════════════════════════════════════════════════════════
    QTextBrowser *extractPage = new QTextBrowser();
    extractPage->setOpenExternalLinks(false);
    extractPage->setHtml(
        "<h2>Extracción de Pistas</h2>"

        "<h3>1. Añadir archivos</h3>"
        "<p>Haz clic en <b>+ Añadir</b> o arrastra archivos directamente a la lista.</p>"

        "<h3>2. Seleccionar pistas</h3>"
        "<p>Haz clic en un archivo para ver sus pistas. Marca las que quieras extraer.</p>"
        "<p>Usa los botones <b>VIDEO / AUDIO / SUBS</b> para selección global por tipo.</p>"

        "<h3>3. Configurar formato</h3>"
        "<p>En <b>Formato de Salida</b> elige el contenedor de salida (MKV, MP4, etc.).</p>"
        "<p>Marca <b>Copiar pista original</b> para extracción rápida sin re-codificar.</p>"

        "<h3>4. Elegir destino</h3>"
        "<p>Selecciona la carpeta donde se guardarán los archivos extraídos.</p>"

        "<h3>5. Extraer</h3>"
        "<p>Pulsa <b>⚙ INICIAR EXTRACCIÓN</b>. Puedes cancelar en cualquier momento.</p>"

        "<h3>Procesamiento por lotes</h3>"
        "<p>Si añades varios archivos, se procesan en orden secuencial.</p>"
        "<p>La barra de progreso muestra el avance de cada archivo.</p>"
    );
    tabs->addTab(extractPage, "Extracción");

    // ════════════════════════════════════════════════════════════════════
    // TAB 2: Traducción
    // ════════════════════════════════════════════════════════════════════
    QTextBrowser *translatePage = new QTextBrowser();
    translatePage->setOpenExternalLinks(false);
    translatePage->setHtml(
        "<h2>Traducción de Subtítulos</h2>"

        "<h3>1. Configurar idiomas</h3>"
        "<p>Selecciona el <b>idioma origen</b> y <b>destino</b> en el panel de traducción.</p>"
        "<p>El idioma origen se <b>detecta automáticamente</b> al seleccionar pistas de subtítulos.</p>"

        "<h3>2. Traducir al extraer</h3>"
        "<p>Marca la casilla <b>«Traducir al extraer»</b> para traducir automáticamente</p>"
        "<p>los subtítulos extraídos al idioma de destino.</p>"
        "<p>El resultado se guarda como <b>archivo.srt</b> (original) y <b>archivo.es.srt</b> (traducido).</p>"

        "<h3>3. Traducción triangular</h3>"
        "<p>Si no existe un modelo directo (ej. Portugués → Español), el sistema usará</p>"
        "<p><b>traducción triangular</b>: Portugués → Inglés → Español.</p>"
        "<p>Se te informará antes de iniciar la traducción.</p>"
        "<p>Esto requiere que ambos modelos intermedios estén instalados.</p>"

        "<h3>4. Gestionar modelos</h3>"
        "<p>Haz clic en <b>Modelos</b> para abrir la gestión de modelos de traducción.</p>"
        "<p>En la pestaña <b>Modelos</b> del diálogo de ajustes:</p>"
        "<ul>"
        "<li><b>Instalados</b> — Modelos descargados. Doble clic para activar/desactivar.</li>"
        "<li><b>Online</b> — Modelos disponibles. Doble clic para descargar.</li>"
        "</ul>"
        "<p>El diálogo muestra qué pares tienen modelo directo (✓) y cuáles</p>"
        "<p>requieren traducción triangular (↻) o no están disponibles (✗).</p>"

        "<h3>5. Descarga de modelos</h3>"
        "<p>Los modelos se descargan desde <b>translatelocally.com</b>.</p>"
        "<p>Los modelos <b>tiny</b> (~20MB) son rápidos. Los <b>base</b> (~50MB) mejor calidad.</p>"
        "<p>Si el idioma detectado no tiene modelo, se recomienda descargarlo.</p>"

        "<h3>Idiomas soportados</h3>"
        "<p>Inglés, Español, Francés, Portugués, Alemán, Italiano, Japonés,</p>"
        "<p>Chino, Coreano, Ruso, Árabe, Hindi, Holandés, Sueco, Polaco, Turco.</p>"
    );
    tabs->addTab(translatePage, "Traducción");

    // ════════════════════════════════════════════════════════════════════
    // TAB 3: Atajos y Ajustes
    // ════════════════════════════════════════════════════════════════════
    QTextBrowser *shortcutsPage = new QTextBrowser();
    shortcutsPage->setOpenExternalLinks(false);
    shortcutsPage->setHtml(
        "<h2>Atajos de Teclado</h2>"

        "<table cellpadding='6' cellspacing='0' border='0'>"
        "<tr><td><b>Ctrl+O</b></td><td>Añadir archivos</td></tr>"
        "<tr><td><b>Del</b></td><td>Quitar archivos seleccionados</td></tr>"
        "<tr><td><b>Ctrl+,</b></td><td>Abrir Configuración y Modelos</td></tr>"
        "<tr><td><b>F1</b></td><td>Mostrar esta ayuda</td></tr>"
        "<tr><td><b>Ctrl+Q</b></td><td>Salir</td></tr>"
        "</table>"

        "<h2>Ajustes</h2>"

        "<h3>Configuración general</h3>"
        "<p>En <b>Ajustes → Configuración y Modelos</b> puedes:</p>"
        "<ul>"
        "<li>Cambiar el directorio de modelos de traducción</li>"
        "<li>Establecer idioma origen y destino por defecto</li>"
        "<li>Activar traducción automática al extraer</li>"
        "</ul>"

        "<h3>Formato de salida</h3>"
        "<p>Los subtítulos se guardan en formato <b>SRT</b> por defecto.</p>"
        "<p>Puedes cambiarlo a ASS o WebVTT en el selector de formato.</p>"

        "<h3>Soporte</h3>"
        "<p>Repositorio: <b>github.com/pdmiguel90-droid/elements-extract</b></p>"
        "<p>Para reportar errores, abre un issue en GitHub.</p>"
    );
    tabs->addTab(shortcutsPage, "Atajos y Ajustes");

    root->addWidget(tabs, 1);

    // ── Botón cerrar ───────────────────────────────────────────────────
    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->addStretch();
    QPushButton *closeBtn = new QPushButton("Cerrar");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    btnRow->addWidget(closeBtn);
    root->addLayout(btnRow);
}
