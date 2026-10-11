# Changelog

Todos los cambios notables de este proyecto se documentan aquí.
Formato basado en [Keep a Changelog](https://keepachangelog.com/es/1.0.0/).

---

## [1.2.0] - 2026-10-11

### Añadido
- Filtro de subtítulos por idioma: selecciona a la vez los subtítulos de un idioma concreto en todos los archivos del lote ("Solo subs")
- Incrustación de la traducción (remux): tras traducir, genera un `.mkv`/`.mp4` con el vídeo/audio originales y la pista de subtítulos traducida, sin re-codificar
- Soporte Qt6 con fallback a Qt5 en tiempo de configuración (QSettings en formato INI portable)
- Empaquetado AppImage con los modelos incluidos
- CI: build + release `.deb`, AppImage, mirror público de modelos en GitHub Pages y vigilancia semanal del catálogo
- Pipeline local para convertir modelos OPUS-MT/Tatoeba al formato Bergamot de la app
- Port a Windows (workflow CI)

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
