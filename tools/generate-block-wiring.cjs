/**
 * One dark sheet: M178 | PC817 | Pro Micro | GC9A01
 * Each pin = one arrow between devices.
 */
const fs = require('fs');
const path = require('path');
const esc = (s) =>
  String(s).replace(/[&<>"']/g, (c) =>
    ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&apos;' }[c])
  );

const BG = '#1a1a1a';
const G = '#86efac';
const B = '#93c5fd';
const P = '#c4b5fd';
const O = '#fdba74';
const S = '#94a3b8';

let n = 2;
const cells = [];
const add = (x) => cells.push(x);

function box(x, y, w, h, title) {
  const id = n++;
  add(
    `<mxCell id="${id}" value="${esc(title)}" style="rounded=0;whiteSpace=wrap;html=1;fillColor=#111111;strokeColor=#ffffff;strokeWidth=2;fontColor=#ffffff;fontSize=18;fontStyle=1;align=center;verticalAlign=top;spacingTop=10;" vertex="1" parent="1"><mxGeometry x="${x}" y="${y}" width="${w}" height="${h}" as="geometry"/></mxCell>`
  );
  return { x, y, w, h };
}

function pin(bx, y, text, side) {
  const id = n++;
  const align = side === 'right' ? 'right' : 'left';
  add(
    `<mxCell id="${id}" value="${esc(text)}" style="text;html=1;strokeColor=none;fillColor=none;align=${align};verticalAlign=middle;fontColor=#ffffff;fontSize=13;fontStyle=1" vertex="1" parent="1"><mxGeometry x="${bx.x + 12}" y="${y}" width="${bx.w - 24}" height="24" as="geometry"/></mxCell>`
  );
}

function arrow(x1, y1, x2, y2, color) {
  const id = n++;
  add(
    `<mxCell id="${id}" style="endArrow=classic;html=1;strokeColor=${color};strokeWidth=2;endFill=1;fillColor=${color};" edge="1" parent="1"><mxGeometry relative="1" as="geometry"><mxPoint x="${x1}" y="${y1}" as="sourcePoint"/><mxPoint x="${x2}" y="${y2}" as="targetPoint"/></mxGeometry></mxCell>`
  );
}

function label(x, y, w, text, color = '#94a3b8') {
  const id = n++;
  add(
    `<mxCell id="${id}" value="${esc(text)}" style="text;html=1;strokeColor=none;fillColor=none;align=left;verticalAlign=middle;fontColor=${color};fontSize=12;" vertex="1" parent="1"><mxGeometry x="${x}" y="${y}" width="${w}" height="24" as="geometry"/></mxCell>`
  );
}

// Layout: 4 columns
const m178 = box(40, 50, 260, 900, 'M178');
const pc817 = box(380, 50, 220, 320, 'PC817');
const pro = box(680, 50, 240, 900, 'Pro Micro');
const tft = box(1000, 50, 240, 900, 'GC9A01');

label(40, 20, 1200, 'Enduro-v2 • один лист • пин → пин', '#64748b');

// --- Gears: M178 → Pro Micro (bypass PC817) ---
const gears = [
  ['pin1 GND', 'GND', S],
  ['pin10 G1', 'D2', G],
  ['pin11 G2', 'D3', G],
  ['pin12 G3', 'D4', G],
  ['pin13 G4', 'D5', G],
  ['pin14 G5', 'D6', G],
  ['pin15 G6', 'D7', G],
  ['pin2 N', 'D8', G],
];
gears.forEach((r, i) => {
  const y = 380 + i * 65;
  pin(m178, y, r[0], 'right');
  pin(pro, y, r[1], 'left');
  // route under PC817 box
  arrow(m178.x + m178.w, y + 12, pro.x, y + 12, r[2]);
});

// --- Opto path: M178 → PC817 → Pro Micro ---
const opto = [
  ['pin16 RPM', 'CH1', 'D0', P],
  ['pin9 SPEED?', 'CH2', 'D1', O],
  ['pin4 HB', 'CH3', 'D9', B],
];
opto.forEach((r, i) => {
  const y = 100 + i * 80;
  pin(m178, y, r[0], 'right');
  pin(pc817, y, r[1], 'left');
  pin(pro, y, r[2], 'left');
  arrow(m178.x + m178.w, y + 12, pc817.x, y + 12, r[3]);
  arrow(pc817.x + pc817.w, y + 12, pro.x, y + 12, r[3]);
});

// GND through opto module note
pin(pc817, 340, 'GND_in/out', 'left');
pin(m178, 340, 'pin1 → GND', 'right');
arrow(m178.x + m178.w, 352, pc817.x, 352, S);

// --- Display: Pro Micro → GC9A01 ---
const disp = [
  ['5V', 'VCC', B],
  ['A2', 'BLK', B],
  ['A1', 'RES', B],
  ['A0', 'DC', B],
  ['D15', 'SCL', B],
  ['D16', 'SDA', B],
  ['D10', 'CS', B],
  ['GND', 'GND', S],
];
disp.forEach((r, i) => {
  const y = 120 + i * 90;
  pin(pro, y, r[0], 'right');
  pin(tft, y, r[1], 'left');
  arrow(pro.x + pro.w, y + 12, tft.x, y + 12, r[2]);
});

label(380, 390, 280, '↓ передачи напрямую', '#64748b');
label(40, 960, 1200, 'Зелёный = передачи (сухой контакт). Фиолет/оранж/синий = через PC817. Голубой = SPI/TFT.', '#64748b');

const xml = `<?xml version="1.0" encoding="UTF-8"?><mxfile host="app.diagrams.net" version="24.7.17"><diagram id="all" name="Схема"><mxGraphModel dx="1600" dy="1100" grid="1" gridSize="10" page="1" pageScale="1" pageWidth="1280" pageHeight="1000" background="${BG}"><root><mxCell id="0"/><mxCell id="1" parent="0"/>${cells.join('')}</root></mxGraphModel></diagram></mxfile>`;

const out = path.resolve(__dirname, '../docs/wiring-blocks-dark.drawio');
fs.writeFileSync(out, xml);
console.log('Wrote', out);
