<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="es">
<context>
    <name>AboutDialog</name>
    <message>
        <location filename="../aboutdialog.cpp" line="19"/>
        <source>Acerca de Elements Extract FFmpeg</source>
        <translation>Acerca de Elements Extract FFmpeg</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="98"/>
        <source>Desarrollado por</source>
        <translation>Desarrollado por</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="99"/>
        <source>Repositorio</source>
        <translation>Repositorio</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="101"/>
        <source>Licencia</source>
        <translation>Licencia</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="115"/>
        <source>Qt %1
Interfaz</source>
        <translation>Qt %1
Interfaz</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="130"/>
        <source>DONAR</source>
        <translation>DONAR</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="144"/>
        <source>Donar con PayPal</source>
        <translation>Donar con PayPal</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="168"/>
        <location filename="../aboutdialog.cpp" line="174"/>
        <source>Copiar</source>
        <translation>Copiar</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="172"/>
        <source>¡Copiada!</source>
        <translation>¡Copiada!</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="162"/>
        <source>Donar con PIX</source>
        <translation>Donar con PIX</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="107"/>
        <source>TECNOLOGÍAS</source>
        <translation>TECNOLOGÍAS</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="196"/>
        <source>© 2026 pdmiguel90-droid  —  GPL-3.0</source>
        <translation>© 2026 pdmiguel90-droid  —  GPL-3.0</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="116"/>
        <source>FFmpeg
Extracción</source>
        <translation>FFmpeg
Extracción</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="41"/>
        <source>Versión 1.2.10  ·  2026</source>
        <translation>Versión 1.2.10  ·  2026</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="57"/>
        <source>Extractor gráfico de pistas de audio, vídeo y subtítulos.
Procesamiento por lotes, filtro de subtítulos por idioma
y traducción automática con incrustación (remux) sin conexión.</source>
        <translation>Extractor gráfico de pistas de audio, vídeo y subtítulos.
Procesamiento por lotes, filtro de subtítulos por idioma
y traducción automática con incrustación (remux) sin conexión.</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="117"/>
        <source>bergamot
Traducción</source>
        <translation>bergamot
Traducción</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="118"/>
        <source>C++17
Lenguaje</source>
        <translation>C++17
Lenguaje</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="192"/>
        <source>Cerrar</source>
        <translation>Cerrar</translation>
    </message>
</context>
<context>
    <name>HelpDialog</name>
    <message>
        <location filename="../helpdialog.cpp" line="13"/>
        <source>Ayuda — Elements Extract</source>
        <translation>Ayuda — Elements Extract</translation>
    </message>
    <message>
        <location filename="../helpdialog.cpp" line="30"/>
        <source>&lt;h2&gt;Extracción de Pistas&lt;/h2&gt;&lt;h3&gt;1. Añadir archivos&lt;/h3&gt;&lt;p&gt;Pulsa &lt;b&gt;Añadir&lt;/b&gt; o arrastra los archivos directamente a la lista del panel izquierdo. Se admiten MKV, MP4, AVI, MOV, TS, M2TS y WebM.&lt;/p&gt;&lt;h3&gt;2. Seleccionar pistas&lt;/h3&gt;&lt;p&gt;Haz clic en un archivo para ver sus pistas (video, audio y subtítulos) con formato, idioma y resolución. Marca las que quieras extraer.&lt;/p&gt;&lt;p&gt;Usa los botones &lt;b&gt;VIDEO / AUDIO / SUBS&lt;/b&gt; para marcar (o desmarcar) todas las pistas de un tipo a la vez, y &lt;b&gt;Limpiar&lt;/b&gt; para quitarlas todas.&lt;/p&gt;&lt;h3&gt;3. Configurar el formato&lt;/h3&gt;&lt;p&gt;En &lt;b&gt;Formato de Salida&lt;/b&gt; elige el contenedor para cada tipo: audio (MKA, M4A, AAC, MP3, FLAC, Opus, WAV), video (MKV, MP4, AVI) y subtítulos (SRT, ASS, VTT), además de la resolución de vídeo.&lt;/p&gt;&lt;p&gt;Con &lt;b&gt;Copiar pista original sin re-codificar&lt;/b&gt; (recomendado) la extracción es rápida y sin pérdida de calidad: el contenedor se mantiene o se remuxea al seleccionado.&lt;/p&gt;&lt;p&gt;Sin OCR, los subtítulos de imagen (PGS, VobSub, DVB, XSUB) se extraen tal cual como &lt;b&gt;.mks&lt;/b&gt; mediante copia de flujo, sin recodificar, y no pueden traducirse. Los subtítulos de texto mantienen el flujo srt/ass/vtt y sí pueden traducirse.&lt;/p&gt;&lt;p&gt;Con &lt;b&gt;OCR de subtítulos de imagen&lt;/b&gt; (opción experimental, desactivada por defecto) la app usa &lt;b&gt;nOCR&lt;/b&gt;, el motor de plantillas de Subtitle Edit, sin Tesseract ni modelos neuronales. Lee las imágenes del subtítulo con ffmpeg y escribe un &lt;b&gt;.srt&lt;/b&gt; que puedes traducir e incrustar igual que cualquier otro subtítulo de texto. El proceso puede tardar varios minutos en un episodio completo; la app va indicando el progreso en el registro. Si el OCR falla, se mantiene la copia original a .mks para no perder la pista. La precisión depende del idioma y de la calidad de la imagen (resolución, antialiasing, ruido); revisa siempre el resultado antes de usarlo.&lt;/p&gt;&lt;p&gt;Base de plantillas: Latin.nocr, de Subtitle Edit (licencia MIT). Detalles y atribución en share/ocr/NOTICE.txt.&lt;/p&gt;&lt;h3&gt;4. Elegir destino y extraer&lt;/h3&gt;&lt;p&gt;Indica la carpeta de salida en &lt;b&gt;Destino&lt;/b&gt;. Los archivos se guardan en una subcarpeta &lt;b&gt;Elements Extract&lt;/b&gt;.&lt;/p&gt;&lt;p&gt;Pulsa &lt;b&gt;INICIAR EXTRACCIÓN&lt;/b&gt; o &lt;b&gt;Ctrl+Enter&lt;/b&gt;. Puedes cancelar con &lt;b&gt;Cancelar&lt;/b&gt; o &lt;b&gt;Escape&lt;/b&gt;.&lt;/p&gt;&lt;h3&gt;Procesamiento por lotes&lt;/h3&gt;&lt;p&gt;Si añades varios archivos se procesan en orden. La barra superior muestra el progreso y el botón &lt;b&gt;LOG&lt;/b&gt; despliega el registro de actividad.&lt;/p&gt;</source>
        <translation>&lt;h2&gt;Extracción de Pistas&lt;/h2&gt;&lt;h3&gt;1. Añadir archivos&lt;/h3&gt;&lt;p&gt;Pulsa &lt;b&gt;Añadir&lt;/b&gt; o arrastra los archivos directamente a la lista del panel izquierdo. Se admiten MKV, MP4, AVI, MOV, TS, M2TS y WebM.&lt;/p&gt;&lt;h3&gt;2. Seleccionar pistas&lt;/h3&gt;&lt;p&gt;Haz clic en un archivo para ver sus pistas (video, audio y subtítulos) con formato, idioma y resolución. Marca las que quieras extraer.&lt;/p&gt;&lt;p&gt;Usa los botones &lt;b&gt;VIDEO / AUDIO / SUBS&lt;/b&gt; para marcar (o desmarcar) todas las pistas de un tipo a la vez, y &lt;b&gt;Limpiar&lt;/b&gt; para quitarlas todas.&lt;/p&gt;&lt;h3&gt;3. Configurar el formato&lt;/h3&gt;&lt;p&gt;En &lt;b&gt;Formato de Salida&lt;/b&gt; elige el contenedor para cada tipo: audio (MKA, M4A, AAC, MP3, FLAC, Opus, WAV), video (MKV, MP4, AVI) y subtítulos (SRT, ASS, VTT), además de la resolución de vídeo.&lt;/p&gt;&lt;p&gt;Con &lt;b&gt;Copiar pista original sin re-codificar&lt;/b&gt; (recomendado) la extracción es rápida y sin pérdida de calidad: el contenedor se mantiene o se remuxea al seleccionado.&lt;/p&gt;&lt;p&gt;Sin OCR, los subtítulos de imagen (PGS, VobSub, DVB, XSUB) se extraen tal cual como &lt;b&gt;.mks&lt;/b&gt; mediante copia de flujo, sin recodificar, y no pueden traducirse. Los subtítulos de texto mantienen el flujo srt/ass/vtt y sí pueden traducirse.&lt;/p&gt;&lt;p&gt;Con &lt;b&gt;OCR de subtítulos de imagen&lt;/b&gt; (opción experimental, desactivada por defecto) la app usa &lt;b&gt;nOCR&lt;/b&gt;, el motor de plantillas de Subtitle Edit, sin Tesseract ni modelos neuronales. Lee las imágenes del subtítulo con ffmpeg y escribe un &lt;b&gt;.srt&lt;/b&gt; que puedes traducir e incrustar igual que cualquier otro subtítulo de texto. El proceso puede tardar varios minutos en un episodio completo; la app va indicando el progreso en el registro. Si el OCR falla, se mantiene la copia original a .mks para no perder la pista. La precisión depende del idioma y de la calidad de la imagen (resolución, antialiasing, ruido); revisa siempre el resultado antes de usarlo.&lt;/p&gt;&lt;p&gt;Base de plantillas: Latin.nocr, de Subtitle Edit (licencia MIT). Detalles y atribución en share/ocr/NOTICE.txt.&lt;/p&gt;&lt;h3&gt;4. Elegir destino y extraer&lt;/h3&gt;&lt;p&gt;Indica la carpeta de salida en &lt;b&gt;Destino&lt;/b&gt;. Los archivos se guardan en una subcarpeta &lt;b&gt;Elements Extract&lt;/b&gt;.&lt;/p&gt;&lt;p&gt;Pulsa &lt;b&gt;INICIAR EXTRACCIÓN&lt;/b&gt; o &lt;b&gt;Ctrl+Enter&lt;/b&gt;. Puedes cancelar con &lt;b&gt;Cancelar&lt;/b&gt; o &lt;b&gt;Escape&lt;/b&gt;.&lt;/p&gt;&lt;h3&gt;Procesamiento por lotes&lt;/h3&gt;&lt;p&gt;Si añades varios archivos se procesan en orden. La barra superior muestra el progreso y el botón &lt;b&gt;LOG&lt;/b&gt; despliega el registro de actividad.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../helpdialog.cpp" line="76"/>
        <source>Extracción</source>
        <translation>Extracción</translation>
    </message>
    <message>
        <location filename="../helpdialog.cpp" line="84"/>
        <source>&lt;h2&gt;Traducción de Subtítulos&lt;/h2&gt;&lt;h3&gt;1. Elegir idiomas&lt;/h3&gt;&lt;p&gt;Selecciona el &lt;b&gt;idioma origen&lt;/b&gt; y el &lt;b&gt;destino&lt;/b&gt; en el panel de traducción. El idioma origen se &lt;b&gt;detecta automáticamente&lt;/b&gt; al seleccionar una pista de subtítulos.&lt;/p&gt;&lt;h3&gt;2. Traducir al extraer&lt;/h3&gt;&lt;p&gt;Marca &lt;b&gt;Traducir al extraer&lt;/b&gt; para traducir automáticamente los subtítulos extraídos. Se guardan dos archivos: &lt;b&gt;archivo.srt&lt;/b&gt; (original) y &lt;b&gt;archivo.IDIOMA.srt&lt;/b&gt; (traducido), donde IDIOMA es el idioma de destino elegido.&lt;/p&gt;&lt;p&gt;Con &lt;b&gt;Traducir subtítulos&lt;/b&gt; puedes traducir en lote subtítulos ya existentes en disco.&lt;/p&gt;&lt;h3&gt;3. Trabajar con un solo idioma&lt;/h3&gt;&lt;p&gt;El desplegable &lt;b&gt;Solo subs&lt;/b&gt; (panel izquierdo) selecciona a la vez los subtítulos de un idioma concreto en todos los archivos del lote; los archivos sin ese idioma se omiten. Con el filtro activo, la traducción usa ese idioma como origen automáticamente.&lt;/p&gt;&lt;h3&gt;4. Incrustar la traducción (remux)&lt;/h3&gt;&lt;p&gt;Marca &lt;b&gt;Incrustar la traducción (remux)&lt;/b&gt; para generar un &lt;b&gt;archivo.IDIOMA.mkv&lt;/b&gt; (o .mp4 si el original es .mp4) con el vídeo y el audio originales y los subtítulos traducidos dentro, sin re-codificar. Si un vídeo tiene varias pistas traducidas, van todas en el mismo archivo (cada una conserva su título original cuando lo tiene).&lt;/p&gt;&lt;p&gt;La app valida cada archivo antes de incrustar: si falta pista de vídeo lo omite, y si el .mp4 tiene códecs no copiables (audio pcm_*, vorbis, flac, dts, truehd…) cambia a .mkv automáticamente. Al terminar se muestra un resumen con los incrustados/omitidos y el motivo de cada fallo. Con &lt;b&gt;Ajustes → Eliminar los .srt intermedios tras incrustarlos&lt;/b&gt; se borran los subtítulos extraídos y traducidos de cada vídeo cuyo remux terminó bien.&lt;/p&gt;&lt;h3&gt;5. Traducción triangular&lt;/h3&gt;&lt;p&gt;Si no existe un modelo directo (p. ej. Portugués → Español), se usa &lt;b&gt;traducción triangular&lt;/b&gt;: Portugués → Inglés → Español. Se te informará antes de empezar y requieren los modelos intermedios instalados.&lt;/p&gt;&lt;p&gt;También se aplica a subtítulos descargados de internet.&lt;/p&gt;&lt;h3&gt;6. Gestionar modelos&lt;/h3&gt;&lt;p&gt;Abre &lt;b&gt;Ajustes → Configuración y Modelos…&lt;/b&gt; (o el botón &lt;b&gt;Modelos&lt;/b&gt;) y revisa la pestaña &lt;b&gt;Modelos de Traducción&lt;/b&gt;:&lt;/p&gt;&lt;ul&gt;&lt;li&gt;&lt;b&gt;Instalados&lt;/b&gt; — modelos descargados; doble clic para activar/desactivar.&lt;/li&gt;&lt;li&gt;&lt;b&gt;Online&lt;/b&gt; — modelos disponibles; doble clic para descargar.&lt;/li&gt;&lt;/ul&gt;&lt;p&gt;El panel señala qué pares tienen modelo directo (✓), cuáles requieren triangulación (↻) y cuáles no están disponibles (✗).&lt;/p&gt;&lt;h3&gt;7. Descarga de modelos&lt;/h3&gt;&lt;p&gt;Los modelos se descargan de &lt;b&gt;translatelocally.com&lt;/b&gt;. Los modelos &lt;b&gt;tiny&lt;/b&gt; (~20 MB) son rápidos y los &lt;b&gt;base&lt;/b&gt; (~50 MB) ofrecen mejor calidad. La traducción funciona &lt;b&gt;sin conexión&lt;/b&gt;.&lt;/p&gt;&lt;h3&gt;Idiomas soportados&lt;/h3&gt;&lt;p&gt;Inglés, Español, Francés, Portugués, Alemán, Italiano, Japonés, Chino, Coreano, Ruso, Árabe, Hindi, Holandés, Sueco, Polaco y Turco.&lt;/p&gt;</source>
        <translation>&lt;h2&gt;Traducción de Subtítulos&lt;/h2&gt;&lt;h3&gt;1. Elegir idiomas&lt;/h3&gt;&lt;p&gt;Selecciona el &lt;b&gt;idioma origen&lt;/b&gt; y el &lt;b&gt;destino&lt;/b&gt; en el panel de traducción. El idioma origen se &lt;b&gt;detecta automáticamente&lt;/b&gt; al seleccionar una pista de subtítulos.&lt;/p&gt;&lt;h3&gt;2. Traducir al extraer&lt;/h3&gt;&lt;p&gt;Marca &lt;b&gt;Traducir al extraer&lt;/b&gt; para traducir automáticamente los subtítulos extraídos. Se guardan dos archivos: &lt;b&gt;archivo.srt&lt;/b&gt; (original) y &lt;b&gt;archivo.IDIOMA.srt&lt;/b&gt; (traducido), donde IDIOMA es el idioma de destino elegido.&lt;/p&gt;&lt;p&gt;Con &lt;b&gt;Traducir subtítulos&lt;/b&gt; puedes traducir en lote subtítulos ya existentes en disco.&lt;/p&gt;&lt;h3&gt;3. Trabajar con un solo idioma&lt;/h3&gt;&lt;p&gt;El desplegable &lt;b&gt;Solo subs&lt;/b&gt; (panel izquierdo) selecciona a la vez los subtítulos de un idioma concreto en todos los archivos del lote; los archivos sin ese idioma se omiten. Con el filtro activo, la traducción usa ese idioma como origen automáticamente.&lt;/p&gt;&lt;h3&gt;4. Incrustar la traducción (remux)&lt;/h3&gt;&lt;p&gt;Marca &lt;b&gt;Incrustar la traducción (remux)&lt;/b&gt; para generar un &lt;b&gt;archivo.IDIOMA.mkv&lt;/b&gt; (o .mp4 si el original es .mp4) con el vídeo y el audio originales y los subtítulos traducidos dentro, sin re-codificar. Si un vídeo tiene varias pistas traducidas, van todas en el mismo archivo (cada una conserva su título original cuando lo tiene).&lt;/p&gt;&lt;p&gt;La app valida cada archivo antes de incrustar: si falta pista de vídeo lo omite, y si el .mp4 tiene códecs no copiables (audio pcm_*, vorbis, flac, dts, truehd…) cambia a .mkv automáticamente. Al terminar se muestra un resumen con los incrustados/omitidos y el motivo de cada fallo. Con &lt;b&gt;Ajustes → Eliminar los .srt intermedios tras incrustarlos&lt;/b&gt; se borran los subtítulos extraídos y traducidos de cada vídeo cuyo remux terminó bien.&lt;/p&gt;&lt;h3&gt;5. Traducción triangular&lt;/h3&gt;&lt;p&gt;Si no existe un modelo directo (p. ej. Portugués → Español), se usa &lt;b&gt;traducción triangular&lt;/b&gt;: Portugués → Inglés → Español. Se te informará antes de empezar y requieren los modelos intermedios instalados.&lt;/p&gt;&lt;p&gt;También se aplica a subtítulos descargados de internet.&lt;/p&gt;&lt;h3&gt;6. Gestionar modelos&lt;/h3&gt;&lt;p&gt;Abre &lt;b&gt;Ajustes → Configuración y Modelos…&lt;/b&gt; (o el botón &lt;b&gt;Modelos&lt;/b&gt;) y revisa la pestaña &lt;b&gt;Modelos de Traducción&lt;/b&gt;:&lt;/p&gt;&lt;ul&gt;&lt;li&gt;&lt;b&gt;Instalados&lt;/b&gt; — modelos descargados; doble clic para activar/desactivar.&lt;/li&gt;&lt;li&gt;&lt;b&gt;Online&lt;/b&gt; — modelos disponibles; doble clic para descargar.&lt;/li&gt;&lt;/ul&gt;&lt;p&gt;El panel señala qué pares tienen modelo directo (✓), cuáles requieren triangulación (↻) y cuáles no están disponibles (✗).&lt;/p&gt;&lt;h3&gt;7. Descarga de modelos&lt;/h3&gt;&lt;p&gt;Los modelos se descargan de &lt;b&gt;translatelocally.com&lt;/b&gt;. Los modelos &lt;b&gt;tiny&lt;/b&gt; (~20 MB) son rápidos y los &lt;b&gt;base&lt;/b&gt; (~50 MB) ofrecen mejor calidad. La traducción funciona &lt;b&gt;sin conexión&lt;/b&gt;.&lt;/p&gt;&lt;h3&gt;Idiomas soportados&lt;/h3&gt;&lt;p&gt;Inglés, Español, Francés, Portugués, Alemán, Italiano, Japonés, Chino, Coreano, Ruso, Árabe, Hindi, Holandés, Sueco, Polaco y Turco.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../helpdialog.cpp" line="143"/>
        <source>&lt;h3&gt;8. Subtítulos online&lt;/h3&gt;&lt;p&gt;Pulsa &lt;b&gt;Buscar subs online&lt;/b&gt; junto a los botones Añadir/Quitar, usa &lt;b&gt;Archivo → Buscar subtítulos en internet…&lt;/b&gt; (Ctrl+Shift+S) o el &lt;b&gt;clic derecho&lt;/b&gt; sobre un archivo de la lista. Puedes seleccionar varios vídeos a la vez para procesarlos en lote.&lt;/p&gt;&lt;p&gt;En el diálogo, marca los archivos y pulsa &lt;b&gt;Buscar en seleccionados&lt;/b&gt; o &lt;b&gt;Buscar en todos&lt;/b&gt;. Haz doble clic en la columna &lt;b&gt;Consulta&lt;/b&gt; para cambiar el título buscado de uno solo; el idioma del desplegable se aplica a toda la búsqueda.&lt;/p&gt;&lt;p&gt;La búsqueda combina dos pasos: primero busca por hash de archivo (la coincidencia exacta de esa versión) y después por título, temporada y capítulo. Se unen los resultados de ambos sin repetir los iguales, porque el hash de un BluRay suele devolver solo los subtítulos en inglés de esa edición aunque el mismo capítulo tenga subtítulos en otros idiomas. Si eliges un idioma concreto, se aplica como filtro en los dos pasos.&lt;/p&gt;&lt;p&gt;El idioma de destino configurado en &lt;b&gt;Ajustes&lt;/b&gt; aparece marcado como &lt;b&gt;(preferido)&lt;/b&gt; en el desplegable y sus resultados se ordenan primero. Si algún archivo no tiene subtítulos en ese idioma, la app lo avisa; al pulsar &lt;b&gt;Descargar y traducir seleccionados&lt;/b&gt; te preguntará si quieres descargar el mejor subtítulo disponible (p. ej. en inglés) y traducirlo al idioma destino mediante la ruta triangular, siempre que los modelos intermedios estén instalados.&lt;/p&gt;&lt;p&gt;Marca las casillas de los resultados que quieres y elige &lt;b&gt;Descargar y traducir seleccionados&lt;/b&gt; o &lt;b&gt;Solo descargar seleccionados&lt;/b&gt;. Puedes combinar varios archivos y varios subtítulos en la misma selección; las descargas se hacen en secuencia y se informa del resultado de cada archivo.&lt;/p&gt;&lt;p&gt;Por defecto se usa el backend &lt;b&gt;XML-RPC de opensubtitles.org&lt;/b&gt; (el mismo que VLSub en VLC): permite buscar y descargar sin cuenta ni registro, usando el enlace directo que devuelve cada resultado. En &lt;b&gt;Ajustes → Subtítulos online&lt;/b&gt; puedes cambiar a la &lt;b&gt;API REST de opensubtitles.com&lt;/b&gt;, que requiere tu propia clave API, usuario y contraseña. Las credenciales se guardan en el almacén seguro del sistema.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;8. Subtítulos online&lt;/h3&gt;&lt;p&gt;Pulsa &lt;b&gt;Buscar subs online&lt;/b&gt; junto a los botones Añadir/Quitar, usa &lt;b&gt;Archivo → Buscar subtítulos en internet…&lt;/b&gt; (Ctrl+Shift+S) o el &lt;b&gt;clic derecho&lt;/b&gt; sobre un archivo de la lista. Puedes seleccionar varios vídeos a la vez para procesarlos en lote.&lt;/p&gt;&lt;p&gt;En el diálogo, marca los archivos y pulsa &lt;b&gt;Buscar en seleccionados&lt;/b&gt; o &lt;b&gt;Buscar en todos&lt;/b&gt;. Haz doble clic en la columna &lt;b&gt;Consulta&lt;/b&gt; para cambiar el título buscado de uno solo; el idioma del desplegable se aplica a toda la búsqueda.&lt;/p&gt;&lt;p&gt;La búsqueda combina dos pasos: primero busca por hash de archivo (la coincidencia exacta de esa versión) y después por título, temporada y capítulo. Se unen los resultados de ambos sin repetir los iguales, porque el hash de un BluRay suele devolver solo los subtítulos en inglés de esa edición aunque el mismo capítulo tenga subtítulos en otros idiomas. Si eliges un idioma concreto, se aplica como filtro en los dos pasos.&lt;/p&gt;&lt;p&gt;El idioma de destino configurado en &lt;b&gt;Ajustes&lt;/b&gt; aparece marcado como &lt;b&gt;(preferido)&lt;/b&gt; en el desplegable y sus resultados se ordenan primero. Si algún archivo no tiene subtítulos en ese idioma, la app lo avisa; al pulsar &lt;b&gt;Descargar y traducir seleccionados&lt;/b&gt; te preguntará si quieres descargar el mejor subtítulo disponible (p. ej. en inglés) y traducirlo al idioma destino mediante la ruta triangular, siempre que los modelos intermedios estén instalados.&lt;/p&gt;&lt;p&gt;Marca las casillas de los resultados que quieres y elige &lt;b&gt;Descargar y traducir seleccionados&lt;/b&gt; o &lt;b&gt;Solo descargar seleccionados&lt;/b&gt;. Puedes combinar varios archivos y varios subtítulos en la misma selección; las descargas se hacen en secuencia y se informa del resultado de cada archivo.&lt;/p&gt;&lt;p&gt;Por defecto se usa el backend &lt;b&gt;XML-RPC de opensubtitles.org&lt;/b&gt; (el mismo que VLSub en VLC): permite buscar y descargar sin cuenta ni registro, usando el enlace directo que devuelve cada resultado. En &lt;b&gt;Ajustes → Subtítulos online&lt;/b&gt; puedes cambiar a la &lt;b&gt;API REST de opensubtitles.com&lt;/b&gt;, que requiere tu propia clave API, usuario y contraseña. Las credenciales se guardan en el almacén seguro del sistema.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../helpdialog.cpp" line="174"/>
        <source>Traducción</source>
        <translation>Traducción</translation>
    </message>
    <message>
        <location filename="../helpdialog.cpp" line="182"/>
        <source>&lt;h2&gt;Atajos de Teclado&lt;/h2&gt;&lt;table cellpadding=&apos;6&apos; cellspacing=&apos;0&apos; border=&apos;0&apos;&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Ctrl+O&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Añadir archivos&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Supr&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Quitar archivos seleccionados&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Ctrl+Enter&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Iniciar extracción&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Escape&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Cancelar el proceso en curso&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Ctrl+,&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Abrir Configuración y Modelos&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Ctrl+Shift+S&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Buscar subtítulos en internet…&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;F1&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Mostrar esta ayuda&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Ctrl+Q&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Salir&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;&lt;h2&gt;Ajustes&lt;/h2&gt;&lt;h3&gt;Configuración y Modelos&lt;/h3&gt;&lt;p&gt;En &lt;b&gt;Ajustes → Configuración y Modelos…&lt;/b&gt; (Ctrl+,) puedes:&lt;/p&gt;&lt;ul&gt;&lt;li&gt;Cambiar el directorio de modelos de traducción&lt;/li&gt;&lt;li&gt;Establecer los idiomas origen y destino por defecto&lt;/li&gt;&lt;li&gt;Activar la traducción automática al extraer&lt;/li&gt;&lt;li&gt;Gestionar la descarga y activación de modelos&lt;/li&gt;&lt;/ul&gt;&lt;h3&gt;Apariencia e idioma&lt;/h3&gt;&lt;ul&gt;&lt;li&gt;&lt;b&gt;Tema&lt;/b&gt; — Automático, Oscuro o Claro (se aplica al instante).&lt;/li&gt;&lt;li&gt;&lt;b&gt;Idioma de interfaz&lt;/b&gt; — Español, Inglés o Portugués; el cambio se aplica en caliente, sin reiniciar.&lt;/li&gt;&lt;/ul&gt;&lt;h3&gt;Soporte&lt;/h3&gt;&lt;p&gt;Repositorio: &lt;b&gt;github.com/pdmiguel90-droid/elements-extract&lt;/b&gt;&lt;/p&gt;&lt;p&gt;Para reportar errores o sugerencias, abre un issue en GitHub.&lt;/p&gt;</source>
        <translation>&lt;h2&gt;Atajos de Teclado&lt;/h2&gt;&lt;table cellpadding=&apos;6&apos; cellspacing=&apos;0&apos; border=&apos;0&apos;&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Ctrl+O&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Añadir archivos&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Supr&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Quitar archivos seleccionados&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Ctrl+Enter&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Iniciar extracción&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Escape&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Cancelar el proceso en curso&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Ctrl+,&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Abrir Configuración y Modelos&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Ctrl+Shift+S&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Buscar subtítulos en internet…&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;F1&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Mostrar esta ayuda&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Ctrl+Q&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Salir&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;&lt;h2&gt;Ajustes&lt;/h2&gt;&lt;h3&gt;Configuración y Modelos&lt;/h3&gt;&lt;p&gt;En &lt;b&gt;Ajustes → Configuración y Modelos…&lt;/b&gt; (Ctrl+,) puedes:&lt;/p&gt;&lt;ul&gt;&lt;li&gt;Cambiar el directorio de modelos de traducción&lt;/li&gt;&lt;li&gt;Establecer los idiomas origen y destino por defecto&lt;/li&gt;&lt;li&gt;Activar la traducción automática al extraer&lt;/li&gt;&lt;li&gt;Gestionar la descarga y activación de modelos&lt;/li&gt;&lt;/ul&gt;&lt;h3&gt;Apariencia e idioma&lt;/h3&gt;&lt;ul&gt;&lt;li&gt;&lt;b&gt;Tema&lt;/b&gt; — Automático, Oscuro o Claro (se aplica al instante).&lt;/li&gt;&lt;li&gt;&lt;b&gt;Idioma de interfaz&lt;/b&gt; — Español, Inglés o Portugués; el cambio se aplica en caliente, sin reiniciar.&lt;/li&gt;&lt;/ul&gt;&lt;h3&gt;Soporte&lt;/h3&gt;&lt;p&gt;Repositorio: &lt;b&gt;github.com/pdmiguel90-droid/elements-extract&lt;/b&gt;&lt;/p&gt;&lt;p&gt;Para reportar errores o sugerencias, abre un issue en GitHub.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../helpdialog.cpp" line="220"/>
        <source>&lt;h3&gt;Actualizaciones&lt;/h3&gt;&lt;p&gt;La aplicación comprueba si hay versiones nuevas al arrancar. También puedes usar &lt;b&gt;Ayuda → Buscar actualizaciones…&lt;/b&gt; cuando quieras. El aviso muestra las notas de la versión y un enlace de descarga; si eliges «No mostrar esta versión», no volverá a aparecer.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Actualizaciones&lt;/h3&gt;&lt;p&gt;La aplicación comprueba si hay versiones nuevas al arrancar. También puedes usar &lt;b&gt;Ayuda → Buscar actualizaciones…&lt;/b&gt; cuando quieras. El aviso muestra las notas de la versión y un enlace de descarga; si eliges «No mostrar esta versión», no volverá a aparecer.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../helpdialog.cpp" line="227"/>
        <source>Atajos y Ajustes</source>
        <translation>Atajos y Ajustes</translation>
    </message>
    <message>
        <location filename="../helpdialog.cpp" line="234"/>
        <source>Cerrar</source>
        <translation>Cerrar</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../mainwindow.cpp" line="239"/>
        <source>↻ Traducción triangular: %1</source>
        <translation>↻ Traducción triangular: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="255"/>
        <source>[ONLINE-SUBS] %1</source>
        <translation>[ONLINE-SUBS] %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="282"/>
        <source>Mostrar / Ocultar ventana</source>
        <translation>Mostrar / Ocultar ventana</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="288"/>
        <source>Cancelar proceso actual</source>
        <translation>Cancelar proceso actual</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="293"/>
        <location filename="../mainwindow.cpp" line="347"/>
        <location filename="../mainwindow.cpp" line="2736"/>
        <source>Salir</source>
        <translation>Salir</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="332"/>
        <location filename="../mainwindow.cpp" line="2731"/>
        <source>Archivo</source>
        <translation>Archivo</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="334"/>
        <location filename="../mainwindow.cpp" line="2734"/>
        <source>Añadir archivos…</source>
        <translation>Añadir archivos…</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="337"/>
        <location filename="../mainwindow.cpp" line="2735"/>
        <source>Buscar subtítulos en internet…</source>
        <translation>Buscar subtítulos en internet…</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="351"/>
        <location filename="../mainwindow.cpp" line="2732"/>
        <source>Ajustes</source>
        <translation>Ajustes</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="353"/>
        <location filename="../mainwindow.cpp" line="2737"/>
        <source>Configuración y Modelos…</source>
        <translation>Configuración y Modelos…</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="357"/>
        <location filename="../mainwindow.cpp" line="2733"/>
        <source>Ayuda</source>
        <translation>Ayuda</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="359"/>
        <location filename="../mainwindow.cpp" line="2738"/>
        <source>Manual de uso</source>
        <translation>Manual de uso</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="369"/>
        <location filename="../mainwindow.cpp" line="2740"/>
        <source>Acerca de…</source>
        <translation>Acerca de…</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="400"/>
        <location filename="../mainwindow.cpp" line="2743"/>
        <source>LOTE DE ARCHIVOS</source>
        <translation>LOTE DE ARCHIVOS</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="413"/>
        <location filename="../mainwindow.cpp" line="2746"/>
        <source>Añadir</source>
        <translation>Añadir</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="414"/>
        <location filename="../mainwindow.cpp" line="2750"/>
        <source>Quitar</source>
        <translation>Quitar</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="415"/>
        <location filename="../mainwindow.cpp" line="2754"/>
        <source>Buscar subs online</source>
        <translation>Buscar subs online</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="416"/>
        <location filename="../mainwindow.cpp" line="2747"/>
        <source>Añadir archivos de video (Ctrl+O)</source>
        <translation>Añadir archivos de video (Ctrl+O)</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="417"/>
        <location filename="../mainwindow.cpp" line="2751"/>
        <source>Quitar archivos seleccionados (Del)</source>
        <translation>Quitar archivos seleccionados (Del)</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="418"/>
        <location filename="../mainwindow.cpp" line="2755"/>
        <source>Buscar y descargar subtítulos para los archivos seleccionados</source>
        <translation>Buscar y descargar subtítulos para los archivos seleccionados</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="433"/>
        <location filename="../mainwindow.cpp" line="2758"/>
        <source>Selecciona un archivo para ver sus pistas</source>
        <translation>Selecciona un archivo para ver sus pistas</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="444"/>
        <location filename="../mainwindow.cpp" line="2759"/>
        <source>VIDEO</source>
        <translation>VIDEO</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="445"/>
        <location filename="../mainwindow.cpp" line="2760"/>
        <source>AUDIO</source>
        <translation>AUDIO</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="446"/>
        <location filename="../mainwindow.cpp" line="2761"/>
        <source>SUBS</source>
        <translation>SUBS</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="447"/>
        <location filename="../mainwindow.cpp" line="2762"/>
        <source>Limpiar</source>
        <translation>Limpiar</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="465"/>
        <location filename="../mainwindow.cpp" line="2763"/>
        <source>Solo subs:</source>
        <translation>Solo subs:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="468"/>
        <location filename="../mainwindow.cpp" line="2360"/>
        <source>Todos los idiomas</source>
        <translation>Todos los idiomas</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="470"/>
        <location filename="../mainwindow.cpp" line="2766"/>
        <source>Selecciona los subtítulos de un idioma concreto en TODOS los archivos del lote.
Mientras esté activo, la extracción y la traducción trabajarán solo con ese idioma.</source>
        <translation>Selecciona los subtítulos de un idioma concreto en TODOS los archivos del lote.
Mientras esté activo, la extracción y la traducción trabajarán solo con ese idioma.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="496"/>
        <location filename="../mainwindow.cpp" line="2772"/>
        <source>FORMATO DE SALIDA</source>
        <translation>FORMATO DE SALIDA</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="518"/>
        <location filename="../mainwindow.cpp" line="2773"/>
        <source>Audio:</source>
        <translation>Audio:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="519"/>
        <location filename="../mainwindow.cpp" line="2774"/>
        <source>Video:</source>
        <translation>Video:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="520"/>
        <location filename="../mainwindow.cpp" line="2775"/>
        <source>Res:</source>
        <translation>Res:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="521"/>
        <location filename="../mainwindow.cpp" line="2776"/>
        <source>Subs:</source>
        <translation>Subs:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="530"/>
        <location filename="../mainwindow.cpp" line="2778"/>
        <source>Copiar pista original sin re-codificar (Rápido y previene errores)</source>
        <translation>Copiar pista original sin re-codificar (Rápido y previene errores)</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="547"/>
        <location filename="../mainwindow.cpp" line="2788"/>
        <source>TRADUCCIÓN DE SUBTÍTULOS</source>
        <translation>TRADUCCIÓN DE SUBTÍTULOS</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="552"/>
        <location filename="../mainwindow.cpp" line="2789"/>
        <source>Origen:</source>
        <translation>Origen:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="558"/>
        <location filename="../mainwindow.cpp" line="2790"/>
        <source>Destino:</source>
        <translation>Destino:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="570"/>
        <location filename="../mainwindow.cpp" line="2792"/>
        <source>Traducir al extraer</source>
        <translation>Traducir al extraer</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="572"/>
        <location filename="../mainwindow.cpp" line="2794"/>
        <source>Si está marcado, al extraer subtítulos se traducirán automáticamente
al idioma de destino seleccionado.</source>
        <translation>Si está marcado, al extraer subtítulos se traducirán automáticamente
al idioma de destino seleccionado.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="580"/>
        <location filename="../mainwindow.cpp" line="2798"/>
        <source>OCR de subtítulos de imagen (PGS/VobSub)</source>
        <translation>OCR de subtítulos de imagen (PGS/VobSub)</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2800"/>
        <source>Convierte PGS/VobSub/DVB/XSUB a .srt con nOCR (sin modelos pesados).
El resultado puede traducirse e incrustarse como cualquier otro subtítulo de texto.</source>
        <translation>Convierte PGS/VobSub/DVB/XSUB a .srt con nOCR (sin modelos pesados).
El resultado puede traducirse e incrustarse como cualquier otro subtítulo de texto.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="590"/>
        <location filename="../mainwindow.cpp" line="2804"/>
        <source>Incrustar la traducción (remux)</source>
        <translation>Incrustar la traducción (remux)</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="592"/>
        <location filename="../mainwindow.cpp" line="2806"/>
        <source>Tras traducir, genera un archivo .mkv/.mp4 con el vídeo y el audio originales
y la pista de subtítulos traducida incrustada, sin re-codificar (rápido y sin pérdida).</source>
        <translation>Tras traducir, genera un archivo .mkv/.mp4 con el vídeo y el audio originales
y la pista de subtítulos traducida incrustada, sin re-codificar (rápido y sin pérdida).</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="598"/>
        <location filename="../mainwindow.cpp" line="2810"/>
        <source>Modelos</source>
        <translation>Modelos</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="599"/>
        <location filename="../mainwindow.cpp" line="2811"/>
        <source>Gestionar modelos de traducción</source>
        <translation>Gestionar modelos de traducción</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="601"/>
        <location filename="../mainwindow.cpp" line="2814"/>
        <source>Traducir subtítulos</source>
        <translation>Traducir subtítulos</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="603"/>
        <location filename="../mainwindow.cpp" line="2816"/>
        <source>Seleccionar subtítulos ya existentes en disco y traducirlos en lote</source>
        <translation>Seleccionar subtítulos ya existentes en disco y traducirlos en lote</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="616"/>
        <location filename="../mainwindow.cpp" line="2819"/>
        <source>Selecciona subtítulos y marca &quot;Traducir al extraer&quot;</source>
        <translation>Selecciona subtítulos y marca &quot;Traducir al extraer&quot;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="628"/>
        <location filename="../mainwindow.cpp" line="2824"/>
        <source>Carpeta donde se guardarán los archivos extraídos…</source>
        <translation>Carpeta donde se guardarán los archivos extraídos…</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="633"/>
        <location filename="../mainwindow.cpp" line="2827"/>
        <source>Seleccionar carpeta de destino</source>
        <translation>Seleccionar carpeta de destino</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="634"/>
        <location filename="../mainwindow.cpp" line="2830"/>
        <source>Abrir</source>
        <translation>Abrir</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="635"/>
        <location filename="../mainwindow.cpp" line="2831"/>
        <source>Abrir carpeta de destino en el explorador</source>
        <translation>Abrir carpeta de destino en el explorador</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="721"/>
        <location filename="../mainwindow.cpp" line="2848"/>
        <source>Sistema listo. Añade archivos y selecciona las pistas a extraer.</source>
        <translation>Sistema listo. Añade archivos y selecciona las pistas a extraer.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="663"/>
        <location filename="../mainwindow.cpp" line="2836"/>
        <source>Cancelar</source>
        <translation>Cancelar</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="666"/>
        <location filename="../mainwindow.cpp" line="2837"/>
        <source>Cancelar el proceso en curso (Escape)</source>
        <translation>Cancelar el proceso en curso (Escape)</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="669"/>
        <location filename="../mainwindow.cpp" line="2840"/>
        <source>INICIAR EXTRACCIÓN</source>
        <translation>INICIAR EXTRACCIÓN</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="673"/>
        <location filename="../mainwindow.cpp" line="2841"/>
        <source>Iniciar la extracción de todas las pistas seleccionadas (Ctrl+Enter)</source>
        <translation>Iniciar la extracción de todas las pistas seleccionadas (Ctrl+Enter)</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="724"/>
        <location filename="../mainwindow.cpp" line="809"/>
        <location filename="../mainwindow.cpp" line="1099"/>
        <location filename="../mainwindow.cpp" line="2728"/>
        <source>Listo</source>
        <translation>Listo</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="726"/>
        <location filename="../mainwindow.cpp" line="2727"/>
        <source>Elements Extract FFmpeg</source>
        <translation>Elements Extract FFmpeg</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="778"/>
        <source>Modo traducción: %1 subtítulo(s) abierto(s) desde el sistema.</source>
        <translation>Modo traducción: %1 subtítulo(s) abierto(s) desde el sistema.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="787"/>
        <source>Añadir Videos</source>
        <translation>Añadir Videos</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="788"/>
        <source>Videos (*.mkv *.mp4 *.avi *.mov *.ts *.m2ts *.webm);;Todos los archivos (*.*)</source>
        <translation>Videos (*.mkv *.mp4 *.avi *.mov *.ts *.m2ts *.webm);;Todos los archivos (*.*)</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="806"/>
        <source>No hay archivos cargados</source>
        <translation>No hay archivos cargados</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="835"/>
        <source>Analizando: </source>
        <translation>Analizando: </translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="837"/>
        <source>Analizando…</source>
        <translation>Analizando…</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="933"/>
        <source>Idioma origen detectado: %1</source>
        <translation>Idioma origen detectado: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="952"/>
        <source>Traducción triangular disponible</source>
        <translation>Traducción triangular disponible</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="955"/>
        <source>No hay modelo directo de &lt;b&gt;%1 → %2&lt;/b&gt;.&lt;br&gt;&lt;br&gt;Pero se usará &lt;b&gt;traducción triangular&lt;/b&gt;:&lt;br&gt;%3&lt;br&gt;&lt;br&gt;Esto puede tardar un poco más, pero funcionará correctamente.</source>
        <translation>No hay modelo directo de &lt;b&gt;%1 → %2&lt;/b&gt;.&lt;br&gt;&lt;br&gt;Pero se usará &lt;b&gt;traducción triangular&lt;/b&gt;:&lt;br&gt;%3&lt;br&gt;&lt;br&gt;Esto puede tardar un poco más, pero funcionará correctamente.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="961"/>
        <location filename="../mainwindow.cpp" line="1011"/>
        <source>¿Qué deseas hacer?</source>
        <translation>¿Qué deseas hacer?</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="962"/>
        <source>Aceptar</source>
        <translation>Aceptar</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="963"/>
        <location filename="../mainwindow.cpp" line="1015"/>
        <source>Cambiar idioma destino</source>
        <translation>Cambiar idioma destino</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="983"/>
        <source>Modelo de traducción no instalado</source>
        <translation>Modelo de traducción no instalado</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="985"/>
        <source>No hay modelo instalado para:&lt;br&gt;&lt;b&gt;%1&lt;/b&gt;&lt;br&gt;&lt;br&gt;Pero existe para descargar: &lt;b&gt;%2&lt;/b&gt;&lt;br&gt;&lt;br&gt;Idioma detectado: %3 → %4</source>
        <translation>No hay modelo instalado para:&lt;br&gt;&lt;b&gt;%1&lt;/b&gt;&lt;br&gt;&lt;br&gt;Pero existe para descargar: &lt;b&gt;%2&lt;/b&gt;&lt;br&gt;&lt;br&gt;Idioma detectado: %3 → %4</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="992"/>
        <source>Modelo de traducción no encontrado</source>
        <translation>Modelo de traducción no encontrado</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1014"/>
        <source>Descargar modelo</source>
        <translation>Descargar modelo</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1016"/>
        <source>Continuar sin traducir</source>
        <translation>Continuar sin traducir</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1115"/>
        <source>No se pudo analizar el archivo.</source>
        <translation>No se pudo analizar el archivo.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1277"/>
        <source>&lt;font color=&apos;#FF8800&apos;&gt;&lt;b&gt;=== INICIANDO EXTRACCIÓN ===&lt;/b&gt;&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FF8800&apos;&gt;&lt;b&gt;=== INICIANDO EXTRACCIÓN ===&lt;/b&gt;&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1291"/>
        <source>&lt;br&gt;&lt;font color=&apos;#FF4444&apos;&gt;&lt;b&gt;[CANCELADO] La traducción fue interrumpida por el usuario.&lt;/b&gt;&lt;/font&gt;</source>
        <translation>&lt;br&gt;&lt;font color=&apos;#FF4444&apos;&gt;&lt;b&gt;[CANCELADO] La traducción fue interrumpida por el usuario.&lt;/b&gt;&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1312"/>
        <source>&lt;br&gt;&lt;font color=&apos;#FF4444&apos;&gt;&lt;b&gt;[CANCELADO] El proceso fue interrumpido por el usuario.&lt;/b&gt;&lt;/font&gt;</source>
        <translation>&lt;br&gt;&lt;font color=&apos;#FF4444&apos;&gt;&lt;b&gt;[CANCELADO] El proceso fue interrumpido por el usuario.&lt;/b&gt;&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3024"/>
        <location filename="../mainwindow.cpp" line="3031"/>
        <source>Traducción</source>
        <translation>Traducción</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3025"/>
        <source>Selecciona idiomas de origen y destino distintos antes de traducir.</source>
        <translation>Selecciona idiomas de origen y destino distintos antes de traducir.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3008"/>
        <source>Seleccionar subtítulos para traducir</source>
        <translation>Seleccionar subtítulos para traducir</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3009"/>
        <source>Subtítulos (*.srt *.ass *.ssa *.vtt);;Todos los archivos (*.*)</source>
        <translation>Subtítulos (*.srt *.ass *.ssa *.vtt);;Todos los archivos (*.*)</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3032"/>
        <source>Selecciona primero una carpeta de destino.</source>
        <translation>Selecciona primero una carpeta de destino.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3037"/>
        <source>&lt;br&gt;&lt;font color=&apos;#FF8800&apos;&gt;&lt;b&gt;=== TRADUCCIÓN POR LOTES ===&lt;/b&gt;&lt;/font&gt;</source>
        <translation>&lt;br&gt;&lt;font color=&apos;#FF8800&apos;&gt;&lt;b&gt;=== TRADUCCIÓN POR LOTES ===&lt;/b&gt;&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3038"/>
        <source>Traduciendo %1 archivo(s): %2 → %3</source>
        <translation>Traduciendo %1 archivo(s): %2 → %3</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3052"/>
        <source>Traduciendo en lote...</source>
        <translation>Traduciendo en lote...</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3065"/>
        <source>Traduciendo archivo %1 / %2…</source>
        <translation>Traduciendo archivo %1 / %2…</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1292"/>
        <location filename="../mainwindow.cpp" line="1313"/>
        <source>Cancelado</source>
        <translation>Cancelado</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="307"/>
        <source>&lt;font color=&apos;#FF4444&apos;&gt;  › No se pudo iniciar ffmpeg. ¿Está instalado?.&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FF4444&apos;&gt;  › No se pudo iniciar ffmpeg. ¿Está instalado?.&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="429"/>
        <location filename="../mainwindow.cpp" line="2744"/>
        <source>PISTAS</source>
        <translation>PISTAS</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="507"/>
        <location filename="../mainwindow.cpp" line="2782"/>
        <source>Original</source>
        <translation>Original</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="625"/>
        <location filename="../mainwindow.cpp" line="2822"/>
        <source>DESTINO</source>
        <translation>DESTINO</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="632"/>
        <location filename="../mainwindow.cpp" line="2826"/>
        <source>Examinar…</source>
        <translation>Examinar…</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="709"/>
        <location filename="../mainwindow.cpp" line="734"/>
        <location filename="../mainwindow.cpp" line="2844"/>
        <source>LOG ▾</source>
        <translation>LOG ▾</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="704"/>
        <location filename="../mainwindow.cpp" line="2845"/>
        <source>Mostrar / ocultar registro de actividad</source>
        <translation>Mostrar / ocultar registro de actividad</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="364"/>
        <location filename="../mainwindow.cpp" line="2739"/>
        <source>Buscar actualizaciones…</source>
        <translation>Buscar actualizaciones…</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="582"/>
        <source>Experimental: convierte PGS/VobSub/DVB/XSUB a .srt con nOCR (sin modelos pesados).
Puede tardar varios minutos por episodio. Revisa siempre el resultado.</source>
        <translation>Experimental: convierte PGS/VobSub/DVB/XSUB a .srt con nOCR (sin modelos pesados).
Puede tardar varios minutos por episodio. Revisa siempre el resultado.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="702"/>
        <location filename="../mainwindow.cpp" line="709"/>
        <location filename="../mainwindow.cpp" line="2844"/>
        <source>LOG ▸</source>
        <translation>LOG ▸</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="994"/>
        <source>No hay modelo instalado para:&lt;br&gt;&lt;b&gt;%1&lt;/b&gt;&lt;br&gt;&lt;br&gt;No se pudo comprobar el catálogo online (sin conexión o aún cargando).&lt;br&gt;&lt;br&gt;Idioma detectado: %2 → %3</source>
        <translation>No hay modelo instalado para:&lt;br&gt;&lt;b&gt;%1&lt;/b&gt;&lt;br&gt;&lt;br&gt;No se pudo comprobar el catálogo online (sin conexión o aún cargando).&lt;br&gt;&lt;br&gt;Idioma detectado: %2 → %3</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1001"/>
        <source>Modelo de traducción no disponible</source>
        <translation>Modelo de traducción no disponible</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1003"/>
        <source>No existe modelo de traducción para:&lt;br&gt;&lt;b&gt;%1&lt;/b&gt;&lt;br&gt;&lt;br&gt;Tampoco hay ruta triangular ni descarga disponible (el ruso, por ejemplo, no tiene modelo publicado).&lt;br&gt;&lt;br&gt;Idioma detectado: %2 → %3</source>
        <translation>No existe modelo de traducción para:&lt;br&gt;&lt;b&gt;%1&lt;/b&gt;&lt;br&gt;&lt;br&gt;Tampoco hay ruta triangular ni descarga disponible (el ruso, por ejemplo, no tiene modelo publicado).&lt;br&gt;&lt;br&gt;Idioma detectado: %2 → %3</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1066"/>
        <source>&lt;font color=&apos;#FFCC00&apos;&gt;El filtro &quot;Solo subs&quot; está activo. Ponlo en &quot;Todos los idiomas&quot; para la selección global de subs.&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FFCC00&apos;&gt;El filtro &quot;Solo subs&quot; está activo. Ponlo en &quot;Todos los idiomas&quot; para la selección global de subs.&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1116"/>
        <source>No se pudo analizar el archivo: %1</source>
        <translation>No se pudo analizar el archivo: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1187"/>
        <source>sin idioma</source>
        <translation>sin idioma</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1189"/>
        <source>No traducible: se extrae como .mks</source>
        <translation>No traducible: se extrae como .mks</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1190"/>
        <source>Traducible como texto</source>
        <translation>Traducible como texto</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1191"/>
        <source>Subtítulo %1 [%2]
%3</source>
        <translation>Subtítulo %1 [%2]
%3</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1216"/>
        <source>+%1 más</source>
        <translation>+%1 más</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1221"/>
        <source>&lt;font color=&apos;#FFCC00&apos;&gt;  › %1 pista(s) de subtítulo de imagen detectadas: OCR de imagen activado (se generará un .srt por pista).&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FFCC00&apos;&gt;  › %1 pista(s) de subtítulo de imagen detectadas: OCR de imagen activado (se generará un .srt por pista).&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1222"/>
        <source>&lt;font color=&apos;#FFCC00&apos;&gt;  › %1 pista(s) de subtítulo de imagen detectadas: se extraen como .mks sin recodificar. Marca «OCR de subtítulos de imagen» para convertirlas a texto.&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FFCC00&apos;&gt;  › %1 pista(s) de subtítulo de imagen detectadas: se extraen como .mks sin recodificar. Marca «OCR de subtítulos de imagen» para convertirlas a texto.&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1279"/>
        <source>%1 pista(s) desmarcadas a mano, excluidas aunque coincidan con el filtro.</source>
        <translation>%1 pista(s) desmarcadas a mano, excluidas aunque coincidan con el filtro.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1301"/>
        <source>&lt;br&gt;&lt;font color=&apos;#FF4444&apos;&gt;&lt;b&gt;[CANCELADO] El remux fue interrumpido por el usuario.&lt;/b&gt;&lt;/font&gt;</source>
        <translation>&lt;br&gt;&lt;font color=&apos;#FF4444&apos;&gt;&lt;b&gt;[CANCELADO] El remux fue interrumpido por el usuario.&lt;/b&gt;&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1337"/>
        <source>&lt;font color=&apos;#FFCC00&apos;&gt;  › OCR: extrayendo fotogramas de la pista %1…&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FFCC00&apos;&gt;  › OCR: extrayendo fotogramas de la pista %1…&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1344"/>
        <source>la base nOCR no está incluida en los recursos</source>
        <translation>la base nOCR no está incluida en los recursos</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1347"/>
        <source>no se pudo cargar la base nOCR: %1</source>
        <translation>no se pudo cargar la base nOCR: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1359"/>
        <source>ffprobe no respondió al consultar la pista de subtítulo</source>
        <translation>ffprobe no respondió al consultar la pista de subtítulo</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1361"/>
        <source>ffprobe no pudo leer la pista de subtítulo %1</source>
        <translation>ffprobe no pudo leer la pista de subtítulo %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1367"/>
        <source>ffprobe no devolvió información de la pista de subtítulo</source>
        <translation>ffprobe no devolvió información de la pista de subtítulo</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1423"/>
        <location filename="../mainwindow.cpp" line="1467"/>
        <source>no se pudo crear el directorio temporal para OCR</source>
        <translation>no se pudo crear el directorio temporal para OCR</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1426"/>
        <location filename="../mainwindow.cpp" line="1470"/>
        <source>ffmpeg no pudo extraer los frames del subtítulo de imagen</source>
        <translation>ffmpeg no pudo extraer los frames del subtítulo de imagen</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1476"/>
        <source>no se pudieron obtener los tiempos de los frames del subtítulo</source>
        <translation>no se pudieron obtener los tiempos de los frames del subtítulo</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1482"/>
        <source>&lt;font color=&apos;#FFCC00&apos;&gt;  › OCR: %1 fotogramas extraídos; reconociendo texto…&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FFCC00&apos;&gt;  › OCR: %1 fotogramas extraídos; reconociendo texto…&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1491"/>
        <source>  › OCR: %1/%2 fotogramas…</source>
        <translation>  › OCR: %1/%2 fotogramas…</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1510"/>
        <source>el OCR no reconoció texto en ningún frame</source>
        <translation>el OCR no reconoció texto en ningún frame</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1527"/>
        <source>no se pudo escribir el archivo OCR: %1</source>
        <translation>no se pudo escribir el archivo OCR: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1549"/>
        <location filename="../mainwindow.cpp" line="2475"/>
        <source>&lt;br&gt;&lt;font color=&apos;#39FF14&apos;&gt;&lt;b&gt;=== ✓ PROCESO FINALIZADO ===&lt;/b&gt;&lt;/font&gt;</source>
        <translation>&lt;br&gt;&lt;font color=&apos;#39FF14&apos;&gt;&lt;b&gt;=== ✓ PROCESO FINALIZADO ===&lt;/b&gt;&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1550"/>
        <source>✓ Finalizado — %1 archivo(s) procesado(s)</source>
        <translation>✓ Finalizado — %1 archivo(s) procesado(s)</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1587"/>
        <source>%1 archivo(s) omitidos: no están en %2.</source>
        <translation>%1 archivo(s) omitidos: no están en %2.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1591"/>
        <source>%1 archivo(s) sin idioma detectado: se traducen como %2; verifica el resultado.</source>
        <translation>%1 archivo(s) sin idioma detectado: se traducen como %2; verifica el resultado.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1601"/>
        <source>Traduciendo %1 archivo(s) de subtítulos...</source>
        <translation>Traduciendo %1 archivo(s) de subtítulos...</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1606"/>
        <location filename="../mainwindow.cpp" line="2272"/>
        <location filename="../mainwindow.cpp" line="3029"/>
        <source>Traducidos</source>
        <translation>Traducidos</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1620"/>
        <location filename="../mainwindow.cpp" line="3131"/>
        <source>Remux omitido: no hay subtítulos traducidos para incrustar.</source>
        <translation>Remux omitido: no hay subtítulos traducidos para incrustar.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1623"/>
        <source>El proceso por lotes ha finalizado correctamente.</source>
        <translation>El proceso por lotes ha finalizado correctamente.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1634"/>
        <source>Procesando %1/%2: %3</source>
        <translation>Procesando %1/%2: %3</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1638"/>
        <location filename="../mainwindow.cpp" line="2581"/>
        <source>&lt;br&gt;&lt;font color=&apos;#00BFFF&apos;&gt;&lt;b&gt;[%1/%2] %3&lt;/b&gt;&lt;/font&gt;</source>
        <translation>&lt;br&gt;&lt;font color=&apos;#00BFFF&apos;&gt;&lt;b&gt;[%1/%2] %3&lt;/b&gt;&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1644"/>
        <source>Extraídos</source>
        <translation>Extraídos</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1644"/>
        <source>Convertidos</source>
        <translation>Convertidos</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1646"/>
        <source>&lt;font color=&apos;#FF4444&apos;&gt;  › No se ha especificado carpeta de destino, omitido.&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FF4444&apos;&gt;  › No se ha especificado carpeta de destino, omitido.&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1675"/>
        <source>&lt;font color=&apos;#FFCC00&apos;&gt;  › Pista %1: %2%3 → OCR de imagen (fallback a .mks si falla).&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FFCC00&apos;&gt;  › Pista %1: %2%3 → OCR de imagen (fallback a .mks si falla).&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1677"/>
        <source>&lt;font color=&apos;#FFCC00&apos;&gt;  › Pista %1: %2%3 → .mks sin recodificar; no traducible (sin OCR).&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FFCC00&apos;&gt;  › Pista %1: %2%3 → .mks sin recodificar; no traducible (sin OCR).&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1730"/>
        <location filename="../mainwindow.cpp" line="1837"/>
        <source>&lt;font color=&apos;#39FF14&apos;&gt;  › Pista %1: OCR → %2 (%3 fotogramas, %4 caracteres, %5 glifos desconocidos).&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#39FF14&apos;&gt;  › Pista %1: OCR → %2 (%3 fotogramas, %4 caracteres, %5 glifos desconocidos).&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1735"/>
        <location filename="../mainwindow.cpp" line="1842"/>
        <source>&lt;font color=&apos;#FFCC00&apos;&gt;  › OCR falló en la pista %1: %2. Se conserva la copia .mks original.&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FFCC00&apos;&gt;  › OCR falló en la pista %1: %2. Se conserva la copia .mks original.&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1802"/>
        <source>&lt;font color=&apos;#FFCC00&apos;&gt;  › Salida .mks conservada (OCR no disponible para todas las pistas): %1&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FFCC00&apos;&gt;  › Salida .mks conservada (OCR no disponible para todas las pistas): %1&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1803"/>
        <source>&lt;font color=&apos;#FFCC00&apos;&gt;  › Salida .mks no traducible (sin OCR): %1&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FFCC00&apos;&gt;  › Salida .mks no traducible (sin OCR): %1&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1805"/>
        <source>&lt;font color=&apos;#FFCC00&apos;&gt;  › Subtítulo omitido para traducción (varias pistas de texto): %1&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FFCC00&apos;&gt;  › Subtítulo omitido para traducción (varias pistas de texto): %1&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1883"/>
        <source>&lt;font color=&apos;#FFCC00&apos;&gt;  › Sin pistas seleccionadas para este archivo, omitido.&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FFCC00&apos;&gt;  › Sin pistas seleccionadas para este archivo, omitido.&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1890"/>
        <source>&lt;font color=&apos;#39FF14&apos;&gt;  › Solo se generaron salidas OCR; no hace falta copiar ninguna pista con ffmpeg.&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#39FF14&apos;&gt;  › Solo se generaron salidas OCR; no hace falta copiar ninguna pista con ffmpeg.&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1936"/>
        <location filename="../mainwindow.cpp" line="1965"/>
        <source>&lt;font color=&apos;#39FF14&apos;&gt;  › OK&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#39FF14&apos;&gt;  › OK&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1952"/>
        <source>error de ffmpeg</source>
        <translation>error de ffmpeg</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1955"/>
        <source>&lt;font color=&apos;#FF4444&apos;&gt;  › Error al incrustar, omitido.&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FF4444&apos;&gt;  › Error al incrustar, omitido.&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1968"/>
        <source>&lt;font color=&apos;#FF4444&apos;&gt;  › Error en este archivo.&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FF4444&apos;&gt;  › Error en este archivo.&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2000"/>
        <source>&lt;font color=&apos;#FFCC00&apos;&gt;     El OCR de imagen no pudo completarse; se conserva la copia original a .mks.&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FFCC00&apos;&gt;     El OCR de imagen no pudo completarse; se conserva la copia original a .mks.&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2001"/>
        <source>&lt;font color=&apos;#FFCC00&apos;&gt;     Los subtítulos de imagen (PGS/VobSub/DVB/XSUB) solo admiten copia de flujo a .mks; activa «OCR de subtítulos de imagen» para convertirlos a texto.&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FFCC00&apos;&gt;     Los subtítulos de imagen (PGS/VobSub/DVB/XSUB) solo admiten copia de flujo a .mks; activa «OCR de subtítulos de imagen» para convertirlos a texto.&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2003"/>
        <source>&lt;font color=&apos;#FFCC00&apos;&gt;     Prueba a activar Copiar pista original sin re-codificar.&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FFCC00&apos;&gt;     Prueba a activar Copiar pista original sin re-codificar.&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2036"/>
        <source>Intermedios eliminados: %1</source>
        <translation>Intermedios eliminados: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2049"/>
        <source>Extracción vacía o fallida, excluida de la traducción: %1</source>
        <translation>Extracción vacía o fallida, excluida de la traducción: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2105"/>
        <source>Buscando actualizaciones…</source>
        <translation>Buscando actualizaciones…</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2123"/>
        <source>Nueva versión disponible</source>
        <translation>Nueva versión disponible</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2125"/>
        <source>Hay una versión nueva de Elements Extract: &lt;b&gt;%1&lt;/b&gt;</source>
        <translation>Hay una versión nueva de Elements Extract: &lt;b&gt;%1&lt;/b&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2128"/>
        <source>Abre la página de la versión para ver los cambios.</source>
        <translation>Abre la página de la versión para ver los cambios.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2131"/>
        <source>Descargar…</source>
        <translation>Descargar…</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2133"/>
        <source>Más tarde</source>
        <translation>Más tarde</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2135"/>
        <source>No mostrar esta versión</source>
        <translation>No mostrar esta versión</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2148"/>
        <source>Ya tienes la versión más reciente (%1).</source>
        <translation>Ya tienes la versión más reciente (%1).</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2151"/>
        <location filename="../mainwindow.cpp" line="2159"/>
        <source>Actualizaciones</source>
        <translation>Actualizaciones</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2160"/>
        <source>No se pudo comprobar si hay actualizaciones: %1</source>
        <translation>No se pudo comprobar si hay actualizaciones: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2191"/>
        <source>Selecciona un vídeo en la lista para buscar subtítulos en internet.</source>
        <translation>Selecciona un vídeo en la lista para buscar subtítulos en internet.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2207"/>
        <source>Subtítulo online descargado: %1</source>
        <translation>Subtítulo online descargado: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2211"/>
        <source>Descargados</source>
        <translation>Descargados</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2223"/>
        <source>No se pudo guardar el subtítulo descargado en: %1</source>
        <translation>No se pudo guardar el subtítulo descargado en: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2228"/>
        <source>Subtítulo guardado en: %1</source>
        <translation>Subtítulo guardado en: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2261"/>
        <source>El idioma del subtítulo (%1) no es compatible con la traducción.</source>
        <translation>El idioma del subtítulo (%1) no es compatible con la traducción.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2293"/>
        <source>Destino</source>
        <translation>Destino</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2429"/>
        <source>El filtro coincide con %1 pistas en %2: desmarca las que no necesites.</source>
        <translation>El filtro coincide con %1 pistas en %2: desmarca las que no necesites.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2433"/>
        <source>Filtro: solo subtítulos en </source>
        <translation>Filtro: solo subtítulos en </translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2451"/>
        <source>&lt;br&gt;&lt;font color=&apos;#FF8800&apos;&gt;&lt;b&gt;=== INCRUSTANDO SUBTÍTULOS (REMUX) ===&lt;/b&gt;&lt;/font&gt;</source>
        <translation>&lt;br&gt;&lt;font color=&apos;#FF8800&apos;&gt;&lt;b&gt;=== INCRUSTANDO SUBTÍTULOS (REMUX) ===&lt;/b&gt;&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2477"/>
        <source>&lt;font color=&apos;#FFCC00&apos;&gt;&lt;b&gt;Incrustados: %1 · Omitidos/error: %2&lt;/b&gt;&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FFCC00&apos;&gt;&lt;b&gt;Incrustados: %1 · Omitidos/error: %2&lt;/b&gt;&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2480"/>
        <source>&lt;font color=&apos;#FF8844&apos;&gt;  › %1&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FF8844&apos;&gt;  › %1&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2482"/>
        <source>✓ Finalizado — %1 incrustados, %2 omitidos</source>
        <translation>✓ Finalizado — %1 incrustados, %2 omitidos</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2489"/>
        <source>Subtítulos incrustados: %1 correctos, %2 con problemas.</source>
        <translation>Subtítulos incrustados: %1 correctos, %2 con problemas.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2490"/>
        <source>Subtítulos incrustados correctamente.</source>
        <translation>Subtítulos incrustados correctamente.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2505"/>
        <source>&lt;font color=&apos;#FFCC00&apos;&gt;  › No se encontró la traducción, se omite esa pista: %1&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FFCC00&apos;&gt;  › No se encontró la traducción, se omite esa pista: %1&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2511"/>
        <source>no se encontró la traducción</source>
        <translation>no se encontró la traducción</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2513"/>
        <source>no hay carpeta de destino configurada</source>
        <translation>no hay carpeta de destino configurada</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2520"/>
        <source>sin pista de vídeo: no se puede incrustar</source>
        <translation>sin pista de vídeo: no se puede incrustar</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2526"/>
        <source>&lt;font color=&apos;#FF8844&apos;&gt;  › Omitido: %1 (%2)&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FF8844&apos;&gt;  › Omitido: %1 (%2)&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2533"/>
        <source>Remuxados</source>
        <translation>Remuxados</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2545"/>
        <source>audio %1</source>
        <translation>audio %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2547"/>
        <source>vídeo %1</source>
        <translation>vídeo %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2552"/>
        <source>&lt;font color=&apos;#FFCC00&apos;&gt;  › %1 no es copiable a MP4 (%2) → se usará MKV&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FFCC00&apos;&gt;  › %1 no es copiable a MP4 (%2) → se usará MKV&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2577"/>
        <source>Incrustando %1/%2: %3</source>
        <translation>Incrustando %1/%2: %3</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2585"/>
        <source>  › Incrustando %1 pista(s) → %2</source>
        <translation>  › Incrustando %1 pista(s) → %2</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2684"/>
        <source>🎵 %1 audio</source>
        <translation>🎵 %1 audio</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2686"/>
        <source>CC %1 subs</source>
        <translation>CC %1 subs</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3148"/>
        <source>&lt;b&gt;Modelos de traducción&lt;/b&gt;&lt;br&gt;&lt;br&gt;Si te falta un modelo para un par de idiomas, descárgalo desde Ajustes → Configuración y Modelos… → pestaña Modelos de Traducción → ONLINE (doble clic). La app te avisará en el registro si el modelo que necesitas existe para descargar.&lt;br&gt;&lt;br&gt;&lt;b&gt;Modelos incluidos:&lt;/b&gt;&lt;br&gt;• English → Español (en→es)&lt;br&gt;• Español → English (es→en)&lt;br&gt;• English → Français (en→fr)&lt;br&gt;• Français → English (fr→en)&lt;br&gt;• English → Português (en→pt)&lt;br&gt;• Português → English (pt→en)&lt;br&gt;&lt;br&gt;&lt;b&gt;Directorio de modelos:&lt;/b&gt;&lt;br&gt;</source>
        <translation>&lt;b&gt;Modelos de traducción&lt;/b&gt;&lt;br&gt;&lt;br&gt;Si te falta un modelo para un par de idiomas, descárgalo desde Ajustes → Configuración y Modelos… → pestaña Modelos de Traducción → ONLINE (doble clic). La app te avisará en el registro si el modelo que necesitas existe para descargar.&lt;br&gt;&lt;br&gt;&lt;b&gt;Modelos incluidos:&lt;/b&gt;&lt;br&gt;• English → Español (en→es)&lt;br&gt;• Español → English (es→en)&lt;br&gt;• English → Français (en→fr)&lt;br&gt;• Français → English (fr→en)&lt;br&gt;• English → Português (en→pt)&lt;br&gt;• Português → English (pt→en)&lt;br&gt;&lt;br&gt;&lt;b&gt;Directorio de modelos:&lt;/b&gt;&lt;br&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2234"/>
        <location filename="../mainwindow.cpp" line="2949"/>
        <source>No se encontró el archivo: %1</source>
        <translation>No se encontró el archivo: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2266"/>
        <location filename="../mainwindow.cpp" line="2958"/>
        <source>&lt;font color=&apos;#FFCC00&apos;&gt;Idioma origen y destino son iguales.&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#FFCC00&apos;&gt;Idioma origen y destino son iguales.&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2279"/>
        <location filename="../mainwindow.cpp" line="2966"/>
        <source>Traduciendo...</source>
        <translation>Traduciendo...</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2281"/>
        <location filename="../mainwindow.cpp" line="2968"/>
        <source>Traduciendo: %1 → %2</source>
        <translation>Traduciendo: %1 → %2</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2983"/>
        <source>Sin traducción %1 → %2: no hay modelo instalado (ni directo ni triangular).</source>
        <translation>Sin traducción %1 → %2: no hay modelo instalado (ni directo ni triangular).</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2990"/>
        <source>Pero sí existe para descargar: %1. Ábrelo en Ajustes → Configuración y Modelos… → pestaña Modelos de Traducción → ONLINE (doble clic para descargar).</source>
        <translation>Pero sí existe para descargar: %1. Ábrelo en Ajustes → Configuración y Modelos… → pestaña Modelos de Traducción → ONLINE (doble clic para descargar).</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2993"/>
        <source>  › No se pudo comprobar el catálogo online (sin conexión o aún cargando). Con internet, abre Ajustes → Configuración y Modelos… → ONLINE para ver si existe.</source>
        <translation>  › No se pudo comprobar el catálogo online (sin conexión o aún cargando). Con internet, abre Ajustes → Configuración y Modelos… → ONLINE para ver si existe.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2996"/>
        <source>  › Tampoco existe en el catálogo online (el ruso, por ejemplo, no tiene modelo publicado). Solo se pueden traducir subtítulos en English, Español, Français o Português: cambia el filtro &quot;Solo subs&quot; o el idioma origen.</source>
        <translation>  › Tampoco existe en el catálogo online (el ruso, por ejemplo, no tiene modelo publicado). Solo se pueden traducir subtítulos en English, Español, Français o Português: cambia el filtro &quot;Solo subs&quot; o el idioma origen.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3046"/>
        <source>Sin modelo de traducción</source>
        <translation>Sin modelo de traducción</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3101"/>
        <source>Traducción completada: </source>
        <translation>Traducción completada: </translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3103"/>
        <source>Traducción guardada: %1</source>
        <translation>Traducción guardada: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3110"/>
        <source>Error en la traducción</source>
        <translation>Error en la traducción</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3112"/>
        <source>Error de traducción: %1</source>
        <translation>Error de traducción: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3116"/>
        <source>Error de traducción</source>
        <translation>Error de traducción</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3123"/>
        <source>Traducción completada</source>
        <translation>Traducción completada</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3137"/>
        <source>La traducción ha finalizado correctamente.</source>
        <translation>La traducción ha finalizado correctamente.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3145"/>
        <source>Gestionar Modelos de Traducción</source>
        <translation>Gestionar Modelos de Traducción</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3173"/>
        <source>Descargando modelo %1: %2%</source>
        <translation>Descargando modelo %1: %2%</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3180"/>
        <source>Modelo descargado: </source>
        <translation>Modelo descargado: </translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3181"/>
        <source>Modelo descargado: %1</source>
        <translation>Modelo descargado: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3186"/>
        <source>Error descargando modelo: </source>
        <translation>Error descargando modelo: </translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3187"/>
        <source>Error descargando %1: %2</source>
        <translation>Error descargando %1: %2</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3232"/>
        <source>Idioma de interfaz cambiado a: %1</source>
        <translation>Idioma de interfaz cambiado a: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3235"/>
        <source>No se pudo cargar la traducción para: %1</source>
        <translation>No se pudo cargar la traducción para: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="3255"/>
        <source>&lt;font color=&apos;#888888&apos;&gt;Configuración actualizada&lt;/font&gt;</source>
        <translation>&lt;font color=&apos;#888888&apos;&gt;Configuración actualizada&lt;/font&gt;</translation>
    </message>
</context>
<context>
    <name>OnlineSubsDialog</name>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="43"/>
        <location filename="../onlinesubsdialog.cpp" line="575"/>
        <source>Pendiente</source>
        <translation>Pendiente</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="48"/>
        <source>Subtítulos online — %1</source>
        <translation>Subtítulos online — %1</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="50"/>
        <source>Subtítulos online — %1 archivos</source>
        <translation>Subtítulos online — %1 archivos</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="63"/>
        <source>%1 archivos seleccionados</source>
        <translation>%1 archivos seleccionados</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="66"/>
        <source>Idioma:</source>
        <translation>Idioma:</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="71"/>
        <source>Todos</source>
        <translation>Todos</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="87"/>
        <source>%1 (preferido)</source>
        <translation>%1 (preferido)</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="104"/>
        <location filename="../onlinesubsdialog.cpp" line="167"/>
        <source>Archivo</source>
        <translation>Archivo</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="104"/>
        <source>Consulta</source>
        <translation>Consulta</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="104"/>
        <location filename="../onlinesubsdialog.cpp" line="168"/>
        <source>Estado</source>
        <translation>Estado</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="104"/>
        <source>Resultados</source>
        <translation>Resultados</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="154"/>
        <source>Buscar en seleccionados</source>
        <translation>Buscar en seleccionados</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="155"/>
        <source>Buscar en todos</source>
        <translation>Buscar en todos</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="167"/>
        <source>Idioma</source>
        <translation>Idioma</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="167"/>
        <source>Título</source>
        <translation>Título</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="167"/>
        <source>Año</source>
        <translation>Año</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="168"/>
        <source>Descargas</source>
        <translation>Descargas</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="168"/>
        <source>Hash</source>
        <translation>Hash</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="204"/>
        <source>Descargar y traducir seleccionados</source>
        <translation>Descargar y traducir seleccionados</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="205"/>
        <source>Solo descargar seleccionados</source>
        <translation>Solo descargar seleccionados</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="206"/>
        <source>Cerrar</source>
        <translation>Cerrar</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="230"/>
        <source>idioma desconocido</source>
        <translation>idioma desconocido</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="233"/>
        <source>Elegir subtítulo</source>
        <translation>Elegir subtítulo</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="234"/>
        <source>El archivo contiene varios subtítulos (%1). Elige cuál usar:</source>
        <translation>El archivo contiene varios subtítulos (%1). Elige cuál usar:</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="241"/>
        <source>Configura la clave API, el usuario y la contraseña en Ajustes → Subtítulos online.</source>
        <translation>Configura la clave API, el usuario y la contraseña en Ajustes → Subtítulos online.</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="299"/>
        <source>Inglés</source>
        <translation>Inglés</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="300"/>
        <source>Español</source>
        <translation>Español</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="301"/>
        <source>Francés</source>
        <translation>Francés</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="302"/>
        <source>Portugués</source>
        <translation>Portugués</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="303"/>
        <source>Alemán</source>
        <translation>Alemán</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="304"/>
        <source>Italiano</source>
        <translation>Italiano</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="400"/>
        <source>Introduce un título para buscar subtítulos.</source>
        <translation>Introduce un título para buscar subtítulos.</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="404"/>
        <location filename="../onlinesubsdialog.cpp" line="414"/>
        <source>Buscando subtítulos…</source>
        <translation>Buscando subtítulos…</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="409"/>
        <source>No hay archivos marcados.</source>
        <translation>No hay archivos marcados.</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="440"/>
        <location filename="../onlinesubsdialog.cpp" line="690"/>
        <location filename="../onlinesubsdialog.cpp" line="691"/>
        <source>Error: %1</source>
        <translation>Error: %1</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="442"/>
        <source>No se encontraron subtítulos.</source>
        <translation>No se encontraron subtítulos.</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="457"/>
        <source>Búsqueda finalizada: %1 resultados.</source>
        <translation>Búsqueda finalizada: %1 resultados.</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="498"/>
        <source>No hay subtítulos en %1 para uno o más archivos. Puedes descargar el subtítulo disponible (p. ej. %2) y traducirlo a %1 mediante traducción triangular.</source>
        <translation>No hay subtítulos en %1 para uno o más archivos. Puedes descargar el subtítulo disponible (p. ej. %2) y traducirlo a %1 mediante traducción triangular.</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="501"/>
        <source>Afecta a %1 archivo(s).</source>
        <translation>Afecta a %1 archivo(s).</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="598"/>
        <source>%1 resultados</source>
        <translation>%1 resultados</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="607"/>
        <source>No hay subtítulos en %1 para uno o más archivos. Se descargará el mejor subtítulo disponible (p. ej. %2) y se traducirá a %1 mediante traducción triangular. ¿Continuar?</source>
        <translation>No hay subtítulos en %1 para uno o más archivos. Se descargará el mejor subtítulo disponible (p. ej. %2) y se traducirá a %1 mediante traducción triangular. ¿Continuar?</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="612"/>
        <source>Traducción triangular</source>
        <translation>Traducción triangular</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="636"/>
        <source>Marca al menos un resultado.</source>
        <translation>Marca al menos un resultado.</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="644"/>
        <location filename="../onlinesubsdialog.cpp" line="673"/>
        <source>Descargando el subtítulo…</source>
        <translation>Descargando el subtítulo…</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="694"/>
        <source>Descarga completada: %1</source>
        <translation>Descarga completada: %1</translation>
    </message>
    <message>
        <location filename="../onlinesubsdialog.cpp" line="706"/>
        <source>%1 descargados, %2 fallidos</source>
        <translation>%1 descargados, %2 fallidos</translation>
    </message>
</context>
<context>
    <name>SettingsDialog</name>
    <message>
        <location filename="../settingsdialog.cpp" line="36"/>
        <location filename="../settingsdialog.cpp" line="381"/>
        <source>Ajustes — Elements Extract</source>
        <translation>Ajustes — Elements Extract</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="54"/>
        <location filename="../settingsdialog.cpp" line="387"/>
        <source>Directorio de Modelos</source>
        <translation>Directorio de Modelos</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="59"/>
        <location filename="../settingsdialog.cpp" line="388"/>
        <source>Examinar...</source>
        <translation>Examinar...</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="67"/>
        <location filename="../settingsdialog.cpp" line="389"/>
        <source>Idiomas por Defecto</source>
        <translation>Idiomas por Defecto</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="69"/>
        <location filename="../settingsdialog.cpp" line="390"/>
        <source>Origen:</source>
        <translation>Origen:</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="76"/>
        <location filename="../settingsdialog.cpp" line="391"/>
        <source>Destino:</source>
        <translation>Destino:</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="88"/>
        <location filename="../settingsdialog.cpp" line="392"/>
        <source>Opciones</source>
        <translation>Opciones</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="90"/>
        <location filename="../settingsdialog.cpp" line="394"/>
        <source>Traducir subtítulos automáticamente al extraer</source>
        <translation>Traducir subtítulos automáticamente al extraer</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="97"/>
        <location filename="../settingsdialog.cpp" line="396"/>
        <source>Eliminar los .srt intermedios tras incrustarlos (remux)</source>
        <translation>Eliminar los .srt intermedios tras incrustarlos (remux)</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="98"/>
        <location filename="../settingsdialog.cpp" line="397"/>
        <source>Borra los subtítulos extraídos y traducidos de cada vídeo cuando su remux termina correctamente. Sin remux no se borra nada.</source>
        <translation>Borra los subtítulos extraídos y traducidos de cada vídeo cuando su remux termina correctamente. Sin remux no se borra nada.</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="108"/>
        <location filename="../settingsdialog.cpp" line="399"/>
        <source>Apariencia</source>
        <translation>Apariencia</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="111"/>
        <location filename="../settingsdialog.cpp" line="400"/>
        <source>Tema:</source>
        <translation>Tema:</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="114"/>
        <location filename="../settingsdialog.cpp" line="126"/>
        <source>Automático (detectar del sistema)</source>
        <translation>Automático (detectar del sistema)</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="115"/>
        <source>Oscuro</source>
        <translation>Oscuro</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="116"/>
        <source>Claro</source>
        <translation>Claro</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="123"/>
        <location filename="../settingsdialog.cpp" line="401"/>
        <source>Idioma de interfaz:</source>
        <translation>Idioma de interfaz:</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="140"/>
        <location filename="../settingsdialog.cpp" line="383"/>
        <source>General</source>
        <translation>General</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="150"/>
        <source>Modelos instalados</source>
        <translation>Modelos instalados</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="152"/>
        <location filename="../settingsdialog.cpp" line="403"/>
        <source>Buscar online</source>
        <translation>Buscar online</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="153"/>
        <location filename="../settingsdialog.cpp" line="404"/>
        <source>Buscar modelos disponibles en translatelocally.com</source>
        <translation>Buscar modelos disponibles en translatelocally.com</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="173"/>
        <location filename="../settingsdialog.cpp" line="406"/>
        <source>INSTALADOS — doble clic para activar/desactivar</source>
        <translation>INSTALADOS — doble clic para activar/desactivar</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="185"/>
        <location filename="../settingsdialog.cpp" line="189"/>
        <location filename="../settingsdialog.cpp" line="408"/>
        <source>Eliminar</source>
        <translation>Eliminar</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="189"/>
        <source>¿Eliminar </source>
        <translation>¿Eliminar </translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="201"/>
        <location filename="../settingsdialog.cpp" line="407"/>
        <source>ONLINE — doble clic para descargar</source>
        <translation>ONLINE — doble clic para descargar</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="219"/>
        <location filename="../settingsdialog.cpp" line="411"/>
        <source>&lt;b&gt;tiny&lt;/b&gt; ~15-25MB rápido · &lt;b&gt;base&lt;/b&gt; ~50MB mejor calidad&lt;br&gt;Doble clic = activar/desactivar (solo activos se usan para traducir)</source>
        <translation>&lt;b&gt;tiny&lt;/b&gt; ~15-25MB rápido · &lt;b&gt;base&lt;/b&gt; ~50MB mejor calidad&lt;br&gt;Doble clic = activar/desactivar (solo activos se usan para traducir)</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="225"/>
        <location filename="../settingsdialog.cpp" line="384"/>
        <source>Modelos de Traducción</source>
        <translation>Modelos de Traducción</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="233"/>
        <location filename="../settingsdialog.cpp" line="413"/>
        <source>Credenciales de OpenSubtitles</source>
        <translation>Credenciales de OpenSubtitles</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="235"/>
        <location filename="../settingsdialog.cpp" line="414"/>
        <source>Backend:</source>
        <translation>Backend:</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="237"/>
        <location filename="../settingsdialog.cpp" line="419"/>
        <source>XML-RPC (enlaces directos, tipo VLC)</source>
        <translation>XML-RPC (enlaces directos, tipo VLC)</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="238"/>
        <location filename="../settingsdialog.cpp" line="420"/>
        <source>API REST (oficial)</source>
        <translation>API REST (oficial)</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="240"/>
        <location filename="../settingsdialog.cpp" line="425"/>
        <source>Clave API:</source>
        <translation>Clave API:</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="241"/>
        <location filename="../settingsdialog.cpp" line="426"/>
        <source>Usuario:</source>
        <translation>Usuario:</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="242"/>
        <location filename="../settingsdialog.cpp" line="427"/>
        <source>Contraseña:</source>
        <translation>Contraseña:</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="261"/>
        <location filename="../settingsdialog.cpp" line="385"/>
        <source>Subtítulos online</source>
        <translation>Subtítulos online</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="268"/>
        <location filename="../settingsdialog.cpp" line="429"/>
        <source>Cerrar</source>
        <translation>Cerrar</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="440"/>
        <source>La búsqueda de subtítulos en &lt;a href=&quot;https://www.opensubtitles.com&quot;&gt;OpenSubtitles.com&lt;/a&gt; es opcional: necesitas una cuenta y tu propia clave API.</source>
        <translation>La búsqueda de subtítulos en &lt;a href=&quot;https://www.opensubtitles.com&quot;&gt;OpenSubtitles.com&lt;/a&gt; es opcional: necesitas una cuenta y tu propia clave API.</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="444"/>
        <source>XML-RPC permite buscar sin credenciales, pero sin una cuenta con permisos de descarga los enlaces devuelven un subtítulo de prueba VIP.</source>
        <translation>XML-RPC permite buscar sin credenciales, pero sin una cuenta con permisos de descarga los enlaces devuelven un subtítulo de prueba VIP.</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="447"/>
        <source>No se pudieron guardar las credenciales en el almacén seguro: %1</source>
        <translation>No se pudieron guardar las credenciales en el almacén seguro: %1</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="450"/>
        <source>Las credenciales se guardan en el almacén seguro del sistema.</source>
        <translation>Las credenciales se guardan en el almacén seguro del sistema.</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="451"/>
        <source>El almacén seguro del sistema no está disponible. Las credenciales no se guardarán: tendrás que introducirlas cada vez.</source>
        <translation>El almacén seguro del sistema no está disponible. Las credenciales no se guardarán: tendrás que introducirlas cada vez.</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="491"/>
        <source>Directorio de modelos</source>
        <translation>Directorio de modelos</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="505"/>
        <source>Buscando...</source>
        <translation>Buscando...</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="532"/>
        <source>Volver a descargar</source>
        <translation>Volver a descargar</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="533"/>
        <source>El modelo &apos;%1&apos; ya está instalado.
¿Descargarlo de nuevo? (reemplaza la copia local)</source>
        <translation>El modelo &apos;%1&apos; ya está instalado.
¿Descargarlo de nuevo? (reemplaza la copia local)</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="598"/>
        <source>Descargando %1...</source>
        <translation>Descargando %1...</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="626"/>
        <source>Error: </source>
        <translation>Error: </translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="745"/>
        <source>↻ actualizar</source>
        <translation>↻ actualizar</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="360"/>
        <source>%1 modelos online</source>
        <translation>%1 modelos online</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="513"/>
        <source>No disponible</source>
        <translation>No disponible</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="513"/>
        <source>Gestor de modelos no disponible.</source>
        <translation>Gestor de modelos no disponible.</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="525"/>
        <location filename="../settingsdialog.cpp" line="627"/>
        <source>Error</source>
        <translation>Error</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="525"/>
        <source>Modelo no encontrado en la lista online.</source>
        <translation>Modelo no encontrado en la lista online.</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="540"/>
        <source>Descargando: </source>
        <translation>Descargando: </translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="594"/>
        <source>Descargando %1: %2%</source>
        <translation>Descargando %1: %2%</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="619"/>
        <source>Descargado y activado: </source>
        <translation>Descargado y activado: </translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="627"/>
        <source>No se pudo descargar </source>
        <translation>No se pudo descargar </translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="694"/>
        <source>%1 instalados, %2 activos</source>
        <translation>%1 instalados, %2 activos</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="705"/>
        <source>&lt;b&gt;Modelos disponibles:&lt;/b&gt;</source>
        <translation>&lt;b&gt;Modelos disponibles:&lt;/b&gt;</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="723"/>
        <source>(no disponible)</source>
        <translation>(no disponible)</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="742"/>
        <source>(ya instalado)</source>
        <translation>(ya instalado)</translation>
    </message>
</context>
<context>
    <name>SubtitleDownloader</name>
    <message>
        <location filename="../subtitledownloader.cpp" line="78"/>
        <source>No se pudo iniciar %1.</source>
        <translation>No se pudo iniciar %1.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="84"/>
        <source>Tiempo de espera agotado al descomprimir con %1.</source>
        <translation>Tiempo de espera agotado al descomprimir con %1.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="90"/>
        <source>Error al descomprimir con %1: %2</source>
        <translation>Error al descomprimir con %1: %2</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="96"/>
        <location filename="../subtitledownloader.cpp" line="1381"/>
        <source>El archivo comprimido no contiene un subtítulo.</source>
        <translation>El archivo comprimido no contiene un subtítulo.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="147"/>
        <source>El subtítulo está comprimido en RAR; instala unrar o 7z, o descarga el subtítulo desde la web de OpenSubtitles.</source>
        <translation>El subtítulo está comprimido en RAR; instala unrar o 7z, o descarga el subtítulo desde la web de OpenSubtitles.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="379"/>
        <source>La respuesta XML-RPC de OpenSubtitles no es válida.</source>
        <translation>La respuesta XML-RPC de OpenSubtitles no es válida.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="384"/>
        <source>La respuesta XML-RPC de OpenSubtitles no contiene valores.</source>
        <translation>La respuesta XML-RPC de OpenSubtitles no contiene valores.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="390"/>
        <source>Error XML-RPC de OpenSubtitles: %1</source>
        <translation>Error XML-RPC de OpenSubtitles: %1</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="508"/>
        <source>Cambiando al backend XML-RPC…</source>
        <translation>Cambiando al backend XML-RPC…</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="509"/>
        <source>Cambiando al backend API REST…</source>
        <translation>Cambiando al backend API REST…</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="685"/>
        <location filename="../subtitledownloader.cpp" line="791"/>
        <source>Faltan credenciales de OpenSubtitles para la API REST (clave API, usuario y contraseña). Configúralas en Ajustes → Subtítulos online.</source>
        <translation>Faltan credenciales de OpenSubtitles para la API REST (clave API, usuario y contraseña). Configúralas en Ajustes → Subtítulos online.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="720"/>
        <source>Iniciando sesión en OpenSubtitles (XML-RPC)…</source>
        <translation>Iniciando sesión en OpenSubtitles (XML-RPC)…</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="741"/>
        <source>Buscando subtítulos por hash…</source>
        <translation>Buscando subtítulos por hash…</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="752"/>
        <source>Introduce un título para buscar subtítulos.</source>
        <translation>Introduce un título para buscar subtítulos.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="766"/>
        <source>Buscando subtítulos por título…</source>
        <translation>Buscando subtítulos por título…</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="797"/>
        <source>El subtítulo seleccionado no tiene identificador de archivo ni enlace de descarga.</source>
        <translation>El subtítulo seleccionado no tiene identificador de archivo ni enlace de descarga.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="810"/>
        <source>Faltan el usuario o la contraseña de OpenSubtitles. Configúralos en Ajustes → Subtítulos online.</source>
        <translation>Faltan el usuario o la contraseña de OpenSubtitles. Configúralos en Ajustes → Subtítulos online.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="822"/>
        <source>Iniciando sesión en OpenSubtitles…</source>
        <translation>Iniciando sesión en OpenSubtitles…</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="839"/>
        <source>Solicitando el subtítulo…</source>
        <translation>Solicitando el subtítulo…</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="847"/>
        <source>Descargando el subtítulo…</source>
        <translation>Descargando el subtítulo…</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="863"/>
        <source>Operación cancelada.</source>
        <translation>Operación cancelada.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="866"/>
        <source>Búsqueda cancelada.</source>
        <translation>Búsqueda cancelada.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="868"/>
        <source>Descarga cancelada.</source>
        <translation>Descarga cancelada.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="892"/>
        <location filename="../subtitledownloader.cpp" line="1126"/>
        <source>Sesión caducada; iniciando sesión de nuevo…</source>
        <translation>Sesión caducada; iniciando sesión de nuevo…</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="901"/>
        <source>Búsqueda por hash no disponible; probando por título…</source>
        <translation>Búsqueda por hash no disponible; probando por título…</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="905"/>
        <source>Búsqueda por título no disponible; se muestran los resultados por hash.</source>
        <translation>Búsqueda por título no disponible; se muestran los resultados por hash.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="959"/>
        <source>HTTP %1: %2</source>
        <translation>HTTP %1: %2</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="1007"/>
        <source>OpenSubtitles (XML-RPC) no devolvió un token de sesión.</source>
        <translation>OpenSubtitles (XML-RPC) no devolvió un token de sesión.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="1039"/>
        <location filename="../subtitledownloader.cpp" line="1134"/>
        <source>La respuesta de OpenSubtitles no es un JSON válido.</source>
        <translation>La respuesta de OpenSubtitles no es un JSON válido.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="1074"/>
        <source>Buscando también por título…</source>
        <translation>Buscando también por título…</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="1096"/>
        <source>No se encontraron subtítulos.</source>
        <translation>No se encontraron subtítulos.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="1110"/>
        <source>OpenSubtitles no devolvió un token de sesión.</source>
        <translation>OpenSubtitles no devolvió un token de sesión.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="1158"/>
        <source>La respuesta de descarga no contiene un enlace ni contenido.</source>
        <translation>La respuesta de descarga no contiene un enlace ni contenido.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="1170"/>
        <source>Subtítulo descargado: %1</source>
        <translation>Subtítulo descargado: %1</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="1210"/>
        <source>No se pudo listar el ZIP: no se encontró %1.</source>
        <translation>No se pudo listar el ZIP: no se encontró %1.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="1216"/>
        <source>No se pudo listar el ZIP: tiempo de espera agotado.</source>
        <translation>No se pudo listar el ZIP: tiempo de espera agotado.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="1221"/>
        <source>No se pudo listar el ZIP: %1</source>
        <translation>No se pudo listar el ZIP: %1</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="1325"/>
        <source>El subtítulo descargado está vacío.</source>
        <translation>El subtítulo descargado está vacío.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="1331"/>
        <source>No se pudo crear un directorio temporal para descomprimir.</source>
        <translation>No se pudo crear un directorio temporal para descomprimir.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="1342"/>
        <source>No se pudo guardar el archivo comprimido temporal.</source>
        <translation>No se pudo guardar el archivo comprimido temporal.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="1353"/>
        <source>El ZIP no contiene archivos.</source>
        <translation>El ZIP no contiene archivos.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="1359"/>
        <source>El ZIP no contiene subtítulos de texto (.srt/.ass/.ssa/.vtt).</source>
        <translation>El ZIP no contiene subtítulos de texto (.srt/.ass/.ssa/.vtt).</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="1364"/>
        <source>Extrayendo %1 del ZIP…</source>
        <translation>Extrayendo %1 del ZIP…</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="1369"/>
        <source>Extrayendo subtítulo de un archivo RAR…</source>
        <translation>Extrayendo subtítulo de un archivo RAR…</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="1375"/>
        <source>No se pudo leer el subtítulo descomprimido.</source>
        <translation>No se pudo leer el subtítulo descomprimido.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="1395"/>
        <source>OpenSubtitles devolvió un subtítulo de prueba VIP; inicia sesión con una cuenta con permisos de descarga o usa la API REST.</source>
        <translation>OpenSubtitles devolvió un subtítulo de prueba VIP; inicia sesión con una cuenta con permisos de descarga o usa la API REST.</translation>
    </message>
    <message>
        <location filename="../subtitledownloader.cpp" line="1403"/>
        <source>No se pudo escribir el subtítulo en %1.</source>
        <translation>No se pudo escribir el subtítulo en %1.</translation>
    </message>
</context>
<context>
    <name>SubtitleTranslator</name>
    <message>
        <location filename="../subtitletranslator.cpp" line="175"/>
        <location filename="../subtitletranslator.cpp" line="272"/>
        <source>No se pudieron leer los subtítulos del archivo: </source>
        <translation>No se pudieron leer los subtítulos del archivo: </translation>
    </message>
    <message>
        <location filename="../subtitletranslator.cpp" line="184"/>
        <location filename="../subtitletranslator.cpp" line="281"/>
        <source>No se encontró el motor de traducción (bergamot).
Asegúrese de que está instalado correctamente.
Buscado en: </source>
        <translation>No se encontró el motor de traducción (bergamot).
Asegúrese de que está instalado correctamente.
Buscado en: </translation>
    </message>
    <message>
        <location filename="../subtitletranslator.cpp" line="194"/>
        <source>No se encontró un modelo de traducción para %1→%2.
Ni modelo directo ni ruta triangular disponible.
Modelos instalados disponibles para: en, es, fr, pt.</source>
        <translation>No se encontró un modelo de traducción para %1→%2.
Ni modelo directo ni ruta triangular disponible.
Modelos instalados disponibles para: en, es, fr, pt.</translation>
    </message>
    <message>
        <location filename="../subtitletranslator.cpp" line="293"/>
        <source>El modelo de traducción %1→%2 no está instalado.
Directorio buscado: %3</source>
        <translation>El modelo de traducción %1→%2 no está instalado.
Directorio buscado: %3</translation>
    </message>
    <message>
        <location filename="../subtitletranslator.cpp" line="325"/>
        <location filename="../subtitletranslator.cpp" line="381"/>
        <location filename="../subtitletranslator.cpp" line="463"/>
        <source>Error al escribir el archivo de salida: </source>
        <translation>Error al escribir el archivo de salida: </translation>
    </message>
    <message>
        <location filename="../subtitletranslator.cpp" line="350"/>
        <location filename="../subtitletranslator.cpp" line="405"/>
        <location filename="../subtitletranslator.cpp" line="480"/>
        <source>No se pudo iniciar el motor de traducción</source>
        <translation>No se pudo iniciar el motor de traducción</translation>
    </message>
    <message>
        <location filename="../subtitletranslator.cpp" line="358"/>
        <source>Tiempo de espera agotado del motor de traducción</source>
        <translation>Tiempo de espera agotado del motor de traducción</translation>
    </message>
    <message>
        <location filename="../subtitletranslator.cpp" line="364"/>
        <location filename="../subtitletranslator.cpp" line="440"/>
        <source>Error del motor de traducción (código %1).</source>
        <translation>Error del motor de traducción (código %1).</translation>
    </message>
    <message>
        <location filename="../subtitletranslator.cpp" line="483"/>
        <source>El motor de traducción terminó inesperadamente</source>
        <translation>El motor de traducción terminó inesperadamente</translation>
    </message>
    <message>
        <location filename="../subtitletranslator.cpp" line="486"/>
        <source>El motor de traducción excedió el tiempo límite</source>
        <translation>El motor de traducción excedió el tiempo límite</translation>
    </message>
    <message>
        <location filename="../subtitletranslator.cpp" line="489"/>
        <source>Error desconocido del motor de traducción</source>
        <translation>Error desconocido del motor de traducción</translation>
    </message>
</context>
<context>
    <name>UpdateChecker</name>
    <message>
        <location filename="../updatechecker.cpp" line="93"/>
        <source>La respuesta de GitHub no es válida.</source>
        <translation>La respuesta de GitHub no es válida.</translation>
    </message>
    <message>
        <location filename="../updatechecker.cpp" line="101"/>
        <source>La versión publicada no se pudo interpretar.</source>
        <translation>La versión publicada no se pudo interpretar.</translation>
    </message>
</context>
</TS>
