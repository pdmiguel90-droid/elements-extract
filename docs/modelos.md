# Mantenimiento de los modelos de traducción

Documento de referencia sobre cómo funciona hoy el ciclo de modelos y cómo
reemplazarlos por otros cuando quieras. No describe implementación pendiente:
describe la infra que ya existe y la única vía real de refresco.

## 1. Estado actual (verificado)

- La app es independiente del motor: descarga "registro" (models.json), descarga
  el tar.gz, valida SHA-256 y lo desempaqueta en `models/<id>`.
- Todo el artefacto "tiny" que hoy distribuye el proyecto (es/en/fr/de) es la
  generación `student.tiny11` de Bergamot, **congelada**:
  - `data.statmt.org/bergamot/models/` (2023-12-16, últimos archivos).
  - El registro de producción de Firefox/Remote Settings sirve **el mismo
    artefacto byte a byte** (verificado con SHA-256 para `esen`), más el par
    `pt↔en` (misma generación, formato v2).
- Firefox NO tiene nada más fresco ni mejor para es/fr/pt: su capa `base`
  (mejor calidad) solo cubre {ar,cs,de,ja,ko,ru,zh}↔en y zh/en/jp↔... (sin es,
  fr ni pt). Los registros "grandes" (~31 MB) del Remote Settings son la
  producción antigua de Firefox (2019-2021).
- Motivo: los modelos son artefactos "entrenados una vez" (ética Bergamot/student).
  Quien quiere mejor calidad hoy recurre a OPUS-MT (ver §4) o a reentrenar.

## 2. Ciclo actual y herramientas

| Pieza | Dónde | Función |
|---|---|---|
| Registro por defecto | `https://translatelocally.com/models.json` | Lista de modelos disponibles (33, sin pt) |
| Override del registro | `modeldownloader.cpp` (constructor) | `ELEMENTS_EXTRACT_MODEL_REGISTRY` (env) → QSettings `[models]/registryUrl` → default |
| Vigilancia | `tools/check-model-updates.py` + `tools/model-catalog.json` | Detecta modelos nuevos / bump de versión / eliminados; exit 1 si hay cambios |
| Mirror | `tools/mirror-models.py` | Descarga tar.gz verificando SHA-256 y genera un `models.json` propio |
| Instalación | App (Settings) | `url` (tar.gz) + `checksum` + `version` → descarga, verifica, extrae |
| CI / Release | `.github/workflows/build.yml` | Compila, test de humo y genera el `.deb`; Release automático al tag `v*` |
| Vigilancia semanal | `.github/workflows/watchdog.yml` | Cron ejecutando `check-model-updates.py --no-write` |

Uso típico para quedar independiente de Mozilla:

```bash
# 1. snapshot del catálogo (una vez) y vigilancia en CI semanal
tools/check-model-updates.py --no-write   # ver familia = cambios

# 2. espejo local/publicable
tools/mirror-models.py --out models-mirror --all
tools/mirror-models.py --out models-mirror --models en-es-tiny,es-en-tiny,...

# 3. apuntar la app a tu registro
ELEMENTS_EXTRACT_MODEL_REGISTRY="file:///ruta/models-mirror/models.json" ./elements-extract
# o config persistente:
#   ~/.config/LocalApp/FFmpegExtractor.conf  →  [models] registryUrl=...
```

Nota: `models/` y `bin/` están en `.gitignore` (salvo el launcher `bin/bergamot`,
el `.deb` empaqueta lo que haya
en `models/` al compilar (`CMakeLists.txt:60`). El "bundle por defecto" actual
(6 pares: es/en/fr + pt) se mantiene tal cual mientras exista en esa carpeta.

En la CI los modelos llegan vía `mirror-models.py` (pares `en-es-tiny`/`es-en-tiny`)
y el motor bergamot **se compila en el propio run** con el workflow reutilizable
`bergamot-linux.yml` (`browsermt/bergamot-translator` v0.4.5, `USE_MKL=OFF` →
OpenBLAS, `BUILD_ARCH=x86-64` portable, Ubuntu 22.04). Los workflows `build`
(.deb) y `appimage` lo consumen vía `download-artifact` (`bergamot-linux64.tar.gz`
con `bergamot.real` + `.so` acompañantes) y siempre llevan motor: ya no hay
espejo externo ni variable `BERGAMOT_BASE_URL` (eliminada).

### Motor para Windows (bergamot.exe)

No existe binario oficial de `bergamot` para Windows, así que el build de Windows
**lo compila y lo empaqueta automáticamente**:

- El workflow reutilizable `bergamot-windows.yml` compila el CLI (target
  `bergamot` de `browsermt/bergamot-translator` v0.4.5) con MSVC + vcpkg
  (`protobuf`, `pcre2`, MKL static) y sube el artefacto `bergamot-win64`.
- El workflow `windows` lo consume (`needs: bergamot`), coloca `bergamot.exe` en
  `bin/` y lo instala en el zip: el artefacto de Windows ya incluye el motor, sin
  pasos manuales ni variables.

Para compilar el motor por separado y publicarlo como asset (por ejemplo para
reutilizarlo), ejecuta el workflow `bergamot-windows` manualmente o haz push de
un tag `bergamot-*`.

### Mirror público en GitHub Pages (opcional)

El workflow `mirror.yml` publica los tar.gz (es/en/fr tiny) + `models.json` en la
rama `gh-pages` cada lunes. Para activarlo:

1. En el repo: **Settings → Pages → Build and deployment → Source: Deploy from
   a branch → `gh-pages` / (root)**.
2. Apunta la app al registro propio:
   `ELEMENTS_EXTRACT_MODEL_REGISTRY="https://<tu-usuario>.github.io/elements-extract/models.json"`
   (o el QSettings `[models]/registryUrl`).

Nota: `pt↔en` no está en el catálogo público de Kaggle/statmt/translateLocally,
así que el mirror no lo incluye: queda fuera por diseño (el bundle lo lleva
empaquetado).

## 3. Qué espera la app de cada modelo

Un directorio de modelo instalado (o el contenido del tar.gz) debe incluir:

```
config.intgemm8bitalpha.yml   # o config.yml; relativo al directorio del modelo
model.intgemm.alphas.bin      # marian int8 (intgemm8shiftAlphaAll)
lex.s2t.bin                   # lexicón de corta lista (50.50)
vocab.spm                     # vocab único (una sola sentencepiece, listada 2x en el config)
model_info.json               # marcador de instalación (la app NO parsea sus campos)
```

- La invocación es genérica: `bergamot --model-config-paths <config> --cpu-threads 2`
  (subtitletranslator.cpp:325), ejecutada dentro del directorio del modelo.
- `tokenizer.json`, `modelMeta.json`, `catalog-entry.yml`, `speed.*.sh` no los
  usa la app (pueden faltar o sobrar).
- `model_info.json` se usa solo como prueba de que la extracción fue completa.
  La versión instalada que la app registra sale del campo `version` del registro,
  no del archivo.

## 4. La vía real de refresco: OPUS-MT (cuando quieras mejor calidad)

OPUS-MT (Helsinki-NLP) sí publica modelos más recientes (serie `tc-big`,
~2023) y cubre `pt↔en`, con mejor calidad que `student.tiny11`. No hay pt↔es
directo en esa serie (resolución triangular pt→en→es o es→pt vía en).

Existe un conversor en el repo: `tools/convert-opusmt.py`. Descarga el mejor
artefacto marian **descargable** del par (Tatoeba-MT-/OPUS-MT-models en
`object.pouta.csc.fi`), lo convierte a int8 bergamot, genera el `lex.s2t.bin`
(obligatorio; este bergamot devuelve identidad sin él) y emite el directorio en
formato de §3 + la entrada JSON del registro:

```bash
# 1. construir marian-conv una sola vez (mismo marian que tu bergamot):
git clone --recurse-submodules https://github.com/browsermt/marian-dev.git
cd marian-dev && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCOMPILE_CUDA=off \
         -DCOMPILE_SERVER=off -DCOMPILE_TESTS=off
make -j$(nproc) && cp marian ../marian-conv   # despacha por argv[0]

# 2. convertir un par (p. ej. en→pt):
tools/convert-opusmt.py --pair eng-por --out /tmp/particulas \
    --bergamot bin/bergamot --marian-conv /ruta/marian-conv

# 3. publicar el tar.gz + entrada del registro que imprime; apuntar la app:
ELEMENTS_EXTRACT_MODEL_REGISTRY="file:///tmp/particulas/models.json" ./elements-extract
```

Limitaciones verificadas (no sorpresa al ejecutar):

- La serie `tc-big` (2023) **no publica su `model.npz`** en descargables para
  es/fr/pt: el par se resuelve a los releases marian clásicos (2021-2022,
  280 MB–1 GB por zip; es la serie de mejor calidad con formato marian accesible).
  Si algún día consigues un `.npz` de tc-big, `--url` lo convierte igual.
- El shortlist se genera por heurística de frecuencias de piezas (sin corpus
  bilingüe): válido para decodificar, pero de menor calidad que uno derivado de
  datos. Puedes aportar el tuyo con `--lex-txt` (formato `trg\tsrc\tprob`).
- Varios modelos OPUS usan truecasing (`source.tcmodel`); bergamot no lo aplica
  → la salida puede perder algo de calidad (el script avisa).
- No probado end-to-end en esta máquina (faltan deps de compilación de marian);
  la validación `--bergamot` traduce una frase y avisa si la salida es identidad.

## 5. Red de seguridad (opcional)

Si quieres blindar los 6 pares contra la desaparición de las fuentes, guarda
una copia estática (no requiere sincronizar con Firefox):

```bash
tools/mirror-models.py --out /respaldo/models-mirror --all        # los 5-6 de statmt
tar -czf /respaldo/en-pt-tiny.tar.gz -C models en-pt-tiny        # pt no existe en catálogos
tar -czf /respaldo/pt-en-tiny.tar.gz -C models pt-en-tiny        # (respaldo manual del bundle local)
```