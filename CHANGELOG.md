# Changelog

Todos los cambios notables de este proyecto se documentan aquí.
Formato basado en [Keep a Changelog](https://keepachangelog.com/es/1.0.0/).

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
