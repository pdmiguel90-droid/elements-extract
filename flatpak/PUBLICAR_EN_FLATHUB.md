# Guía de Publicación en Flathub

Pasos detallados para publicar **Elements Extract FFmpeg** en [Flathub](https://flathub.org).

---

## Requisitos previos

1. **Repositorio público** en GitHub/GitLab con una tag de versión (`v1.0.0`)
2. **App-ID único** en formato dominio inverso: `io.github.pdmiguel90droid.FFmpegExtractor`
3. **Archivo `.metainfo.xml`** válido (AppStream) — ya incluido en `flatpak/`
4. **Archivo `.desktop`** — ya creado en la raíz del proyecto

---

## Paso 1 — Instalar las herramientas de Flatpak

```bash
# Debian/Ubuntu
sudo apt install flatpak flatpak-builder

# Fedora
sudo dnf install flatpak flatpak-builder

# Arch
sudo pacman -S flatpak flatpak-builder
```

Añadir el repositorio Flathub:
```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
```

---

## Paso 2 — Instalar el SDK de KDE

```bash
flatpak install flathub org.kde.Sdk//5.15
flatpak install flathub org.kde.Platform//5.15
```

---

## Paso 3 — Build local para pruebas

```bash
cd elements-extract/flatpak/

# Compilar el paquete localmente
flatpak-builder --force-clean build-dir io.github.pdmiguel90droid.FFmpegExtractor.yml

# Instalar y probar localmente
flatpak-builder --user --install --force-clean build-dir io.github.pdmiguel90droid.FFmpegExtractor.yml

# Ejecutar
flatpak run io.github.pdmiguel90droid.FFmpegExtractor
```

---

## Paso 4 — Validar antes de publicar

```bash
# Validar el metainfo XML (AppStream)
flatpak run --command=appstreamcli org.freedesktop.appstream-glib validate \
    flatpak/io.github.pdmiguel90droid.FFmpegExtractor.metainfo.xml

# Verificar el .desktop
desktop-file-validate ffmpeg-extractor.desktop
```

---

## Paso 5 — Publicar en Flathub

1. **Hacer fork** de [https://github.com/flathub/flathub](https://github.com/flathub/flathub)

2. **Crear la rama** con el nombre de tu app:
   ```bash
   git checkout -b new-app
   mkdir io.github.pdmiguel90droid.FFmpegExtractor
   cp flatpak/io.github.pdmiguel90droid.FFmpegExtractor.yml io.github.pdmiguel90droid.FFmpegExtractor/
   ```

3. **Abrir un Pull Request** en el repositorio de Flathub  
   Título: `Add io.github.pdmiguel90droid.FFmpegExtractor`

4. **Esperar la revisión** del equipo de Flathub (~1-2 semanas)  
   El equipo verifica: seguridad del sandbox, metadatos, licencia, calidad del código.

5. Una vez aprobado, la app aparece en [https://flathub.org](https://flathub.org) y los usuarios pueden instalarla con:
   ```bash
   flatpak install flathub io.github.pdmiguel90droid.FFmpegExtractor
   ```

---

## Paso 6 — Publicar actualizaciones futuras

Cuando publiques una nueva versión (`v1.1.0`):

1. Actualizar el `tag` y el `sha256` en el manifiesto `.yml`
2. Actualizar el `<release>` en el `.metainfo.xml`
3. Abrir un PR en el repositorio `flathub/io.github.pdmiguel90droid.FFmpegExtractor`

---

## Notas importantes sobre el sandbox

El manifiesto ya incluye los permisos mínimos necesarios:
- `--filesystem=home` → para acceder a los archivos del usuario
- `--socket=wayland` + `--socket=x11` → para mostrarse en el escritorio
- `--talk-name=org.freedesktop.portal.Desktop` → para "Abrir carpeta"

> **⚠️ Evitar** usar `--filesystem=host` o `--share=network` a menos que sea estrictamente necesario — el equipo de Flathub lo rechazará.

---

## Guía de publicación en Debian (alternativa)

Para crear un paquete `.deb` nativo:

```bash
# Instalar herramientas
sudo apt install devscripts debhelper

# Estructura esperada:
elements-extract-1.0.0/
├── debian/
│   ├── control       # metadatos del paquete
│   ├── rules         # instrucciones de build
│   ├── changelog     # historial de versiones (formato específico)
│   ├── copyright     # licencia
│   └── install       # qué archivos instalar y dónde
└── [código fuente]

# Compilar el .deb
dpkg-buildpackage -us -uc -b
```

Para publicar en Debian oficial, necesitas un "sponsor" dentro del proyecto.  
Alternativa más rápida: publicar en un **PPA de Launchpad** (Ubuntu).
