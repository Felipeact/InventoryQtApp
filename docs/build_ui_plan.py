#!/usr/bin/env python3
"""Render the desktop UI redesign plan markdown into a styled PDF."""
import sys
import datetime
from pathlib import Path

import markdown
from pygments.formatters import HtmlFormatter
from weasyprint import HTML, CSS

HERE = Path(__file__).parent
MD_FILE = HERE / "Vantori-Desktop-UI-Redesign-Plan.md"
CSS_FILE = HERE / "style.css"
OUT_FILE = HERE / "Vantori-Desktop-UI-Redesign-Plan.pdf"

COVER = """
<div class="cover">
  <div class="glyph">◆</div>
  <h1>Vantori — Desktop UI/UX<br>Redesign Plan</h1>
  <div class="sub">An API‑driven, role‑aware redesign of the Qt desktop client, aligned with
  the Vantori web app: navigation, page‑by‑page layout, component library, user flows,
  and UX improvements.</div>
  <div class="pills">
    <span class="pill">Desktop · Qt / C++</span>
    <span class="pill">Aligned with · Next.js web</span>
    <span class="pill">Role‑aware · RBAC</span>
  </div>
  <div class="meta">
    <strong>Repository:</strong> felipeact/InventoryQtApp<br>
    <strong>Generated:</strong> {date} &nbsp;·&nbsp; <strong>Version:</strong> 1.0
  </div>
</div>
"""


def main() -> int:
    md_text = MD_FILE.read_text(encoding="utf-8")
    md = markdown.Markdown(
        extensions=["extra", "codehilite", "toc", "sane_lists", "admonition"],
        extension_configs={
            "codehilite": {"guess_lang": False, "noclasses": False},
            "toc": {"toc_depth": "1-3"},
        },
    )
    body_html = md.convert(md_text)
    toc_html = md.toc
    pygments_css = HtmlFormatter(style="monokai").get_style_defs(".codehilite")
    html = f"""<!DOCTYPE html>
<html lang="en"><head><meta charset="utf-8"></head><body>
{COVER.format(date=datetime.date.today().strftime("%B %d, %Y"))}
<div class="toc-page"><h1>Table of Contents</h1><div class="toc">{toc_html}</div></div>
{body_html}
</body></html>"""
    base_css = CSS_FILE.read_text(encoding="utf-8")
    HTML(string=html, base_url=str(HERE)).write_pdf(
        OUT_FILE, stylesheets=[CSS(string=base_css), CSS(string=pygments_css)]
    )
    print(f"Wrote {OUT_FILE} ({OUT_FILE.stat().st_size // 1024} KB)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
