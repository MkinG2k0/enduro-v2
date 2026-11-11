# Enduro-v2

Кастомная приборка мотоцикла: **Arduino Pro Micro 5V/16 MHz** + **GC9A01** + **CHMCU-2317 (MCP23017)** + сигналы штатной панели **Janxin M178A-005-1P**.

Подробная распиновка: **[docs/HARDWARE.md](docs/HARDWARE.md)**.  
Схемы (draw.io):  
- блоки со стрелками (тёмная): **[docs/wiring-blocks-dark.drawio](docs/wiring-blocks-dark.drawio)**  
- подробная: **[docs/wiring-enduro-v3.drawio](docs/wiring-enduro-v3.drawio)**  
Открыть в [app.diagrams.net](https://app.diagrams.net) → File → Open from → Device.  
Фото разъёма M178: **[docs/M178-CONNECTOR.md](docs/M178-CONNECTOR.md)**.

## Текущий статус прошивки

Rider UI **2.2.0**: дуга RPM + скорость + передача + HB.

**Автовыбор источника (без спец-флага):**
- USB, жгут передач не виден → **DEMO**
- На байке (любая линия N/1…6 = LOW) → **прод**, значения с датчиков
- Serial `LIVE`/`SET` — ручной override; `AUTO` — снова авто

## Build & upload

```bash
pio run -e promicro -t upload
pio device monitor -b 115200
```

Если плата 3.3V / 8 MHz:

```bash
pio run -e promicro8 -t upload
```

## USB Serial — управление с ПК

В мониторе (115200) или из `tools/ui-constructor.html` (Chrome + Web Serial):

| Команда | Действие |
|---------|----------|
| `HELP` | список команд |
| `AUTO` | авто: USB→DEMO / байк→сигналы |
| `DEMO 1` / `DEMO 0` | принудительное демо / вернуться в AUTO |
| `LIVE 1` / `LIVE 0` | ручные значения вместо датчиков |
| `SPEED 80` `RPM 6500` `GEAR N` `HB 1` | удержание значений |
| `SET S=72 R=6500 G=0 HB=0` | пакет значений |
| `UI rpmMax=12000 rOut=110 ...` | live UI-конфиг → мгновенно на экран |
| `UI GET` / `UI RESET` | прочитать / сбросить конфиг |

Live UI из браузера:

```bash
npx --yes serve D:/Project/Main/enduro-v2/tools -p 5173
```

Открыть http://localhost:5173/ui-constructor.html → **Connect** → крутить слайдеры.  
`fontId` меняется только перепрошивкой (`UI_FONT_ID` в `src/ui_config.h`).

## Краткая распиновка Pro Micro

| Устройство | Сигнал | Pin |
|------------|--------|-----|
| PC817 OUT1 | RPM (interrupt) | **D0** |
| PC817 OUT2 | SPEED (interrupt) | **D1** |
| GC9A01 | DC / RES / BLK / CS | **A0 / A1 / A2** / D10 |
| GC9A01 | SCK / MOSI | D15 / D16 |
| **D2..D7** | Gear 1..6 | switch → GND |
| **D8** | Neutral | switch → GND |
| PC817 → **D9** | High Beam (12 V) | через оптрон |

**RPM только на D0** — D2 занят SDA.

## Калибровка RPM

1. Завести двигатель, смотреть штатный тахо M178 и Serial/экран (Hz + RPM).
2. Точки: ~2000, 3000, 4000, 5000 RPM.
3. `RPM_PULSES_PER_REV ≈ (Hz × 60) / RPM_штатный` → прописать в `src/rpm.h`.
