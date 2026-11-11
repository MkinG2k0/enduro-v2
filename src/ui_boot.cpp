#include "ui.h"

#include <Adafruit_GFX.h>

#include "ui_arc.h"
#include "ui_colors.h"
#include "ui_font.h"

static void bootPaintSection(Adafruit_GC9A01A &tft, uint8_t k, uint16_t color) {
  const float sec = arcSectionSweep();
  const float gap = arcGapSweep();
  const float deg0 = ARC_START_DEG - (float)k * (sec + gap);
  arcFillSpan(tft, deg0, deg0 - sec, color);
}

static void bootSweep(Adafruit_GC9A01A &tft) {
  const uint8_t kc = arcKCount();
  const uint8_t stepMs = UI_BOOT_STEP_MS;
  for (uint8_t k = 0; k < kc; k++) {
    bootPaintSection(tft, k, arcColorForRpm((int)((k + 1) * arcDivRpm())));
    if (stepMs) {
      delay(stepMs);
    }
  }
  for (int k = (int)kc - 1; k >= 0; k--) {
    bootPaintSection(tft, (uint8_t)k, COL_TRACK);
    if (stepMs) {
      delay(stepMs > 2 ? (uint8_t)(stepMs - 2) : stepMs);
    }
  }
  arcClearGaps(tft);
  arcDrawSectionTicks(tft);
}

static void bootShowVersion(Adafruit_GC9A01A &tft) {
  uiDrawTextCentered(tft, "ENDURO", 100, UI_FONT_SIZE_BOOT, COL_TEXT, false);
  uiDrawTextCentered(tft, ENDURO_FW_VERSION, 128, UI_FONT_SIZE_BOOT, COL_DIM,
                     false);
  const uint16_t hold = UI_BOOT_HOLD_MS;
  if (hold) {
    delay(hold);
  }
  tft.fillRect(30, 90, 180, 55, COL_BG);
}

void uiBootAnim(Adafruit_GC9A01A &tft, uint8_t blkPin) {
  digitalWrite(blkPin, LOW);
  tft.fillScreen(COL_BG);
  if (ARC_SHOW_OUTER_RING) {
    tft.drawCircle(UI_CX, UI_CY, 118, COL_RING);
    tft.drawCircle(UI_CX, UI_CY, 117, COL_RING);
  }
  if (UI_SHOW_ARC) {
    arcDrawGauge(tft, 0);
  }
  digitalWrite(blkPin, HIGH);

  if (UI_SHOW_ARC && UI_BOOT_ANIM) {
    bootSweep(tft);
  }
  if (UI_BOOT_SHOW_VER) {
    bootShowVersion(tft);
  }
}
