# Elements Extract — FFmpeg

> Descargaste una película o una serie… y le falta algo: **el subtítulo en tu idioma**.
> **Translating subtitles for your movies & series, 100 % offline.**

[![Build Linux](https://github.com/pdmiguel90-droid/elements-extract/actions/workflows/build.yml/badge.svg)](https://github.com/pdmiguel90-droid/elements-extract/actions/workflows/build.yml)
[![Build Windows](https://github.com/pdmiguel90-droid/elements-extract/actions/workflows/windows.yml/badge.svg)](https://github.com/pdmiguel90-droid/elements-extract/actions/workflows/windows.yml)
[![AppImage](https://github.com/pdmiguel90-droid/elements-extract/actions/workflows/appimage.yml/badge.svg)](https://github.com/pdmiguel90-droid/elements-extract/actions/workflows/appimage.yml)
[![License: GPL-3.0-or-later](https://img.shields.io/badge/License-GPL--3.0--or--later-blue.svg)](LICENSE)
[![Qt6](https://img.shields.io/badge/Qt-6.x-green.svg)](https://www.qt.io/)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-orange.svg)

**Elements Extract** es un extractor gráfico de pistas (audio, vídeo, subtítulos) con traducción de subtítulos **totalmente offline**: tus archivos nunca salen de tu máquina (motor local [bergamot-translator](https://github.com/browsermt/bergamot-translator)).

| Ventana principal | Filtro de subtítulos por idioma |
|---|---|
| ![Ventana principal](screenshots/main-window.png) | ![Filtro Solo subs](screenshots/subtitle-filter.png) |

## 🌐 Idiomas / Languages / Idiomas

| Idioma | README |
|--------|--------|
| 🇪🇸 Español | [README.es.md](README.es.md) |
| 🇺🇸 English | [README.en.md](README.en.md) |
| 🇧🇷 Português | [README.pt.md](README.pt.md) |

La interfaz también habla inglés y portugués (detección automática del sistema).

## ✨ Qué hace

- 📼 **Extracción por lotes** de audio, vídeo y subtítulos (MKV, MP4, AVI…).
- 🌍 **Traducción offline** en↔es/fr/pt (+ triangular vía inglés) con modelos descargables.
- 🎯 **Filtro "Solo subs"** por idioma en todo el lote.
- 📦 **Remux**: un solo `.mkv`/`.mp4` con todas las pistas traducidas, sin re-codificar (repliegue MP4→MKV, limpieza opcional de intermedios).
- 🖥️ Qt6, tema oscuro/claro, español/inglés/portugués.

Detalles por idioma en los README localizados. Mantenimiento de modelos: [docs/modelos.md](docs/modelos.md).

## 📥 Instalar

- **Debian/Ubuntu**: `.deb` de [Releases](https://github.com/pdmiguel90-droid/elements-extract/releases) (`sudo dpkg -i elements-extract-*.deb`).
- **Cualquier distro**: AppImage de [Releases](https://github.com/pdmiguel90-droid/elements-extract/releases) (`chmod +x` y ejecutar).
- **Windows**: zip `*-win64.zip` de [Releases](https://github.com/pdmiguel90-droid/elements-extract/releases) (trae `ffmpeg.exe` + motor `bergamot.exe`).
- **Flatpak / Flathub**: en preparación (ver `flatpak/`).

Necesitas `ffmpeg` en el sistema (solo para `.deb`/compilado; AppImage y Windows lo traen).

## 🔨 Compilar

```bash
cmake -S . -B build-qt6 -DCMAKE_BUILD_TYPE=Release
cmake --build build-qt6 --parallel
./build-qt6/elements-extract
```

Ver [README.es.md](README.es.md#compilar-e-instalar) para dependencias por distro.

## 🤝 Contribuir y seguridad

- [CONTRIBUTING.md](CONTRIBUTING.md) · [SECURITY.md](SECURITY.md) · [CHANGELOG.md](CHANGELOG.md)

## 📄 Licencia

[GPL-3.0-or-later](LICENSE) © pdmiguel90-droid. Traducción local vía bergamot-translator (MPL-2.0) y modelos de [translatelocally.com](https://translatelocally.com).
