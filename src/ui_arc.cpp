#include "ui_arc.h"

#include <math.h>

#include "ui_colors.h"

int arcDivRpm() {
  const int d = UI_RPM_DIV;
  return (d < 1) ? 1000 : d;
}

uint8_t arcKCount() {
  const int div = arcDivRpm();
  const int n = UI_RPM_MAX / div;
  if (n < 1) {
    return 1;
  }
  return (n > 255) ? 255 : (uint8_t)n;
}

float arcStepDeg() {
  float s = ARC_DRAW_STEP_DEG;
  return (s < 0.25f) ? 0.25f : s;
}

float arcGapSweep() {
  return (ARC_GAP_DEG < 0.0f) ? 0.0f : ARC_GAP_DEG;
}

float arcSectionSweep() {
  const uint8_t kc = arcKCount();
  if (kc == 0) {
    return ARC_SWEEP_DEG;
  }
  const float gaps = (float)(kc - 1) * arcGapSweep();
  float sec = (ARC_SWEEP_DEG - gaps) / (float)kc;
  return (sec < 0.5f) ? 0.5f : sec;
}

uint16_t arcGapSteps() {
  if (arcGapSweep() <= 0.0f) {
    return 0;
  }
  uint16_t n = (uint16_t)((arcGapSweep() / arcStepDeg()) + 0.5f);
  return (n < 1) ? 1 : n;
}

uint16_t arcSectionSteps() {
  uint16_t n = (uint16_t)((arcSectionSweep() / arcStepDeg()) + 0.5f);
  return (n < 1) ? 1 : n;
}

uint16_t arcWedgeCount() {
  return (uint16_t)arcKCount() * arcSectionSteps();
}

uint16_t arcWedgesForRpm(int rpm) {
  const uint16_t n = arcWedgeCount();
  if (rpm <= 0) {
    return 0;
  }
  if (rpm >= UI_RPM_MAX) {
    return n;
  }
  return (uint16_t)(((uint32_t)rpm * (uint32_t)n) / (uint32_t)UI_RPM_MAX);
}

uint16_t arcColorForRpm(int rpm) {
  if (rpm >= UI_RPM_RED) {
    return COL_ERR;
  }
  if (rpm >= UI_RPM_WARN) {
    return COL_WARN;
  }
  return COL_OK;
}

void arcPolar(int r, float deg, int16_t &x, int16_t &y) {
  const float rad = deg * (3.14159265f / 180.0f);
  x = (int16_t)(UI_CX + (int16_t)lroundf(cosf(rad) * (float)r));
  y = (int16_t)(UI_CY - (int16_t)lroundf(sinf(rad) * (float)r));
}

void arcFillSpanAt(Adafruit_GC9A01A &tft, int rOut, int rIn, float degStart,
                   float degEnd, uint16_t color) {
  if (degEnd >= degStart) {
    return;
  }
  const float step = arcStepDeg();
  for (float a = degStart; a > degEnd;) {
    float b = a - step;
    if (b < degEnd) {
      b = degEnd;
    }
    int16_t axo, ayo, axi, ayi, bxo, byo, bxi, byi;
    arcPolar(rOut, a, axo, ayo);
    arcPolar(rIn, a, axi, ayi);
    arcPolar(rOut, b, bxo, byo);
    arcPolar(rIn, b, bxi, byi);
    tft.fillTriangle(axo, ayo, bxo, byo, axi, ayi, color);
    tft.fillTriangle(axi, ayi, bxo, byo, bxi, byi, color);
    a = b;
  }
}

void arcFillSpan(Adafruit_GC9A01A &tft, float degStart, float degEnd,
                 uint16_t color) {
  arcFillSpanAt(tft, ARC_R_OUT, ARC_R_IN, degStart, degEnd, color);
}

/** Wedge angular width — sectionSweep / steps so kc sections fill sweep exactly. */
static float wedgeWidthDeg() {
  return arcSectionSweep() / (float)arcSectionSteps();
}

static float wedgeDeg0(uint16_t w) {
  const uint16_t sec = arcSectionSteps();
  const uint16_t k = w / sec;
  const uint16_t j = w % sec;
  const float sectionStart =
      ARC_START_DEG - (float)k * (arcSectionSweep() + arcGapSweep());
  return sectionStart - (float)j * wedgeWidthDeg();
}

static uint16_t colorForWedge(uint16_t w) {
  const uint16_t sec = arcSectionSteps();
  const int rpmMark = (int)((w / sec) + 1) * arcDivRpm();
  return arcColorForRpm(rpmMark);
}

static void paintWedge(Adafruit_GC9A01A &tft, uint16_t w, uint16_t color) {
  const float a0 = wedgeDeg0(w);
  arcFillSpanAt(tft, ARC_R_OUT, ARC_R_IN, a0, a0 - wedgeWidthDeg(), color);
}

void arcDrawSectionTicks(Adafruit_GC9A01A &tft) {
  if (!ARC_SHOW_TICKS) {
    return;
  }
  const float sec = arcSectionSweep();
  const float gap = arcGapSweep();
  const uint8_t kc = arcKCount();
  for (uint8_t k = 0; k <= kc; k++) {
    const float deg = ARC_START_DEG - (float)k * (sec + gap);
    int16_t x0, y0, x1, y1;
    arcPolar(ARC_R_IN, deg, x0, y0);
    arcPolar(ARC_R_OUT + 5, deg, x1, y1);
    tft.drawLine(x0, y0, x1, y1, COL_BG);
  }
}

void arcClearGaps(Adafruit_GC9A01A &tft) {
  const float sec = arcSectionSweep();
  const float gap = arcGapSweep();
  if (gap <= 0.0f) {
    return;
  }
  const uint8_t kc = arcKCount();
  for (uint8_t k = 0; k + 1 < kc; k++) {
    const float gap0 = ARC_START_DEG - (float)k * (sec + gap) - sec;
    arcFillSpan(tft, gap0, gap0 - gap, COL_BG);
  }
}

void arcDrawGauge(Adafruit_GC9A01A &tft, int rpm) {
  const uint16_t n = arcWedgeCount();
  const uint16_t filled = arcWedgesForRpm(rpm);
  for (uint16_t w = 0; w < n; w++) {
    paintWedge(tft, w, (w < filled) ? colorForWedge(w) : COL_TRACK);
  }
  arcClearGaps(tft);
  arcDrawSectionTicks(tft);
}

void arcDrawCutoff(Adafruit_GC9A01A &tft, int rpm, bool bright) {
  const uint16_t filled = arcWedgesForRpm(rpm);
  const uint16_t lit = bright ? COL_TEXT : COL_ERR;
  for (uint16_t w = 0; w < filled; w++) {
    paintWedge(tft, w, lit);
  }
}

uint16_t arcUpdateWedges(Adafruit_GC9A01A &tft, uint16_t fromW, uint16_t toW,
                         uint16_t maxWedges) {
  if (fromW == toW || maxWedges == 0) {
    return fromW;
  }
  if (toW > fromW) {
    const uint16_t room = (uint16_t)(toW - fromW);
    const uint16_t step = (room > maxWedges) ? maxWedges : room;
    const uint16_t end = (uint16_t)(fromW + step);
    for (uint16_t w = fromW; w < end; w++) {
      paintWedge(tft, w, colorForWedge(w));
    }
    return end;
  }
  const uint16_t room = (uint16_t)(fromW - toW);
  const uint16_t step = (room > maxWedges) ? maxWedges : room;
  const uint16_t end = (uint16_t)(fromW - step);
  for (uint16_t w = end; w < fromW; w++) {
    paintWedge(tft, w, COL_TRACK);
  }
  /* Shared radial edge with previous wedge was overwritten — restore. */
  if (end > 0) {
    paintWedge(tft, (uint16_t)(end - 1), colorForWedge((uint16_t)(end - 1)));
  }
  return end;
}
