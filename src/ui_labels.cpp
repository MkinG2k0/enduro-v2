#include "ui_labels.h"

#include "ui_arc.h"
#include "ui_colors.h"
#include "ui_font.h"

static TextSlot g_spdSlot = {};
static TextSlot g_rpmSlot = {};
static TextSlot g_gearSlot = {};

static void trimSides(Adafruit_GC9A01A &tft, const TextSlot &old, int16_t x,
                      int16_t y, uint16_t w, uint16_t h) {
  (void)y;
  (void)h;
  if (!old.valid) {
    return;
  }
  const int16_t ox2 = (int16_t)(old.x + (int16_t)old.w);
  const int16_t nx2 = (int16_t)(x + (int16_t)w);
  if (old.x < x) {
    tft.fillRect(old.x, old.y, (int16_t)(x - old.x), (int16_t)old.h, COL_BG);
  }
  if (ox2 > nx2) {
    tft.fillRect(nx2, old.y, (int16_t)(ox2 - nx2), (int16_t)old.h, COL_BG);
  }
}

static void paintCentered(Adafruit_GC9A01A &tft, TextSlot &slot,
                          const char *text, int anchorY, uint8_t size,
                          uint16_t color, bool vCenter) {
  int16_t x = 0;
  int16_t y = 0;
  uint16_t w = 0;
  uint16_t h = 0;

#if UI_FONT_ID == 0
  /* Opaque cells — draw first, trim only width leftovers (9↔10). */
  uiDrawTextCentered(tft, text, anchorY, size, color, vCenter, &x, &y, &w, &h);
  trimSides(tft, slot, x, y, w, h);
#else
  clearSlot(tft, slot);
  uiDrawTextCentered(tft, text, anchorY, size, color, vCenter, &x, &y, &w, &h);
#endif

  slot.x = x;
  slot.y = y;
  slot.w = w;
  slot.h = h;
  slot.valid = (w > 0 && h > 0);
}

void uiLabelsReset() {
  g_spdSlot.valid = false;
  g_rpmSlot.valid = false;
  g_gearSlot.valid = false;
}

void uiPaintSpeed(Adafruit_GC9A01A &tft, int speedKmh) {
  if (!UI_SHOW_SPEED) {
    return;
  }
  char buf[4];
  if (speedKmh > 999) {
    speedKmh = 999;
  }
  itoa(speedKmh, buf, 10);
  paintCentered(tft, g_spdSlot, buf, UI_CY, UI_FONT_SIZE_SPEED, COL_TEXT, true);
}

void uiPaintRpmNum(Adafruit_GC9A01A &tft, int rpm) {
  if (!UI_SHOW_RPM) {
    return;
  }
  char buf[8];
  itoa(rpm, buf, 10);
  paintCentered(tft, g_rpmSlot, buf, UI_RPM_Y, UI_FONT_SIZE_RPM, COL_DIM, false);
}

void uiPaintGear(Adafruit_GC9A01A &tft, int gear) {
  if (!UI_SHOW_GEAR) {
    return;
  }
  char gbuf[2];
  uint16_t gcol = COL_OK;
  if (gear < 0) {
    gbuf[0] = '-';
    gbuf[1] = 0;
    gcol = COL_ERR;
  } else if (gear == 0) {
    gbuf[0] = 'N';
    gbuf[1] = 0;
    gcol = COL_ACCENT;
  } else {
    gbuf[0] = (char)('0' + gear);
    gbuf[1] = 0;
  }
  paintCentered(tft, g_gearSlot, gbuf, UI_GEAR_Y, UI_FONT_SIZE_GEAR, gcol,
                false);
}

void uiPaintHb(Adafruit_GC9A01A &tft, bool hb) {
  if (!UI_SHOW_HB) {
    return;
  }
  const float half = UI_HB_SWEEP_DEG * 0.5f;
  arcFillSpanAt(tft, UI_HB_R_OUT, UI_HB_R_IN, UI_HB_CENTER_DEG + half,
                UI_HB_CENTER_DEG - half, hb ? COL_HB : COL_TRACK);
}

void uiPaintLabels(Adafruit_GC9A01A &tft, int speedKmh, int rpmShown, int gear,
                   bool hb) {
  uiPaintRpmNum(tft, rpmShown);
  uiPaintSpeed(tft, speedKmh);
  uiPaintGear(tft, gear);
  uiPaintHb(tft, hb);
}
