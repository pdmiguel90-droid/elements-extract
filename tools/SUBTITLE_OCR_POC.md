# Prueba de concepto: OCR de subtítulos con nOCR

`subtitle-ocr-poc.py` demuestra que una base `.nocr` de Subtitle Edit puede leer
subtítulos de imagen (PGS) sin Tesseract ni modelos neuronales. Usa solo Python 3,
Pillow, numpy y ffmpeg. No se instala nada en la app y la base se descarga a caché.

## Por qué nOCR

- Motor de plantillas: cada glifo es un conjunto de líneas de prueba (foreground /
  background) escaladas a la caja candidata. No hay red neuronal.
- Licencia MIT, compatible con el GPL-3.0 de Elements Extract (con atribución).
- `Latin.nocr`: 857.110 bytes sin comprimir (537.165 bytes con gzip);
  2.371 glifos (97 expandidos), 142 caracteres. Cabe en el binario o se descarga.

## Formatos reales (importante)

El enunciado describía Variantes que no coinciden con las fuentes de referencia. El POC
implementa lo que ffmpeg y Subtitle Edit leen realmente:

- `.nocr` V2: prefijo `V2`; flags `0x10` = corto, `0x20` = itálica, bits bajos =
  `expandCount`; texto = 1 byte de longitud **UTF-8** seguido de esos bytes (no UTF-16LE);
  líneas = **cantidad explícita** de segmentos `(inicio, fin)` con 1 o 2 bytes por
  coordenada (no listas terminadas en centinela). V1 usa 2 bytes para tamaño/margen.
- PGS: PCS `0x16`, PDS `0x14`, ODS `0x15`, WDS `0x17`, END `0x80` (no `0x1C/0x17/0x18`).
- `.sup` crudo: cada paquete lleva `PG` + PTS (4 bytes) + DTS (4 bytes) + **un**
  segmento. No son paquetes PES MPEG; los PES pertenecen a MPEG-PS/TS.
- PGS usa alpha de 8 bits por entrada de paleta (el alpha de 2 bits es de VobSub).

## Comandos ejecutados

```bash
cd /home/pm/Descargas/ffmpeg_extractor
python3 tools/subtitle-ocr-poc.py make-sample --out /tmp/ocr_poc
python3 tools/subtitle-ocr-poc.py load-db --db share/ocr/Latin.nocr
ffprobe -hide_banner -i /tmp/ocr_poc/sample.sup
ffmpeg -hide_banner -v error -i /tmp/ocr_poc/sample.sup \
  -filter_complex "[0:s]format=rgba[v]" -map "[v]" \
  -f image2 -fps_mode passthrough /tmp/ocr_poc/frame_%03d.png
python3 tools/subtitle-ocr-poc.py run \
  --input /tmp/ocr_poc/sample.sup --db share/ocr/Latin.nocr \
  --reference /tmp/ocr_poc/sample.reference.srt --out /tmp/ocr_poc/output.default.srt
python3 tools/subtitle-ocr-poc.py run \
  --input /tmp/ocr_poc/sample.sup --db share/ocr/Latin.nocr \
  --reference /tmp/ocr_poc/sample.reference.srt --out /tmp/ocr_poc/output.ocr.srt \
  --max-wrong 50 --space-gap-factor 0.35
```

Sin `--db` se usa `~/.cache/elements-extract/Latin.nocr` y se descarga si falta.
También se puede pasar un PNG a `run --input`; el SRT de referencia es opcional
(sin él se escriben los tiempos del PGS y no se calcula accuracy).

## Resultados reales

`ffprobe -hide_banner -i /tmp/ocr_poc/sample.sup`:

```
Input #0, sup, from '/tmp/ocr_poc/sample.sup':
  Duration: N/A, start: 1.000000, bitrate: N/A
  Stream #0:0: Subtitle: hdmv_pgs_subtitle, 1920x1080
```

ffmpeg extrae 7 PNG (el primero y los dos últimos vacíos, los dos textos en los
intermedios). Los recuentos de píxeles opacos coinciden con el decodificador del POC:
2097 para "Hello world." y 2074 para "This is a test.".

`load-db`: versión V2, 2371 glifos (2274 normales + 97 expandidos), 807 itálicos,
ancho/alto modales 15–21 / 21–29 px, 142 caracteres disponibles. El parser consume
los 857.110 bytes sin residuos, lo que valida la interoperabilidad del formato.

`run` con los parámetros por defecto del motor (gap 0.25, `max_wrong=25`):

```
frame 1: 'He l lo wo rl d .'  glyphs=11 unknown=0 accuracy=70.6%
frame 2: 'Th is i s a □es□.'  glyphs=12 unknown=2 accuracy=76.5%
accuracy: 73.5% (25 matched / 27 reference chars, insertions penalized)
confusions: [(('', ' '), 7), (('t', '□'), 2)]
```

Con `--max-wrong 50 --space-gap-factor 0.35`:

```
frame 1: 'Hello world.'  glyphs=11 unknown=0 accuracy=100.0%
frame 2: 'This is a test.'  glyphs=12 unknown=0 accuracy=100.0%
accuracy: 100.0% (27 matched / 27 reference chars)
```

Tiempos correctos en el SRT: 00:00:01,000→00:00:03,000 y 00:00:04,000→00:00:06,000.

## Limitaciones encontradas

- El factor de espacio de NOcrEngine (0.25 × alto de línea) parte demasiado cerca en
  esta fuente DejaVu Sans sintética: separa letras con huecos de 8–10 px. Hace falta
  0.35 (≈13 px) para distinguirlos del espacio de palabra (17–23 px). En fuentes de
  subtítulo reales habrá que calibrar o usar un umbral adaptativo.
- La `t` de DejaVu Sans difiere lo suficiente de la plantilla de la base como para
  necesitar el pase DeepSeek con presupuesto ampliado (39 píxeles de error).
- No se implementó la cascada de glifos expandidos (ligaduras, caracteres partidos),
  ni itálica contextual, ni corrección ortográfica. Las mayúsculas/minúsculas de
  `cosuvwxz` se resuelven con la métrica de altura del propio renglón.
- El `.sup` no especifica duración: el fin de cada subtitle se infiere del siguiente
  paquete. El SRT de referencia se usa solo para comparar y para los tiempos.
- Con ffmpeg 5.1.9, `ffmpeg -i muestra.sup -f image2 -vsync 0 /tmp/frame_%03d.png`
  falla con *"Output file #0 does not contain any stream"*: image2 no asigna
  automáticamente un stream de subtítulo bitmap a un codificador PNG. La variante
  con `-filter_complex "[0:s]format=rgba[v]" -map "[v]"` funciona y es la usada
  arriba. Las FFprobe/ffmpeg de este entorno son 5.1.9.
- La muestra es limpia, en inglés y con una sola fuente; no mide PGS real con ruido,
  antialiasing agresivo, colores ni varios idiomas.

## Estimación de integración en la app Qt6

Trabajo a portar a C++ dentro de Elements Extract:

1. Parser `.nocr` V1/V2 y construcción del índice por tamaño: 1–2 días.
2. Decodificación/segmentación de bitmap (proyección horizontal y vertical, márgenes,
   huecos): 2–3 días. Conviene reutilizar el QImage/QImage::alphaChannel ya presente.
3. Matcher (cascada de pasos, escalado de líneas con redondeo away-from-zero,
   presupuesto de errores y desempate por ajuste): 3–5 días.
4. Postprocesado (espacios, case twins, itálica, expandidos, fix-list): 3–5 días.
5. Integración con la cola de trabajo, SRT, pruebas y ajuste con muestras reales: 3–5 días.

Total honesto: **2–3 semanas** de un desarrollador familiar con el código para una
integración razonable; más si se quiere calidad cercana a Subtitle Edit en fuentes
variadas. La base se puede empaquetar como recurso Qt o descargar a caché (537 KB).

Reutilizable sin cambios: el ffmpeg/ffprobe ya usado para extraer PGS/VobSub, el
pipeline de traducción y remux, y el escritor de SRT/ASS. Añadir OCR no obliga a
recodificar el vídeo: solo sustituye la salida `.mks` por `.srt` opcional.

Recomendación: **implementar nOCR en C++**, empezando por el parser y el matcher, y
validar con varias películas reales antes de activarlo por defecto. Es ligero,
compatible con la licencia y encaja con la arquitectura actual; el riesgo está en la
calidad del segmentador, no en el tamaño ni en dependencias.

## Validación con PGS real (Suits S01E01)

Medido sobre la pista PGS real del MKV de Suits (1920x1080, 3155 fotogramas):

- Extracción de PNG con ffmpeg: **2 min 23 s**; 58 MB temporales.
- Reconocimiento (motor C++ integrado): **~5 min 40 s**; 1576 fotogramas con texto,
  43749 glifos, 469 desconocidos (**1,1 %**).
- Antes del arreglo de binarización el porcentaje de desconocidos era del **46 %**
  y el texto salía tipo `l-d□n□□ngs□men□□enM□`. El problema era que la máscara
  incluía el contorno negro del PGS además del relleno blanco. Ahora se descarta
  el contorno (gray <= 160) cuando ambos componentes existen.
- El umbral de espacio entre palabras se calcula a partir de la distribución de
  huecos de cada imagen, porque el factor fijo 0,35 que funciona con la muestra
  sintética parte las palabras en el PGS real.
- Resultado real: `I'd been working six months when Mr. Hunt started asking me to
  work more.`, `Order dinner.`, `The next day, I requested a change in assignment.`
- Quedan errores típicos (`l` por `I` en algunas fuentes) y el proceso es lento,
  así que la opción sigue siendo **experimental y desactivada por defecto**; la
  app muestra el progreso en el registro y conviene revisar el SRT resultante.
