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
y el motor bergamot se descarga de la variable de GitHub Actions
`BERGAMOT_BASE_URL` (URL con `bergamot.real` y `libopenblas.so.0`). Sin esa
variable el `.deb` compilado sale sin motor de traducción.

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

Formato de publicación: PyTorch/safetensors. Para Bergamot hay que producir el
artefacto marian cuantizado. Pasos:

1. Obtener el tarball marian del modelo (`model.npz`, `srcvocab`/`trgvocab`,
   `config.yml`) desde la publicación OPUS-MT (~ `data.statmt.org/OPUS-MT/`).
2. Convertir a int8 bergamot (repo `browsermt/bergamot-translator`):
   `marian-conv --gemm-type intgemm8`  → `model.intgemm.alphas.bin`.
3. Generar el lexicón `lex.s2t.bin` (herramientas `browsermt/bergamot-models`,
   `quick_lex` con corpus bilingüe + sentencepiece).
4. Empaquetar el directorio según §3 (config + vocab único), crear `tar.gz`,
   calcular `sha256sum` y añadir la entrada al `models.json` (`shortName`,
   `url`, `checksum`, `size`, `version` >= 2, `srcTags`/`trgTag`).
5. Validar antes de publicar:
   ```bash
   cd model-dir && ../bin/bergamot --model-config-paths config.intgemm8bitalpha.yml --cpu-threads 2
   printf 'Hello world\n' | ...
   ```
6. Publicar y apuntar la app al nuevo registro.

Es un pipeline de ingeniería, no un clic: para la v1 de la app se recomienda
mantener la generación tiny congelada (que funciona y es lo que Firefox sirve
hoy) y resolver pt↔en (ya incluido en el bundle).

## 5. Red de seguridad (opcional)

Si quieres blindar los 6 pares contra la desaparición de las fuentes, guarda
una copia estática (no requiere sincronizar con Firefox):

```bash
tools/mirror-models.py --out /respaldo/models-mirror --all        # los 5-6 de statmt
tar -czf /respaldo/en-pt-tiny.tar.gz -C models en-pt-tiny        # pt no existe en catálogos
tar -czf /respaldo/pt-en-tiny.tar.gz -C models pt-en-tiny        # (respaldo manual del bundle local)
```