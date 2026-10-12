#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
ROOT="$PWD"
CACHE="$ROOT/tools/.cache"
APPDIR="$CACHE/AppDir"

VER="$(grep -oP 'project\(ElementsExtract VERSION \K[0-9.]+' CMakeLists.txt)"
echo "[*] version $VER"

if ! command -v qmake6 >/dev/null 2>&1; then
    echo "ERROR: Qt6 no disponible (falta qmake6). Aborto para no empaquetar Qt5 con plugins Qt6."
    exit 1
fi
QMAKE=qmake6

QT6_CMAKE="/usr/lib/x86_64-linux-gnu/cmake/Qt6"
EXTRA=""
if [ -d "$QT6_CMAKE" ]; then
    EXTRA="-DQt6_DIR=$QT6_CMAKE"
fi
echo "[*] cmake Qt6 hint: ${EXTRA:-<detección del sistema>}"

cmake -S . -B build-appimage -DCMAKE_BUILD_TYPE=Release $EXTRA
cmake --build build-appimage -j"$(nproc)"
cmake --install build-appimage --prefix "$APPDIR/usr"

QTPLUGINS="$("$QMAKE" -query QT_INSTALL_PLUGINS)"

mkdir -p "$APPDIR/usr/lib/elements-extract"
for sub in platforms imageformats iconengines platforminputcontexts xcbglintegrations; do
    [ -d "$QTPLUGINS/$sub" ] && cp -r "$QTPLUGINS/$sub" "$APPDIR/usr/lib/elements-extract/"
done
[ -f "$APPDIR/usr/share/elements-extract/bin/libopenblas.so.0" ] && cp "$APPDIR/usr/share/elements-extract/bin/libopenblas.so.0" "$APPDIR/usr/lib/"
printf '[Paths]\nPlugins = ../lib/elements-extract\n' > "$APPDIR/usr/bin/qt.conf"

cp io.github.pdmiguel90droid.ElementsExtract.desktop "$APPDIR/"
cp ffmpeg-extractor.svg "$APPDIR/io.github.pdmiguel90droid.ElementsExtract.svg"
# Nombres legacy de builds anteriores: duplicarían el lanzador y el icono
rm -f "$APPDIR/ffmpeg-extractor.desktop" "$APPDIR/ffmpeg-extractor.svg" \
      "$APPDIR/usr/share/applications/ffmpeg-extractor.desktop" \
      "$APPDIR/usr/share/icons/hicolor/scalable/apps/ffmpeg-extractor.svg"

mkdir -p "$CACHE"
for t in linuxdeploy-x86_64.AppImage appimagetool-x86_64.AppImage; do
    if [ ! -x "$CACHE/$t" ]; then
        if [ "$t" = "linuxdeploy-x86_64.AppImage" ]; then
            REPO="linuxdeploy/linuxdeploy"
        else
            REPO="AppImage/appimagetool"
        fi
        curl -fL "https://github.com/$REPO/releases/download/continuous/$t" -o "$CACHE/$t"
        chmod +x "$CACHE/$t"
    fi
done

export APPIMAGE_EXTRACT_AND_RUN=1
export PATH="$CACHE:$PATH"
OUT="$ROOT/elements-extract-$VER-x86_64.AppImage"
rm -f "$OUT"
"$CACHE/linuxdeploy-x86_64.AppImage" \
    --appdir "$APPDIR" \
    -e "$APPDIR/usr/bin/elements-extract" \
    -d "$APPDIR/io.github.pdmiguel90droid.ElementsExtract.desktop" \
    -i "$APPDIR/io.github.pdmiguel90droid.ElementsExtract.svg"

cat > "$APPDIR/.AppRun.new" <<EOF
#!/bin/bash
HERE="\$(dirname "\$(readlink -f "\$0")")"
export QT_PLUGIN_PATH="\$HERE/usr/lib/elements-extract"
export LD_LIBRARY_PATH="\$HERE/usr/lib\${LD_LIBRARY_PATH:+:\$LD_LIBRARY_PATH}"
exec "\$HERE/usr/bin/elements-extract" "\$@"
EOF
chmod +x "$APPDIR/.AppRun.new"
mv -f "$APPDIR/.AppRun.new" "$APPDIR/AppRun"

# Update-info para AppImageHub y actualizaciones delta (zsync). El .zsync
# se genera junto al AppImage en los Releases (tags v*).
UPDATE_INFO="${UPDATE_INFO:-gh-releases-zsync|pdmiguel90-droid|elements-extract|latest|elements-extract-*x86_64.AppImage.zsync}"
if [ -n "$UPDATE_INFO" ]; then
    "$CACHE/appimagetool-x86_64.AppImage" -u "$UPDATE_INFO" "$APPDIR" "$OUT"
else
    "$CACHE/appimagetool-x86_64.AppImage" "$APPDIR" "$OUT"
fi
sha256sum "$OUT"
echo "[*] AppImage: $OUT"