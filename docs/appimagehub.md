# AppImageHub — envío al catálogo

El catálogo se alimenta solo con la URL del AppImage: el bot lo inspecciona
y publica la ficha (no hay que redactar nada más).

## Alta (lo hace el mantenedor en github.com/AppImage/appimage.github.io)

1. Fork + nuevo PR que agregue **un solo fichero** `data/Elements_Extract`
   (sin extensión) con **una sola línea**:
   `https://github.com/pdmiguel90-droid/elements-extract`
   El bot toma el AppImage del Release más nuevo (`*-x86_64.AppImage`).
2. El bot comenta el resultado: captura si pasa, error con pista si falla.
   Re-test con el comentario `/retest`.

## Checklist verificado en este repo (v1.2.9)

- [x] AppImage tipo 2 autónomo, construido en Ubuntu 22.04 (glibc 2.35,
      LTS más antiguo con soporte) → `error-glibc-too-new` OK.
- [x] Arranca sin red (el catálogo de modelos es async; sin conexión solo
      avisa, no bloquea) → `error-app-exits` OK.
- [x] Interfaz en inglés con locale C (fallback `C/POSIX → en` en
      `main.cpp` + `changeUILanguage`; verificado con `LC_ALL=C`).
- [x] Ventana principal visible < 30 s (probado).
- [x] Capturas en inglés (hechas con `LC_ALL=C` y `language=en`) en `screenshots/`; AppImageHub exige capturas en inglés.
- [x] `desktop-file-validate` limpio.
- [x] Metainfo AppStream embebido (`usr/share/metainfo/*.metainfo.xml`),
      `appstreamcli` limpio salvo URLs que requieren push.
- [x] `appdir-lint.sh` sin fatales (el aviso `*appdata.xml` es de su versión
      vieja: Flathub exige `*.metainfo.xml`, que es lo que llevamos).
- [x] `update-info` zsync embebido + `.zsync` publicado en cada Release `v*`.
- [x] `README.en.md` presente (el repo es ES-primero con README en inglés).

## Tareas al publicar cada versión

1. Subir el tag y los artefactos del Release (AppImage, `.zsync`, SHA256SUMS).
2. Actualizar `<release>` del metainfo, `CHANGELOG.md` y las capturas si cambió la UI.
3. Comentar `/retest` en el PR #10408 de `AppImage/appimage.github.io` para que
   el bot vuelva a leer el AppImage más reciente y regenere la ficha. El catálogo
   oficial es `https://appimage.github.io/Elements_Extract/` (el dominio
   `www.appimagehub.com` no es el destino de ese PR).
4. Mantener los metadatos del repo al día (descripción, topics y README en inglés)
   para que el feed `https://appimage.github.io/feed.json` muestre la ficha correcta.
