#!/usr/bin/env python3
"""Convierte un modelo OPUS-MT / Tatoeba (formato marian) al formato
bergamot que usa Elements Extract, y emite la entrada de su registro.

Uso típico:
  1. Construir marian-conv una sola vez (parte de browsermt/marian-dev):
       git clone https://github.com/browsermt/marian-dev.git
       cd marian-dev && mkdir build && cd build
       cmake .. -DCMAKE_BUILD_TYPE=Release -DCOMPILE_CUDA=off \
                -DCOMPILE_SERVER=off -DCOMPILE_TESTS=off
       make -j$(nproc)
       cp marian ../marian-conv        # marian despacha por nombre de binario
  2. Convertir un par (resuelve el mejor artefacto marian descargable):
       tools/convert-opusmt.py --pair eng-por --out /tmp/particulas \
           --bergamot ./bin/bergamot
  3. Publicar el tar.gz + entrada del registro (se imprime la entrada JSON).
  4. Apuntar la app: ELEMENTS_EXTRACT_MODEL_REGISTRY="file:///tmp/particulas/models.json"

Formato emitido (idéntico al que la app instala):
  model.intgemm.alphas.bin   (marian int8, gemm-precision int8shiftAlphaAll)
  lex.s2t.bin                (shortlist binario; sin él este bergamot devuelve identidad)
  source.spm, target.spm     (vocab de origen y destino)
  config.intgemm8bitalpha.yml
  model_info.json

Limitaciones conocidas:
  - El shortlist se genera con heurística (frecuencias de piezas), no con datos
    bilingües: válido, pero de menor calidad que uno derivado de corpus. Puedes
    aportar el tuyo con --lex-txt.
  - Los modelos OPUS usan 'truecasing' (source.tcmodel); bergamot no aplica ese
    preprocesado, así que la salida puede perder algo de calidad. Si el zip trae
    un tcmodel y no usas --no-tcmodel, se advierte.
  - La serie tc-big (2023) no publica su model.npz en descargables para es/pt/fr;
    usa --url si consigues un .npz marian de esa serie.
"""

import argparse
import hashlib
import json
import os
import re
import sys
import tarfile
import urllib.request
import zipfile

TATOEBA_REPO = "https://object.pouta.csc.fi/Tatoeba-MT-models"
OPUS_REPO = "https://object.pouta.csc.fi/OPUS-MT-models"
HDRS = {"User-Agent": "elements-extract-opusmt/1.0"}

LANG3_TO_2 = {
    "eng": "en", "spa": "es", "fra": "fr", "por": "pt", "deu": "de",
    "ita": "it", "nld": "nl", "rus": "ru", "pol": "pl", "ukr": "uk",
}
LANG3_NAME = {
    "eng": "English", "spa": "Spanish", "fra": "French", "por": "Portuguese",
    "deu": "German", "ita": "Italian", "nld": "Dutch", "rus": "Russian",
    "pol": "Polish", "ukr": "Ukrainian",
}

PROBE_SRC_EN = "Hello world, this is a quick test of the translation engine."
PROBE_SRC_ES = "Hola mundo, esto es una prueba rapida del motor de traduccion."
PROBE_SRC_FR = "Bonjour le monde, ceci est un test rapide du moteur de traduction."
PROBE_SRC_PT = "Ola mundo, isto e um teste rapido do motor de traducao."
PROBE_BY_DIR = {"eng": PROBE_SRC_EN, "es": PROBE_SRC_ES, "spa": PROBE_SRC_ES,
                "fr": PROBE_SRC_FR, "fra": PROBE_SRC_FR, "pt": PROBE_SRC_PT,
                "por": PROBE_SRC_PT}

CONFIG_TEMPLATE = """relative-paths: true
models:
  - model.intgemm.alphas.bin
vocabs:
  - source.spm
  - target.spm
shortlist:
    - lex.s2t.bin
    - false
beam-size: 1
normalize: 1.0
word-penalty: 0
mini-batch: 64
maxi-batch: 1000
maxi-batch-sort: src
workspace: 2000
max-length-factor: 2.5
skip-cost: true
gemm-precision: int8shiftAlphaAll
mini-batch-words: -1
max-length-break: 200
"""


LANG2_TO_3 = {v: k for k, v in LANG3_TO_2.items()}


def normalize_pair(pair):
    if "-" not in pair:
        return pair
    s, t = pair.split("-", 1)
    return "-".join([LANG2_TO_3.get(s, s), LANG2_TO_3.get(t, t)])


def fetch(url, timeout=120):
    req = urllib.request.Request(url, headers=HDRS)
    with urllib.request.urlopen(req, timeout=timeout) as resp:
        return resp.read()


def list_index(repo):
    return fetch(repo + "/index.txt", timeout=180).decode("utf-8", "replace").splitlines()


def resolve_zip(pair, repo):
    lines = list_index(repo)
    cands = []
    prefix = pair + "/"
    for ln in lines:
        if not ln.startswith(prefix):
            continue
        name = ln[len(prefix):]
        if not (name.endswith(".zip") and "eval" not in name and "test" not in name):
            continue
        cands.append(name)
    def weight(name):
        s = 0
        if "transformer-big" in name:
            s += 4
        elif "transformer-align" in name:
            s += 3
        else:
            s += 1
        for tok in ("2023", "2022", "2021", "2020", "2019"):
            s += int(tok) if tok in name else 0
        if "bt" in name:
            s -= 1
        return s
    if not cands:
        return None
    return repo + "/" + pair + "/" + max(sorted(cands), key=weight)


def spm_vocab(data):
    pieces = []
    i = 0
    n = len(data)
    ok = False
    while i < n:
        if data[i] != 0x0A:
            i += 1
            continue
        i += 1
        length = 0
        shift = 0
        while True:
            b = data[i]
            i += 1
            length |= (b & 0x7F) << shift
            if not (b & 0x80):
                break
            shift += 7
        if i + length > n:
            return None
        sub = data[i:i + length]
        i += length
        if len(sub) >= 2 and sub[0] == 0x0A:
            plen = sub[1]
            if 2 + plen <= len(sub):
                pieces.append(sub[2:2 + plen].decode("utf-8", "replace"))
                ok = True
    return pieces if ok else None


def build_lex_txt(src_pieces, trg_pieces, per=12, top_src=4000):
    out = []
    lim = min(len(src_pieces), top_src)
    for sid in range(lim):
        src = src_pieces[sid]
        if not src.strip() or src.startswith("<"):
            continue
        tg = []
        for tid in range(min(per, len(trg_pieces))):
            t = trg_pieces[tid]
            if t.strip() and not t.startswith("<") and t != src:
                tg.append(t)
        if tg:
            for t in tg:
                out.append(t + "\t" + src + "\t1.0")
    return "\n".join(out)


def main():
    ap = argparse.ArgumentParser(description="Convierte OPUS-MT/Tatoeba (marian) a formato bergamot de Elements Extract")
    ap.add_argument("--pair", help="Par 3 letras src-trg (ej. eng-por). Con --url no hace falta.")
    ap.add_argument("--url", help="URL directa al zip/tar.gz marian (requerido si no hay --pair)")
    ap.add_argument("--repo", choices=["tatoeba", "opus"], default="tatoeba")
    ap.add_argument("--out", default="opus-partiendo", help="Directorio de salida")
    ap.add_argument("--marian-conv", default="marian-conv", help="Ruta a marian-conv")
    ap.add_argument("--bergamot", help="Ruta al binario bergamot.real de la app para validar")
    ap.add_argument("--lex-txt", help="Lexicón de texto alternativo para el shortlist")
    ap.add_argument("--no-verify", action="store_true")
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--keep", action="store_true", help="Conservar ficheros extraídos")
    ap.add_argument("--num-processors", type=int, default=2)
    args = ap.parse_args()

    if not args.url and not args.pair:
        ap.error("se necesita --pair o --url")

    repo = TATOEBA_REPO if args.repo == "tatoeba" else OPUS_REPO

    if args.url:
        url = args.url
        base_name = os.path.basename(url).split(".")[0]
        pair_id = base_name
    else:
        pair = normalize_pair(args.pair)
        pair_id = pair
        url = resolve_zip(pair, repo)
        if not url:
            print(f"[ERROR] sin zip marian para {args.pair} en {repo}")
            sys.exit(1)

    print("[par] " + (args.pair or pair_id))
    print("[zip] " + url)

    if args.dry_run:
        print("[dry-run] se haría: descargar, marian-conv, shortlist, config, tar.gz, registro")
        return 0

    os.makedirs(args.out, exist_ok=True)

    zip_path = os.path.join(args.out, os.path.basename(url))
    if not os.path.exists(zip_path):
        print("[descarga] " + zip_path)
        with open(zip_path, "wb") as f:
            f.write(fetch(url, timeout=900))

    names = []
    with zipfile.ZipFile(zip_path) as z:
        names = z.namelist()
    npz = next((n for n in names if n.endswith(".npz")), None)
    src_spm = next((n for n in names if n.endswith("source.spm")), None)
    trg_spm = next((n for n in names if n.endswith("target.spm")), None)
    tcmodel = next((n for n in names if n.endswith("source.tcmodel")), None)
    if not (npz and src_spm and trg_spm):
        print("[ERROR] el zip no contiene model.npz + source.spm + target.spm")
        print("  ficheros:", names[:20])
        sys.exit(1)

    if tcmodel:
        print("[aviso] el modelo usa truecasing (source.tcmodel); bergamot no lo aplica")

    with zipfile.ZipFile(zip_path) as z:
        z.extract(npz, path=args.out)
        z.extract(src_spm, path=args.out)
        z.extract(trg_spm, path=args.out)

    src_path = os.path.join(args.out, os.path.basename(src_spm))
    trg_path = os.path.join(args.out, os.path.basename(trg_spm))

    model_bin = os.path.join(args.out, "model.intgemm.alphas.bin")
    if not os.path.exists(model_bin):
        print("[convert] marian-conv -> model.intgemm.alphas.bin")
        r = os.system(f"{args.marian_conv} -f '{os.path.join(args.out, os.path.basename(npz))}'"
                      f" -t '{model_bin}' --gemm-type intgemm8 2>&1")
        if r != 0:
            print("[ERROR] marian-conv falló (¿construido? ver docstring del script)")
            sys.exit(1)

    lex_bin = os.path.join(args.out, "lex.s2t.bin")
    if not os.path.exists(lex_bin):
        lex_txt_path = os.path.join(args.out, "lex.txt")
        if args.lex_txt:
            lex_txt = open(args.lex_txt, encoding="utf-8").read()
        else:
            src_pieces = spm_vocab(open(src_path, "rb").read())
            trg_pieces = spm_vocab(open(trg_path, "rb").read())
            if not src_pieces or not trg_pieces:
                print("[ERROR] no se pudieron leer los vocabs sentencepiece")
                sys.exit(1)
            lex_txt = build_lex_txt(src_pieces, trg_pieces)
            with open(lex_txt_path, "w", encoding="utf-8") as f:
                f.write(lex_txt)
            print(f"[lex] texto del shortlist: {lex_txt_path}")
        r = os.system(
            f"{args.marian_conv} --shortlist '{lex_txt_path}' 100 100 0 "
            f"--dump '{lex_bin}' --vocabs '{src_path}' '{trg_path}' 2>&1")
        if r != 0:
            print("[ERROR] generación del shortlist falló")
            sys.exit(1)

    srcdir = args.out
    cfg = os.path.join(srcdir, "config.intgemm8bitalpha.yml")
    if os.path.exists(model_bin) and not os.path.exists(cfg):
        with open(cfg, "w", encoding="utf-8") as f:
            f.write(CONFIG_TEMPLATE)

    src = pair_id.split("-")[0]
    trg = pair_id.split("-")[1] if "-" in pair_id else ""
    dm = re.search(r"(\d{4})-(\d{2})-(\d{2})", os.path.basename(url))
    release_ver = int(dm.group(1) + dm.group(2) + dm.group(3)) if dm else 1
    short = f"{LANG3_TO_2.get(src, src[:2])}-{LANG3_TO_2.get(trg, trg[:2])}-opus"
    with open(os.path.join(srcdir, "model_info.json"), "w", encoding="utf-8") as f:
        json.dump({
            "modelName": f"{LANG3_NAME.get(src, src)}-{LANG3_NAME.get(trg, trg)}",
            "shortName": short,
            "type": "opus",
            "src": LANG3_NAME.get(src, src),
            "trg": LANG3_NAME.get(trg, trg),
            "srcTags": {LANG3_TO_2.get(src, src): LANG3_NAME.get(src, src)} if src in LANG3_NAME else {},
            "trgTag": LANG3_TO_2.get(trg, trg) or "",
            "repository": "OPUS-MT",
            "version": release_ver,
            "API": 1,
        }, f, indent=2, ensure_ascii=False)
    print("[config] " + cfg)
    print("[model_info] " + os.path.join(srcdir, "model_info.json"))

    if args.bergamot and not args.no_verify:
        probe = PROBE_BY_DIR.get(src, PROBE_SRC_EN)
        out = os.popen(f"cd '{srcdir}' && printf '%s\\n' '{probe}' | "
                       f"timeout 120 '{args.bergamot}' --model-config-paths config.intgemm8bitalpha.yml "
                       f"--cpu-threads {args.num_processors} 2>&1").read()
        if probe.strip() in out or not out.strip():
            print(f"[ADVERTENCIA] validación: la salida no cambia respecto al input (probe={probe!r})")
            print("  salida: " + (out.strip()[:160] or "(vacía)"))
        else:
            print("[validación] tradujo OK -> " + out.strip().splitlines()[0][:120])

    tar_path = os.path.join(args.out, short + ".tar.gz")
    with tarfile.open(tar_path, "w:gz") as t:
        for fn in ("model.intgemm.alphas.bin", "lex.s2t.bin", "source.spm", "target.spm",
                   "config.intgemm8bitalpha.yml", "model_info.json"):
            fp = os.path.join(srcdir, fn)
            if os.path.exists(fp):
                t.add(fp, arcname=fn)
    checksum = hashlib.sha256(open(tar_path, "rb").read()).hexdigest()
    size = os.path.getsize(tar_path)

    entry = {
        "shortName": short,
        "modelName": f"{LANG3_NAME.get(src, src)}-{LANG3_NAME.get(trg, trg)}",
        "type": "opus",
        "src": LANG3_NAME.get(src, src),
        "trg": LANG3_NAME.get(trg, trg),
        "srcTags": {LANG3_TO_2.get(src, src): LANG3_NAME.get(src, src)},
        "trgTag": LANG3_TO_2.get(trg, trg) or "",
        "version": release_ver,
        "url": f"file://{os.path.abspath(tar_path)}",
        "checksum": checksum,
        "size": size,
    }
    print("\n[registro] limpia la vía para tu models.json:")
    print(json.dumps({"models": [entry]}, indent=2, ensure_ascii=False))

    if not args.keep:
        for nm in os.listdir(args.out):
            if nm not in (os.path.basename(tar_path), os.path.basename(zip_path), "models.json"):
                p = os.path.join(args.out, nm)
                if os.path.isfile(p):
                    os.remove(p)
    return 0


if __name__ == "__main__":
    sys.exit(main())