#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
ROOT="$PWD"
CACHE="$ROOT/tools/.cache"
APPDIR="$CACHE/AppDir"

VER="$(grep -oP 'project\(ElementsExtract VERSION \K[0-9.]+' CMakeLists.txt)"
echo "[*] version $VER"

if command -v qmake6 >/dev/null 2>&1; then QMAKE=qmake6; else QMAKE=qmake; fi
QTPLUGINS="$("$QMAKE" -query QT_INSTALL_PLUGINS)"

cmake -S . -B build-appimage -DCMAKE_BUILD_TYPE=Release >/dev/null
cmake --build build-appimage -j"$(nproc)" >/dev/null
cmake --install build-appimage --prefix "$APPDIR/usr"

mkdir -p "$APPDIR/usr/lib/elements-extract"
for sub in platforms imageformats iconengines platforminputcontexts xcbglintegrations; do
    [ -d "$QTPLUGINS/$sub" ] && cp -r "$QTPLUGINS/$sub" "$APPDIR/usr/lib/elements-extract/"
done
[ -f "$APPDIR/usr/share/elements-extract/bin/libopenblas.so.0" ] && cp "$APPDIR/usr/share/elements-extract/bin/libopenblas.so.0" "$APPDIR/usr/lib/"
printf '[Paths]\nPlugins = ../lib/elements-extract\n' > "$APPDIR/usr/bin/qt.conf"

cp ffmpeg-extractor.desktop "$APPDIR/"
cp ffmpeg-extractor.svg "$APPDIR/"

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
    -d "$APPDIR/ffmpeg-extractor.desktop" \
    -i "$APPDIR/ffmpeg-extractor.svg"

cat > "$APPDIR/.AppRun.new" <<EOF
#!/bin/bash
HERE="\$(dirname "\$(readlink -f "\$0")")"
export QT_PLUGIN_PATH="\$HERE/usr/lib/elements-extract"
export LD_LIBRARY_PATH="\$HERE/usr/lib\${LD_LIBRARY_PATH:+:\$LD_LIBRARY_PATH}"
exec "\$HERE/usr/bin/elements-extract" "\$@"
EOF
chmod +x "$APPDIR/.AppRun.new"
mv -f "$APPDIR/.AppRun.new" "$APPDIR/AppRun"

"$CACHE/appimagetool-x86_64.AppImage" "$APPDIR" "$OUT"
sha256sum "$OUT"
echo "[*] AppImage: $OUT"