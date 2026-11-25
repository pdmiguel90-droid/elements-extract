# Metadatos del repositorio en GitHub

Estos datos no se pueden cambiar desde el código: hay que aplicarlos en
**Settings → General** del repo o con `gh` (el usuario hará el push manual y
también puede ejecutar estos comandos cuando quiera).

- **Repository name**: `elements-extract`
- **Description**: `Extract audio, video and subtitle tracks in batch; translate subtitles fully offline with bundled Bergamot models, search OpenSubtitles optionally and remux translations without re-encoding.`
- **Website**: `https://appimage.github.io/Elements_Extract/`
- **Topics**: `qt6`, `ffmpeg`, `cpp`, `desktop-application`, `subtitles`, `subtitle-translation`, `offline`, `bergamot`, `opensubtitles`, `ocr`, `remux`, `appimage`, `linux`, `windows`, `gpl-3`
- **About → Include in the home page**: marcado (README en inglés disponible como `README.en.md`).

Comandos (requieren `gh auth login` con permisos de escritura):

```bash
gh repo edit pdmiguel90-droid/elements-extract \
  --description "Extract audio, video and subtitle tracks in batch; translate subtitles fully offline with bundled Bergamot models, search OpenSubtitles optionally and remux translations without re-encoding." \
  --homepage "https://appimage.github.io/Elements_Extract/" \
  --add-topic qt6 --add-topic ffmpeg --add-topic cpp \
  --add-topic desktop-application --add-topic subtitles \
  --add-topic subtitle-translation --add-topic offline \
  --add-topic bergamot --add-topic opensubtitles --add-topic ocr \
  --add-topic remux --add-topic appimage --add-topic linux \
  --add-topic windows --add-topic gpl-3
```

Tras aplicarlos, el feed de AppImageHub
(`https://appimage.github.io/feed.json`) mostrará la descripción y el enlace
correctos. No hace falta tocar el PR #10408 salvo para comentar `/retest`
después de publicar la release v1.2.13.
