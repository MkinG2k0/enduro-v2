/**
 * Enduro wiring diagrams — contiguous pins per device (FW 1.5.1+).
 * Right edge: bike harness D0..D9 | Left edge: TFT A2..A0 + SPI 15/16/10
 */
const fs = require('fs');
const path = require('path');

const esc = (s) =>
  String(s).replace(/[&<>"']/g, (c) => ({
    '&': '&amp;',
    '<': '&lt;',
    '>': '&gt;',
    '"': '&quot;',
    "'": '&apos;',
  })[c]);

const pages = [];
let cells, svg, n;

function page(name, subtitle) {
  cells = [];
  svg = [];
  n = 2;
  pages.push({ name, cells, svg });
  box(20, 15, 1160, 48, name, '#15334a', '#ffffff', 24);
  note(25, 70, 1150, 45, subtitle);
}

function box(x, y, w, h, label, fill = '#ffffff', stroke = '#334155', size = 15) {
  const id = n++;
  const fontColor = fill === '#15334a' ? '#ffffff' : '#172b3a';
  cells.push(
    `<mxCell id="${id}" value="${esc(label)}" style="rounded=0;whiteSpace=wrap;html=0;fillColor=${fill};strokeColor=${stroke};fontColor=${fontColor};fontSize=${size};" vertex="1" parent="1"><mxGeometry x="${x}" y="${y}" width="${w}" height="${h}" as="geometry"/></mxCell>`
  );
  svg.push(`<rect x="${x}" y="${y}" width="${w}" height="${h}" fill="${fill}" stroke="${stroke}"/>`);
  label.split('\n').forEach((t, i, a) => {
    const dy = (i - (a.length - 1) / 2) * (size + 5) + size * 0.35;
    svg.push(
      `<text x="${x + w / 2}" y="${y + h / 2 + dy}" text-anchor="middle" font-family="Arial,sans-serif" font-size="${size}" fill="${fill === '#15334a' ? '#fff' : '#172b3a'}">${esc(t)}</text>`
    );
  });
  return id;
}

function note(x, y, w, h, t) {
  box(x, y, w, h, t, '#f1f5f9', '#cbd5e1', 14);
}

function wire(x1, y1, x2, y2, color = '#047857', dashed = false) {
  const id = n++;
  cells.push(
    `<mxCell id="${id}" style="endArrow=none;startArrow=none;strokeColor=${color};strokeWidth=2;${dashed ? 'dashed=1;' : ''}" edge="1" parent="1"><mxGeometry relative="1" as="geometry"><mxPoint x="${x1}" y="${y1}" as="sourcePoint"/><mxPoint x="${x2}" y="${y2}" as="targetPoint"/></mxGeometry></mxCell>`
  );
  svg.push(
    `<path d="M${x1} ${y1} L${x2} ${y2}" stroke="${color}" stroke-width="2" ${dashed ? 'stroke-dasharray="7 5"' : ''} fill="none"/>`
  );
}

// ─── 01 Overview ─────────────────────────────────────────────
page(
  '01 / Обзор системы',
  'Enduro-v2 • FW 1.5.1-gpio-gear • Без MCP • правый край = байк D0..D9 • левый = TFT'
);
box(40, 140, 220, 80, 'Janxin M178A\nштатная приборка\nразъём 18 pin', '#dbeafe');
box(360, 140, 200, 80, 'PC817\nоптрон\n(12 V → 5 V)', '#fef3c7');
box(680, 140, 240, 80, 'Pro Micro\n5 V / 16 MHz\nATmega32U4', '#dcfce7');
box(960, 140, 180, 80, 'GC9A01\n240×240 SPI', '#e0e7ff');
wire(260, 180, 360, 180, '#64748b');
wire(560, 180, 680, 180, '#64748b');
wire(920, 180, 960, 180, '#2563eb');

box(40, 280, 1100, 50, 'Питание стенд: USB → Pro Micro 5V/GND → экран. Борт +12 V (M178 pin 3) пока не используется.', '#fff7ed');

const overview = [
  ['Передачи 1..6 + N', 'Жгут: D2..D7 = 1..6, D8 = N', 'зелёный'],
  ['Дальний (pin 4)', 'PC817 → D9 (сразу после Gear 6)', 'синий'],
  ['RPM / SPEED', 'PC817 → D0 / D1 (верх правого ряда)', 'фиолет / оранж'],
  ['Дисплей', 'Левый край: A0/A1/A2 + D15/D16/D10', 'индиго'],
];
overview.forEach((r, i) => {
  const y = 360 + i * 55;
  box(40, y, 280, 45, r[0], '#f8fafc');
  box(360, y, 780, 45, r[1], '#ffffff');
});

note(
  40,
  600,
  1100,
  70,
  'CHMCU-2317 / MCP23017 не используется. I2C свободен (можно позже).\nОбщая GND борт↔Arduino нужна для прямого чтения передач.'
);
note(
  40,
  690,
  1100,
  70,
  '+12 V (pin 3 зажигание, pin 4 дальний, лампы) — НИКОГДА напрямую на Pro Micro.\nТолько через PC817 или buck 12→5 (питание платы в будущем).'
);
note(
  40,
  780,
  1100,
  70,
  'Свободно: A3, D14 (дырка в ленте дисплея).\nЗанято: D0..D9 байк, A0..A2 + D10/D15/D16 TFT.'
);

// ─── 02 Gears ────────────────────────────────────────────────
page(
  '02 / M178 → передачи → правый край',
  'Один шлейф D2..D9: Gear1..6, N, HB. Замыкание на массу. LOW = активно.'
);
box(40, 140, 300, 45, 'M178 контакт / сигнал', '#dbeafe');
box(820, 140, 320, 45, 'Pro Micro GPIO', '#dcfce7');

box(40, 195, 1100, 36, 'Правый ряд платы — передачи подряд', '#bbf7d0');
const gears = [
  ['10 / Gear 1', 'D2'],
  ['11 / Gear 2', 'D3'],
  ['12 / Gear 3', 'D4'],
  ['13 / Gear 4', 'D5'],
  ['14 / Gear 5', 'D6'],
  ['15 / Gear 6', 'D7'],
  ['2 / Neutral N', 'D8'],
];
gears.forEach((r, i) => {
  const y = 245 + i * 48;
  box(40, y, 300, 40, r[0]);
  box(820, y, 320, 40, r[1], '#dcfce7');
  wire(340, y + 20, 820, y + 20, '#047857');
});

box(40, 590, 300, 40, '4 / Дальний +12 V', '#dbeafe');
box(400, 590, 280, 40, 'PC817 →', '#fef3c7');
box(820, 590, 320, 40, 'D9 High Beam', '#dbeafe');
wire(340, 610, 400, 610, '#2563eb');
wire(680, 610, 820, 610, '#2563eb');

box(40, 650, 300, 45, '1 / GND масса байка', '#e2e8f0');
box(820, 650, 320, 45, 'GND Pro Micro', '#e2e8f0');
wire(340, 672, 820, 672, '#334155');

note(
  40,
  720,
  1100,
  55,
  'Общая земля обязательна. Только сухой контакт на GND — не +12 V.\nHB на D9 сразу после Gear 6 — один жгут D2..D9.'
);
note(
  40,
  795,
  1100,
  55,
  'Прошивка: src/gear_inputs.h + high_beam.h — FW 1.5.1-gpio-gear.\nERR на экране = 0 или >1 линии в LOW.'
);
note(
  40,
  870,
  1100,
  50,
  'Выше по тому же краю: D0=RPM, D1=SPEED (через PC817).'
);

// ─── 03 Opto + TFT ───────────────────────────────────────────
page(
  '03 / PC817 (+12 V) и дисплей GC9A01',
  'Оптрон для RPM / SPEED / дальнего. Экран — левый край платы.'
);

box(40, 130, 250, 40, 'M178 / борт', '#dbeafe');
box(420, 130, 220, 40, 'PC817', '#fef3c7');
box(860, 130, 280, 40, 'Pro Micro', '#dcfce7');

const opto = [
  ['16 / RPM', 'CH1 IN → OUT', 'D0 INPUT_PULLUP', '#9333ea', false],
  ['9 / SPEED?', 'CH2 IN → OUT', 'D1 INPUT_PULLUP', '#d97706', true],
  ['4 / Дальний +12 V', 'CH3 IN → OUT', 'D9 INPUT_PULLUP', '#2563eb', false],
];
opto.forEach((r, i) => {
  const y = 190 + i * 70;
  box(40, y, 250, 50, r[0]);
  box(420, y, 220, 50, r[1], '#fef3c7');
  box(860, y, 280, 50, r[2]);
  wire(290, y + 25, 420, y + 25, r[3], r[4]);
  wire(640, y + 25, 860, y + 25, r[3], r[4]);
});

box(40, 420, 250, 50, '1 / GND борт', '#e2e8f0');
box(420, 420, 220, 50, 'GND_in модуля', '#e2e8f0');
wire(290, 445, 420, 445, '#334155');
box(860, 420, 280, 50, 'GND_out → GND Arduino', '#e2e8f0');
wire(640, 445, 860, 445, '#334155');

note(
  40,
  500,
  1100,
  100,
  'GC9A01 (левый край сверху↓): VCC→5V, A2→BLK, A1→RES, A0→DC,\nD15→SCL, D14=NC, D16→SDA, D10→CS, GND→GND.\nSPI 8 MHz. BLK = digital HIGH (на A2 нет PWM у 32U4).'
);
note(
  40,
  620,
  1100,
  65,
  'PC817: сторона мотоцикла и сторона Arduino связаны только светом.\nДля передач (прямые GPIO) общая GND — развязки нет.'
);
note(
  40,
  705,
  1100,
  65,
  'SPEED pin 9 — гипотеза. RPM_PULSES_PER_REV в rpm.h = 1.0 до калибровки.\nНе подключать: CHMCU/MCP, MPU-6050.'
);
note(
  40,
  790,
  1100,
  55,
  'Файлы: docs/wiring-enduro-v3.drawio • docs/wiring-blocks-dark.drawio • HARDWARE.md'
);

// ─── 04 Power pin map ────────────────────────────────────────
page(
  '04 / Карта пинов Pro Micro',
  'Правый край = байк • Левый край = TFT • 5V/16MHz'
);

const pinMap = [
  ['D0', 'RPM ← PC817', '#ede9fe'],
  ['D1', 'SPEED ← PC817', '#ffedd5'],
  ['D2', 'Gear 1', '#dcfce7'],
  ['D3', 'Gear 2', '#dcfce7'],
  ['D4', 'Gear 3', '#dcfce7'],
  ['D5', 'Gear 4', '#dcfce7'],
  ['D6', 'Gear 5', '#dcfce7'],
  ['D7', 'Gear 6', '#dcfce7'],
  ['D8', 'Neutral', '#dcfce7'],
  ['D9', 'High Beam ← PC817', '#dbeafe'],
  ['D10', 'TFT CS', '#e0e7ff'],
  ['D14', 'NC (дырка ленты)', '#f1f5f9'],
  ['D15', 'TFT SCK', '#e0e7ff'],
  ['D16', 'TFT MOSI', '#e0e7ff'],
  ['A0', 'TFT DC', '#e0e7ff'],
  ['A1', 'TFT RES', '#e0e7ff'],
  ['A2', 'TFT BLK (digital ON)', '#e0e7ff'],
  ['A3', 'свободен', '#f1f5f9'],
  ['5V', 'питание TFT (+ USB)', '#fee2e2'],
  ['GND', 'байк + TFT + PC817 out', '#e2e8f0'],
];
pinMap.forEach((r, i) => {
  const col = i < 10 ? 0 : 1;
  const row = i % 10;
  const x = 40 + col * 580;
  const y = 140 + row * 55;
  box(x, y, 120, 45, r[0], '#15334a', '#ffffff', 16);
  box(x + 130, y, 400, 45, r[1], r[2]);
});

note(
  40,
  720,
  1100,
  70,
  'RAW не для 5V логики. USB питает стенд.\nПозже: M178 pin 3 (+12 V) → buck 12→5 → шина 5V.'
);
note(
  40,
  810,
  1100,
  70,
  'Сборка: pio run -e promicro -t upload\nЖгуты: 1) правый D0..D9 байк  2) левый TFT'
);

const out = path.resolve(__dirname, '../docs');
const drawioName = 'wiring-enduro-v3.drawio';
fs.writeFileSync(
  path.join(out, drawioName),
  `<?xml version="1.0" encoding="UTF-8"?><mxfile host="app.diagrams.net" version="24.7.17">${pages
    .map(
      (p, i) =>
        `<diagram id="enduro-${i + 1}" name="${esc(p.name)}"><mxGraphModel dx="1200" dy="1020" grid="1" gridSize="10" page="1" pageScale="1" pageWidth="1200" pageHeight="1020"><root><mxCell id="0"/><mxCell id="1" parent="0"/>${p.cells.join('')}</root></mxGraphModel></diagram>`
    )
    .join('')}</mxfile>`
);
pages.forEach((p, i) =>
  fs.writeFileSync(
    path.join(out, `wiring-enduro-v3-${i + 1}.svg`),
    `<svg xmlns="http://www.w3.org/2000/svg" width="1200" height="1020" viewBox="0 0 1200 1020"><rect width="1200" height="1020" fill="#fff"/>${p.svg.join('')}</svg>`
  )
);
fs.copyFileSync(path.join(out, drawioName), path.join(out, 'wiring-m178-v2.drawio'));
console.log('Generated', drawioName, '+', pages.length, 'SVG pages');
