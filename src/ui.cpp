#include "ui.h"

#include <Adafruit_GFX.h>

#include "ui_arc.h"
#include "ui_colors.h"
#include "ui_labels.h"

static constexpr uint16_t kArcWedgesPerFrame = 12;

static Adafruit_GC9A01A *g_tft = nullptr;
static int g_lastArcW = -1;
static int g_lastSpeed = -1;
static int g_lastRpmNum = -1;
static int g_lastGear = -99;
static bool g_lastHb = false;
static bool g_cutOn = false;
static bool g_cutPhase = false;
static uint32_t g_cutHoldUntil = 0;

static void resetState() {
  uiLabelsReset();
  g_lastArcW = -1;
  g_lastSpeed = -1;
  g_lastRpmNum = -1;
  g_lastGear = -99;
  g_lastHb = false;
  g_cutOn = false;
  g_cutPhase = false;
  g_cutHoldUntil = 0;
}

void uiBegin(Adafruit_GC9A01A &tft) {
  g_tft = &tft;
  resetState();
}

void uiReload() {
  if (g_tft == nullptr) {
    return;
  }
  resetState();
  uiDrawChrome();
}

void uiDrawChrome() {
  g_tft->fillScreen(COL_BG);
  if (ARC_SHOW_OUTER_RING) {
    g_tft->drawCircle(UI_CX, UI_CY, 118, COL_RING);
    g_tft->drawCircle(UI_CX, UI_CY, 117, COL_RING);
  }
  if (UI_SHOW_ARC) {
    arcDrawGauge(*g_tft, 0);
    g_lastArcW = 0;
  } else {
    g_lastArcW = -1;
  }
}

bool uiUpdate(int speedKmh, int rpm, int gear, bool hb, bool force) {
  if (rpm < 0) {
    rpm = 0;
  }
  if (speedKmh < 0) {
    speedKmh = 0;
  }

  const int rpmShown = (rpm / 50) * 50;
  const int arcQ = (rpm / 100) * 100;
  const uint16_t arcTarget = arcWedgesForRpm(arcQ);
  const bool cutWant = UI_SHOW_ARC && UI_CUTOFF_EN && UI_RPM_CUTOFF > 0 &&
                       rpm >= UI_RPM_CUTOFF;
  const uint16_t period =
      (UI_CUTOFF_MS < 80) ? 80 : (uint16_t)UI_CUTOFF_MS;

  const bool arcDirty =
      UI_SHOW_ARC &&
      (force || g_lastArcW < 0 || (uint16_t)g_lastArcW != arcTarget);
  const bool rpmDirty =
      UI_SHOW_RPM && (force || rpmShown != g_lastRpmNum);
  const int dSpd = (speedKmh > g_lastSpeed) ? (speedKmh - g_lastSpeed)
                                            : (g_lastSpeed - speedKmh);
  const bool spdDirty =
      UI_SHOW_SPEED &&
      (force || dSpd >= 2 || (speedKmh == 0 && g_lastSpeed != 0));
  const bool gearDirty =
      UI_SHOW_GEAR && (force || gear != g_lastGear);
  const bool hbDirty = UI_SHOW_HB && (force || hb != g_lastHb);

  if (rpmDirty) {
    uiPaintRpmNum(*g_tft, rpmShown);
    g_lastRpmNum = rpmShown;
  }
  if (spdDirty) {
    uiPaintSpeed(*g_tft, speedKmh);
    g_lastSpeed = speedKmh;
  }
  if (gearDirty) {
    uiPaintGear(*g_tft, gear);
    g_lastGear = gear;
  }
  if (hbDirty || (UI_SHOW_HB && gearDirty)) {
    uiPaintHb(*g_tft, hb);
    g_lastHb = hb;
  }

  if (!UI_SHOW_ARC) {
    g_cutOn = false;
    g_lastArcW = -1;
    return false;
  }

  if (cutWant) {
    const uint32_t now = millis();
    bool entered = false;
    bool phaseDirty = false;
    if (!g_cutOn) {
      g_cutOn = true;
      g_cutPhase = true;
      entered = true;
      phaseDirty = true;
    } else if (now >= g_cutHoldUntil) {
      g_cutPhase = !g_cutPhase;
      phaseDirty = true;
    }
    if (phaseDirty || arcDirty || force) {
      if (entered || force) {
        arcDrawGauge(*g_tft, arcQ);
      }
      arcDrawCutoff(*g_tft, arcQ, g_cutPhase);
      g_lastArcW = (int)arcTarget;
      /* Only phase changes arm hold — RPM ticks must not freeze blink. */
      if (phaseDirty) {
        g_cutHoldUntil = millis() + period;
      }
    }
  } else {
    const bool leftCut = g_cutOn;
    g_cutOn = false;
    if (arcDirty || leftCut || force) {
      if (force || g_lastArcW < 0 || leftCut) {
        arcDrawGauge(*g_tft, arcQ);
        uiLabelsReset();
        uiPaintLabels(*g_tft, speedKmh, rpmShown, gear, hb);
        g_lastSpeed = speedKmh;
        g_lastRpmNum = rpmShown;
        g_lastGear = gear;
        g_lastHb = hb;
        g_lastArcW = (int)arcTarget;
      } else {
        g_lastArcW = (int)arcUpdateWedges(*g_tft, (uint16_t)g_lastArcW,
                                          arcTarget, kArcWedgesPerFrame);
      }
    }
  }

  const bool arcBusy =
      g_lastArcW >= 0 && (uint16_t)g_lastArcW != arcTarget;
  return arcBusy || cutWant;
}
