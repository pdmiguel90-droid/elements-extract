#!/usr/bin/env python3
"""Clona el catálogo de modelos bergamot a un directorio local y genera un
`models.json` propio, independiente de Mozilla / data.statmt.org.

Uso típico:
  1. Bajar los pares que uses (o todos):
       tools/mirror-models.py --out models-mirror --all
  2. Publicar esa carpeta en tu host (o dejarla local), y apuntar la app:
       ELEMENTS_EXTRACT_MODEL_REGISTRY="https://tu-host/models.json" ./elements-extract
       (o, en local: ELEMENTS_EXTRACT_MODEL_REGISTRY="file:///ruta/models-mirror/models.json")

Comportamiento:
  - Descarga cada tar.gz solo si falta o si el SHA-256 no coincide (resume).
  - Verifica siempre el checksum SHA-256 del registro.
  - Escribe models.json con el MISMO esquema que parsea la app
    (shortName, modelName, srcTags, trgTag, type, url, checksum, size, version).
  - Con --dry-run solo genera models.json sin descargar.

Sugerencia de "periodicidad": ejecútalo junto a tools/check-model-updates.py en una
CI semanal; cuando detecte bump de versión, regenera el mirror (nunca sobrescribes
la versión antigua si quieres poder volver atrás: usa --out con la versión).
"""

import argparse
import hashlib
import json
import os
import ssl
import sys
import urllib.request

if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")

DEFAULT_REGISTRY = "https://translatelocally.com/models.json"

HDRS = {"User-Agent": "elements-extract-mirror/1.0"}


def fetch_json(url, timeout=30):
    req = urllib.request.Request(url, headers=HDRS)
    with urllib.request.urlopen(req, timeout=timeout) as resp:
        return json.loads(resp.read().decode("utf-8"))


def sha256_of(path, chunk=1 << 20):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        while True:
            b = f.read(chunk)
            if not b:
                break
            h.update(b)
    return h.hexdigest()


def download(url, dest, timeout=120):
    req = urllib.request.Request(url, headers=HDRS)
    tmp = dest + ".part"
    try:
        with urllib.request.urlopen(req, timeout=timeout) as resp, open(tmp, "wb") as out:
            total = resp.headers.get("Content-Length")
            done = 0
            while True:
                b = resp.read(1 << 20)
                if not b:
                    break
                out.write(b)
                done += len(b)
                if total:
                    pct = done * 100 / int(total)
                    sys.stdout.write(f"\r  {dest.split('/')[-1]}  {pct:5.1f}%  ({done}/{total} B)")
                    sys.stdout.flush()
            sys.stdout.write("\n")
        os.replace(tmp, dest)
    except Exception:
        if os.path.exists(tmp):
            os.remove(tmp)
        raise


def main():
    ap = argparse.ArgumentParser(description="Crea/espeja un mirror del catálogo de modelos bergamot.")
    ap.add_argument("--registry", default=DEFAULT_REGISTRY)
    ap.add_argument("--out", default="models-mirror", help="Directorio de salida del mirror")
    ap.add_argument("--models", default="", help="Lista de ids separados por coma (ej. en-es-tiny,es-en-tiny). Vacío = todos")
    ap.add_argument("--all", action="store_true", help="Mirror de TODOS los modelos del catálogo")
    ap.add_argument("--base-url", help="URL base pública del mirror (para models.json). Si va vacío se usa file:// con ruta absoluta")
    ap.add_argument("--dry-run", action="store_true", help="Solo genera models.json, no descarga")
    ap.add_argument("--force", action="store_true", help="Volver a descargar aunque el archivo ya exista y verifique sha256")
    args = ap.parse_args()

    raw = fetch_json(args.registry)
    if isinstance(raw, dict):
        raw = raw.get("models", [])

    wanted = set(args.models.split(",")) if args.models else None
    entries = [m for m in raw if wanted is None or m.get("shortName") in wanted]

    os.makedirs(args.out, exist_ok=True)

    if not args.base_url:
        if args.dry_run:
            base = f"file://{os.path.abspath(args.out)}"
        else:
            print("AVISO: --base-url vacío: usaré file:// a la ruta local del mirror.")
            base = f"file://{os.path.abspath(args.out)}"
    else:
        base = args.base_url.rstrip("/")

    ok = download_bad = missing_bad = skipped = 0
    for m in sorted(entries, key=lambda x: x.get("shortName", "")):
        mid = m.get("shortName")
        dest = os.path.join(args.out, mid + ".tar.gz")
        local_ok = False
        if not args.dry_run:
            if os.path.exists(dest) and not args.force:
                if sha256_of(dest) == m.get("checksum"):
                    local_ok = True
                else:
                    print(f"  [corrupto] {mid}: checksum no coincide -> redescarga")
            if not local_ok:
                try:
                    download(m.get("url"), dest)
                    sha = sha256_of(dest)
                    if sha != m.get("checksum"):
                        print(f"  [ERROR checksum] {mid}: {sha} != {m.get('checksum')}")
                        os.remove(dest)
                        download_bad += 1
                        continue
                    ok += 1
                    print(f"  [OK] {mid} -> {dest}")
                except Exception as e:
                    print(f"  [ERROR descarga] {mid}: {e}")
                    download_bad += 1
                    continue
            else:
                skipped += 1

    emitted = []
    for m in entries:
        e = dict(m)
        e["url"] = f"{base}/{m.get('shortName')}.tar.gz"
        emitted.append(e)

    with open(os.path.join(args.out, "models.json"), "w", encoding="utf-8") as f:
        json.dump({"models": emitted}, f, indent=2, ensure_ascii=False)

    if args.dry_run:
        print(f"\n[dry-run] models.json generado para {len(entries)} modelos (sin descargar).")
    else:
        print(f"\nResumen: {ok} descargados, {skipped} ya válidos, {download_bad} con error.")
    print(f"Registro del mirror: {os.path.join(args.out, 'models.json')}")
    print(f"URL para la app: {base}/models.json")
    return 1 if download_bad else 0


if __name__ == "__main__":
    sys.exit(main())