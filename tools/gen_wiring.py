#!/usr/bin/env python3
"""Clear Enduro wiring card for Pro Micro — tables only, no wire spaghetti."""

from __future__ import annotations

import os
from PIL import Image, ImageDraw, ImageFont

DOCS = os.path.join(os.path.dirname(__file__), "..", "docs")
OUTS = [
    os.path.join(DOCS, "wiring-promicro-gc9a01.png"),
    os.path.join(DOCS, "wiring.png"),
]
W, H = 1400, 1680


def font(size: int):
    for path in (
        r"C:\Windows\Fonts\segoeui.ttf",
        r"C:\Windows\Fonts\arialbd.ttf",
        r"C:\Windows\Fonts\arial.ttf",
    ):
        if os.path.exists(path):
            return ImageFont.truetype(path, size)
    return ImageFont.load_default()


def main() -> None:
    img = Image.new("RGB", (W, H), "#f4f6f8")
    d = ImageDraw.Draw(img)

    f_title = font(40)
    f_sub = font(20)
    f_h = font(26)
    f_row = font(22)
    f_pin = font(22)
    f_tiny = font(16)

    d.rectangle((0, 0, W, 120), fill="#0f172a")
    d.text((40, 18), "ENDURO — распиновка", fill="#ffffff", font=f_title)
    d.text(
        (40, 68),
        "Плата: Pro Micro (ATmega32U4, 5V/16MHz)   |   экран GC9A01   |   CHMCU-2317",
        fill="#94a3b8",
        font=f_sub,
    )

    def panel(x, y, w, h, title, accent):
        d.rounded_rectangle((x, y, x + w, y + h), radius=18, fill="#ffffff", outline="#cbd5e1", width=2)
        d.rounded_rectangle((x, y, x + w, y + 56), radius=18, fill=accent)
        d.rectangle((x, y + 28, x + w, y + 56), fill=accent)
        d.text((x + 24, y + 12), title, fill="#ffffff", font=f_h)

    def row(x, y, left, right, note, stripe=False):
        if stripe:
            d.rectangle((x + 8, y - 4, x + 1312, y + 38), fill="#f1f5f9")
        d.text((x + 28, y), left, fill="#0f172a", font=f_row)
        d.text((x + 280, y), "→", fill="#64748b", font=f_row)
        d.text((x + 340, y), right, fill="#0369a1", font=f_pin)
        d.text((x + 560, y + 2), note, fill="#64748b", font=f_tiny)

    panel(40, 150, 1320, 520, "1) Экран GC9A01 (круглый 1.28\")  →  Pro Micro", "#b45309")
    d.text((68, 224), "Пин экрана", fill="#78716c", font=f_tiny)
    d.text((340, 224), "Пин Pro Micro", fill="#78716c", font=f_tiny)
    d.text((560, 224), "Назначение", fill="#78716c", font=f_tiny)
    d.line((64, 248, 1340, 248), fill="#e2e8f0", width=2)

    tft = [
        ("GND", "GND", "земля"),
        ("VCC", "VCC", "питание 5V"),
        ("SCL", "15  (SCK)", "SPI clock"),
        ("SDA", "16  (MOSI)", "SPI data"),
        ("RES", "8", "reset"),
        ("DC", "7", "data / command"),
        ("CS", "10", "chip select"),
        ("BLK", "9", "подсветка"),
    ]
    y = 265
    for i, (a, b, c) in enumerate(tft):
        row(40, y, a, b, c, stripe=(i % 2 == 0))
        y += 44

    panel(40, 700, 1320, 400, "2) CHMCU-2317 (MCP23017)  →  Pro Micro", "#0369a1")
    d.text((68, 774), "Пин модуля", fill="#64748b", font=f_tiny)
    d.text((340, 774), "Пин Pro Micro", fill="#64748b", font=f_tiny)
    d.text((560, 774), "Назначение", fill="#64748b", font=f_tiny)
    d.line((64, 798, 1340, 798), fill="#e2e8f0", width=2)

    mcp = [
        ("VCC", "5V", "питание 5V"),
        ("GND", "GND", "земля"),
        ("SCL / SCK", "3  (SCL)", "I2C clock"),
        ("SDA / SI", "2  (SDA)", "I2C data"),
        ("A0/A1/A2 addr", "GND", "адрес 0x20"),
    ]
    y = 815
    for i, (a, b, c) in enumerate(mcp):
        row(40, y, a, b, c, stripe=(i % 2 == 0))
        y += 44

    panel(40, 1140, 1320, 480, "3) Важно", "#14532d")
    tips = [
        "• Все GND вместе: Pro Micro + экран + CHMCU",
        "• Питание модулей с пина 5V на Pro Micro",
        "• Экран SCL/SDA = SPI на пинах 15 и 16",
        "• CHMCU SCL/SDA = I2C только на пинах 3 и 2",
        "• GPIO A0=N, A2..A7=передачи; +12V на MCP нельзя",
        "• Прошивка:  pio run -e promicro -t upload",
        "• Это НЕ Arduino Nano — у Pro Micro другие номера пинов",
    ]
    y = 1220
    for t in tips:
        d.text((70, y), t, fill="#0f172a", font=f_row)
        y += 48

    os.makedirs(DOCS, exist_ok=True)
    for out in OUTS:
        path = os.path.abspath(out)
        img.save(path, "PNG")
        print(path)

if __name__ == "__main__":
    main()
