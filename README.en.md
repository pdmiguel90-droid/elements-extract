# Elements Extract — FFmpeg

> You just downloaded a movie or a series… and it's missing the one thing it should have: **the subtitle in your language**.

**Elements Extract** is a graphical track extractor for audio, video and subtitles with batch processing that also **translates subtitles into your language 100 % offline**, right on your own machine.

![License: GPL-3.0](https://img.shields.io/badge/License-GPL%20v3-blue.svg)
![Qt6](https://img.shields.io/badge/Qt-6.x-green.svg)
![Platform](https://img.shields.io/badge/Platform-Linux-orange.svg)
![Build](https://img.shields.io/badge/Build-CMake-lightgrey.svg)

---

## 🎯 Why this project exists

This project started from a personal need. When I download a movie or a series, sometimes this happens:

- The subtitle I need is **not available** or doesn't exist in my language.
- The only one I can find is **badly synced** or poor quality.
- I don't want to depend on any website to search for it or to "get" it.

So I thought: *what if I could take the file I already have on disk and translate its subtitle myself, at home, without internet?*

**Elements Extract** is exactly that: an **offline** tool that extracts the tracks from your file and **translates the subtitles into the language you want using your own computer**, with no need to rely on the internet to find them.

> 🔒 **Privacy:** your files never leave your machine. Translation runs locally using neural translation models (via [bergamot-translator](https://github.com/browsermt/bergamot-translator)). You only need internet **once**, to download the language model; after that you can translate as much as you want, wherever you are.

---

## Screenshots

*(pending)*

---

## Features

- 📂 **Batch processing** — add multiple files at once
- 🎯 **Per-file track selection** — choose exactly which tracks to extract from each file
- ▶ **Global selection by type** — VIDEO / AUDIO / SUBS buttons select every track of a type across the whole batch
- 📊 **Metadata panel** — shows duration, size, resolution, number of tracks when you select a file
- 🔄 **Configurable output formats** — choose MKA, M4A, AAC, MP3, FLAC, OPUS, WAV for audio; MKV, MP4, AVI for video; SRT, ASS, VTT for subtitles
- 📈 **Real progress bar** — percentage computed from file duration
- ✋ **Cancellation** — cancel the job at any time
- 💾 **Persistence** — remembers output folder, formats and window size between sessions
- 🖱️ **Drag & Drop** — drop files straight onto the window
- ⌨️ **Keyboard shortcuts** — `Ctrl+O`, `Del`, `Ctrl+Enter`, `Escape`, `F1`
- 🌐 **Offline subtitle translation** — translate extracted subtitles to the language you want (en, es, fr, pt)
- ➡️ **Mux the translation (remux)** — the translated subtitle is embedded into a new .mkv/.mp4 with the original video and audio, without re-encoding (MKVToolNix style)
- 🎯 **Subtitle language filter** — select and translate only the subtitles of a given language across the whole batch
- 🔄 **Triangular translation** — when no direct model exists (e.g. pt→es), it uses intermediates (pt→en→es)
- 🤖 **Automatic language detection** — detects subtitle language from file metadata
- ⬇️ **Downloadable models** — download translation models from translatelocally.com (once only)

---

## Supported formats

### Input containers

| Format | Video tracks | Audio tracks | Subtitles | Notes |
|---------|----------------|-----------------|------------|-------|
| **MKV** (Matroska) | Multiple | Multiple | SRT, ASS, PGS, DVB | Most complete — supports almost everything |
| **MP4** | 1-2 | Multiple | mov_text, cea-608 | Web standard — universal compatibility |
| **AVI** | 1 | 1-2 | No subtitles | Legacy format |
| **TS/M2TS** | Multiple | Multiple | DVB, PGS | Blu-ray and broadcast |
| **OGG/OGM** | Multiple | Multiple | SRT | Open source |
| **FLV** | 1 | 1-2 | No subtitles | Flash (obsolete) |
| **WMV** | 1 | 1-2 | SRT | Windows Media |

### Supported codecs (via ffmpeg)

- **Video**: H.264, H.265/HEVC, VP8, VP9, AV1, MPEG-2, MPEG-4, Theora
- **Audio**: AAC, MP3, FLAC, Opus, Vorbis, AC3, DTS, PCM, ALAC
- **Subtitles**: SRT, ASS/SSA, PGS (Blu-ray), DVB, WebVTT, mov_text

### Output formats

| Type | Formats |
|------|----------|
| **Audio** | MKA, M4A, AAC, MP3, FLAC, OPUS, WAV |
| **Video** | MKV, MP4, AVI |
| **Subtitles** | SRT, ASS, VTT |

### Subtitle translation

| Pair | Availability |
|-----|---------------|
| English → Español | ✅ Direct model |
| Español → English | ✅ Direct model |
| English → Français | ✅ Direct model |
| Français → English | ✅ Direct model |
| English → Português | ✅ Direct model |
| Português → English | ✅ Direct model |
| Português → Español | ↻ Triangular (pt→en→es) |
| Español → Português | ↻ Triangular (es→en→pt) |
| Francés → Español | ↻ Triangular (fr→en→es) |

---

## Dependencies

| Dependency | Minimum version | Used for |
|-------------|----------------|-----|
| Qt6 Widgets | 6.2 | Graphical interface |
| FFmpeg      | 4.0  | Track extraction engine |
| ffprobe     | 4.0  | Stream and metadata analysis |
| CMake       | 3.5  | Build system |
| C++ compiler| C++17 | GCC ≥ 7 / Clang ≥ 5 |

> **Note on ffmpeg versions**: 4.x+ recommended for better PGS (Blu-ray) subtitle support and modern codecs (AV1, Opus). Any ffmpeg from the last 3 years works fine.

### Install dependencies on Debian/Ubuntu

```bash
sudo apt install qt6-base-dev libgl1-mesa-dev cmake build-essential ffmpeg
```

### Install dependencies on Fedora

```bash
sudo dnf install qt6-qtbase-devel cmake gcc-c++ ffmpeg
```

### Install dependencies on Arch Linux

```bash
sudo pacman -S qt6-base cmake base-devel ffmpeg
```

---

## Build and install

```bash
# Clone the repository
git clone https://github.com/pdmiguel90-droid/elements-extract.git
cd elements-extract

# Create the build directory
mkdir build && cd build

# Configure and build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run directly
./elements-extract
```

### System installation (optional)

```bash
# Copy the binary
sudo cp build/elements-extract /usr/local/bin/elements-extract

# Register in the applications menu
cp elements-extract.desktop ~/.local/share/applications/
```

### Debian package (.deb)

```bash
cd build
cpack -G DEB
sudo dpkg -i elements-extract-1.2.0-Linux.deb
```

---

## Usage

### Extract tracks

1. **Add files**: click *Add* or drag files onto the list (`Ctrl+O`)
2. **Select tracks**: click a file → tick the tracks to extract
3. **Global selection**: use the *VIDEO / AUDIO / SUBS* buttons to select a type across all files in the batch
4. **Output format**: choose the desired container in the *Output Format* section
5. **Destination**: pick the output folder
6. **Extract**: press *START EXTRACTION* or `Ctrl+Enter`

### Translate subtitles

1. Select the **target language** (en, es, fr, pt)
2. Tick **"Translate when extracting"**
3. Extract as usual — the translated subtitles are saved as `file.en.srt`
4. If no direct model exists (e.g. pt→es), **triangular translation** is used automatically

> 💡 **Work with a single language**: the **"Only subs"** dropdown selects the subtitles of a given language across all batch files at once (files without that language are skipped). While the filter is active, translation uses that language as source automatically.

> 🎬 **Embed the translation**: with **"Embed the translation (remux)"** ticked, a new `.mkv`/`.mp4` is produced (`file.<lang>.mkv`) with the original video and audio plus the translated subtitle track embedded (no re-encoding). Output goes to `Elements Extract/Remuxados`. Before embedding, each file is validated: files without a video track are skipped, and if the `.mp4` has codecs that can't be stream-copied (audio `pcm_*`, `vorbis`, `flac`, `dts`, `truehd`…), it automatically falls back to `.mkv`. A final summary shows how many were embedded/skipped and the reason for each failure.

### Model management

1. Go to **Settings → Settings and Models** (`Ctrl+,`)
2. In the **Translation Models** tab:
   - **Installed**: downloaded models — double-click to enable/disable
   - **Online**: available models — double-click to download
3. The dialog shows which pairs have a direct model (✓) and which need triangular translation (↻)

### Keyboard shortcuts

| Shortcut | Action |
|-------|--------|
| `Ctrl+O` | Add files |
| `Del` | Remove selected files |
| `Ctrl+Enter` | Start extraction |
| `Escape` | Cancel extraction |
| `F1` | User manual |
| `Ctrl+Q` | Quit |

---

## Contributing

1. Fork the repository
2. Create a branch: `git checkout -b feature/my-improvement`
3. Make your changes and commit: `git commit -m 'feat: description'`
4. Push the branch: `git push origin feature/my-improvement`
5. Open a Pull Request

---

## License

This project is licensed under the **GNU General Public License v3.0**.
See the [LICENSE](LICENSE) file for details.

---

## Credits

Developed by **pdmiguel90-droid**  
Built with [Qt6](https://www.qt.io/), [FFmpeg](https://ffmpeg.org/) and C++17.