# Elements Extract FFmpeg

> Extractor gráfico de pistas de audio, video y subtítulos — procesamiento en lote con selección individual por archivo.

![License: GPL-3.0](https://img.shields.io/badge/License-GPL%20v3-blue.svg)
![Qt5](https://img.shields.io/badge/Qt-5.x-green.svg)
![Platform](https://img.shields.io/badge/Platform-Linux-orange.svg)
![Build](https://img.shields.io/badge/Build-CMake-lightgrey.svg)

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

---

## Dependencias

| Dependencia | Versión mínima | Uso |
|-------------|----------------|-----|
| Qt5 Widgets | 5.12 | Interfaz gráfica |
| FFmpeg      | 4.0  | Motor de extracción de pistas |
| ffprobe     | 4.0  | Análisis de streams y metadatos |
| CMake       | 3.5  | Sistema de compilación |
| C++ compiler| C++17 | GCC ≥ 7 / Clang ≥ 5 |

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
git clone https://github.com/tuusuario/ffmpeg-extractor.git
cd ffmpeg-extractor

# Crear directorio de build
mkdir build && cd build

# Configurar y compilar
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Ejecutar directamente
./FFmpegExtractor
```

### Instalación en el sistema (opcional)

```bash
# Copiar el binario
sudo cp build/FFmpegExtractor /usr/local/bin/ffmpeg-extractor

# Registrar en el menú de aplicaciones
cp ffmpeg-extractor.desktop ~/.local/share/applications/
```

---

## Uso

1. **Añadir archivos**: Haz clic en *Añadir* o arrastra archivos a la lista (`Ctrl+O`)
2. **Seleccionar pistas**: Haz clic en un archivo → marca las pistas a extraer con los checkboxes
3. **Selección global**: Usa los botones *VIDEO / AUDIO / SUBS* para seleccionar un tipo en todos los archivos del lote
4. **Formato de salida**: Elige el contenedor deseado en la sección *Formato de Salida*
5. **Destino**: Selecciona la carpeta de salida
6. **Extraer**: Pulsa *INICIAR EXTRACCIÓN* o `Ctrl+Enter`

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

## Distribución como Flatpak

Ver [`flatpak/`](flatpak/) para el manifiesto y las instrucciones de compilación y publicación en Flathub.

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

Desarrollado por **[Tu Nombre]**  
Construido con [Qt5](https://www.qt.io/), [FFmpeg](https://ffmpeg.org/) y C++17.
