# Flatpak / Flathub — notas de publicación

Manifiesto borrador: [`io.github.pdmiguel90droid.ElementsExtract.yml`](io.github.pdmiguel90droid.ElementsExtract.yml)
(`app-id: io.github.pdmiguel90droid.ElementsExtract`, runtime `org.kde.Platform//6.8`).

## Probar en local

```bash
flatpak-builder --user --install --force-clean build-dir \
  flatpak/io.github.pdmiguel90droid.ElementsExtract.yml
flatpak run io.github.pdmiguel90droid.ElementsExtract
```

## Pendientes antes de enviar a Flathub

1. **Fijar tag** en el módulo `elements-extract` (`branch: main` → `tag: vX.Y.Z`).
2. **Motor bergamot**: el módulo `extra-data` ya está esbozado (comentado) en
   el manifiesto. Para activarlo:
   `git tag bergamot-v0.4.5-1 && git push origin bergamot-v0.4.5-1`
   → el workflow publica `bergamot-linux64.tar.gz` (+ `.sha256`) en el Release;
   copiar el sha256 y el tamaño en bytes al bloque, descomentarlo y probar con
   `flatpak-builder --run`. Sin motor, la app extrae y hace remux, pero no traduce.
3. **Modelos**: los 6 `*-tiny` pueden viajar dentro del flatpak
   (`/app/share/elements-extract/models`, solo lectura). Las descargas van a
   `~/.var/app/…/data` (el código ya prefiere dir escribible). Opcional:
   más modelos como `extra-data`.
4. **Metainfo**: rellenar `<update_contact>` con email real
   (Flathub lo pide para avisos de compilación).
5. **Enviar**: fork de `flathub` + repo `flathub/io.github.pdmiguel90droid.ElementsExtract`
   con este manifiesto (renombrado sin la carpeta `flatpak/`), pasar
   `flatpak-builder --lint` y abrir PR. Guía: https://docs.flathub.org/docs/for-app-authors/submission/

## AppImageHub

1. El AppImage ya lleva `update-info` (`gh-releases-zsync…`) y el `.zsync`
   se genera al compilar (`tools/build-appimage.sh`); los Releases con tag
   `v*` publican ambos.
2. Abrir PR a [appimage.github.io](https://github.com/AppImage/appimage.github.io)
   con la entrada de la app (nombre, `appid: io.github.pdmiguel90droid.ElementsExtract`,
   enlaces al Release, capturas de `screenshots/`). Guía:
   https://appimage.github.io/docs/submit/
