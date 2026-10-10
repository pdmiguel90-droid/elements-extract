# Elements Extract — FFmpeg

> Descargaste una película o una serie… y le falta lo único que tiene que tener: **el subtítulo en tu idioma**.

**Elements Extract** es un extractor gráfico de pistas de audio, vídeo y subtítulos con procesamiento en lote, que además **traduce subtítulos a tu idioma de forma 100 % offline**, directamente en tu equipo.

![License: GPL-3.0](https://img.shields.io/badge/License-GPL%20v3-blue.svg)
![Qt5](https://img.shields.io/badge/Qt-5.x-green.svg)
![Platform](https://img.shields.io/badge/Platform-Linux-orange.svg)
![Build](https://img.shields.io/badge/Build-CMake-lightgrey.svg)

---

## 🎯 Por qué existe este proyecto

Este proyecto nació de una necesidad personal. Cuando descargo una película o una serie, a veces pasa esto:

- El subtítulo que necesito **no está disponible** o no existe en mi idioma.
- El único que encuentro está **mal sincronizado** o es de baja calidad.
- No quiero depender de ninguna web para buscarlo ni para "conseguirlo".

Entonces pensé: *¿y si pudiera tomar el archivo que ya tengo en el disco y traducir su subtítulo yo mismo, en mi casa, sin internet?*

**Elements Extract** es exactamente eso: una herramienta **offline** que extrae las pistas de tu archivo y **traduce los subtítulos al idioma que quieras con tu propio equipo**, sin depender de internet para encontrarlos.

> 🔒 **Privacidad:** tus archivos nunca salen de tu máquina. La traducción se realiza localmente con modelos de traducción neuronal (vía [bergamot-translator](https://github.com/browsermt/bergamot-translator)). Solo necesitas internet **una vez**, para descargar el modelo de idioma; después puedes traducir todo lo que quieras, estés donde estés.

---

## Capturas

*(pendiente de agregar)*

---

## Características

- 📂 **Procesamiento en lote** — añade múltiples archivos a la vez
- 🎯 **Selección individual de pistas** — marca exactamente qué pistas extraer de cada archivo
- ▶ **Selección global por tipo** — botones VIDEO / AUDIO / SUBS para seleccionar todas las pistas de un tipo en todo el lote
- 📊 **Panel de metadatos** — muestra duración, tamaño, resolución, número de pistas al seleccionar un archivo
- 🔄 **Formatos de salida configurables** — elige entre MKA, M4A, AAC, MP3, FLAC, OPUS, WAV para audio; MKV, MP4, AVI para video; SRT, ASS, VTT para subtítulos
- 📈 **Barra de progreso real** — calcula el porcentaje en base a la duración del archivo
- ✋ **Cancelación** — cancela el proceso en cualquier momento
- 💾 **Persistencia** — guarda la carpeta de destino, formatos y tamaño de ventana entre sesiones
- 🖱️ **Drag & Drop** — arrastra archivos directamente a la ventana
- ⌨️ **Atajos de teclado** — `Ctrl+O`, `Del`, `Ctrl+Enter`, `Escape`, `F1`
- 🌐 **Traducción de subtítulos offline** — traduce subtítulos extraídos al idioma que quieras (en, es, fr, pt)
- 🔄 **Traducción triangular** — si no hay modelo directo (ej. pt→es), usa intermediarios (pt→en→es)
- 🤖 **Detección automática de idioma** — detecta el idioma de los subtítulos desde la metadata del archivo
- ⬇️ **Modelos descargables** — descarga modelos de traducción desde translatelocally.com (solo una vez)

---

## Formatos soportados

### Contenedores de entrada

| Formato | Pistas de video | Pistas de audio | Subtítulos | Notas |
|---------|----------------|-----------------|------------|-------|
| **MKV** (Matroska) | Múltiples | Múltiples | SRT, ASS, PGS, DVB | El más completo — soporta casi todo |
| **MP4** | 1-2 | Múltiples | mov_text, cea-608 | Estándar web — compatibilidad universal |
| **AVI** | 1 | 1-2 | Sin subtítulos | Formato legacy |
| **TS/M2TS** | Múltiples | Múltiples | DVB, PGS | Blu-ray y broadcast |
| **OGG/OGM** | Múltiples | Múltiples | SRT | Open source |
| **FLV** | 1 | 1-2 | Sin subtítulos | Flash (obsoleto) |
| **WMV** | 1 | 1-2 | SRT | Windows Media |

### Códecs soportados (vía ffmpeg)

- **Video**: H.264, H.265/HEVC, VP8, VP9, AV1, MPEG-2, MPEG-4, Theora
- **Audio**: AAC, MP3, FLAC, Opus, Vorbis, AC3, DTS, PCM, ALAC
- **Subtítulos**: SRT, ASS/SSA, PGS (Blu-ray), DVB, WebVTT, mov_text

### Formatos de salida

| Tipo | Formatos |
|------|----------|
| **Audio** | MKA, M4A, AAC, MP3, FLAC, OPUS, WAV |
| **Video** | MKV, MP4, AVI |
| **Subtítulos** | SRT, ASS, VTT |

### Traducción de subtítulos

| Par | Disponibilidad |
|-----|---------------|
| English → Español | ✅ Modelo directo |
| Español → English | ✅ Modelo directo |
| English → Français | ✅ Modelo directo |
| Français → English | ✅ Modelo directo |
| English → Português | ✅ Modelo directo |
| Português → English | ✅ Modelo directo |
| Português → Español | ↻ Via triangular (pt→en→es) |
| Español → Português | ↻ Via triangular (es→en→pt) |
| Francés → Español | ↻ Via triangular (fr→en→es) |

---

## Dependencias

| Dependencia | Versión mínima | Uso |
|-------------|----------------|-----|
| Qt5 Widgets | 5.12 | Interfaz gráfica |
| FFmpeg      | 4.0  | Motor de extracción de pistas |
| ffprobe     | 4.0  | Análisis de streams y metadatos |
| CMake       | 3.5  | Sistema de compilación |
| C++ compiler| C++17 | GCC ≥ 7 / Clang ≥ 5 |

> **Nota sobre versiones de ffmpeg**: Versión 4.x+ recomendada para mejor soporte de subtítulos PGS (Blu-ray) y códecs modernos (AV1, Opus). Cualquier ffmpeg de los últimos 3 años funciona correctamente.

### Instalar dependencias en Debian/Ubuntu

```bash
sudo apt install qt5-default qtbase5-dev cmake build-essential ffmpeg
```

### Instalar dependencias en Fedora

```bash
sudo dnf install qt5-qtbase-devel cmake gcc-c++ ffmpeg
```

### Instalar dependencias en Arch Linux

```bash
sudo pacman -S qt5-base cmake base-devel ffmpeg
```

---

## Compilar e instalar

```bash
# Clonar el repositorio
git clone https://github.com/pdmiguel90-droid/elements-extract.git
cd elements-extract

# Crear directorio de build
mkdir build && cd build

# Configurar y compilar
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Ejecutar directamente
./elements-extract
```

### Instalación en el sistema (opcional)

```bash
# Copiar el binario
sudo cp build/elements-extract /usr/local/bin/elements-extract

# Registrar en el menú de aplicaciones
cp elements-extract.desktop ~/.local/share/applications/
```

### Paquete Debian (.deb)

```bash
cd build
cpack -G DEB
sudo dpkg -i elements-extract-1.2.0-Linux.deb
```

---

## Uso

### Extraer pistas

1. **Añadir archivos**: Haz clic en *Añadir* o arrastra archivos a la lista (`Ctrl+O`)
2. **Seleccionar pistas**: Haz clic en un archivo → marca las pistas a extraer con los checkboxes
3. **Selección global**: Usa los botones *VIDEO / AUDIO / SUBS* para seleccionar un tipo en todos los archivos del lote
4. **Formato de salida**: Elige el contenedor deseado en la sección *Formato de Salida*
5. **Destino**: Selecciona la carpeta de salida
6. **Extraer**: Pulsa *INICIAR EXTRACCIÓN* o `Ctrl+Enter`

### Traducir subtítulos

1. Selecciona el **idioma destino** (es, en, fr, pt)
2. Marca la casilla **«Traducir al extraer»**
3. Extrae normalmente — los subtítulos traducidos se guardan como `archivo.es.srt`
4. Si no hay modelo directo (ej. pt→es), se usa **traducción triangular** automáticamente

### Gestión de modelos

1. Ve a **Ajustes → Configuración y Modelos** (`Ctrl+,`)
2. En la pestaña **Modelos de Traducción**:
   - **Instalados**: modelos descargados — doble clic para activar/desactivar
   - **Online**: modelos disponibles — doble clic para descargar
3. El diálogo muestra qué pares tienen modelo directo (✓) y cuáles requieren traducción triangular (↻)

### Atajos de teclado

| Atajo | Acción |
|-------|--------|
| `Ctrl+O` | Añadir archivos |
| `Del` | Quitar archivos seleccionados |
| `Ctrl+Enter` | Iniciar extracción |
| `Escape` | Cancelar extracción |
| `F1` | Manual de uso |
| `Ctrl+Q` | Salir |

---

## Contribuir

1. Haz un fork del repositorio
2. Crea una rama: `git checkout -b feature/mi-mejora`
3. Realiza tus cambios y haz commit: `git commit -m 'feat: descripción'`
4. Sube la rama: `git push origin feature/mi-mejora`
5. Abre un Pull Request

---

## Licencia

Este proyecto está licenciado bajo la **GNU General Public License v3.0**.
Ver el archivo [LICENSE](LICENSE) para más detalles.

---

## Créditos

Desarrollado por **pdmiguel90-droid**  
Construido con [Qt5](https://www.qt.io/), [FFmpeg](https://ffmpeg.org/) y C++17.