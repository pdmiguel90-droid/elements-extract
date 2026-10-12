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

## Checklist verificado en este repo (v1.2.0)

- [x] AppImage tipo 2 autónomo, construido en Ubuntu 22.04 (glibc 2.35,
      LTS más antiguo con soporte) → `error-glibc-too-new` OK.
- [x] Arranca sin red (el catálogo de modelos es async; sin conexión solo
      avisa, no bloquea) → `error-app-exits` OK.
- [x] Interfaz en inglés con locale C (fallback `C/POSIX → en` en
      `main.cpp` + `changeUILanguage`; verificado con `LC_ALL=C`).
- [x] Ventana principal visible < 30 s (probado).
- [x] `desktop-file-validate` limpio.
- [x] Metainfo AppStream embebido (`usr/share/metainfo/*.metainfo.xml`),
      `appstreamcli` limpio salvo URLs que requieren push.
- [x] `appdir-lint.sh` sin fatales (el aviso `*appdata.xml` es de su versión
      vieja: Flathub exige `*.metainfo.xml`, que es lo que llevamos).
- [x] `update-info` zsync embebido + `.zsync` publicado en cada Release `v*`.
- [x] `README.en.md` presente (el repo es ES-primero con README en inglés).
