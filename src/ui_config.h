/**
 * Enduro UI config — defaults match tools/ui-constructor.html.
 * Runtime fields live in g_ui (Serial / Web Serial can update).
 * GENERATED_BY: ui-constructor
 * GENERATED_AT: manual-baseline
 */
#pragma once

#include <Arduino.h>

/* ---- canvas (fixed) ---- */
static constexpr int UI_CX = 120;
static constexpr int UI_CY = 120;

/* ---- font family (flash-time only; all sets won't fit) ----
 * Must be #define — ui_font.h selects includes via #if.
 * 0 = Default GFX (5x7)
 * 1 = Picopixel
 * 2 = FreeMonoBold9
 * 3 = FreeMonoBold12
 * 4 = FreeSansBold9
 * 5 = FreeSansBold12
 */
#define UI_FONT_ID 0

/* ---- palette RGB565 (fixed) ---- */
static constexpr uint16_t COL_BG = 0x0000;
static constexpr uint16_t COL_RING = 0x4208;
static constexpr uint16_t COL_TRACK = 0x2104;
static constexpr uint16_t COL_OK = 0x07E0;
static constexpr uint16_t COL_WARN = 0xFFE0;
static constexpr uint16_t COL_ERR = 0xF800;
static constexpr uint16_t COL_TEXT = 0xFFFF;
static constexpr uint16_t COL_DIM = 0x7BEF;
static constexpr uint16_t COL_ACCENT = 0x07FF;
/* Soft ice-blue (#5C9BFF) — high beam without harsh pure blue */
static constexpr uint16_t COL_HB = 0x5CDF;

struct UiCfg {
  int rpmMax;
  int rpmWarn;
  int rpmRed;
  int rpmDiv; /* RPM per arc division (default 1000) */
  int rOut;
  int thickness;
  float startDeg;
  float sweepDeg;
  float gapDeg;
  float drawStepDeg;
  bool showOuterRing;
  bool showTicks;
  bool showArc;   /* RPM gauge wedges */
  bool showRpm;   /* RPM digits */
  bool showSpeed; /* speed digits */
  bool showGear;  /* gear / N */
  bool showHb;    /* high-beam arc */
  uint8_t szSpeed;
  uint8_t szRpm;
  uint8_t szGear;
  uint8_t szBoot;
  int rpmY;
  int gearY;
  float hbCenterDeg;
  float hbSweepDeg;
  int hbROut; /* outer radius — keeps concentric arc (“height”) */
  int hbThickness;
  bool bootAnim;
  bool bootShowVer;
  uint16_t bootHoldMs;
  uint8_t bootStepMs;
  bool cutOffEn;     /* shift-light flash at rpmCutOff */
  int rpmCutOff;     /* RPM threshold, e.g. 8000 */
  uint8_t cutOffMs;  /* half-period of red↔white blink */
};

extern UiCfg g_ui;

void uiCfgReset();
/** Load from EEPROM; false → leave g_ui as defaults. */
bool uiCfgLoad();
/** Persist current g_ui to EEPROM. */
bool uiCfgSave();
/** Invalidate EEPROM blob so next boot uses defaults. */
void uiCfgErase();

/* Compat macros — existing UI code reads these. */
#define UI_RPM_MAX (g_ui.rpmMax)
#define UI_RPM_WARN (g_ui.rpmWarn)
#define UI_RPM_RED (g_ui.rpmRed)
#define UI_RPM_DIV (g_ui.rpmDiv)
#define ARC_R_OUT (g_ui.rOut)
#define ARC_THICKNESS (g_ui.thickness)
#define ARC_R_IN (g_ui.rOut - g_ui.thickness)
#define ARC_START_DEG (g_ui.startDeg)
#define ARC_SWEEP_DEG (g_ui.sweepDeg)
#define ARC_GAP_DEG (g_ui.gapDeg)
#define ARC_DRAW_STEP_DEG (g_ui.drawStepDeg)
#define ARC_SHOW_OUTER_RING (g_ui.showOuterRing)
#define ARC_SHOW_TICKS (g_ui.showTicks)
#define UI_SHOW_ARC (g_ui.showArc)
#define UI_SHOW_RPM (g_ui.showRpm)
#define UI_SHOW_SPEED (g_ui.showSpeed)
#define UI_SHOW_GEAR (g_ui.showGear)
#define UI_SHOW_HB (g_ui.showHb)
#define UI_FONT_SIZE_SPEED (g_ui.szSpeed)
#define UI_FONT_SIZE_RPM (g_ui.szRpm)
#define UI_FONT_SIZE_GEAR (g_ui.szGear)
#define UI_FONT_SIZE_BOOT (g_ui.szBoot)
#define UI_RPM_Y (g_ui.rpmY)
#define UI_GEAR_Y (g_ui.gearY)
#define UI_HB_CENTER_DEG (g_ui.hbCenterDeg)
#define UI_HB_SWEEP_DEG (g_ui.hbSweepDeg)
#define UI_HB_R_OUT (g_ui.hbROut)
#define UI_HB_THICKNESS (g_ui.hbThickness)
#define UI_HB_R_IN (g_ui.hbROut - g_ui.hbThickness)
#define UI_BOOT_ANIM (g_ui.bootAnim)
#define UI_BOOT_SHOW_VER (g_ui.bootShowVer)
#define UI_BOOT_HOLD_MS (g_ui.bootHoldMs)
#define UI_BOOT_STEP_MS (g_ui.bootStepMs)
#define UI_CUTOFF_EN (g_ui.cutOffEn)
#define UI_RPM_CUTOFF (g_ui.rpmCutOff)
#define UI_CUTOFF_MS (g_ui.cutOffMs)
