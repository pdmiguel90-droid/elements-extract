#!/usr/bin/env python3
"""Vigila el catálogo de modelos de traducción (bergamot) usado por Elements Extract.

Comportamiento:
  1. Descarga el catálogo remoto (registro JSON).
  2. Compara con un snapshot local (tools/model-catalog.json) y reporta:
       - modelos NUEVOS (no estaban antes),
       - modelos con BUMB de versión (availability de actualización),
       - modelos ELIMINADOS del catálogo,
       - fecha "Last-Modified" de los archivos (con --dates).
  3. Opcionalmente (--models-dir) cruza con lo que tienes instalado y con las
     versiones registradas en el QSettings de la app para listar qué pares
     instalados tienen actualización disponible.
  4. Actualiza el snapshot (o --no-write para solo inspeccionar).

Úsalo para decidir CUÁNDO vale la pena volver a empaquetar la cadena de modelos
de tu mirror (o para que una CI avise de deriva del catálogo).

Registros compatibles: el formato de https://translatelocally.com/models.json
(campos: shortName, modelName, srcTags, trgTag, type, url, checksum, size, version).
También acepta una lista JSON directamente (cuando la raíz es un array).
"""

import argparse
import configparser
import hashlib
import json
import os
import ssl
import sys
import urllib.request

DEFAULT_REGISTRY = "https://translatelocally.com/models.json"
DEFAULT_SNAPSHOT = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                "model-catalog.json")
DEFAULT_QSETTINGS = os.path.expanduser(
    "~/.config/LocalApp/FFmpegExtractor.conf")


def fetch_json(url, timeout=30):
    req = urllib.request.Request(url, headers={"User-Agent": "elements-extract-check-models/1.0"})
    with urllib.request.urlopen(req, timeout=timeout) as resp:
        return json.loads(resp.read().decode("utf-8"))


def normalize(entry):
    src_tags = entry.get("srcTags") or {}
    src = (list(src_tags.keys())[0] if src_tags
           else str(entry.get("src", "") or "").lower())
    return {
        "id": entry.get("shortName", ""),
        "name": entry.get("modelName", entry.get("name", "")),
        "src": src,
        "trg": str(entry.get("trgTag") or entry.get("trg") or "").lower(),
        "type": entry.get("type", ""),
        "url": entry.get("url", ""),
        "checksum": entry.get("checksum", ""),
        "size": entry.get("size", 0),
        "version": str(entry.get("version", "0")),
    }


def catalog_to_map(catalog):
    return {m["id"]: m for m in catalog if m["id"]}


def read_qsettings_versions(path):
    """Lee la sección [installedModelVersions] del QSettings (formato INI)."""
    versions = {}
    if not os.path.exists(path):
        return versions
    cp = configparser.ConfigParser()
    try:
        cp.read(path)
    except configparser.Error:
        return versions
    if cp.has_section("installedModelVersions"):
        versions = dict(cp.items("installedModelVersions"))
    return versions


def installed_ids(models_dir):
    if not models_dir or not os.path.isdir(models_dir):
        return set()
    return {d for d in os.listdir(models_dir)
            if os.path.isdir(os.path.join(models_dir, d))}


def last_modified(url, timeout=15):
    req = urllib.request.Request(url, method="HEAD",
                                 headers={"User-Agent": "elements-extract-check-models/1.0"})
    try:
        with urllib.request.urlopen(req, timeout=timeout) as resp:
            return resp.headers.get("Last-Modified") or resp.headers.get("Date") or "?"
    except Exception:
        return "?"


def main():
    ap = argparse.ArgumentParser(
        description="Comprueba el estado / deriva del catálogo de modelos bergamot.")
    ap.add_argument("--registry", default=os.environ.get("ELEMENTS_EXTRACT_MODEL_REGISTRY", DEFAULT_REGISTRY))
    ap.add_argument("--snapshot", default=DEFAULT_SNAPSHOT)
    ap.add_argument("--models-dir", help="Directorio de modelos instalados (apps: ./models)")
    ap.add_argument("--qsettings", default=DEFAULT_QSETTINGS)
    ap.add_argument("--dates", action="store_true", help="Consulta Last-Modified de cada URL (más lento)")
    ap.add_argument("--no-write", action="store_true", help="No actualizar el snapshot")
    args = ap.parse_args()

    print(f"Registro: {args.registry}")
    try:
        raw = fetch_json(args.registry)
    except Exception as e:
        print(f"[ERROR] No se pudo leer el registro: {e}", file=sys.stderr)
        return 2

    if isinstance(raw, dict):
        raw = raw.get("models", [])
    catalog = catalog_to_map([normalize(m) for m in raw])

    old = {}
    if os.path.exists(args.snapshot):
        try:
            old = json.load(open(args.snapshot, encoding="utf-8"))
        except Exception:
            old = {}

    change = False
    def ver_num(v):
        try:
            return tuple(int(x) for x in str(v).split("."))
        except Exception:
            return (0,)

    print(f"\n{len(catalog)} modelos en el catálogo\n")
    for mid in sorted(catalog):
        m = catalog[mid]
        oldv = old.get(mid, {}).get("version") if (old and mid in old) else None
        if oldv is None and mid not in (old or {}):
            if old is not None:
                print(f"  [NUEVO] {mid}  v{m['version']}  ({m['src']}->{m['trg']}, {m['type']})")
                change = True
        elif oldv is not None and ver_num(m["version"]) > ver_num(oldv):
            print(f"  [VERSIÓN] {mid}: v{oldv} -> v{m['version']}  ({m['src']}->{m['trg']})")
            change = True
        else:
            status = "ok"
            if oldv is not None and ver_num(m["version"]) < ver_num(oldv):
                status = "?! v" + str(m["version"]) + " (menor que el anterior v" + str(oldv) + ")"
            extra = ""
            if args.dates:
                extra = f"  [{last_modified(m['url'])}]"
            print(f"  [{'ok' if status=='ok' else status}] {mid}  v{m['version']}  {m['type']}{extra}")

    if old is not None:
        removed = [mid for mid in old if mid not in catalog]
        if removed:
            print("\n  [ELIMINADOS del catálogo] " + ", ".join(sorted(removed)))
            change = True

    if args.models_dir:
        installed = installed_ids(args.models_dir)
        installed_versions = read_qsettings_versions(args.qsettings)
        print("\nEstado local:")
        for mid in sorted(installed):
            if mid in catalog:
                inv = installed_versions.get(mid)
                up = ver_num(catalog[mid]["version"]) > ver_num(inv) if inv else (
                    "version local desconocida")
                flag = f"  -> v{catalog[mid]['version']} disponible" if isinstance(up, bool) and up else ""
                print(f"  Instalado: {mid}  (local v{inv or '?'}){flag}")
            else:
                print(f"  Instalado y NO está en el catálogo: {mid}")

    if not args.no_write:
        with open(args.snapshot, "w", encoding="utf-8") as f:
            json.dump({m["id"]: m for m in catalog.values()}, f, indent=2, ensure_ascii=False)
        print(f"\nSnapshot actualizado: {args.snapshot}")
    else:
        print("\nSnapshot NO actualizado (--no-write)")

    return 1 if change else 0


if __name__ == "__main__":
    sys.exit(main())