# Changelog

Todos los cambios notables de este proyecto se documentan aquí.
Formato basado en [Keep a Changelog](https://keepachangelog.com/es/1.0.0/).

---

## [1.2.13] - 2026-11-25

### Añadido
- Ajustes → Modelos de Traducción separa dos apartados: **incluidos (no eliminables)** —los seis `tiny` de `en↔es/fr/pt`, ~142 MB— y **descargados**, con idioma, nombre, tamaño aproximado, versión y marca `activo`. El botón Eliminar se desactiva para los incluidos y tampoco se pueden volver a descargar.
- La lista **online** muestra todos los modelos del registro (31 únicos, duplicados eliminados), ordenados por idioma, con badges `⚡ tiny` / `★ base`, `↻ actualizar` y un **buscador** que filtra ambas listas por ID, idioma, nombre o tamaño.
- Los idiomas dejan de estar fijos en `en/es/fr/pt`: al descargar un modelo de otro par (de, cs, is, pl…), ese idioma aparece automáticamente en los selectores de origen/destino y se usa en rutas directas o triangulares. Ya no se oculta ningún modelo.
- Diálogo **Acerca de** rediseñado: scroll, carta de presentación por bloques, aviso de privacidad y siete chips tecnológicos (Qt, C++17, FFmpeg, bergamot, nOCR, OpenSubtitles y GitHub Releases).
- Nuevas capturas en inglés de modelos y About, referenciadas en los README y en el metainfo; documento `docs/github-metadata.md` con la descripción/topics/homepage pendientes de aplicar en GitHub.

### Corregido
- **Windows ya traduce**: los workflows extraen los tar.gz con `--strip-components=1` y empaquetan los seis modelos en la ruta plana que busca la app. El zip de v1.2.12 dejaba la configuración en `models/en-es-tiny/enes.student.tiny11/` y `isModelInstalled()` devolvía falso.
- Compatibilidad con instalaciones antiguas: el traductor y el gestor de modelos reconocen también el layout anidado de v1.2.12, así que no hace falta reinstalar para volver a traducir.
- Resolución de alias de idioma: un modelo del registro con ID `hbs-eng-tiny` se usa correctamente para `hbs→en`, y se respeta el directorio de modelos personalizado al detectar idiomas y rutas.
- CMake solo instala el launcher `bergamot` y `libopenblas` de Linux en builds UNIX; el zip de Windows no mezcla binarios de otros sistemas.
- Ayuda, READMEs (es/en/pt), `models/README.md`, `docs/modelos.md`, metainfo y traducciones en/pt actualizados con el estado real de los modelos incluidos, el buscador y la descarga de cualquier par del registro.

### Nota
- No se empaquetan modelos `base`: los incluidos son los seis `tiny`. No se requiere descarga inicial para en↔es/fr/pt.

---

## [1.2.12] - 2026-11-22

### Corregido
- El build de Windows compila con Qt 6.8: se usa `RedirectPolicyAttribute` / `NoLessSafeRedirectPolicy`, los nombres válidos en Qt 6, en el descargador de subtítulos y en el notificador de versiones.
- El flag `-s` de GCC/Clang ya no se pasa a MSVC.
- Se eliminaron los avisos de API obsoleta de `QCheckBox::stateChanged` y `QString::fromUtf16`.

---

## [1.2.11] - 2026-11-22

### Corregido
- El build de Windows ya no falla en la configuración: se eliminó la dependencia de zlib. La base de plantillas nOCR se distribuye sin comprimir (857 KB) y se carga directamente desde recursos.
- Los workflows de Linux y los README ya no instalan ni documentan `zlib1g-dev`.

---

## [1.2.10] - 2026-11-22

### Corregido
- Los botones «Cancelar» e «INICIAR EXTRACCIÓN» ya no quedan ocultos al abrir el registro: viven en un pie fijo fuera del área desplazable de opciones.
- El registro de actividad tiene una altura máxima para no reducir el área de trabajo.

---

## [1.2.9] - 2026-11-22

### Añadido
- Notificador de versiones nuevas mediante la API de GitHub Releases: comprobación automática al arrancar y acción manual «Buscar actualizaciones…» en el menú Ayuda, con notas de la versión y enlace de descarga.
- La versión detectada se puede ignorar para no volver a mostrar el aviso; los fallos de red nunca interrumpen el arranque.

---

## [1.2.8] - 2026-11-22

### Corregido
- Las compilaciones limpias de CI volvían a fallar: se instala `zlib1g-dev` en las workflows de Linux y se versiona `share/ocr/Latin.nocr.gz`, que estaba excluido por el patrón `*.gz` de `.gitignore` y es necesario para `resources.qrc`
- Capturas de pantalla actualizadas y en inglés para AppImageHub, con el diálogo de subtítulos online y el backend XML-RPC
- Documentación pública al día en los cuatro README: funciones online, OCR experimental y dependencia zlib
- Checklist de AppImageHub actualizado a la versión 1.2.8

---

## [1.2.7] - 2026-11-22

### Añadido
- Backend XML-RPC de opensubtitles.org como opción predeterminada, con búsqueda y descarga anónimas mediante enlaces directos al estilo de VLSub en VLC; en Ajustes se puede volver a la API REST de opensubtitles.com
- OCR opcional de subtítulos de imagen con nOCR: extrae los fotogramas con ffmpeg, genera un .srt traducible e incorporable y conserva la copia .mks original si el OCR falla
- Base de plantillas Latin.nocr de Subtitle Edit incluida como recurso (537 KB, licencia MIT) y atribución en share/ocr/NOTICE.txt
- El OCR se marca como experimental: muestra progreso en el registro y avisa de que un episodio completo puede tardar varios minutos. Corregida la binarización de PGS con contorno negro (antes se reconocía el contorno en vez del relleno, con hasta un 46 % de glifos desconocidos; con el PGS real de Suits baja al 1,1 %) y el espaciado entre palabras

### Corregido
- La búsqueda online combinaba solo el hash del archivo y, si ese lanzamiento tenía subtítulos en inglés, no llegaban los de otros idiomas. Ahora se busca también por título/temporada/capítulo y se fusionan ambos resultados sin duplicados, aplicando el filtro de idioma en las dos consultas

---

## [1.2.6] - 2026-11-22

### Añadido
- Preferencia del idioma de destino de Ajustes en la búsqueda de subtítulos online: aparece marcado como «(preferido)» y en negrita en el desplegable, y sus resultados se ordenan primero
- Aviso y confirmación al descargar cuando un vídeo no tiene subtítulos en el idioma preferido: ofrece descargar el mejor subtítulo disponible en otro idioma y traducirlo mediante la ruta triangular, sin botones duplicados
- Ayuda actualizada con el nuevo flujo online, la preferencia de idioma, la traducción triangular aplicada a subtítulos descargados y el atajo Ctrl+Shift+S

---

## [1.2.5] - 2026-11-22

### Añadido
- Búsqueda manual y por lote de subtítulos online: selecciona varios vídeos desde el botón «Buscar subs online», el menú **Archivo → Buscar subtítulos en internet…** o el clic derecho sobre la lista; edita la consulta de cada archivo, marca los archivos/resultados que quieres procesar y descarga o traduce los subtítulos de todos ellos en secuencia

### Seguridad
- Las credenciales de OpenSubtitles (clave API, usuario y contraseña) se guardan ahora en el almacén seguro del sistema: Secret Service en Linux, Credential Manager en Windows y Keychain en macOS
- Se migran automáticamente las credenciales anteriores al almacén seguro y se eliminan los antiguos valores en texto plano de `QSettings` (`onlineSubs/apiKey`, `onlineSubs/username` y `onlineSubs/password`) al iniciar la aplicación o al abrir Ajustes

---

## [1.2.4] - 2026-11-22

### Añadido
- Búsqueda y descarga de subtítulos en internet mediante la API de OpenSubtitles.com (búsqueda por hash del vídeo con fallback por título, descarga de ZIP/GZIP/RAR y detección de SRT/ASS/VTT), con opción de traducirlos y/o incrustarlos con el flujo existente. Las credenciales se configuran en Ajustes → Subtítulos online y nunca se incluyen en el código

---

## [1.2.3] - 2026-11-22

### Añadido
- Extracción de subtítulos de imagen (PGS de Blu-ray, VobSub/DVD, DVB, XSUB) como `.mks` mediante copia de flujo, sin recodificar
- Aviso en el panel de metadatos y en el registro de los tipos de subtítulos presentes en cada archivo (texto/imagen, idioma y si se pueden traducir)
- Mensajes de error más útiles: detalle del stderr de ffprobe al analizar y últimas líneas de ffmpeg al fallar una extracción

### Corregido
- Los subtítulos de imagen ya no se intentan convertir a SRT/ASS/VTT (ffmpeg no puede hacerlo sin OCR) ni se envían a la traducción

---

## [1.2.2] - 2026-11-02

### Añadido
- Traducción de subtítulos sueltos: la app puede abrirse directamente con archivos `.srt`, `.ass`, `.ssa`, `.vtt` (clic derecho / "Abrir con...") y traducirlos sin pasar por la extracción de vídeo
- Servicemenu de traducción en el `.deb` para integrarse en el menú contextual del escritorio
- MimeType `text/x-srt;application/x-subrip` en el `.desktop` para que el sistema ofrezca la app al hacer clic derecho en subtítulos

---

## [1.2.1] - 2026-10-22

### Corregido
- Interfaz en inglés cuando el locale del sistema es `C`/`POSIX` (entornos de CI y tests automáticos de AppImageHub)
- Windows: se empaqueta `ffprobe.exe` junto a `ffmpeg.exe` y la búsqueda de ejecutables es portable (`QStandardPaths::findExecutable`)
- CI: publicación del espejo de modelos en `gh-pages` (exponer `GITHUB_TOKEN` al step de push)

---

## [1.2.0] - 2026-10-11

### Añadido
- Filtro de subtítulos por idioma: selecciona a la vez los subtítulos de un idioma concreto en todos los archivos del lote ("Solo subs")
- Incrustación de la traducción (remux): tras traducir, genera un `.mkv`/`.mp4` con el vídeo/audio originales y la pista de subtítulos traducida, sin re-codificar
- Migración completa a Qt6 (mandatorio): dependencias, empaquetado .deb y AppImage usan Qt6
- Empaquetado AppImage con los modelos incluidos
- CI: build + release `.deb`, AppImage, mirror público de modelos en GitHub Pages y vigilancia semanal del catálogo
- Pipeline local para convertir modelos OPUS-MT/Tatoeba al formato Bergamot de la app
- Port a Windows (workflow CI)

### Cambiado
- Remux con validación previa: se omite el archivo si no tiene pista de vídeo y, si el `.mp4` contiene códecs no copiables (audio `pcm_*`, `vorbis`, `flac`, `dts`, `truehd`…), se incrusta automáticamente en `.mkv` en lugar de generar un MP4 defectuoso
- Remux con notificación de errores: se registra el motivo real de cada fallo y al terminar se muestra un resumen con los archivos incrustados/omitidos
- Ayuda (F1), README (es/en/pt) y diálogo "Acerca de" actualizados con el filtro por idioma y el remux

### Corregido
- CI Windows: se usa el runner `windows-2022` (VS 2022) y se localiza dinámicamente la ruta real de Qt6 instalada por aqtinstall (`qt/<ver>/msvc2022_64`)
- CI Windows: `windeployqt` invoca el binario de `stage/bin` y `ffmpeg.exe` se instala junto al ejecutable; la búsqueda del motor `bergamot` añade la extensión `.exe` en Windows
- CI Linux/AppImage: se instalan los headers de OpenGL/EGL requeridos por Qt6 y se fuerza la detección de Qt6

---

## [1.0.0] - 2025-05-12

### Añadido
- Interfaz principal con tema oscuro (fondo #121212, acentos naranja y verde neón)
- Lista de archivos con soporte para lote múltiple (MKV, MP4, AVI, MOV, TS, M2TS, WebM)
- Drag & Drop de archivos desde el explorador de archivos
- Panel de pistas (streams) por archivo con checkboxes individuales
- Selección global por tipo: VIDEO, AUDIO, SUBS para todo el lote
- Combos de formato de salida: Audio (MKA, M4A, AAC, MP3, FLAC, OPUS, WAV), Video (MKV, MP4, AVI), Subs (SRT, ASS, VTT)
- Barra de progreso real basada en la salida `time=` de FFmpeg
- Panel de metadatos del archivo: duración, tamaño, resolución, número de pistas
- Diálogo "Acerca de" con información del autor y tecnologías utilizadas
- Manual de uso integrado (F1)
- Barra de menú: Archivo, Ayuda
- Barra de estado con información del proceso en curso
- Botón Cancelar para abortar el proceso en cualquier momento
- Botón "Abrir carpeta" para abrir el destino en el explorador
- Iconos del tema del sistema en todos los botones y acciones del menú
- Icono de ventana generado programáticamente
- Atajos de teclado: Ctrl+O, Del, Ctrl+Enter, Escape, F1, Ctrl+Q
- Persistencia de configuración con QSettings (carpeta destino, formatos, geometría de ventana)
- Archivo .desktop para integración con el menú de aplicaciones del sistema
- Reordenamiento de archivos en el lote por drag & drop interno
- Retroalimentación visual: archivos con pistas seleccionadas resaltados en naranja

---

## [Próximas versiones]

### Planificado para v1.1.0
- [ ] Notificación de escritorio al finalizar el proceso
- [ ] Perfiles de extracción (presets guardables)
- [ ] Soporte para renombrado de archivos de salida personalizado
- [ ] Modo oscuro/claro seleccionable
