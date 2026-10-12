<?xml version='1.0' encoding='utf-8'?>
<TS version="2.1" language="en">
<context>
    <name>AboutDialog</name>
    <message>
        <location filename="../aboutdialog.cpp" line="19" />
        <source>Acerca de Elements Extract FFmpeg</source>
        <translation>About Elements Extract FFmpeg</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="41" />
        <source>Versión 1.2.0  ·  2026</source>
        <translation>Version 1.2.0  ·  2026</translation>
    </message>
    <message>
        <source>Extractor gráfico de pistas de audio, vídeo y subtítulos.
Procesamiento por lotes con selección individual por archivo
y traducción automática de subtítulos sin conexión.</source>
        <translation type="vanished">Graphical extractor of audio, video and subtitle tracks.
Batch processing with per-file track selection
and offline automatic subtitle translation.</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="98" />
        <source>Desarrollado por</source>
        <translation>Developed by</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="99" />
        <source>Repositorio</source>
        <translation>Repository</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="101" />
        <source>Licencia</source>
        <translation>License</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="115" />
        <source>Qt %1
Interfaz</source>
        <translation>Qt %1 Interface</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="130" />
        <source>DONAR</source>
        <translation>DONATE</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="144" />
        <source>Donar con PayPal</source>
        <translation>Donate with PayPal</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="168" />
        <location filename="../aboutdialog.cpp" line="174" />
        <source>Copiar</source>
        <translation>Copy</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="172" />
        <source>¡Copiada!</source>
        <translation>Copied!</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="162" />
        <source>Donar con PIX</source>
        <translation>Donate with PIX</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="107" />
        <source>TECNOLOGÍAS</source>
        <translation>TECHNOLOGY</translation>
    </message>
    <message>
        <source>Qt 5
Interfaz</source>
        <translation type="vanished">Qt 5
Interface</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="196" />
        <source>© 2026 pdmiguel90-droid  —  GPL-3.0</source>
        <translation>© 2026 pdmiguel90-droid  —  GPL-3.0</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="116" />
        <source>FFmpeg
Extracción</source>
        <translation>FFmpeg
Extraction</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="57" />
        <source>Extractor gráfico de pistas de audio, vídeo y subtítulos.
Procesamiento por lotes, filtro de subtítulos por idioma
y traducción automática con incrustación (remux) sin conexión.</source>
        <translation>Graphical extractor for audio, video and subtitle tracks.
Batch processing, subtitle filtering by language
and automatic translation with embedding (remux), offline.</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="117" />
        <source>bergamot
Traducción</source>
        <translation>bergamot
Translation</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="118" />
        <source>C++17
Lenguaje</source>
        <translation>C++17
Language</translation>
    </message>
    <message>
        <location filename="../aboutdialog.cpp" line="192" />
        <source>Cerrar</source>
        <translation>Close</translation>
    </message>
</context>
<context>
    <name>HelpDialog</name>
    <message>
        <location filename="../helpdialog.cpp" line="13" />
        <source>Ayuda — Elements Extract</source>
        <translation>Help — Elements Extract</translation>
    </message>
    <message>
        <location filename="../helpdialog.cpp" line="61" />
        <source>Extracción</source>
        <translation>Extraction</translation>
    </message>
    <message>
        <source>&lt;h2&gt;Traducción de Subtítulos&lt;/h2&gt;&lt;h3&gt;1. Elegir idiomas&lt;/h3&gt;&lt;p&gt;Selecciona el &lt;b&gt;idioma origen&lt;/b&gt; y el &lt;b&gt;destino&lt;/b&gt; en el panel de traducción. El idioma origen se &lt;b&gt;detecta automáticamente&lt;/b&gt; al seleccionar una pista de subtítulos.&lt;/p&gt;&lt;h3&gt;2. Traducir al extraer&lt;/h3&gt;&lt;p&gt;Marca &lt;b&gt;Traducir al extraer&lt;/b&gt; para traducir automáticamente los subtítulos extraídos. Se guardan dos archivos: &lt;b&gt;archivo.srt&lt;/b&gt; (original) y &lt;b&gt;archivo.IDIOMA.srt&lt;/b&gt; (traducido), donde IDIOMA es el idioma de destino elegido.&lt;/p&gt;&lt;p&gt;Con &lt;b&gt;Traducir subtítulos&lt;/b&gt; puedes traducir en lote subtítulos ya existentes en disco.&lt;/p&gt;&lt;h3&gt;3. Trabajar con un solo idioma&lt;/h3&gt;&lt;p&gt;El desplegable &lt;b&gt;Solo subs&lt;/b&gt; (panel izquierdo) selecciona a la vez los subtítulos de un idioma concreto en todos los archivos del lote; los archivos sin ese idioma se omiten. Con el filtro activo, la traducción usa ese idioma como origen automáticamente.&lt;/p&gt;&lt;h3&gt;4. Incrustar la traducción (remux)&lt;/h3&gt;&lt;p&gt;Marca &lt;b&gt;Incrustar la traducción (remux)&lt;/b&gt; para generar un &lt;b&gt;archivo.IDIOMA.mkv&lt;/b&gt; (o .mp4 si el original es .mp4) con el vídeo y el audio originales y el subtítulo traducido dentro, sin re-codificar.&lt;/p&gt;&lt;p&gt;La app valida cada archivo antes de incrustar: si falta pista de vídeo lo omite, y si el .mp4 tiene códecs no copiables (audio pcm_*, vorbis, flac, dts, truehd…) cambia a .mkv automáticamente. Al terminar se muestra un resumen con los incrustados/omitidos y el motivo de cada fallo.&lt;/p&gt;&lt;h3&gt;5. Traducción triangular&lt;/h3&gt;&lt;p&gt;Si no existe un modelo directo (p. ej. Portugués → Español), se usa &lt;b&gt;traducción triangular&lt;/b&gt;: Portugués → Inglés → Español. Se te informará antes de empezar y requieren los modelos intermedios instalados.&lt;/p&gt;&lt;h3&gt;6. Gestionar modelos&lt;/h3&gt;&lt;p&gt;Abre &lt;b&gt;Ajustes → Configuración y Modelos…&lt;/b&gt; (o el botón &lt;b&gt;Modelos&lt;/b&gt;) y revisa la pestaña &lt;b&gt;Modelos de Traducción&lt;/b&gt;:&lt;/p&gt;&lt;ul&gt;&lt;li&gt;&lt;b&gt;Instalados&lt;/b&gt; — modelos descargados; doble clic para activar/desactivar.&lt;/li&gt;&lt;li&gt;&lt;b&gt;Online&lt;/b&gt; — modelos disponibles; doble clic para descargar.&lt;/li&gt;&lt;/ul&gt;&lt;p&gt;El panel señala qué pares tienen modelo directo (✓), cuáles requieren triangulación (↻) y cuáles no están disponibles (✗).&lt;/p&gt;&lt;h3&gt;7. Descarga de modelos&lt;/h3&gt;&lt;p&gt;Los modelos se descargan de &lt;b&gt;translatelocally.com&lt;/b&gt;. Los modelos &lt;b&gt;tiny&lt;/b&gt; (~20 MB) son rápidos y los &lt;b&gt;base&lt;/b&gt; (~50 MB) ofrecen mejor calidad. La traducción funciona &lt;b&gt;sin conexión&lt;/b&gt;.&lt;/p&gt;&lt;h3&gt;Idiomas soportados&lt;/h3&gt;&lt;p&gt;Inglés, Español, Francés, Portugués, Alemán, Italiano, Japonés, Chino, Coreano, Ruso, Árabe, Hindi, Holandés, Sueco, Polaco y Turco.&lt;/p&gt;</source>
        <translation type="vanished">&lt;h2&gt;Subtitle Translation&lt;/h2&gt;&lt;h3&gt;1. Choose languages&lt;/h3&gt;&lt;p&gt;Select the &lt;b&gt;source&lt;/b&gt; and &lt;b&gt;target&lt;/b&gt; language in the translation panel. The source language is &lt;b&gt;detected automatically&lt;/b&gt; when you select a subtitle track.&lt;/p&gt;&lt;h3&gt;2. Translate when extracting&lt;/h3&gt;&lt;p&gt;Tick &lt;b&gt;Translate when extracting&lt;/b&gt; to automatically translate the extracted subtitles. Two files are saved: &lt;b&gt;file.srt&lt;/b&gt; (original) and &lt;b&gt;file.LANG.srt&lt;/b&gt; (translated), where LANG is the chosen target language.&lt;/p&gt;&lt;p&gt;With &lt;b&gt;Translate subtitles&lt;/b&gt; you can batch-translate subtitles that already exist on disk.&lt;/p&gt;&lt;h3&gt;3. Work with a single language&lt;/h3&gt;&lt;p&gt;The &lt;b&gt;Only subs&lt;/b&gt; dropdown (left panel) selects the subtitles of a specific language across all batch files at once; files without that language are skipped. While the filter is active, translation uses that language as source automatically.&lt;/p&gt;&lt;h3&gt;4. Embed the translation (remux)&lt;/h3&gt;&lt;p&gt;Tick &lt;b&gt;Embed the translation (remux)&lt;/b&gt; to produce a &lt;b&gt;file.LANG.mkv&lt;/b&gt; (or .mp4 if the original is .mp4) with the original video and audio and the translated subtitle embedded, without re-encoding.&lt;/p&gt;&lt;p&gt;The app validates each file before embedding: files without a video track are skipped, and if the .mp4 has codecs that can't be stream-copied (audio pcm_*, vorbis, flac, dts, truehd…) it switches to .mkv automatically. At the end a summary shows the embedded/skipped files and the reason for each failure.&lt;/p&gt;&lt;h3&gt;5. Triangular translation&lt;/h3&gt;&lt;p&gt;If no direct model exists (e.g. Portuguese → Spanish), &lt;b&gt;triangular translation&lt;/b&gt; is used: Portuguese → English → Spanish. You will be informed before starting and it requires the intermediate models installed.&lt;/p&gt;&lt;h3&gt;6. Manage models&lt;/h3&gt;&lt;p&gt;Open &lt;b&gt;Settings → Settings and Models…&lt;/b&gt; (or the &lt;b&gt;Models&lt;/b&gt; button) and check the &lt;b&gt;Translation Models&lt;/b&gt; tab:&lt;/p&gt;&lt;ul&gt;&lt;li&gt;&lt;b&gt;Installed&lt;/b&gt; — downloaded models; double-click to enable/disable.&lt;/li&gt;&lt;li&gt;&lt;b&gt;Online&lt;/b&gt; — available models; double-click to download.&lt;/li&gt;&lt;/ul&gt;&lt;p&gt;The panel shows which pairs have a direct model (✓), which require triangulation (↻) and which are not available (✗).&lt;/p&gt;&lt;h3&gt;7. Downloading models&lt;/h3&gt;&lt;p&gt;Models are downloaded from &lt;b&gt;translatelocally.com&lt;/b&gt;. &lt;b&gt;tiny&lt;/b&gt; models (~20 MB) are fast and &lt;b&gt;base&lt;/b&gt; models (~50 MB) offer better quality. Translation works &lt;b&gt;offline&lt;/b&gt;.&lt;/p&gt;&lt;h3&gt;Supported languages&lt;/h3&gt;&lt;p&gt;English, Spanish, French, Portuguese, German, Italian, Japanese, Chinese, Korean, Russian, Arabic, Hindi, Dutch, Swedish, Polish and Turkish.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../helpdialog.cpp" line="128" />
        <source>Traducción</source>
        <translation>Translation</translation>
    </message>
    <message>
        <location filename="../helpdialog.cpp" line="30" />
        <source>&lt;h2&gt;Extracción de Pistas&lt;/h2&gt;&lt;h3&gt;1. Añadir archivos&lt;/h3&gt;&lt;p&gt;Pulsa &lt;b&gt;Añadir&lt;/b&gt; o arrastra los archivos directamente a la lista del panel izquierdo. Se admiten MKV, MP4, AVI, MOV, TS, M2TS y WebM.&lt;/p&gt;&lt;h3&gt;2. Seleccionar pistas&lt;/h3&gt;&lt;p&gt;Haz clic en un archivo para ver sus pistas (video, audio y subtítulos) con formato, idioma y resolución. Marca las que quieras extraer.&lt;/p&gt;&lt;p&gt;Usa los botones &lt;b&gt;VIDEO / AUDIO / SUBS&lt;/b&gt; para marcar (o desmarcar) todas las pistas de un tipo a la vez, y &lt;b&gt;Limpiar&lt;/b&gt; para quitarlas todas.&lt;/p&gt;&lt;h3&gt;3. Configurar el formato&lt;/h3&gt;&lt;p&gt;En &lt;b&gt;Formato de Salida&lt;/b&gt; elige el contenedor para cada tipo: audio (MKA, M4A, AAC, MP3, FLAC, Opus, WAV), video (MKV, MP4, AVI) y subtítulos (SRT, ASS, VTT), además de la resolución de vídeo.&lt;/p&gt;&lt;p&gt;Con &lt;b&gt;Copiar pista original sin re-codificar&lt;/b&gt; (recomendado) la extracción es rápida y sin pérdida de calidad: el contenedor se mantiene o se remuxea al seleccionado.&lt;/p&gt;&lt;h3&gt;4. Elegir destino y extraer&lt;/h3&gt;&lt;p&gt;Indica la carpeta de salida en &lt;b&gt;Destino&lt;/b&gt;. Los archivos se guardan en una subcarpeta &lt;b&gt;Elements Extract&lt;/b&gt;.&lt;/p&gt;&lt;p&gt;Pulsa &lt;b&gt;INICIAR EXTRACCIÓN&lt;/b&gt; o &lt;b&gt;Ctrl+Enter&lt;/b&gt;. Puedes cancelar con &lt;b&gt;Cancelar&lt;/b&gt; o &lt;b&gt;Escape&lt;/b&gt;.&lt;/p&gt;&lt;h3&gt;Procesamiento por lotes&lt;/h3&gt;&lt;p&gt;Si añades varios archivos se procesan en orden. La barra superior muestra el progreso y el botón &lt;b&gt;LOG&lt;/b&gt; despliega el registro de actividad.&lt;/p&gt;</source>
        <translation>&lt;h2&gt;Track Extraction&lt;/h2&gt;&lt;h3&gt;1. Add files&lt;/h3&gt;&lt;p&gt;Press &lt;b&gt;Add&lt;/b&gt; or drag the files directly into the list in the left panel. MKV, MP4, AVI, MOV, TS, M2TS and WebM are supported.&lt;/p&gt;&lt;h3&gt;2. Select tracks&lt;/h3&gt;&lt;p&gt;Click a file to see its tracks (video, audio and subtitles) with format, language and resolution. Tick the ones you want to extract.&lt;/p&gt;&lt;p&gt;Use the &lt;b&gt;VIDEO / AUDIO / SUBS&lt;/b&gt; buttons to check (or uncheck) all tracks of a type at once, and &lt;b&gt;Clear&lt;/b&gt; to remove them all.&lt;/p&gt;&lt;h3&gt;3. Configure the format&lt;/h3&gt;&lt;p&gt;In &lt;b&gt;Output Format&lt;/b&gt; choose the container for each type: audio (MKA, M4A, AAC, MP3, FLAC, Opus, WAV), video (MKV, MP4, AVI) and subtitles (SRT, ASS, VTT), plus the video resolution.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Copy original track without re-encoding&lt;/b&gt; (recommended) makes extraction fast and lossless: the container is kept or remuxed to the selected one.&lt;/p&gt;&lt;h3&gt;4. Choose destination and extract&lt;/h3&gt;&lt;p&gt;Set the output folder in &lt;b&gt;Destination&lt;/b&gt;. Files are saved in an &lt;b&gt;Elements Extract&lt;/b&gt; subfolder.&lt;/p&gt;&lt;p&gt;Press &lt;b&gt;START EXTRACTION&lt;/b&gt; or &lt;b&gt;Ctrl+Enter&lt;/b&gt;. You can cancel with &lt;b&gt;Cancel&lt;/b&gt; or &lt;b&gt;Escape&lt;/b&gt;.&lt;/p&gt;&lt;h3&gt;Batch processing&lt;/h3&gt;&lt;p&gt;If you add several files they are processed in order. The top bar shows progress and the &lt;b&gt;LOG&lt;/b&gt; button expands the activity log.&lt;/p&gt;</translation>
    </message>
    <message>
        <source>&lt;h2&gt;Traducción de Subtítulos&lt;/h2&gt;&lt;h3&gt;1. Elegir idiomas&lt;/h3&gt;&lt;p&gt;Selecciona el &lt;b&gt;idioma origen&lt;/b&gt; y el &lt;b&gt;destino&lt;/b&gt; en el panel de traducción. El idioma origen se &lt;b&gt;detecta automáticamente&lt;/b&gt; al seleccionar una pista de subtítulos.&lt;/p&gt;&lt;h3&gt;2. Traducir al extraer&lt;/h3&gt;&lt;p&gt;Marca &lt;b&gt;Traducir al extraer&lt;/b&gt; para traducir automáticamente los subtítulos extraídos. Se guardan dos archivos: &lt;b&gt;archivo.srt&lt;/b&gt; (original) y &lt;b&gt;archivo.IDIOMA.srt&lt;/b&gt; (traducido), donde IDIOMA es el idioma de destino elegido.&lt;/p&gt;&lt;p&gt;Con &lt;b&gt;Traducir subtítulos&lt;/b&gt; puedes traducir en lote subtítulos ya existentes en disco.&lt;/p&gt;&lt;h3&gt;3. Traducción triangular&lt;/h3&gt;&lt;p&gt;Si no existe un modelo directo (p. ej. Portugués → Español), se usa &lt;b&gt;traducción triangular&lt;/b&gt;: Portugués → Inglés → Español. Se te informará antes de empezar y requieren los modelos intermedios instalados.&lt;/p&gt;&lt;h3&gt;4. Gestionar modelos&lt;/h3&gt;&lt;p&gt;Abre &lt;b&gt;Ajustes → Configuración y Modelos…&lt;/b&gt; (o el botón &lt;b&gt;Modelos&lt;/b&gt;) y revisa la pestaña &lt;b&gt;Modelos de Traducción&lt;/b&gt;:&lt;/p&gt;&lt;ul&gt;&lt;li&gt;&lt;b&gt;Instalados&lt;/b&gt; — modelos descargados; doble clic para activar/desactivar.&lt;/li&gt;&lt;li&gt;&lt;b&gt;Online&lt;/b&gt; — modelos disponibles; doble clic para descargar.&lt;/li&gt;&lt;/ul&gt;&lt;p&gt;El panel señala qué pares tienen modelo directo (✓), cuáles requieren triangulación (↻) y cuáles no están disponibles (✗).&lt;/p&gt;&lt;h3&gt;5. Descarga de modelos&lt;/h3&gt;&lt;p&gt;Los modelos se descargan de &lt;b&gt;translatelocally.com&lt;/b&gt;. Los modelos &lt;b&gt;tiny&lt;/b&gt; (~20 MB) son rápidos y los &lt;b&gt;base&lt;/b&gt; (~50 MB) ofrecen mejor calidad. La traducción funciona &lt;b&gt;sin conexión&lt;/b&gt;.&lt;/p&gt;&lt;h3&gt;Idiomas soportados&lt;/h3&gt;&lt;p&gt;Inglés, Español, Francés, Portugués, Alemán, Italiano, Japonés, Chino, Coreano, Ruso, Árabe, Hindi, Holandés, Sueco, Polaco y Turco.&lt;/p&gt;</source>
        <translation type="vanished">&lt;h2&gt;Subtitle Translation&lt;/h2&gt;&lt;h3&gt;1. Choose languages&lt;/h3&gt;&lt;p&gt;Select the &lt;b&gt;source language&lt;/b&gt; and the &lt;b&gt;target language&lt;/b&gt; in the translation panel. The source language is &lt;b&gt;automatically detected&lt;/b&gt; when you select a subtitle track.&lt;/p&gt;&lt;h3&gt;2. Translate on extract&lt;/h3&gt;&lt;p&gt;Tick &lt;b&gt;Translate on extract&lt;/b&gt; to automatically translate the extracted subtitles. Two files are saved: &lt;b&gt;file.srt&lt;/b&gt; (original) and &lt;b&gt;file.LANG.srt&lt;/b&gt; (translated), where LANG is the chosen target language.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Translate subtitles&lt;/b&gt; lets you batch-translate subtitle files that already exist on disk.&lt;/p&gt;&lt;h3&gt;3. Pivot translation&lt;/h3&gt;&lt;p&gt;If no direct model exists (e.g. Portuguese &amp;#8594; Spanish), a &lt;b&gt;pivot translation&lt;/b&gt; is used: Portuguese &amp;#8594; English &amp;#8594; Spanish. You will be informed before starting; the intermediate models must be installed.&lt;/p&gt;&lt;h3&gt;4. Manage models&lt;/h3&gt;&lt;p&gt;Open &lt;b&gt;Settings &amp;#8594; Settings and Models…&lt;/b&gt; (or the &lt;b&gt;Models&lt;/b&gt; button) and check the &lt;b&gt;Translation Models&lt;/b&gt; tab:&lt;/p&gt;&lt;ul&gt;&lt;li&gt;&lt;b&gt;Installed&lt;/b&gt; &amp;#8212; downloaded models; double-click to enable/disable.&lt;/li&gt;&lt;li&gt;&lt;b&gt;Online&lt;/b&gt; &amp;#8212; available models; double-click to download.&lt;/li&gt;&lt;/ul&gt;&lt;p&gt;The panel shows which pairs have a direct model (&amp;#10003;), which need pivoting (&amp;#8635;) and which are not available (&amp;#10007;).&lt;/p&gt;&lt;h3&gt;5. Model download&lt;/h3&gt;&lt;p&gt;Models are downloaded from &lt;b&gt;translatelocally.com&lt;/b&gt;. &lt;b&gt;tiny&lt;/b&gt; models (~20 MB) are fast and &lt;b&gt;base&lt;/b&gt; (~50 MB) offer better quality. Translation works &lt;b&gt;offline&lt;/b&gt;.&lt;/p&gt;&lt;h3&gt;Supported languages&lt;/h3&gt;&lt;p&gt;English, Spanish, French, Portuguese, German, Italian, Japanese, Chinese, Korean, Russian, Arabic, Hindi, Dutch, Swedish, Polish and Turkish.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../helpdialog.cpp" line="69" />
        <source>&lt;h2&gt;Traducción de Subtítulos&lt;/h2&gt;&lt;h3&gt;1. Elegir idiomas&lt;/h3&gt;&lt;p&gt;Selecciona el &lt;b&gt;idioma origen&lt;/b&gt; y el &lt;b&gt;destino&lt;/b&gt; en el panel de traducción. El idioma origen se &lt;b&gt;detecta automáticamente&lt;/b&gt; al seleccionar una pista de subtítulos.&lt;/p&gt;&lt;h3&gt;2. Traducir al extraer&lt;/h3&gt;&lt;p&gt;Marca &lt;b&gt;Traducir al extraer&lt;/b&gt; para traducir automáticamente los subtítulos extraídos. Se guardan dos archivos: &lt;b&gt;archivo.srt&lt;/b&gt; (original) y &lt;b&gt;archivo.IDIOMA.srt&lt;/b&gt; (traducido), donde IDIOMA es el idioma de destino elegido.&lt;/p&gt;&lt;p&gt;Con &lt;b&gt;Traducir subtítulos&lt;/b&gt; puedes traducir en lote subtítulos ya existentes en disco.&lt;/p&gt;&lt;h3&gt;3. Trabajar con un solo idioma&lt;/h3&gt;&lt;p&gt;El desplegable &lt;b&gt;Solo subs&lt;/b&gt; (panel izquierdo) selecciona a la vez los subtítulos de un idioma concreto en todos los archivos del lote; los archivos sin ese idioma se omiten. Con el filtro activo, la traducción usa ese idioma como origen automáticamente.&lt;/p&gt;&lt;h3&gt;4. Incrustar la traducción (remux)&lt;/h3&gt;&lt;p&gt;Marca &lt;b&gt;Incrustar la traducción (remux)&lt;/b&gt; para generar un &lt;b&gt;archivo.IDIOMA.mkv&lt;/b&gt; (o .mp4 si el original es .mp4) con el vídeo y el audio originales y los subtítulos traducidos dentro, sin re-codificar. Si un vídeo tiene varias pistas traducidas, van todas en el mismo archivo (cada una conserva su título original cuando lo tiene).&lt;/p&gt;&lt;p&gt;La app valida cada archivo antes de incrustar: si falta pista de vídeo lo omite, y si el .mp4 tiene códecs no copiables (audio pcm_*, vorbis, flac, dts, truehd…) cambia a .mkv automáticamente. Al terminar se muestra un resumen con los incrustados/omitidos y el motivo de cada fallo. Con &lt;b&gt;Ajustes → Eliminar los .srt intermedios tras incrustarlos&lt;/b&gt; se borran los subtítulos extraídos y traducidos de cada vídeo cuyo remux terminó bien.&lt;/p&gt;&lt;h3&gt;5. Traducción triangular&lt;/h3&gt;&lt;p&gt;Si no existe un modelo directo (p. ej. Portugués → Español), se usa &lt;b&gt;traducción triangular&lt;/b&gt;: Portugués → Inglés → Español. Se te informará antes de empezar y requieren los modelos intermedios instalados.&lt;/p&gt;&lt;h3&gt;6. Gestionar modelos&lt;/h3&gt;&lt;p&gt;Abre &lt;b&gt;Ajustes → Configuración y Modelos…&lt;/b&gt; (o el botón &lt;b&gt;Modelos&lt;/b&gt;) y revisa la pestaña &lt;b&gt;Modelos de Traducción&lt;/b&gt;:&lt;/p&gt;&lt;ul&gt;&lt;li&gt;&lt;b&gt;Instalados&lt;/b&gt; — modelos descargados; doble clic para activar/desactivar.&lt;/li&gt;&lt;li&gt;&lt;b&gt;Online&lt;/b&gt; — modelos disponibles; doble clic para descargar.&lt;/li&gt;&lt;/ul&gt;&lt;p&gt;El panel señala qué pares tienen modelo directo (✓), cuáles requieren triangulación (↻) y cuáles no están disponibles (✗).&lt;/p&gt;&lt;h3&gt;7. Descarga de modelos&lt;/h3&gt;&lt;p&gt;Los modelos se descargan de &lt;b&gt;translatelocally.com&lt;/b&gt;. Los modelos &lt;b&gt;tiny&lt;/b&gt; (~20 MB) son rápidos y los &lt;b&gt;base&lt;/b&gt; (~50 MB) ofrecen mejor calidad. La traducción funciona &lt;b&gt;sin conexión&lt;/b&gt;.&lt;/p&gt;&lt;h3&gt;Idiomas soportados&lt;/h3&gt;&lt;p&gt;Inglés, Español, Francés, Portugués, Alemán, Italiano, Japonés, Chino, Coreano, Ruso, Árabe, Hindi, Holandés, Sueco, Polaco y Turco.&lt;/p&gt;</source>
        <translation>&lt;h2&gt;Subtitle Translation&lt;/h2&gt;&lt;h3&gt;1. Choose languages&lt;/h3&gt;&lt;p&gt;Select the &lt;b&gt;source&lt;/b&gt; and &lt;b&gt;target&lt;/b&gt; language in the translation panel. The source language is &lt;b&gt;detected automatically&lt;/b&gt; when you select a subtitle track.&lt;/p&gt;&lt;h3&gt;2. Translate when extracting&lt;/h3&gt;&lt;p&gt;Tick &lt;b&gt;Translate when extracting&lt;/b&gt; to automatically translate the extracted subtitles. Two files are saved: &lt;b&gt;file.srt&lt;/b&gt; (original) and &lt;b&gt;file.LANG.srt&lt;/b&gt; (translated), where LANG is the chosen target language.&lt;/p&gt;&lt;p&gt;With &lt;b&gt;Translate subtitles&lt;/b&gt; you can batch-translate subtitles that already exist on disk.&lt;/p&gt;&lt;h3&gt;3. Work with a single language&lt;/h3&gt;&lt;p&gt;The &lt;b&gt;Only subs&lt;/b&gt; dropdown (left panel) selects the subtitles of a specific language across all batch files at once; files without that language are skipped. While the filter is active, translation uses that language as source automatically.&lt;/p&gt;&lt;h3&gt;4. Embed the translation (remux)&lt;/h3&gt;&lt;p&gt;Tick &lt;b&gt;Embed the translation (remux)&lt;/b&gt; to produce a &lt;b&gt;file.LANG.mkv&lt;/b&gt; (or .mp4 if the original is .mp4) with the original video and audio and the translated subtitles embedded, without re-encoding. If a video has several translated tracks, they all go into the same file (each keeps its original title when it has one).&lt;/p&gt;&lt;p&gt;The app validates each file before embedding: files without a video track are skipped, and if the .mp4 has codecs that can't be stream-copied (audio pcm_*, vorbis, flac, dts, truehd…) it switches to .mkv automatically. At the end a summary shows the embedded/skipped files and the reason for each failure. With &lt;b&gt;Settings → Delete intermediate .srt files after embedding&lt;/b&gt;, the extracted and translated subtitles of each video whose remux finished correctly are deleted.&lt;/p&gt;&lt;h3&gt;5. Triangular translation&lt;/h3&gt;&lt;p&gt;If no direct model exists (e.g. Portuguese → Spanish), &lt;b&gt;triangular translation&lt;/b&gt; is used: Portuguese → English → Spanish. You will be informed before starting and it requires the intermediate models installed.&lt;/p&gt;&lt;h3&gt;6. Manage models&lt;/h3&gt;&lt;p&gt;Open &lt;b&gt;Settings → Settings and Models…&lt;/b&gt; (or the &lt;b&gt;Models&lt;/b&gt; button) and check the &lt;b&gt;Translation Models&lt;/b&gt; tab:&lt;/p&gt;&lt;ul&gt;&lt;li&gt;&lt;b&gt;Installed&lt;/b&gt; — downloaded models; double-click to enable/disable.&lt;/li&gt;&lt;li&gt;&lt;b&gt;Online&lt;/b&gt; — available models; double-click to download.&lt;/li&gt;&lt;/ul&gt;&lt;p&gt;The panel shows which pairs have a direct model (✓), which require triangulation (↻) and which are not available (✗).&lt;/p&gt;&lt;h3&gt;7. Downloading models&lt;/h3&gt;&lt;p&gt;Models are downloaded from &lt;b&gt;translatelocally.com&lt;/b&gt;. &lt;b&gt;tiny&lt;/b&gt; models (~20 MB) are fast and &lt;b&gt;base&lt;/b&gt; models (~50 MB) offer better quality. Translation works &lt;b&gt;offline&lt;/b&gt;.&lt;/p&gt;&lt;h3&gt;Supported languages&lt;/h3&gt;&lt;p&gt;English, Spanish, French, Portuguese, German, Italian, Japanese, Chinese, Korean, Russian, Arabic, Hindi, Dutch, Swedish, Polish and Turkish.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../helpdialog.cpp" line="136" />
        <source>&lt;h2&gt;Atajos de Teclado&lt;/h2&gt;&lt;table cellpadding='6' cellspacing='0' border='0'&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Ctrl+O&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Añadir archivos&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Supr&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Quitar archivos seleccionados&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Ctrl+Enter&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Iniciar extracción&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Escape&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Cancelar el proceso en curso&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Ctrl+,&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Abrir Configuración y Modelos&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;F1&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Mostrar esta ayuda&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Ctrl+Q&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Salir&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;&lt;h2&gt;Ajustes&lt;/h2&gt;&lt;h3&gt;Configuración y Modelos&lt;/h3&gt;&lt;p&gt;En &lt;b&gt;Ajustes → Configuración y Modelos…&lt;/b&gt; (Ctrl+,) puedes:&lt;/p&gt;&lt;ul&gt;&lt;li&gt;Cambiar el directorio de modelos de traducción&lt;/li&gt;&lt;li&gt;Establecer los idiomas origen y destino por defecto&lt;/li&gt;&lt;li&gt;Activar la traducción automática al extraer&lt;/li&gt;&lt;li&gt;Gestionar la descarga y activación de modelos&lt;/li&gt;&lt;/ul&gt;&lt;h3&gt;Apariencia e idioma&lt;/h3&gt;&lt;ul&gt;&lt;li&gt;&lt;b&gt;Tema&lt;/b&gt; — Automático, Oscuro o Claro (se aplica al instante).&lt;/li&gt;&lt;li&gt;&lt;b&gt;Idioma de interfaz&lt;/b&gt; — Español, Inglés o Portugués; el cambio se aplica en caliente, sin reiniciar.&lt;/li&gt;&lt;/ul&gt;&lt;h3&gt;Soporte&lt;/h3&gt;&lt;p&gt;Repositorio: &lt;b&gt;github.com/pdmiguel90-droid/elements-extract&lt;/b&gt;&lt;/p&gt;&lt;p&gt;Para reportar errores o sugerencias, abre un issue en GitHub.&lt;/p&gt;</source>
        <translation>&lt;h2&gt;Keyboard Shortcuts&lt;/h2&gt;&lt;table cellpadding='6' cellspacing='0' border='0'&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Ctrl+O&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Add files&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Del&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Remove selected files&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Ctrl+Enter&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Start extraction&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Escape&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Cancel the running process&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Ctrl+,&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Open Settings and Models&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;F1&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Show this help&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;b&gt;Ctrl+Q&lt;/b&gt;&lt;/td&gt;&lt;td&gt;Quit&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;&lt;h2&gt;Settings&lt;/h2&gt;&lt;h3&gt;Settings and Models&lt;/h3&gt;&lt;p&gt;In &lt;b&gt;Settings &amp;#8594; Settings and Models…&lt;/b&gt; (Ctrl+,) you can:&lt;/p&gt;&lt;ul&gt;&lt;li&gt;Change the translation models directory&lt;/li&gt;&lt;li&gt;Set the default source and target languages&lt;/li&gt;&lt;li&gt;Enable automatic translation on extract&lt;/li&gt;&lt;li&gt;Manage model download and activation&lt;/li&gt;&lt;/ul&gt;&lt;h3&gt;Appearance and language&lt;/h3&gt;&lt;ul&gt;&lt;li&gt;&lt;b&gt;Theme&lt;/b&gt; &amp;#8212; Automatic, Dark or Light (applies instantly).&lt;/li&gt;&lt;li&gt;&lt;b&gt;Interface language&lt;/b&gt; &amp;#8212; Spanish, English or Portuguese; the change applies on the fly, no restart needed.&lt;/li&gt;&lt;/ul&gt;&lt;h3&gt;Support&lt;/h3&gt;&lt;p&gt;Repository: &lt;b&gt;github.com/pdmiguel90-droid/elements-extract&lt;/b&gt;&lt;/p&gt;&lt;p&gt;To report bugs or suggestions, open an issue on GitHub.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../helpdialog.cpp" line="171" />
        <source>Atajos y Ajustes</source>
        <translation>Shortcuts and Settings</translation>
    </message>
    <message>
        <location filename="../helpdialog.cpp" line="178" />
        <source>Cerrar</source>
        <translation>Close</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../mainwindow.cpp" line="114" />
        <source>↻ Traducción triangular: %1</source>
        <translation>↻ Pivot translation: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="132" />
        <source>Mostrar / Ocultar ventana</source>
        <translation>Show / Hide window</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="138" />
        <source>Cancelar proceso actual</source>
        <translation>Cancel current process</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="143" />
        <location filename="../mainwindow.cpp" line="188" />
        <location filename="../mainwindow.cpp" line="1917" />
        <source>Salir</source>
        <translation>Quit</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="157" />
        <source>&lt;font color='#FF4444'&gt;  › No se pudo iniciar ffmpeg. ¿Está instalado?.&lt;/font&gt;</source>
        <translation>&lt;font color='#FF4444'&gt;  › Could not start ffmpeg. Is it installed?.&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="181" />
        <location filename="../mainwindow.cpp" line="1913" />
        <source>Archivo</source>
        <translation>File</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="183" />
        <location filename="../mainwindow.cpp" line="1916" />
        <source>Añadir archivos…</source>
        <translation>Add files…</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="192" />
        <location filename="../mainwindow.cpp" line="1914" />
        <source>Ajustes</source>
        <translation>Settings</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="194" />
        <location filename="../mainwindow.cpp" line="1918" />
        <source>Configuración y Modelos…</source>
        <translation>Settings and Models…</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="198" />
        <location filename="../mainwindow.cpp" line="1915" />
        <source>Ayuda</source>
        <translation>Help</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="200" />
        <location filename="../mainwindow.cpp" line="1919" />
        <source>Manual de uso</source>
        <translation>User manual</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="205" />
        <location filename="../mainwindow.cpp" line="1920" />
        <source>Acerca de…</source>
        <translation>About…</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="236" />
        <location filename="../mainwindow.cpp" line="1923" />
        <source>LOTE DE ARCHIVOS</source>
        <translation>FILE BATCH</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="248" />
        <location filename="../mainwindow.cpp" line="1926" />
        <source>Añadir</source>
        <translation> Add</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="249" />
        <location filename="../mainwindow.cpp" line="1930" />
        <source>Quitar</source>
        <translation> Remove</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="250" />
        <location filename="../mainwindow.cpp" line="1927" />
        <source>Añadir archivos de video (Ctrl+O)</source>
        <translation>Add video files (Ctrl+O)</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="251" />
        <location filename="../mainwindow.cpp" line="1931" />
        <source>Quitar archivos seleccionados (Del)</source>
        <translation>Remove selected files (Del)</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="259" />
        <location filename="../mainwindow.cpp" line="1924" />
        <source>PISTAS</source>
        <translation>TRACKS</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="263" />
        <location filename="../mainwindow.cpp" line="1934" />
        <source>Selecciona un archivo para ver sus pistas</source>
        <translation>Select a file to view its tracks</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="274" />
        <location filename="../mainwindow.cpp" line="1935" />
        <source>VIDEO</source>
        <translation>VIDEO</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="275" />
        <location filename="../mainwindow.cpp" line="1936" />
        <source>AUDIO</source>
        <translation>AUDIO</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="276" />
        <location filename="../mainwindow.cpp" line="1937" />
        <source>SUBS</source>
        <translation>SUBS</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="277" />
        <location filename="../mainwindow.cpp" line="1938" />
        <source>Limpiar</source>
        <translation> Clear</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="295" />
        <location filename="../mainwindow.cpp" line="1939" />
        <source>Solo subs:</source>
        <translation>Only subs:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="298" />
        <location filename="../mainwindow.cpp" line="1547" />
        <source>Todos los idiomas</source>
        <translation>All languages</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="300" />
        <location filename="../mainwindow.cpp" line="1942" />
        <source>Selecciona los subtítulos de un idioma concreto en TODOS los archivos del lote.
Mientras esté activo, la extracción y la traducción trabajarán solo con ese idioma.</source>
        <translation>Select the subtitles of a specific language in ALL batch files. While active, extraction and translation will work only with that language.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="326" />
        <location filename="../mainwindow.cpp" line="1948" />
        <source>FORMATO DE SALIDA</source>
        <translation>OUTPUT FORMAT</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="337" />
        <location filename="../mainwindow.cpp" line="1958" />
        <source>Original</source>
        <translation>Original</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="348" />
        <location filename="../mainwindow.cpp" line="1949" />
        <source>Audio:</source>
        <translation>Audio:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="349" />
        <location filename="../mainwindow.cpp" line="1950" />
        <source>Video:</source>
        <translation>Video:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="350" />
        <location filename="../mainwindow.cpp" line="1951" />
        <source>Res:</source>
        <translation>Res:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="351" />
        <location filename="../mainwindow.cpp" line="1952" />
        <source>Subs:</source>
        <translation>Subs:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="360" />
        <location filename="../mainwindow.cpp" line="1954" />
        <source>Copiar pista original sin re-codificar (Rápido y previene errores)</source>
        <translation>Copy original track without re-encoding (Fast and prevents errors)</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="377" />
        <location filename="../mainwindow.cpp" line="1964" />
        <source>TRADUCCIÓN DE SUBTÍTULOS</source>
        <translation>SUBTITLE TRANSLATION</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="382" />
        <location filename="../mainwindow.cpp" line="1965" />
        <source>Origen:</source>
        <translation>Source:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="388" />
        <location filename="../mainwindow.cpp" line="1966" />
        <source>Destino:</source>
        <translation>Target:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="400" />
        <location filename="../mainwindow.cpp" line="1968" />
        <source>Traducir al extraer</source>
        <translation>Translate on extract</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="402" />
        <location filename="../mainwindow.cpp" line="1970" />
        <source>Si está marcado, al extraer subtítulos se traducirán automáticamente
al idioma de destino seleccionado.</source>
        <translation>When checked, extracted subtitles will be automatically translated
to the selected target language.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="410" />
        <location filename="../mainwindow.cpp" line="1974" />
        <source>Incrustar la traducción (remux)</source>
        <translation>Embed the translation (remux)</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="412" />
        <location filename="../mainwindow.cpp" line="1976" />
        <source>Tras traducir, genera un archivo .mkv/.mp4 con el vídeo y el audio originales
y la pista de subtítulos traducida incrustada, sin re-codificar (rápido y sin pérdida).</source>
        <translation>After translating, produces a .mkv/.mp4 file with the original video and audio and the translated subtitle track embedded, without re-encoding (fast and lossless).</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="418" />
        <location filename="../mainwindow.cpp" line="1980" />
        <source>Modelos</source>
        <translation>Models</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="419" />
        <location filename="../mainwindow.cpp" line="1981" />
        <source>Gestionar modelos de traducción</source>
        <translation>Manage translation models</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="421" />
        <location filename="../mainwindow.cpp" line="1984" />
        <source>Traducir subtítulos</source>
        <translation>Translate subtitles</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="423" />
        <location filename="../mainwindow.cpp" line="1986" />
        <source>Seleccionar subtítulos ya existentes en disco y traducirlos en lote</source>
        <translation>Select existing subtitle files on disk and translate them in batch</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="436" />
        <location filename="../mainwindow.cpp" line="1989" />
        <source>Selecciona subtítulos y marca "Traducir al extraer"</source>
        <translation>Select subtitles and check "Translate on extract"</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="448" />
        <location filename="../mainwindow.cpp" line="1994" />
        <source>Carpeta donde se guardarán los archivos extraídos…</source>
        <translation>Folder where extracted files will be saved…</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="453" />
        <location filename="../mainwindow.cpp" line="1997" />
        <source>Seleccionar carpeta de destino</source>
        <translation>Select destination folder</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="454" />
        <location filename="../mainwindow.cpp" line="2000" />
        <source>Abrir</source>
        <translation> Open</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="455" />
        <location filename="../mainwindow.cpp" line="2001" />
        <source>Abrir carpeta de destino en el explorador</source>
        <translation>Open destination folder in file explorer</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="529" />
        <location filename="../mainwindow.cpp" line="2018" />
        <source>Sistema listo. Añade archivos y selecciona las pistas a extraer.</source>
        <translation>System ready. Add files and select the tracks to extract.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="767" />
        <source>Modelo de traducción no instalado</source>
        <translation>Translation model not installed</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="769" />
        <source>No hay modelo instalado para:&lt;br&gt;&lt;b&gt;%1&lt;/b&gt;&lt;br&gt;&lt;br&gt;Pero existe para descargar: &lt;b&gt;%2&lt;/b&gt;&lt;br&gt;&lt;br&gt;Idioma detectado: %3 → %4</source>
        <translation>No model installed for:&lt;br&gt;&lt;b&gt;%1&lt;/b&gt;&lt;br&gt;&lt;br&gt;But it is available for download: &lt;b&gt;%2&lt;/b&gt;&lt;br&gt;&lt;br&gt;Detected language: %3 → %4</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="778" />
        <source>No hay modelo instalado para:&lt;br&gt;&lt;b&gt;%1&lt;/b&gt;&lt;br&gt;&lt;br&gt;No se pudo comprobar el catálogo online (sin conexión o aún cargando).&lt;br&gt;&lt;br&gt;Idioma detectado: %2 → %3</source>
        <translation>No model installed for:&lt;br&gt;&lt;b&gt;%1&lt;/b&gt;&lt;br&gt;&lt;br&gt;The online catalog could not be checked (offline or still loading).&lt;br&gt;&lt;br&gt;Detected language: %2 → %3</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="785" />
        <source>Modelo de traducción no disponible</source>
        <translation>Translation model not available</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="787" />
        <source>No existe modelo de traducción para:&lt;br&gt;&lt;b&gt;%1&lt;/b&gt;&lt;br&gt;&lt;br&gt;Tampoco hay ruta triangular ni descarga disponible (el ruso, por ejemplo, no tiene modelo publicado).&lt;br&gt;&lt;br&gt;Idioma detectado: %2 → %3</source>
        <translation>No translation model exists for:&lt;br&gt;&lt;b&gt;%1&lt;/b&gt;&lt;br&gt;&lt;br&gt;There is no triangular path or download available either (Russian, for example, has no published model).&lt;br&gt;&lt;br&gt;Detected language: %2 → %3</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1024" />
        <source>%1 pista(s) desmarcadas a mano, excluidas aunque coincidan con el filtro.</source>
        <translation>%1 track(s) manually unchecked, excluded even though they match the filter.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1116" />
        <source>%1 archivo(s) sin idioma detectado: se traducen como %2; verifica el resultado.</source>
        <translation>%1 file(s) with undetected language: translated as %2; please verify the result.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1428" />
        <source>Intermedios eliminados: %1</source>
        <translation>Intermediates deleted: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1441" />
        <source>Extracción vacía o fallida, excluida de la traducción: %1</source>
        <translation>Empty or failed extraction, excluded from translation: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1616" />
        <source>El filtro coincide con %1 pistas en %2: desmarca las que no necesites.</source>
        <translation>The filter matches %1 tracks in %2: uncheck the ones you don't need.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1692" />
        <source>&lt;font color='#FFCC00'&gt;  › No se encontró la traducción, se omite esa pista: %1&lt;/font&gt;</source>
        <translation>&lt;font color='#FFCC00'&gt;  › Translation not found, skipping that track: %1&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1772" />
        <source>  › Incrustando %1 pista(s) → %2</source>
        <translation>  › Embedding %1 track(s) → %2</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2205" />
        <source>Sin modelo de traducción</source>
        <translation>No translation model</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2307" />
        <source>&lt;b&gt;Modelos de traducción&lt;/b&gt;&lt;br&gt;&lt;br&gt;Si te falta un modelo para un par de idiomas, descárgalo desde Ajustes → Configuración y Modelos… → pestaña Modelos de Traducción → ONLINE (doble clic). La app te avisará en el registro si el modelo que necesitas existe para descargar.&lt;br&gt;&lt;br&gt;&lt;b&gt;Modelos incluidos:&lt;/b&gt;&lt;br&gt;• English → Español (en→es)&lt;br&gt;• Español → English (es→en)&lt;br&gt;• English → Français (en→fr)&lt;br&gt;• Français → English (fr→en)&lt;br&gt;• English → Português (en→pt)&lt;br&gt;• Português → English (pt→en)&lt;br&gt;&lt;br&gt;&lt;b&gt;Directorio de modelos:&lt;/b&gt;&lt;br&gt;</source>
        <translation>&lt;b&gt;Translation models&lt;/b&gt;&lt;br&gt;&lt;br&gt;If you are missing a model for a language pair, download it from Settings → Settings and Models… → Translation Models tab → ONLINE (double-click). The app will tell you in the log if the model you need is available for download.&lt;br&gt;&lt;br&gt;&lt;b&gt;Included models:&lt;/b&gt;&lt;br&gt;• English → Spanish (en→es)&lt;br&gt;• Spanish → English (es→en)&lt;br&gt;• English → French (en→fr)&lt;br&gt;• French → English (fr→en)&lt;br&gt;• English → Portuguese (en→pt)&lt;br&gt;• Portuguese → English (pt→en)&lt;br&gt;&lt;br&gt;&lt;b&gt;Models directory:&lt;/b&gt;&lt;br&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="471" />
        <location filename="../mainwindow.cpp" line="2006" />
        <source>Cancelar</source>
        <translation> Cancel</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="445" />
        <location filename="../mainwindow.cpp" line="1992" />
        <source>DESTINO</source>
        <translation>DESTINATION</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="452" />
        <location filename="../mainwindow.cpp" line="1996" />
        <source>Examinar…</source>
        <translation>Browse…</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="474" />
        <location filename="../mainwindow.cpp" line="2007" />
        <source>Cancelar el proceso en curso (Escape)</source>
        <translation>Cancel the current process (Escape)</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="477" />
        <location filename="../mainwindow.cpp" line="2010" />
        <source>INICIAR EXTRACCIÓN</source>
        <translation>  START EXTRACTION</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="481" />
        <location filename="../mainwindow.cpp" line="2011" />
        <source>Iniciar la extracción de todas las pistas seleccionadas (Ctrl+Enter)</source>
        <translation>Start extraction of all selected tracks (Ctrl+Enter)</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="519" />
        <location filename="../mainwindow.cpp" line="542" />
        <location filename="../mainwindow.cpp" line="2014" />
        <source>LOG ▾</source>
        <translation>LOG ▾</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="514" />
        <location filename="../mainwindow.cpp" line="2015" />
        <source>Mostrar / ocultar registro de actividad</source>
        <translation>Show / hide activity log</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="512" />
        <location filename="../mainwindow.cpp" line="519" />
        <location filename="../mainwindow.cpp" line="2014" />
        <source>LOG ▸</source>
        <translation>LOG ▸</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="532" />
        <location filename="../mainwindow.cpp" line="594" />
        <location filename="../mainwindow.cpp" line="882" />
        <location filename="../mainwindow.cpp" line="1910" />
        <source>Listo</source>
        <translation>Ready</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="534" />
        <location filename="../mainwindow.cpp" line="1909" />
        <source>Elements Extract FFmpeg</source>
        <translation>Elements Extract FFmpeg</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="572" />
        <source>Añadir Videos</source>
        <translation>Add Videos</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="573" />
        <source>Videos (*.mkv *.mp4 *.avi *.mov *.ts *.m2ts *.webm);;Todos los archivos (*.*)</source>
        <translation>Videos (*.mkv *.mp4 *.avi *.mov *.ts *.m2ts *.webm);;All files (*.*)</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="591" />
        <source>No hay archivos cargados</source>
        <translation>No files loaded</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="620" />
        <source>Analizando: </source>
        <translation>Analyzing: </translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="622" />
        <source>Analizando…</source>
        <translation>Analyzing…</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="717" />
        <source>Idioma origen detectado: %1</source>
        <translation>Detected source language: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="736" />
        <source>Traducción triangular disponible</source>
        <translation>Pivot translation available</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="739" />
        <source>No hay modelo directo de &lt;b&gt;%1 → %2&lt;/b&gt;.&lt;br&gt;&lt;br&gt;Pero se usará &lt;b&gt;traducción triangular&lt;/b&gt;:&lt;br&gt;%3&lt;br&gt;&lt;br&gt;Esto puede tardar un poco más, pero funcionará correctamente.</source>
        <translation>No direct model for &lt;b&gt;%1 → %2&lt;/b&gt;.&lt;br&gt;&lt;br&gt;But &lt;b&gt;pivot translation&lt;/b&gt; will be used:&lt;br&gt;%3&lt;br&gt;&lt;br&gt;This may take a bit longer, but it will work correctly.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="745" />
        <location filename="../mainwindow.cpp" line="795" />
        <source>¿Qué deseas hacer?</source>
        <translation>What would you like to do?</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="746" />
        <source>Aceptar</source>
        <translation>Accept</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="747" />
        <location filename="../mainwindow.cpp" line="799" />
        <source>Cambiar idioma destino</source>
        <translation>Change target language</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="776" />
        <source>Modelo de traducción no encontrado</source>
        <translation>Translation model not found</translation>
    </message>
    <message>
        <source>No se encontró modelo de traducción para:&lt;br&gt;&lt;b&gt;%1&lt;/b&gt;&lt;br&gt;&lt;br&gt;Tampoco hay ruta triangular disponible.&lt;br&gt;&lt;br&gt;Idioma detectado: %2 → %3</source>
        <translation type="vanished">Translation model not found for:&lt;br&gt;&lt;b&gt;%1&lt;/b&gt;&lt;br&gt;&lt;br&gt;No pivot path is available either.&lt;br&gt;&lt;br&gt;Detected language: %2 → %3</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="798" />
        <source>Descargar modelo</source>
        <translation>Download model</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="800" />
        <source>Continuar sin traducir</source>
        <translation>Continue without translating</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="889" />
        <source>No se pudo analizar el archivo.</source>
        <translation>Could not analyze the file.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="890" />
        <source>&lt;font color='#FF4444'&gt;[ERROR] No se pudo analizar el archivo.&lt;/font&gt;</source>
        <translation>&lt;font color='#FF4444'&gt;[ERROR] Could not analyze the file.&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1022" />
        <source>&lt;font color='#FF8800'&gt;&lt;b&gt;=== INICIANDO EXTRACCIÓN ===&lt;/b&gt;&lt;/font&gt;</source>
        <translation>&lt;font color='#FF8800'&gt;&lt;b&gt;=== STARTING EXTRACTION ===&lt;/b&gt;&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1036" />
        <source>&lt;br&gt;&lt;font color='#FF4444'&gt;&lt;b&gt;[CANCELADO] La traducción fue interrumpida por el usuario.&lt;/b&gt;&lt;/font&gt;</source>
        <translation>&lt;br&gt;&lt;font color='#FF4444'&gt;&lt;b&gt;[CANCELLED] Translation was interrupted by the user.&lt;/b&gt;&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1057" />
        <source>&lt;br&gt;&lt;font color='#FF4444'&gt;&lt;b&gt;[CANCELADO] El proceso fue interrumpido por el usuario.&lt;/b&gt;&lt;/font&gt;</source>
        <translation>&lt;br&gt;&lt;font color='#FF4444'&gt;&lt;b&gt;[CANCELLED] The process was interrupted by the user.&lt;/b&gt;&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2158" />
        <source>Pero sí existe para descargar: %1. Ábrelo en Ajustes → Configuración y Modelos… → pestaña Modelos de Traducción → ONLINE (doble clic para descargar).</source>
        <translation>But it is available for download: %1. Open Settings → Settings and Models… → Translation Models tab → ONLINE (double-click to download).</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2161" />
        <source>  › No se pudo comprobar el catálogo online (sin conexión o aún cargando). Con internet, abre Ajustes → Configuración y Modelos… → ONLINE para ver si existe.</source>
        <translation>› The online catalog could not be checked (offline or still loading). With internet access, open Settings → Settings and Models… → ONLINE to see if it exists.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2164" />
        <source>  › Tampoco existe en el catálogo online (el ruso, por ejemplo, no tiene modelo publicado). Solo se pueden traducir subtítulos en English, Español, Français o Português: cambia el filtro "Solo subs" o el idioma origen.</source>
        <translation>› It does not exist in the online catalog either (Russian, for example, has no published model). Only subtitles in English, Spanish, French or Portuguese can be translated: change the "Only subs" filter or the source language.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1145" />
        <location filename="../mainwindow.cpp" line="2290" />
        <source>Remux omitido: no hay subtítulos traducidos para incrustar.</source>
        <translation>Remux skipped: no translated subtitles to embed.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2178" />
        <location filename="../mainwindow.cpp" line="2190" />
        <source>Traducción</source>
        <translation>Translation</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2179" />
        <source>Selecciona idiomas de origen y destino distintos antes de traducir.</source>
        <translation>Select different source and target languages before translating.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2184" />
        <source>Seleccionar subtítulos para traducir</source>
        <translation>Select subtitles to translate</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2185" />
        <source>Subtítulos (*.srt *.ass *.ssa *.vtt);;Todos los archivos (*.*)</source>
        <translation>Subtitles (*.srt *.ass *.ssa *.vtt);;All files (*.*)</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2191" />
        <source>Selecciona primero una carpeta de destino.</source>
        <translation>Select an output folder first.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2196" />
        <source>&lt;br&gt;&lt;font color='#FF8800'&gt;&lt;b&gt;=== TRADUCCIÓN POR LOTES ===&lt;/b&gt;&lt;/font&gt;</source>
        <translation>&lt;br&gt;&lt;font color='#FF8800'&gt;&lt;b&gt;=== BATCH TRANSLATION ===&lt;/b&gt;&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2197" />
        <source>Traduciendo %1 archivo(s): %2 → %3</source>
        <translation>Translating %1 file(s): %2 → %3</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2211" />
        <source>Traduciendo en lote...</source>
        <translation>Translating in batch...</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2224" />
        <source>Traduciendo archivo %1 / %2…</source>
        <translation>Translating file %1 / %2…</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1037" />
        <location filename="../mainwindow.cpp" line="1058" />
        <source>Cancelado</source>
        <translation>Cancelled</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="850" />
        <source>&lt;font color='#FFCC00'&gt;El filtro "Solo subs" está activo. Ponlo en "Todos los idiomas" para la selección global de subs.&lt;/font&gt;</source>
        <translation>&lt;font color='#FFCC00'&gt;The "Only subs" filter is active. Set it to "All languages" for global subtitle selection.&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1046" />
        <source>&lt;br&gt;&lt;font color='#FF4444'&gt;&lt;b&gt;[CANCELADO] El remux fue interrumpido por el usuario.&lt;/b&gt;&lt;/font&gt;</source>
        <translation>&lt;br&gt;&lt;font color='#FF4444'&gt;&lt;b&gt;[CANCELLED] The remux was interrupted by the user.&lt;/b&gt;&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1074" />
        <location filename="../mainwindow.cpp" line="1662" />
        <source>&lt;br&gt;&lt;font color='#39FF14'&gt;&lt;b&gt;=== ✓ PROCESO FINALIZADO ===&lt;/b&gt;&lt;/font&gt;</source>
        <translation>&lt;br&gt;&lt;font color='#39FF14'&gt;&lt;b&gt;=== ✓ PROCESS FINISHED ===&lt;/b&gt;&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1075" />
        <source>✓ Finalizado — %1 archivo(s) procesado(s)</source>
        <translation>✓ Finished — %1 file(s) processed</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2151" />
        <source>Sin traducción %1 → %2: no hay modelo instalado (ni directo ni triangular).</source>
        <translation>No translation %1 → %2: no model installed (neither direct nor triangular).</translation>
    </message>
    <message>
        <source>  › Solo se pueden traducir subtítulos en English, Español, Français o Português. Cambia el filtro "Solo subs" o el idioma origen, o instala el modelo desde "Gestionar modelos".</source>
        <translation type="vanished">  › Only subtitles in English, Spanish, French or Portuguese can be translated. Change the "Only subs" filter or the source language, or install the model from "Manage models".</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1112" />
        <source>%1 archivo(s) omitidos: no están en %2.</source>
        <translation>%1 file(s) skipped: they are not in %2.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1126" />
        <source>Traduciendo %1 archivo(s) de subtítulos...</source>
        <translation>Translating %1 subtitle file(s)...</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1131" />
        <location filename="../mainwindow.cpp" line="2188" />
        <source>Traducidos</source>
        <translation>Translated</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1148" />
        <source>El proceso por lotes ha finalizado correctamente.</source>
        <translation>The batch process has completed successfully.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1158" />
        <source>Procesando %1/%2: %3</source>
        <translation>Processing %1/%2: %3</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1162" />
        <location filename="../mainwindow.cpp" line="1768" />
        <source>&lt;br&gt;&lt;font color='#00BFFF'&gt;&lt;b&gt;[%1/%2] %3&lt;/b&gt;&lt;/font&gt;</source>
        <translation>&lt;br&gt;&lt;font color='#00BFFF'&gt;&lt;b&gt;[%1/%2] %3&lt;/b&gt;&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1168" />
        <source>Extraídos</source>
        <translation>Extracted</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1168" />
        <source>Convertidos</source>
        <translation>Converted</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1170" />
        <source>&lt;font color='#FF4444'&gt;  › No se ha especificado carpeta de destino, omitido.&lt;/font&gt;</source>
        <translation>&lt;font color='#FF4444'&gt;  › No output folder specified, skipped.&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1281" />
        <source>&lt;font color='#FFCC00'&gt;  › Subtítulo omitido para traducción (varias pistas de texto o formato de imagen): %1&lt;/font&gt;</source>
        <translation>&lt;font color='#FFCC00'&gt;  › Subtitle skipped for translation (multiple text tracks or image format): %1&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1327" />
        <source>&lt;font color='#FFCC00'&gt;  › Sin pistas seleccionadas para este archivo, omitido.&lt;/font&gt;</source>
        <translation>&lt;font color='#FFCC00'&gt;  › No tracks selected for this file, skipped.&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1363" />
        <location filename="../mainwindow.cpp" line="1392" />
        <source>&lt;font color='#39FF14'&gt;  › OK&lt;/font&gt;</source>
        <translation>&lt;font color='#39FF14'&gt;  › OK&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1379" />
        <source>error de ffmpeg</source>
        <translation>ffmpeg error</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1382" />
        <source>&lt;font color='#FF4444'&gt;  › Error al incrustar, omitido.&lt;/font&gt;</source>
        <translation>&lt;font color='#FF4444'&gt;  › Error embedding, skipped.&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1395" />
        <source>&lt;font color='#FF4444'&gt;  › Error en este archivo.&lt;/font&gt;</source>
        <translation>&lt;font color='#FF4444'&gt;  › Error on this file.&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1494" />
        <source>Destino</source>
        <translation>Destination</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1620" />
        <source>Filtro: solo subtítulos en </source>
        <translation>Filter: only subtitles in </translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1638" />
        <source>&lt;br&gt;&lt;font color='#FF8800'&gt;&lt;b&gt;=== INCRUSTANDO SUBTÍTULOS (REMUX) ===&lt;/b&gt;&lt;/font&gt;</source>
        <translation>&lt;br&gt;&lt;font color='#FF8800'&gt;&lt;b&gt;=== EMBEDDING SUBTITLES (REMUX) ===&lt;/b&gt;&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1664" />
        <source>&lt;font color='#FFCC00'&gt;&lt;b&gt;Incrustados: %1 · Omitidos/error: %2&lt;/b&gt;&lt;/font&gt;</source>
        <translation>&lt;font color='#FFCC00'&gt;&lt;b&gt;Embedded: %1 · Skipped/error: %2&lt;/b&gt;&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1667" />
        <source>&lt;font color='#FF8844'&gt;  › %1&lt;/font&gt;</source>
        <translation>&lt;font color='#FF8844'&gt;  › %1&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1669" />
        <source>✓ Finalizado — %1 incrustados, %2 omitidos</source>
        <translation>✓ Finished — %1 embedded, %2 skipped</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1676" />
        <source>Subtítulos incrustados: %1 correctos, %2 con problemas.</source>
        <translation>Subtitles embedded: %1 successful, %2 with issues.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1677" />
        <source>Subtítulos incrustados correctamente.</source>
        <translation>Subtitles embedded successfully.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1698" />
        <source>no se encontró la traducción</source>
        <translation>translation not found</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1700" />
        <source>no hay carpeta de destino configurada</source>
        <translation>no output folder configured</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1707" />
        <source>sin pista de vídeo: no se puede incrustar</source>
        <translation>no video track: cannot embed</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1713" />
        <source>&lt;font color='#FF8844'&gt;  › Omitido: %1 (%2)&lt;/font&gt;</source>
        <translation>&lt;font color='#FF8844'&gt;  › Skipped: %1 (%2)&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1720" />
        <source>Remuxados</source>
        <translation>Remuxed</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1732" />
        <source>audio %1</source>
        <translation>audio %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1734" />
        <source>vídeo %1</source>
        <translation>video %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1739" />
        <source>&lt;font color='#FFCC00'&gt;  › %1 no es copiable a MP4 (%2) → se usará MKV&lt;/font&gt;</source>
        <translation>&lt;font color='#FFCC00'&gt;  › %1 is not MP4-compatible (%2) → MKV will be used&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1764" />
        <source>Incrustando %1/%2: %3</source>
        <translation>Embedding %1/%2: %3</translation>
    </message>
    <message>
        <source>  › Incrustando %1 → %2</source>
        <translation type="vanished">  › Embedding %1 → %2</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1870" />
        <source>🎵 %1 audio</source>
        <translation>🎵 %1 audio</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="1871" />
        <source>CC %1 subs</source>
        <translation>CC %1 subs</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2117" />
        <source>No se encontró el archivo: %1</source>
        <translation>File not found: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2126" />
        <source>&lt;font color='#FFCC00'&gt;Idioma origen y destino son iguales.&lt;/font&gt;</source>
        <translation>&lt;font color='#FFCC00'&gt;Source and target languages are the same.&lt;/font&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2134" />
        <source>Traduciendo...</source>
        <translation>Translating...</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2136" />
        <source>Traduciendo: %1 → %2</source>
        <translation>Translating: %1 → %2</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2260" />
        <source>Traducción completada: </source>
        <translation>Translation completed: </translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2262" />
        <source>Traducción guardada: %1</source>
        <translation>Translation saved: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2269" />
        <source>Error en la traducción</source>
        <translation>Translation error</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2271" />
        <source>Error de traducción: %1</source>
        <translation>Translation error: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2275" />
        <source>Error de traducción</source>
        <translation>Translation error</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2282" />
        <source>Traducción completada</source>
        <translation>Translation completed</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2296" />
        <source>La traducción ha finalizado correctamente.</source>
        <translation>The translation has completed successfully.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2304" />
        <source>Gestionar Modelos de Traducción</source>
        <translation>Manage Translation Models</translation>
    </message>
    <message>
        <source>&lt;b&gt;Modelos de traducción&lt;/b&gt;&lt;br&gt;&lt;br&gt;Los modelos se descargan automáticamente cuando se necesita un par de idiomas no disponible.&lt;br&gt;&lt;br&gt;&lt;b&gt;Modelos incluidos:&lt;/b&gt;&lt;br&gt;• English → Español (en→es)&lt;br&gt;• Español → English (es→en)&lt;br&gt;• English → Français (en→fr)&lt;br&gt;• Français → English (fr→en)&lt;br&gt;• English → Português (en→pt)&lt;br&gt;• Português → English (pt→en)&lt;br&gt;&lt;br&gt;&lt;b&gt;Directorio de modelos:&lt;/b&gt;&lt;br&gt;</source>
        <translation type="vanished">&lt;b&gt;Translation models&lt;/b&gt;&lt;br&gt;&lt;br&gt;Models are downloaded automatically when a language pair is needed.&lt;br&gt;&lt;br&gt;&lt;b&gt;Included models:&lt;/b&gt;&lt;br&gt;• English → Spanish (en→es)&lt;br&gt;• Spanish → English (es→en)&lt;br&gt;• English → French (en→fr)&lt;br&gt;• French → English (fr→en)&lt;br&gt;• English → Portuguese (en→pt)&lt;br&gt;• Portuguese → English (pt→en)&lt;br&gt;&lt;br&gt;&lt;b&gt;Models directory:&lt;/b&gt;&lt;br&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2332" />
        <source>Descargando modelo %1: %2%</source>
        <translation>Downloading model %1: %2%</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2339" />
        <source>Modelo descargado: </source>
        <translation>Model downloaded: </translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2340" />
        <source>Modelo descargado: %1</source>
        <translation>Model downloaded: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2345" />
        <source>Error descargando modelo: </source>
        <translation>Error downloading model: </translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2346" />
        <source>Error descargando %1: %2</source>
        <translation>Error downloading %1: %2</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2388" />
        <source>Idioma de interfaz cambiado a: %1</source>
        <translation>Interface language changed to: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2391" />
        <source>No se pudo cargar la traducción para: %1</source>
        <translation>Could not load translation for: %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="2410" />
        <source>&lt;font color='#888888'&gt;Configuración actualizada&lt;/font&gt;</source>
        <translation>&lt;font color='#888888'&gt;Settings updated&lt;/font&gt;</translation>
    </message>
</context>
<context>
    <name>SettingsDialog</name>
    <message>
        <location filename="../settingsdialog.cpp" line="35" />
        <location filename="../settingsdialog.cpp" line="310" />
        <source>Ajustes — Elements Extract</source>
        <translation>Settings — Elements Extract</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="53" />
        <location filename="../settingsdialog.cpp" line="315" />
        <source>Directorio de Modelos</source>
        <translation>Models Directory</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="58" />
        <location filename="../settingsdialog.cpp" line="316" />
        <source>Examinar...</source>
        <translation>Browse...</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="66" />
        <location filename="../settingsdialog.cpp" line="317" />
        <source>Idiomas por Defecto</source>
        <translation>Default Languages</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="68" />
        <location filename="../settingsdialog.cpp" line="318" />
        <source>Origen:</source>
        <translation>Source:</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="75" />
        <location filename="../settingsdialog.cpp" line="319" />
        <source>Destino:</source>
        <translation>Target:</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="87" />
        <location filename="../settingsdialog.cpp" line="320" />
        <source>Opciones</source>
        <translation>Options</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="89" />
        <location filename="../settingsdialog.cpp" line="322" />
        <source>Traducir subtítulos automáticamente al extraer</source>
        <translation>Automatically translate subtitles when extracting</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="96" />
        <location filename="../settingsdialog.cpp" line="324" />
        <source>Eliminar los .srt intermedios tras incrustarlos (remux)</source>
        <translation>Delete intermediate .srt files after embedding (remux)</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="97" />
        <location filename="../settingsdialog.cpp" line="325" />
        <source>Borra los subtítulos extraídos y traducidos de cada vídeo cuando su remux termina correctamente. Sin remux no se borra nada.</source>
        <translation>Deletes the extracted and translated subtitles of each video when its remux finishes correctly. Nothing is deleted without remux.</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="107" />
        <location filename="../settingsdialog.cpp" line="327" />
        <source>Apariencia</source>
        <translation>Appearance</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="110" />
        <location filename="../settingsdialog.cpp" line="328" />
        <source>Tema:</source>
        <translation>Theme:</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="113" />
        <location filename="../settingsdialog.cpp" line="125" />
        <source>Automático (detectar del sistema)</source>
        <translation>Automatic (detect from system)</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="114" />
        <source>Oscuro</source>
        <translation>Dark</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="115" />
        <source>Claro</source>
        <translation>Light</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="122" />
        <location filename="../settingsdialog.cpp" line="329" />
        <source>Idioma de interfaz:</source>
        <translation>Interface language:</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="139" />
        <location filename="../settingsdialog.cpp" line="312" />
        <source>General</source>
        <translation>General</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="149" />
        <source>Modelos instalados</source>
        <translation>Installed models</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="151" />
        <location filename="../settingsdialog.cpp" line="331" />
        <source>Buscar online</source>
        <translation>Search online</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="152" />
        <location filename="../settingsdialog.cpp" line="332" />
        <source>Buscar modelos disponibles en translatelocally.com</source>
        <translation>Search for available models on translatelocally.com</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="172" />
        <location filename="../settingsdialog.cpp" line="334" />
        <source>INSTALADOS — doble clic para activar/desactivar</source>
        <translation>INSTALLED — double-click to enable/disable</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="184" />
        <location filename="../settingsdialog.cpp" line="188" />
        <location filename="../settingsdialog.cpp" line="336" />
        <source>Eliminar</source>
        <translation>Remove</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="188" />
        <source>¿Eliminar </source>
        <translation>Remove </translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="200" />
        <location filename="../settingsdialog.cpp" line="335" />
        <source>ONLINE — doble clic para descargar</source>
        <translation>ONLINE — double-click to download</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="218" />
        <location filename="../settingsdialog.cpp" line="339" />
        <source>&lt;b&gt;tiny&lt;/b&gt; ~15-25MB rápido · &lt;b&gt;base&lt;/b&gt; ~50MB mejor calidad&lt;br&gt;Doble clic = activar/desactivar (solo activos se usan para traducir)</source>
        <translation>&lt;b&gt;tiny&lt;/b&gt; ~15-25MB fast · &lt;b&gt;base&lt;/b&gt; ~50MB better quality&lt;br&gt;Double-click = enable/disable (only enabled models are used for translating)</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="224" />
        <location filename="../settingsdialog.cpp" line="313" />
        <source>Modelos de Traducción</source>
        <translation>Translation Models</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="231" />
        <location filename="../settingsdialog.cpp" line="341" />
        <source>Cerrar</source>
        <translation>Close</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="349" />
        <source>Directorio de modelos</source>
        <translation>Models directory</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="363" />
        <source>Buscando...</source>
        <translation>Searching...</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="390" />
        <source>Volver a descargar</source>
        <translation>Download again</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="391" />
        <source>El modelo '%1' ya está instalado.
¿Descargarlo de nuevo? (reemplaza la copia local)</source>
        <translation>Model '%1' is already installed.
Download it again? (replaces the local copy)</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="456" />
        <source>Descargando %1...</source>
        <translation>Downloading %1...</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="484" />
        <source>Error: </source>
        <translation>Error: </translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="603" />
        <source>↻ actualizar</source>
        <translation>↻ update</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="289" />
        <source>%1 modelos online</source>
        <translation>%1 models online</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="371" />
        <source>No disponible</source>
        <translation>Not available</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="371" />
        <source>Gestor de modelos no disponible.</source>
        <translation>Model manager not available.</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="383" />
        <location filename="../settingsdialog.cpp" line="485" />
        <source>Error</source>
        <translation>Error</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="383" />
        <source>Modelo no encontrado en la lista online.</source>
        <translation>Model not found in the online list.</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="398" />
        <source>Descargando: </source>
        <translation>Downloading: </translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="452" />
        <source>Descargando %1: %2%</source>
        <translation>Downloading %1: %2%</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="477" />
        <source>Descargado y activado: </source>
        <translation>Downloaded and enabled: </translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="485" />
        <source>No se pudo descargar </source>
        <translation>Could not download </translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="552" />
        <source>%1 instalados, %2 activos</source>
        <translation>%1 installed, %2 enabled</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="563" />
        <source>&lt;b&gt;Modelos disponibles:&lt;/b&gt;</source>
        <translation>&lt;b&gt;Available models:&lt;/b&gt;</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="581" />
        <source>(no disponible)</source>
        <translation>(not available)</translation>
    </message>
    <message>
        <location filename="../settingsdialog.cpp" line="600" />
        <source>(ya instalado)</source>
        <translation>(already installed)</translation>
    </message>
</context>
<context>
    <name>SubtitleTranslator</name>
    <message>
        <location filename="../subtitletranslator.cpp" line="175" />
        <location filename="../subtitletranslator.cpp" line="272" />
        <source>No se pudieron leer los subtítulos del archivo: </source>
        <translation>Could not read the subtitles from file: </translation>
    </message>
    <message>
        <location filename="../subtitletranslator.cpp" line="184" />
        <location filename="../subtitletranslator.cpp" line="281" />
        <source>No se encontró el motor de traducción (bergamot).
Asegúrese de que está instalado correctamente.
Buscado en: </source>
        <translation>Translation engine (bergamot) not found.
Make sure it is installed correctly.
Searched in: </translation>
    </message>
    <message>
        <location filename="../subtitletranslator.cpp" line="194" />
        <source>No se encontró un modelo de traducción para %1→%2.
Ni modelo directo ni ruta triangular disponible.
Modelos instalados disponibles para: en, es, fr, pt.</source>
        <translation>Translation model not found for %1→%2.
Neither direct model nor pivot path available.
Installed models available for: en, es, fr, pt.</translation>
    </message>
    <message>
        <location filename="../subtitletranslator.cpp" line="293" />
        <source>El modelo de traducción %1→%2 no está instalado.
Directorio buscado: %3</source>
        <translation>Translation model %1→%2 is not installed.
Directory searched: %3</translation>
    </message>
    <message>
        <location filename="../subtitletranslator.cpp" line="325" />
        <location filename="../subtitletranslator.cpp" line="381" />
        <location filename="../subtitletranslator.cpp" line="463" />
        <source>Error al escribir el archivo de salida: </source>
        <translation>Error writing the output file: </translation>
    </message>
    <message>
        <location filename="../subtitletranslator.cpp" line="350" />
        <location filename="../subtitletranslator.cpp" line="405" />
        <location filename="../subtitletranslator.cpp" line="480" />
        <source>No se pudo iniciar el motor de traducción</source>
        <translation>Could not start the translation engine</translation>
    </message>
    <message>
        <location filename="../subtitletranslator.cpp" line="358" />
        <source>Tiempo de espera agotado del motor de traducción</source>
        <translation>Translation engine timed out</translation>
    </message>
    <message>
        <location filename="../subtitletranslator.cpp" line="364" />
        <location filename="../subtitletranslator.cpp" line="440" />
        <source>Error del motor de traducción (código %1).</source>
        <translation>Translation engine error (code %1).</translation>
    </message>
    <message>
        <location filename="../subtitletranslator.cpp" line="483" />
        <source>El motor de traducción terminó inesperadamente</source>
        <translation>The translation engine terminated unexpectedly</translation>
    </message>
    <message>
        <location filename="../subtitletranslator.cpp" line="486" />
        <source>El motor de traducción excedió el tiempo límite</source>
        <translation>The translation engine exceeded the time limit</translation>
    </message>
    <message>
        <location filename="../subtitletranslator.cpp" line="489" />
        <source>Error desconocido del motor de traducción</source>
        <translation>Unknown translation engine error</translation>
    </message>
</context>
</TS>