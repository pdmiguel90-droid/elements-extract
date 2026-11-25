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
  - Los modelos ausentes del catálogo público (los dos pares `pt`) se toman de
    `bundled-models.json`, con URL y SHA-256 fijados, y se empaquetan con el
    mismo formato de tar.gz.
  - Si un id solicitado no existe en ninguna fuente, termina con error en vez
    de omitirlo silenciosamente.
  - Con --dry-run solo genera models.json sin descargar.

Sugerencia de "periodicidad": ejecútalo junto a tools/check-model-updates.py en una
CI semanal; cuando detecte bump de versión, regenera el mirror (nunca sobrescribes
la versión antigua si quieres poder volver atrás: usa --out con la versión).
"""

import argparse
import gzip
import hashlib
import io
import json
import os
import ssl
import sys
import tarfile
import urllib.request

if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")

DEFAULT_REGISTRY = "https://translatelocally.com/models.json"
EXTRA_REGISTRY = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                              "bundled-models.json")

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


def fetch_bytes(url, timeout=120):
    """Descarga un fichero suelto; GCS puede servirlo con gzip transparente."""
    req = urllib.request.Request(url, headers=HDRS)
    with urllib.request.urlopen(req, timeout=timeout) as resp:
        data = resp.read()
    if data[:2] == b"\x1f\x8b":
        data = gzip.decompress(data)
    return data


def build_bundled_tarball(m, dest):
    """Empaqueta un modelo del catálogo local con el layout que espera la app."""
    payload = {}
    for f in m["files"]:
        data = fetch_bytes(f["url"])
        sha = hashlib.sha256(data).hexdigest()
        if sha != f["sha256"] or (f.get("size") and len(data) != f["size"]):
            raise ValueError("checksum/tamaño incorrecto en %s: %s (%d B)" %
                             (f["name"], sha, len(data)))
        payload[f["name"]] = data
    payload["config.intgemm8bitalpha.yml"] = m["config"].encode("utf-8")
    payload["model_info.json"] = (
        json.dumps(m["model_info"], indent=4, ensure_ascii=False) + "\n"
    ).encode("utf-8")

    tmp = dest + ".part"
    with open(tmp, "wb") as out:
        with gzip.GzipFile(filename="", mode="wb", fileobj=out, mtime=0) as gz:
            with tarfile.open(fileobj=gz, mode="w") as tar:
                for name in sorted(payload):
                    info = tarfile.TarInfo("%s/%s" % (m["shortName"], name))
                    info.size = len(payload[name])
                    info.mtime = 0
                    info.mode = 0o644
                    info.uid = info.gid = 0
                    info.uname = info.gname = ""
                    tar.addfile(info, io.BytesIO(payload[name]))
    m["checksum"] = sha256_of(tmp)
    m["size"] = os.path.getsize(tmp)
    os.replace(tmp, dest)


def main():
    ap = argparse.ArgumentParser(description="Crea/espeja un mirror del catálogo de modelos bergamot.")
    ap.add_argument("--registry", default=DEFAULT_REGISTRY)
    ap.add_argument("--out", default="models-mirror", help="Directorio de salida del mirror")
    ap.add_argument("--models", default="", help="Lista de ids separados por coma (ej. en-es-tiny,es-en-tiny). Vacío = todos")
    ap.add_argument("--all", action="store_true", help="Mirror de TODOS los modelos del catálogo")
    ap.add_argument("--base-url", help="URL base pública del mirror (para models.json). Si va vacío se usa file:// con ruta absoluta")
    ap.add_argument("--dry-run", action="store_true", help="Solo genera models.json, no descarga")
    ap.add_argument("--force", action="store_true", help="Volver a descargar aunque el archivo ya exista y verifique sha256")
    ap.add_argument("--extra-registry", default=EXTRA_REGISTRY,
                    help="Catálogo local de modelos ausentes del registro público")
    args = ap.parse_args()

    raw = fetch_json(args.registry)
    if isinstance(raw, dict):
        raw = raw.get("models", [])

    wanted = set(x for x in args.models.split(",") if x) if args.models else None

    # Los pares pt no están en el catálogo público: se toman del catálogo local.
    extras = {}
    if args.extra_registry and os.path.exists(args.extra_registry):
        with open(args.extra_registry, encoding="utf-8") as f:
            extras = json.load(f)
        extras.pop("_comment", None)
    known = {m.get("shortName") for m in raw}
    for mid, m in extras.items():
        if mid not in known and (wanted is None or mid in wanted):
            raw.append(m)

    entries = [m for m in raw if wanted is None or m.get("shortName") in wanted]
    missing = (sorted(wanted - {m.get("shortName") for m in entries})
               if wanted is not None else [])
    for mid in missing:
        print(f"  [ERROR id] {mid}: no está en el registro ni en el catálogo local")

    os.makedirs(args.out, exist_ok=True)

    if not args.base_url:
        if args.dry_run:
            base = f"file://{os.path.abspath(args.out)}"
        else:
            print("AVISO: --base-url vacío: usaré file:// a la ruta local del mirror.")
            base = f"file://{os.path.abspath(args.out)}"
    else:
        base = args.base_url.rstrip("/")

    ok = download_bad = skipped = 0
    missing_bad = len(missing)
    for m in sorted(entries, key=lambda x: x.get("shortName", "")):
        mid = m.get("shortName")
        dest = os.path.join(args.out, mid + ".tar.gz")
        local_ok = False
        if not args.dry_run:
            if os.path.exists(dest) and not args.force and m.get("checksum"):
                if sha256_of(dest) == m["checksum"]:
                    local_ok = True
                else:
                    print(f"  [corrupto] {mid}: checksum no coincide -> redescarga")
            if not local_ok:
                try:
                    if m.get("files"):
                        build_bundled_tarball(m, dest)
                    else:
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
        for key in ("files", "config", "model_info"):
            e.pop(key, None)
        e["url"] = f"{base}/{m.get('shortName')}.tar.gz"
        emitted.append(e)

    with open(os.path.join(args.out, "models.json"), "w", encoding="utf-8") as f:
        json.dump({"models": emitted}, f, indent=2, ensure_ascii=False)

    if args.dry_run:
        print(f"\n[dry-run] models.json generado para {len(entries)} modelos (sin descargar).")
    else:
        print(f"\nResumen: {ok} descargados, {skipped} ya válidos, "
              f"{download_bad} con error, {missing_bad} ids no encontrados.")
    print(f"Registro del mirror: {os.path.join(args.out, 'models.json')}")
    print(f"URL para la app: {base}/models.json")
    return 1 if (download_bad or missing_bad) else 0


if __name__ == "__main__":
    sys.exit(main())