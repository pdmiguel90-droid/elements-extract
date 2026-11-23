#!/usr/bin/env python3
"""Self-contained nOCR proof of concept: PGS sample generator, .nocr parser and matcher.

Only Python 3 stdlib + Pillow + numpy are required. No Tesseract, no neural models.
The .nocr layout follows Subtitle Edit's NOcrChar/NOcrLine (MIT); the PGS writer
follows libavformat/supdec.c + libavcodec/pgssubdec.c so ffmpeg can decode it.
"""

import argparse
import difflib
import gzip
import math
import statistics
import struct
import sys
import urllib.request
from collections import Counter, defaultdict
from dataclasses import dataclass, field
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw, ImageFont

DB_URL = "https://raw.githubusercontent.com/SubtitleEdit/subtitleedit/master/Ocr/Latin.nocr"
DEFAULT_DB = Path.home() / ".cache" / "elements-extract" / "Latin.nocr"
DEFAULT_FONT = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
PTS_PER_SECOND = 90000

SEG_PCS, SEG_PDS, SEG_ODS, SEG_WDS, SEG_END = 0x16, 0x14, 0x15, 0x17, 0x80
SIZE_TWINS = "cosuvwxz"
X_HEIGHT_LETTERS = "aemnr"
TALL_LETTERS = "bdfhklt" + "ABDEFGHIJKLMNPQRTY"
SENSITIVE = set("Oo0'-:\"")


# ---------------------------------------------------------------- .nocr format

@dataclass(frozen=True)
class OcrPoint:
    x: int
    y: int


@dataclass(frozen=True)
class OcrLine:
    start: OcrPoint
    end: OcrPoint

    def scaled_points(self, glyph, width, height):
        p1 = scale_point(self.start, glyph, width, height)
        p2 = scale_point(self.end, glyph, width, height)
        if p1 == p2:
            yield p1.x, p1.y
            return
        dx, dy = p2.x - p1.x, p2.y - p1.y
        if abs(dx) > abs(dy):
            x1, y1, x2, y2 = (p1.x, p1.y, p2.x, p2.y) if dx > 0 else (p2.x, p2.y, p1.x, p1.y)
            factor = (y2 - y1) / (x2 - x1)
            for x in range(x1, x2 + 1):
                yield x, round_away(y1 + factor * (x - x1))
        else:
            x1, y1, x2, y2 = (p1.x, p1.y, p2.x, p2.y) if dy > 0 else (p2.x, p2.y, p1.x, p1.y)
            factor = (x2 - x1) / (y2 - y1)
            for y in range(y1, y2 + 1):
                yield round_away(x1 + factor * (y - y1)), y


def round_away(value):
    return math.floor(value + 0.5) if value >= 0 else math.ceil(value - 0.5)


def scale_point(point, glyph, width, height):
    return OcrPoint(round_away(point.x * width / glyph.width),
                    round_away(point.y * height / glyph.height))


@dataclass
class Glyph:
    text: str
    width: int
    height: int
    margin_top: int
    italic: bool
    expand: int
    foreground: list = field(default_factory=list)
    background: list = field(default_factory=list)

    @property
    def line_count(self):
        return len(self.foreground) + len(self.background)

    @property
    def aspect(self):
        return self.height * 100.0 / self.width

    @property
    def sensitive(self):
        return self.text in SENSITIVE


class Cursor:
    def __init__(self, data, pos=0):
        self.data = data
        self.pos = pos

    def u8(self):
        value = self.data[self.pos]
        self.pos += 1
        return value

    def u16(self):
        value = int.from_bytes(self.data[self.pos:self.pos + 2], "big")
        self.pos += 2
        return value

    def take(self, count):
        value = self.data[self.pos:self.pos + count]
        self.pos += count
        return value


def read_lines(cursor, short):
    count = cursor.u8() if short else cursor.u16()
    lines = []
    for _ in range(count):
        if short:
            x1, y1, x2, y2 = cursor.take(4)
        else:
            x1, y1, x2, y2 = cursor.u16(), cursor.u16(), cursor.u16(), cursor.u16()
        lines.append(OcrLine(OcrPoint(x1, y1), OcrPoint(x2, y2)))
    return lines


def read_glyph(cursor, version):
    """V1 is width/height/margin(2 bytes each), italic, expand, then text and lines.

    V2: flags 0x10 = short (1-byte size/margin + 1-byte line count and coords),
    0x20 = italic, low nibble = expand count. Text is a byte length followed by UTF-8.
    Lines are an explicit count of (start, end) segments, not sentinel-terminated lists.
    """
    if version == 2:
        flags = cursor.u8()
        short = bool(flags & 0x10)
        italic = bool(flags & 0x20)
        expand = flags & 0x0F
        width = cursor.u8() if short else cursor.u16()
        height = cursor.u8() if short else cursor.u16()
        margin = cursor.u8() if short else cursor.u16()
    else:
        short = False
        width, height, margin = cursor.u16(), cursor.u16(), cursor.u16()
        italic = cursor.u8() != 0
        expand = cursor.u8()
    text = cursor.take(cursor.u8()).decode("utf-8", "replace")
    foreground = read_lines(cursor, short)
    background = read_lines(cursor, short)
    if not (0 < width <= 1920 and 0 < height <= 1080 and "\0" not in text):
        return None
    return Glyph(text, width, height, margin, italic, expand, foreground, background)


def load_db(path):
    raw = path.read_bytes()
    if raw[:2] == b"\x1f\x8b":
        raw = gzip.decompress(raw)
    version = 2 if raw[:2] == b"V2" else 1
    cursor = Cursor(raw, 2 if version == 2 else 0)
    glyphs = []
    while cursor.pos + 4 < len(raw):
        glyph = read_glyph(cursor, version)
        if glyph is None:
            break
        glyphs.append(glyph)
    return version, glyphs


def fetch_db(path):
    if path.is_file():
        return path
    path.parent.mkdir(parents=True, exist_ok=True)
    request = urllib.request.Request(DB_URL, headers={"User-Agent": "elements-extract-nocr-poc/1.0"})
    with urllib.request.urlopen(request, timeout=60) as response:
        payload = response.read()
    path.write_bytes(payload)
    return path


# --------------------------------------------------------------------- matcher

class MatchPass:
    def __init__(self, aspect=math.inf, size=math.inf, margin=math.inf,
                 min_lines=0, sensitivity=0, allowance=0, errors=lambda _: 0,
                 deep_seek=False):
        self.aspect, self.size, self.margin = aspect, size, margin
        self.min_lines, self.sensitivity, self.allowance = min_lines, sensitivity, allowance
        self.errors, self.deep_seek = errors, deep_seek


MATCH_PASSES = [
    MatchPass(aspect=15, size=5, margin=5),
    MatchPass(allowance=1, size=4, margin=8, errors=lambda _: 1),
    MatchPass(allowance=1, size=8, margin=8, errors=lambda _: 1),
    MatchPass(allowance=2, aspect=20, margin=15, errors=lambda m: min(3, m)),
    MatchPass(allowance=10, aspect=20, margin=15, min_lines=41, sensitivity=-1,
              errors=lambda m: min(20, m)),
    MatchPass(allowance=10, aspect=30, margin=15, min_lines=41, sensitivity=1,
              errors=lambda _: 10),
    MatchPass(deep_seek=True, aspect=60, margin=17, min_lines=51, errors=lambda m: m),
]


class Db:
    def __init__(self, version, glyphs):
        self.version = version
        self.glyphs = [g for g in glyphs if g.expand == 0]
        self.expanded = [g for g in glyphs if g.expand > 0]
        self.by_size = defaultdict(list)
        for glyph in self.glyphs:
            self.by_size[(glyph.width << 16) | glyph.height].append(glyph)

    def match(self, mask, top_margin, max_wrong=25, deep_seek=False):
        exact = self.by_size.get((mask.shape[1] << 16) | mask.shape[0], ())
        for glyph in exact:
            if abs(glyph.margin_top - top_margin) < 5 and match_errors(mask, glyph, 0) >= 0:
                return glyph
        width, height = mask.shape[1], mask.shape[0]
        aspect = height * 100.0 / width
        for p in MATCH_PASSES:
            if p.deep_seek and not deep_seek:
                continue
            if max_wrong < p.allowance:
                continue
            budget = min(p.errors(max_wrong), 40)
            best, best_errors, best_fit = None, math.inf, math.inf
            for glyph in self.glyphs:
                if p.aspect != math.inf and abs(aspect - glyph.aspect) >= p.aspect:
                    continue
                if p.size != math.inf and (abs(width - glyph.width) >= p.size or
                                           abs(height - glyph.height) >= p.size):
                    continue
                if abs(glyph.margin_top - top_margin) >= p.margin:
                    continue
                if p.min_lines and glyph.line_count < p.min_lines:
                    continue
                if p.sensitivity == -1 and glyph.sensitive:
                    continue
                if p.sensitivity == 1 and not glyph.sensitive:
                    continue
                errors = match_errors(mask, glyph, min(budget, best_errors))
                if errors < 0:
                    continue
                fit = (abs(glyph.margin_top - top_margin) +
                       abs(glyph.height - height) + abs(glyph.width - width))
                if errors < best_errors or (errors == best_errors and fit < best_fit):
                    best, best_errors, best_fit = glyph, errors, fit
            if best is not None:
                return best
        return None

    def best_variant_error(self, mask, top_margin, text, italic, max_wrong=25):
        best = -1
        for glyph in self.glyphs:
            if glyph.italic != italic or glyph.text != text:
                continue
            budget = max_wrong if best < 0 else best
            errors = match_errors(mask, glyph, budget)
            if errors >= 0 and (best < 0 or errors < best):
                best = errors
        return best


def match_errors(mask, glyph, allowed):
    if glyph.line_count < 1:
        return -1
    height, width = mask.shape
    errors = 0
    for lines, expected in ((glyph.foreground, True), (glyph.background, False)):
        for line in lines:
            for x, y in line.scaled_points(glyph, width, height):
                if 0 <= x < width and 0 <= y < height and bool(mask[y, x]) != expected:
                    errors += 1
                    if errors > allowed:
                        return -1
    return errors


# ---------------------------------------------------------- PGS sample generator

def rle_encode(indices, width, height):
    out = bytearray()

    def emit_run(color, run):
        if run <= 0:
            return
        if color:
            out.extend([color] * run)
            return
        while run > 0:
            chunk = min(run, 0x3FFF)
            if chunk > 63:
                out.extend((0, 0x40 | (chunk >> 8), chunk & 0xFF))
            else:
                out.extend((0, chunk))
            run -= chunk

    for y in range(height):
        row = indices[y * width:(y + 1) * width]
        x = 0
        while x < width:
            color = int(row[x])
            end = x + 1
            if color == 0:
                while end < width and row[end] == 0:
                    end += 1
            emit_run(color, end - x)
            x = end
        if y < height - 1:
            out.extend((0, 0))
    return bytes(out)


def rle_decode(data, width, height):
    pixels = np.zeros(width * height, dtype=np.uint8)
    x = y = pos = 0
    while pos < len(data) and y < height:
        first = data[pos]
        pos += 1
        if first:
            if x < width:
                pixels[y * width + x] = first
                x += 1
            continue
        if pos >= len(data):
            break
        flags = data[pos]
        pos += 1
        if flags == 0:
            x, y = 0, y + 1
            continue
        run = flags & 0x3F
        color = 0
        if flags & 0x40:
            run = (run << 8) | data[pos]
            pos += 1
        if flags & 0x80:
            color = data[pos]
            pos += 1
        for _ in range(run):
            if x >= width:
                break
            pixels[y * width + x] = color
            x += 1
    return pixels.reshape(height, width)


def pgs_segment(kind, payload):
    return bytes((kind, len(payload) >> 8, len(payload) & 0xFF)) + payload


def pcs(width, height, number, state, objects, palette_id=0):
    body = struct.pack(">HHBHBBBB", width, height, 0, number, state, 0, palette_id, len(objects))
    for obj_id, window_id, x, y in objects:
        body += struct.pack(">HBBHH", obj_id, window_id, 0, x, y)
    return pgs_segment(SEG_PCS, body)


def pds(palette_id=0):
    # YCbCr limited range; alpha is 8-bit in PGS (the 2-bit alpha belongs to VobSub).
    entries = bytes((0, 16, 128, 128, 0, 1, 235, 128, 128, 255))
    return pgs_segment(SEG_PDS, bytes((palette_id, 1)) + entries)


def ods(obj_id, indices, width, height):
    rle = rle_encode(indices, width, height)
    header = struct.pack(">HBB", obj_id, 1, 0x80) + (len(rle) + 4).to_bytes(3, "big")
    header += struct.pack(">HH", width, height)
    return pgs_segment(SEG_ODS, header + rle)


def wds(window_id, x, y, width, height):
    return pgs_segment(SEG_WDS, struct.pack(">BBHHHH", window_id, 1, x, y, width, height))


def pg_packet(pts, pgs_segment_bytes):
    # ffmpeg's sup demuxer reads exactly one segment after each 10-byte PG header.
    return b"PG" + struct.pack(">II", pts, 0) + pgs_segment_bytes


def render_line(text, font_path, size):
    font = ImageFont.truetype(font_path, size)
    probe = ImageDraw.Draw(Image.new("L", (1, 1)))
    box = probe.textbbox((0, 0), text, font=font)
    width, height = box[2] - box[0] + 8, box[3] - box[1] + 8
    image = Image.new("L", (width, height), 0)
    ImageDraw.Draw(image).text((4 - box[0], 4 - box[1]), text, font=font, fill=255)
    return image


def make_sample(args):
    out = Path(args.out)
    out.mkdir(parents=True, exist_ok=True)
    width, height = 1920, 1080
    cues = [(1.0, 3.0, "Hello world."), (4.0, 6.0, "This is a test.")]
    packets = []
    for number, (start, end, text) in enumerate(cues, 1):
        gray = render_line(text, args.font, args.font_size)
        indices = np.array(gray, dtype=np.uint8) // 255
        obj_w, obj_h = gray.size
        x, y = (width - obj_w) // 2, int(height * 0.8)
        display_set = [pcs(width, height, number, 0x40, [(number, 0, x, y)]),
                       pds(), ods(number, indices.ravel(), obj_w, obj_h),
                       wds(0, x, y, obj_w, obj_h), pgs_segment(SEG_END, b"")]
        packets.extend(pg_packet(int(start * PTS_PER_SECOND), part) for part in display_set)
        gray.save(out / f"sample_frame_{number:02d}.png")
    display_set = [pcs(width, height, 99, 0x40, []), pds(), pgs_segment(SEG_END, b"")]
    packets.extend(pg_packet(int(6.0 * PTS_PER_SECOND), part) for part in display_set)
    sup = out / "sample.sup"
    sup.write_bytes(b"".join(packets))

    def stamp(seconds):
        ms = int(round(seconds * 1000))
        return f"{ms // 3600000:02d}:{ms // 60000 % 60:02d}:{ms // 1000 % 60:02d},{ms % 1000:03d}"

    srt = "".join(f"{i}\n{stamp(start)} --> {stamp(end)}\n{text}\n\n"
                  for i, (start, end, text) in enumerate(cues, 1))
    reference = out / "sample.reference.srt"
    reference.write_text(srt, encoding="utf-8")
    print(f"PGS:      {sup} ({sup.stat().st_size} bytes, {len(packets)} packets)")
    print(f"Reference:{reference}")
    print(f"PNGs:     {out}/sample_frame_*.png")


# ------------------------------------------------------------------ PGS reading

@dataclass
class Frame:
    pts: int
    end_pts: int
    rgba: np.ndarray
    source: str


def decode_pgs(path):
    raw = Path(path).read_bytes()
    packets, pos = [], 0
    while pos + 13 <= len(raw):
        if raw[pos:pos + 2] != b"PG":
            raise ValueError(f"bad PG magic at byte {pos}")
        pts = int.from_bytes(raw[pos + 2:pos + 6], "big")
        kind = raw[pos + 10]
        length = int.from_bytes(raw[pos + 11:pos + 13], "big")
        pos += 13
        if pos + length > len(raw):
            raise ValueError(f"truncated PGS segment at byte {pos}")
        packets.append((pts, kind, raw[pos:pos + length]))
        pos += length
    groups = []
    for packet in packets:
        if not groups or groups[-1][0] != packet[0]:
            groups.append((packet[0], []))
        groups[-1][1].append(packet[1:])

    frames = []
    for pts, segments in groups:
        palette, objects, composition = {}, {}, []
        for kind, payload in segments:
            if kind == SEG_PDS:
                for i in range(2, len(payload) - 4, 5):
                    index, yv, cr, cb, alpha = payload[i:i + 5]
                    palette[index] = yuv_to_rgb(yv, cr, cb) + (alpha,)
            elif kind == SEG_ODS:
                obj_id = int.from_bytes(payload[:2], "big")
                if payload[3] & 0x80:
                    obj_w = int.from_bytes(payload[7:9], "big")
                    obj_h = int.from_bytes(payload[9:11], "big")
                    objects[obj_id] = rle_decode(payload[11:], obj_w, obj_h)
            elif kind == SEG_PCS:
                composition = []
                count = payload[10]
                for i in range(count):
                    off = 11 + i * 8
                    composition.append((int.from_bytes(payload[off:off + 2], "big"),
                                        int.from_bytes(payload[off + 4:off + 6], "big"),
                                        int.from_bytes(payload[off + 6:off + 8], "big")))
        if not composition:
            for frame in frames:
                if frame.end_pts <= frame.pts:
                    frame.end_pts = pts
            continue
        parts = []
        for obj_id, x, y in composition:
            if obj_id not in objects:
                continue
            indices = objects[obj_id]
            h, w = indices.shape
            rgba = np.zeros((h, w, 4), dtype=np.uint8)
            for index, color in palette.items():
                rgba[indices == index] = color
            parts.append((x, y, rgba))
        if not parts:
            continue
        min_x = min(p[0] for p in parts)
        min_y = min(p[1] for p in parts)
        max_x = max(p[0] + p[2].shape[1] for p in parts)
        max_y = max(p[1] + p[2].shape[0] for p in parts)
        canvas = np.zeros((max_y - min_y, max_x - min_x, 4), dtype=np.uint8)
        for x, y, rgba in parts:
            h, w = rgba.shape[:2]
            canvas[y - min_y:y - min_y + h, x - min_x:x - min_x + w] = rgba
        frames.append(Frame(pts, pts + 1, canvas, f"PGS packet @ {pts / PTS_PER_SECOND:.2f}s"))
    for i, frame in enumerate(frames):
        if frame.end_pts <= frame.pts:
            frame.end_pts = frames[i + 1].pts if i + 1 < len(frames) else frame.pts + PTS_PER_SECOND
    return frames


def yuv_to_rgb(y, cr, cb):
    c, d, e = y - 16, cb - 128, cr - 128
    r = (298 * c + 459 * e + 128) >> 8
    g = (298 * c - 55 * d - 136 * e + 128) >> 8
    b = (298 * c + 541 * d + 128) >> 8
    return tuple(max(0, min(255, v)) for v in (r, g, b))


def load_png(path):
    image = Image.open(path)
    rgba = np.array(image.convert("RGBA"))
    alpha = rgba[:, :, 3]
    if alpha.min() >= 150:
        gray = np.array(image.convert("L"))
        bright = int((gray > 128).sum())
        dark = int((gray < 128).sum())
        mask = gray > 128 if bright < dark else gray < 128
        rgba[:, :, 3] = mask.astype(np.uint8) * 255
    return [Frame(0, 5 * PTS_PER_SECOND, rgba, str(path))]


# --------------------------------------------------------------- segmentation

@dataclass
class Item:
    mask: np.ndarray
    x: int
    top_margin: int
    gap_before: int
    line_height: int
    new_line: bool


def bands(has_content, length, min_size=1):
    start, spans = -1, []
    for i in range(length):
        if has_content(i):
            if start < 0:
                start = i
        elif start >= 0:
            if i - start >= min_size:
                spans.append((start, i - 1))
            start = -1
    if start >= 0 and length - start >= min_size:
        spans.append((start, length - 1))
    return spans


def segment(mask, min_line_height=2):
    items = []
    for top, bottom in bands(lambda y: bool(mask[y].any()), mask.shape[0], min_line_height):
        previous_right, first = 0, True
        for left, right in bands(lambda x: bool(mask[top:bottom + 1, x].any()), mask.shape[1]):
            t, b = top, bottom
            while t < b and not mask[t, left:right + 1].any():
                t += 1
            while b > t and not mask[b, left:right + 1].any():
                b -= 1
            items.append(Item(mask[t:b + 1, left:right + 1], left, t - top,
                              -1 if first else left - previous_right - 1,
                              bottom - top + 1, first))
            previous_right, first = right, False
    return items


# ---------------------------------------------------------------- recognition

def median(values):
    return int(statistics.median(values)) if values else 0


def recognize(db, frame, max_wrong=25, deep_seek=True, space_gap_factor=0.25, space_min_gap=6):
    alpha = frame.rgba[:, :, 3]
    mask = alpha > 150
    if not mask.any():
        return "", 0, 0
    items = segment(mask)
    matched = []
    unknowns = 0
    for item in items:
        glyph = db.match(item.mask, item.top_margin,
                         max_wrong=max_wrong, deep_seek=deep_seek)
        if glyph is None:
            unknowns += 1
        matched.append((item, glyph))

    x_heights = [it.mask.shape[0] for it, g in matched
                 if g and g.text in X_HEIGHT_LETTERS]
    cap_heights = [it.mask.shape[0] for it, g in matched
                   if g and g.text in TALL_LETTERS]
    x_height, cap_height = median(x_heights), median(cap_heights)
    can_judge = x_height > 0 and cap_height > x_height

    text = []
    for item, glyph in matched:
        if item.new_line and text:
            text.append("\n")
        elif item.gap_before >= max(space_min_gap, round(space_gap_factor * item.line_height)):
            text.append(" ")
        if glyph is None:
            text.append("□")
            continue
        value = glyph.text
        if can_judge and len(value) == 1 and value.lower() in SIZE_TWINS:
            height = item.mask.shape[0]
            value = value.upper() if abs(height - cap_height) < abs(height - x_height) else value.lower()
        text.append(value)
    return "".join(text), len(items), unknowns


# ---------------------------------------------------------------- SRT helpers

def parse_srt(path):
    cues = []
    blocks = Path(path).read_text(encoding="utf-8-sig").strip().split("\n\n")
    for block in blocks:
        lines = block.splitlines()
        if len(lines) < 2 or "-->" not in lines[1]:
            continue
        start, end = lines[1].split("-->")
        def to_ticks(value):
            hh, mm, rest = value.strip().replace(",", ".").split(":")
            return int((int(hh) * 3600 + int(mm) * 60 + float(rest)) * PTS_PER_SECOND)
        cues.append((to_ticks(start), to_ticks(end), " ".join(lines[2:])))
    return cues


def stamp(pts):
    ms = pts * 1000 // PTS_PER_SECOND
    return f"{ms // 3600000:02d}:{ms // 60000 % 60:02d}:{ms // 1000 % 60:02d},{ms % 1000:03d}"


def write_srt(path, cues):
    content = "".join(f"{i}\n{stamp(start)} --> {stamp(end)}\n{text}\n\n"
                      for i, (start, end, text) in enumerate(cues, 1))
    Path(path).write_text(content, encoding="utf-8")


def accuracy(reference, hypothesis):
    ref, hyp = reference.strip(), hypothesis.strip()
    matcher = difflib.SequenceMatcher(None, ref, hyp, autojunk=False)
    matches = sum(block.size for block in matcher.get_matching_blocks())
    confused = []
    for tag, i1, i2, j1, j2 in matcher.get_opcodes():
        if tag == "replace":
            confused.extend(zip(ref[i1:i2], hyp[j1:j2]))
        elif tag == "delete":
            confused.extend((c, "") for c in ref[i1:i2])
        elif tag == "insert":
            confused.extend(("", c) for c in hyp[j1:j2])
    return matches / max(len(ref), len(hyp), 1), matches, confused


# ----------------------------------------------------------------------- CLI

def load_db_command(args):
    path = fetch_db(args.db)
    version, glyphs = load_db(path)
    plain = [g for g in glyphs if not g.expand]
    expanded = [g for g in glyphs if g.expand]
    widths = Counter(g.width for g in plain)
    heights = Counter(g.height for g in plain)
    chars = "".join(sorted(set(c for g in plain for c in g.text if c.isprintable())))
    print(f"file:       {path} ({path.stat().st_size} bytes compressed)")
    print(f"version:    V{version}")
    print(f"glyphs:     {len(glyphs)} ({len(plain)} plain, {len(expanded)} expanded)")
    print(f"italic:     {sum(g.italic for g in glyphs)}")
    print(f"width mode: {widths.most_common(5)}")
    print(f"height mode:{heights.most_common(5)}")
    print(f"characters ({len(chars)}): {chars}")


def run_command(args):
    db_path = fetch_db(args.db)
    version, glyphs = load_db(db_path)
    db = Db(version, glyphs)
    frames = decode_pgs(args.input) if args.input.lower().endswith(".sup") else load_png(args.input)
    reference_cues = parse_srt(args.reference) if args.reference else []
    cues = []
    total_matches = total_ref_chars = 0
    all_confused = []
    for index, frame in enumerate(frames):
        text, glyph_count, unknowns = recognize(
            db, frame, max_wrong=args.max_wrong, deep_seek=args.deep_seek,
            space_gap_factor=args.space_gap_factor)
        if reference_cues and index < len(reference_cues):
            start, end, expected = reference_cues[index]
            score, matches, confused = accuracy(expected, text)
            total_matches += matches
            total_ref_chars += len(expected.strip())
            all_confused += confused
            print(f"frame {index + 1}: {text!r}  glyphs={glyph_count} unknown={unknowns} "
                  f"accuracy={score * 100:.1f}%")
            print(f"  reference: {expected!r}")
            if confused:
                print(f"  confused:  {confused}")
        else:
            start, end = frame.pts, frame.end_pts
            print(f"frame {index + 1}: {text!r}  glyphs={glyph_count} unknown={unknowns}")
        cues.append((start, end, text))
    if not reference_cues:
        print("accuracy:   n/a (no --reference)")
    else:
        total_hyp_chars = sum(len(text.strip()) for _, _, text in cues)
        print(f"accuracy:   {100 * total_matches / max(total_ref_chars, total_hyp_chars):.1f}% "
              f"({total_matches} matched / {total_ref_chars} reference chars, case-sensitive, "
              "insertions penalized)")
        if all_confused:
            print(f"confusions: {Counter(all_confused).most_common()}")
    write_srt(args.out, cues)
    print(f"SRT:        {args.out}")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="command", required=True)

    sample = sub.add_parser("make-sample", help="generate a valid .sup, a reference .srt and PNGs")
    sample.add_argument("--out", default="/tmp/ocr_poc")
    sample.add_argument("--font", default=DEFAULT_FONT)
    sample.add_argument("--font-size", type=int, default=48)
    sample.set_defaults(func=make_sample)

    db = sub.add_parser("load-db", help="download and parse an nOCR database (gzip supported)")
    db.add_argument("--db", type=Path, default=DEFAULT_DB)
    db.set_defaults(func=load_db_command)

    run = sub.add_parser("run", help="recognize text from a .sup or PNG")
    run.add_argument("--input", required=True)
    run.add_argument("--db", type=Path, default=DEFAULT_DB)
    run.add_argument("--reference", help="reference SRT used for timings and accuracy")
    run.add_argument("--out", default="/tmp/ocr_poc/output.ocr.srt")
    run.add_argument("--max-wrong", type=int, default=25)
    run.add_argument("--space-gap-factor", type=float, default=0.25)
    run.add_argument("--no-deep-seek", dest="deep_seek", action="store_false")
    run.set_defaults(deep_seek=True)
    run.set_defaults(func=run_command)

    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    try:
        main()
    except (ValueError, OSError) as exc:
        sys.exit(f"error: {exc}")
