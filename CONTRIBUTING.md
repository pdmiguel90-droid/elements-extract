# Contribuir / Contributing

¡Gracias por ayudar a mejorar Elements Extract! / Thanks for helping!

## Cómo contribuir

1. Abre un *issue* (error con log, o propuesta) antes de cambios grandes.
2. Haz *fork*, crea una rama (`feat/...`, `fix/...`, `docs/...`).
3. Abre un *pull request* contra `main` completando la plantilla.

## Reglas de código

- Qt6 + C++17, sin dependencias nuevas sin discutirlo antes.
- Los mensajes visibles al usuario van con `tr()` (español como fuente).
- Si agregas cadenas: actualiza `translations/en.ts` y `translations/pt.ts`
  (`lupdate . -ts translations/en.ts translations/pt.ts`, traducir,
  `lrelease` a los 4 `.qm`) — `es.ts` queda como fuente, sin tocar.
- Actualiza `CHANGELOG.md` si el cambio es visible al usuario.
- Sin secretos, rutas personales ni binarios en los commits
  (`.deb`, `.AppImage`, `build*/` están ignorados).

## Compilación rápida

```bash
cmake -S . -B build-qt6 -DCMAKE_BUILD_TYPE=Release
cmake --build build-qt6 --parallel
./build-qt6/elements-extract
```

## Reportar errores

Usa la plantilla de *bug report* e incluye siempre el bloque del panel
LOG (trae el comando `ffmpeg` exacto) y, si aplica, la salida de:

```bash
ffprobe -v quiet -show_entries stream=index,codec_name,codec_type:stream_tags=language -of flat ARCHIVO
```
