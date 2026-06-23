"""Generate Vantori/Inventory app launcher icons.

Renders the brand mark (a 3D box wireframe, matching the web logo) in white on
the indigo brand gradient. Outputs three assets used by flutter_launcher_icons:
  - app_icon.png            full-bleed gradient + glyph (iOS + Android legacy)
  - app_icon_foreground.png transparent + glyph in the adaptive safe zone
  - app_icon_background.png gradient only (adaptive background)

High quality via supersampling + LANCZOS downsample. Pure Pillow, no network.
"""
import os
from PIL import Image, ImageDraw

SS = 4                 # supersample factor
OUT = 1024             # final icon size
S = OUT * SS

C0 = (99, 102, 241)    # #6366F1 brand-500 (top-left)
C1 = (67, 56, 202)     # #4338CA brand-700 (bottom-right)
WHITE = (255, 255, 255)

# Brand mark geometry in a 24x24 design space (matches web Logo.tsx).
A = (12, 2); B = (3, 6.5); C = (3, 17.5)
D = (12, 22); E = (21, 17.5); F = (21, 6.5)
G = (12, 11)                     # front-top-center vertex
HEXAGON = [A, F, E, D, C, B, A]  # silhouette (closed)
SPOKES = [(B, G), (F, G), (D, G)]
VERTS = [A, B, C, D, E, F, G]


def gradient(size):
    """Smooth diagonal gradient via a tiny base image upscaled."""
    small = Image.new("RGB", (64, 64))
    px = small.load()
    for y in range(64):
        for x in range(64):
            t = (x + y) / (2 * 63)
            px[x, y] = tuple(int(C0[i] + (C1[i] - C0[i]) * t) for i in range(3))
    return small.resize((size, size), Image.BICUBIC)


def to_px(p, scale, cx, cy):
    return (cx + (p[0] - 12) * scale, cy + (p[1] - 12) * scale)


def draw_glyph(img, glyph_frac, color=WHITE):
    d = ImageDraw.Draw(img)
    size = img.size[0]
    scale = (glyph_frac * size) / 20.0        # design height is 20 units
    cx = cy = size / 2
    w = int(round(1.7 * scale))               # stroke width
    hexp = [to_px(p, scale, cx, cy) for p in HEXAGON]
    d.line(hexp, fill=color, width=w, joint="curve")
    for s in SPOKES:
        d.line([to_px(s[0], scale, cx, cy), to_px(s[1], scale, cx, cy)],
               fill=color, width=w)
    r = w / 2.0                                # round every cap/join
    for v in VERTS:
        x, y = to_px(v, scale, cx, cy)
        d.ellipse([x - r, y - r, x + r, y + r], fill=color)


os.makedirs("assets/icon", exist_ok=True)

# 1) Full icon — gradient + glyph (iOS App Store + Android legacy).
base = gradient(S)
draw_glyph(base, 0.56)
base.resize((OUT, OUT), Image.LANCZOS).save("assets/icon/app_icon.png")

# 2) Adaptive background — gradient only.
gradient(S).resize((OUT, OUT), Image.LANCZOS).save(
    "assets/icon/app_icon_background.png")

# 3) Adaptive foreground — transparent, glyph kept inside the safe zone.
fg = Image.new("RGBA", (S, S), (0, 0, 0, 0))
draw_glyph(fg, 0.42)
fg.resize((OUT, OUT), Image.LANCZOS).save("assets/icon/app_icon_foreground.png")

print("Generated:")
for f in ("app_icon.png", "app_icon_background.png", "app_icon_foreground.png"):
    im = Image.open(f"assets/icon/{f}")
    print(f"  assets/icon/{f}  {im.size[0]}x{im.size[1]} {im.mode}")
