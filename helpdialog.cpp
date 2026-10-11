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
    setWindowTitle(tr("Ayuda — Elements Extract"));
    setMinimumSize(660, 540);
    resize(720, 580);
    setModal(true);

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
        tr(
        "<h2>Extracción de Pistas</h2>"

        "<h3>1. Añadir archivos</h3>"
        "<p>Pulsa <b>Añadir</b> o arrastra los archivos directamente a la lista "
        "del panel izquierdo. Se admiten MKV, MP4, AVI, MOV, TS, M2TS y WebM.</p>"

        "<h3>2. Seleccionar pistas</h3>"
        "<p>Haz clic en un archivo para ver sus pistas (video, audio y subtítulos) "
        "con formato, idioma y resolución. Marca las que quieras extraer.</p>"
        "<p>Usa los botones <b>VIDEO / AUDIO / SUBS</b> para marcar (o desmarcar) "
        "todas las pistas de un tipo a la vez, y <b>Limpiar</b> para quitarlas todas.</p>"

        "<h3>3. Configurar el formato</h3>"
        "<p>En <b>Formato de Salida</b> elige el contenedor para cada tipo: "
        "audio (MKA, M4A, AAC, MP3, FLAC, Opus, WAV), video (MKV, MP4, AVI) "
        "y subtítulos (SRT, ASS, VTT), además de la resolución de vídeo.</p>"
        "<p>Con <b>Copiar pista original sin re-codificar</b> (recomendado) la "
        "extracción es rápida y sin pérdida de calidad: el contenedor se mantiene "
        "o se remuxea al seleccionado.</p>"

        "<h3>4. Elegir destino y extraer</h3>"
        "<p>Indica la carpeta de salida en <b>Destino</b>. Los archivos se guardan "
        "en una subcarpeta <b>Elements Extract</b>.</p>"
        "<p>Pulsa <b>INICIAR EXTRACCIÓN</b> o <b>Ctrl+Enter</b>. "
        "Puedes cancelar con <b>Cancelar</b> o <b>Escape</b>.</p>"

        "<h3>Procesamiento por lotes</h3>"
        "<p>Si añades varios archivos se procesan en orden. La barra superior "
        "muestra el progreso y el botón <b>LOG</b> despliega el registro de actividad.</p>"
        ));
    tabs->addTab(extractPage, tr("Extracción"));

    // ════════════════════════════════════════════════════════════════════
    // TAB 2: Traducción
    // ════════════════════════════════════════════════════════════════════
    QTextBrowser *translatePage = new QTextBrowser();
    translatePage->setOpenExternalLinks(false);
    translatePage->setHtml(
        tr(
        "<h2>Traducción de Subtítulos</h2>"

        "<h3>1. Elegir idiomas</h3>"
        "<p>Selecciona el <b>idioma origen</b> y el <b>destino</b> en el panel de "
        "traducción. El idioma origen se <b>detecta automáticamente</b> al "
        "seleccionar una pista de subtítulos.</p>"

        "<h3>2. Traducir al extraer</h3>"
        "<p>Marca <b>Traducir al extraer</b> para traducir automáticamente los "
        "subtítulos extraídos. Se guardan dos archivos: <b>archivo.srt</b> (original) "
        "y <b>archivo.IDIOMA.srt</b> (traducido), donde IDIOMA es el idioma de destino elegido.</p>"
        "<p>Con <b>Traducir subtítulos</b> puedes traducir en lote subtítulos ya "
        "existentes en disco.</p>"

        "<h3>3. Trabajar con un solo idioma</h3>"
        "<p>El desplegable <b>Solo subs</b> (panel izquierdo) selecciona a la vez "
        "los subtítulos de un idioma concreto en todos los archivos del lote; "
        "los archivos sin ese idioma se omiten. Con el filtro activo, la "
        "traducción usa ese idioma como origen automáticamente.</p>"

        "<h3>4. Incrustar la traducción (remux)</h3>"
        "<p>Marca <b>Incrustar la traducción (remux)</b> para generar un "
        "<b>archivo.IDIOMA.mkv</b> (o .mp4 si el original es .mp4) con el vídeo "
        "y el audio originales y el subtítulo traducido dentro, sin re-codificar.</p>"
        "<p>La app valida cada archivo antes de incrustar: si falta pista de "
        "vídeo lo omite, y si el .mp4 tiene códecs no copiables (audio pcm_*, "
        "vorbis, flac, dts, truehd…) cambia a .mkv automáticamente. "
        "Al terminar se muestra un resumen con los incrustados/omitidos y el "
        "motivo de cada fallo.</p>"

        "<h3>5. Traducción triangular</h3>"
        "<p>Si no existe un modelo directo (p. ej. Portugués → Español), se usa "
        "<b>traducción triangular</b>: Portugués → Inglés → Español. Se te "
        "informará antes de empezar y requieren los modelos intermedios instalados.</p>"

        "<h3>6. Gestionar modelos</h3>"
        "<p>Abre <b>Ajustes → Configuración y Modelos…</b> (o el botón <b>Modelos</b>) "
        "y revisa la pestaña <b>Modelos de Traducción</b>:</p>"
        "<ul>"
        "<li><b>Instalados</b> — modelos descargados; doble clic para activar/desactivar.</li>"
        "<li><b>Online</b> — modelos disponibles; doble clic para descargar.</li>"
        "</ul>"
        "<p>El panel señala qué pares tienen modelo directo (✓), cuáles requieren "
        "triangulación (↻) y cuáles no están disponibles (✗).</p>"

        "<h3>7. Descarga de modelos</h3>"
        "<p>Los modelos se descargan de <b>translatelocally.com</b>. "
        "Los modelos <b>tiny</b> (~20 MB) son rápidos y los <b>base</b> (~50 MB) "
        "ofrecen mejor calidad. La traducción funciona <b>sin conexión</b>.</p>"

        "<h3>Idiomas soportados</h3>"
        "<p>Inglés, Español, Francés, Portugués, Alemán, Italiano, Japonés, "
        "Chino, Coreano, Ruso, Árabe, Hindi, Holandés, Sueco, Polaco y Turco.</p>"
        ));
    tabs->addTab(translatePage, tr("Traducción"));

    // ════════════════════════════════════════════════════════════════════
    // TAB 3: Atajos y Ajustes
    // ════════════════════════════════════════════════════════════════════
    QTextBrowser *shortcutsPage = new QTextBrowser();
    shortcutsPage->setOpenExternalLinks(false);
    shortcutsPage->setHtml(
        tr(
        "<h2>Atajos de Teclado</h2>"

        "<table cellpadding='6' cellspacing='0' border='0'>"
        "<tr><td><b>Ctrl+O</b></td><td>Añadir archivos</td></tr>"
        "<tr><td><b>Supr</b></td><td>Quitar archivos seleccionados</td></tr>"
        "<tr><td><b>Ctrl+Enter</b></td><td>Iniciar extracción</td></tr>"
        "<tr><td><b>Escape</b></td><td>Cancelar el proceso en curso</td></tr>"
        "<tr><td><b>Ctrl+,</b></td><td>Abrir Configuración y Modelos</td></tr>"
        "<tr><td><b>F1</b></td><td>Mostrar esta ayuda</td></tr>"
        "<tr><td><b>Ctrl+Q</b></td><td>Salir</td></tr>"
        "</table>"

        "<h2>Ajustes</h2>"

        "<h3>Configuración y Modelos</h3>"
        "<p>En <b>Ajustes → Configuración y Modelos…</b> (Ctrl+,) puedes:</p>"
        "<ul>"
        "<li>Cambiar el directorio de modelos de traducción</li>"
        "<li>Establecer los idiomas origen y destino por defecto</li>"
        "<li>Activar la traducción automática al extraer</li>"
        "<li>Gestionar la descarga y activación de modelos</li>"
        "</ul>"

        "<h3>Apariencia e idioma</h3>"
        "<ul>"
        "<li><b>Tema</b> — Automático, Oscuro o Claro (se aplica al instante).</li>"
        "<li><b>Idioma de interfaz</b> — Español, Inglés o Portugués; "
        "el cambio se aplica en caliente, sin reiniciar.</li>"
        "</ul>"

        "<h3>Soporte</h3>"
        "<p>Repositorio: <b>github.com/pdmiguel90-droid/elements-extract</b></p>"
        "<p>Para reportar errores o sugerencias, abre un issue en GitHub.</p>"
        ));
    tabs->addTab(shortcutsPage, tr("Atajos y Ajustes"));

    root->addWidget(tabs, 1);

    // ── Botón cerrar ───────────────────────────────────────────────────
    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->addStretch();
    QPushButton *closeBtn = new QPushButton(QIcon(":/icons/check.svg"), tr("Cerrar"));
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    btnRow->addWidget(closeBtn);
    root->addLayout(btnRow);
}