#include "serial_ui.h"

#include <string.h>

#include "remote.h"
#include "ui_config.h"

/** Lightweight float parse — avoids linking strtod (~0.6KB). */
static float parseF(const char *s) {
  float neg = 1.0f;
  if (*s == '-') {
    neg = -1.0f;
    s++;
  } else if (*s == '+') {
    s++;
  }
  float v = 0.0f;
  while (*s >= '0' && *s <= '9') {
    v = v * 10.0f + (float)(*s - '0');
    s++;
  }
  if (*s == '.') {
    s++;
    float place = 0.1f;
    while (*s >= '0' && *s <= '9') {
      v += (float)(*s - '0') * place;
      place *= 0.1f;
      s++;
    }
  }
  return neg * v;
}

static bool setKey(const char *k, const char *v) {
  const float fv = parseF(v);
  const long iv = atol(v);
  if (!strcmp(k, "rpmMax")) {
    g_ui.rpmMax = (int)iv;
  } else if (!strcmp(k, "rpmWarn")) {
    g_ui.rpmWarn = (int)iv;
  } else if (!strcmp(k, "rpmRed")) {
    g_ui.rpmRed = (int)iv;
  } else if (!strcmp(k, "rpmDiv")) {
    g_ui.rpmDiv = (iv < 1) ? 1000 : (int)iv;
  } else if (!strcmp(k, "rOut")) {
    g_ui.rOut = (int)iv;
  } else if (!strcmp(k, "thick") || !strcmp(k, "thickness")) {
    g_ui.thickness = (int)iv;
  } else if (!strcmp(k, "start")) {
    g_ui.startDeg = fv;
  } else if (!strcmp(k, "sweep")) {
    g_ui.sweepDeg = fv;
  } else if (!strcmp(k, "gap")) {
    g_ui.gapDeg = fv;
  } else if (!strcmp(k, "step")) {
    g_ui.drawStepDeg = fv;
  } else if (!strcmp(k, "outerRing")) {
    g_ui.showOuterRing = iv != 0;
  } else if (!strcmp(k, "ticks")) {
    g_ui.showTicks = iv != 0;
  } else if (!strcmp(k, "showArc")) {
    g_ui.showArc = iv != 0;
  } else if (!strcmp(k, "showRpm")) {
    g_ui.showRpm = iv != 0;
  } else if (!strcmp(k, "showSpeed")) {
    g_ui.showSpeed = iv != 0;
  } else if (!strcmp(k, "showGear")) {
    g_ui.showGear = iv != 0;
  } else if (!strcmp(k, "showHb")) {
    g_ui.showHb = iv != 0;
  } else if (!strcmp(k, "szSpeed")) {
    g_ui.szSpeed = (uint8_t)iv;
  } else if (!strcmp(k, "szRpm")) {
    g_ui.szRpm = (uint8_t)iv;
  } else if (!strcmp(k, "szGear")) {
    g_ui.szGear = (uint8_t)iv;
  } else if (!strcmp(k, "szBoot")) {
    g_ui.szBoot = (uint8_t)iv;
  } else if (!strcmp(k, "rpmY")) {
    g_ui.rpmY = (int)iv;
  } else if (!strcmp(k, "gearY")) {
    g_ui.gearY = (int)iv;
  } else if (!strcmp(k, "hbCenter")) {
    g_ui.hbCenterDeg = fv;
  } else if (!strcmp(k, "hbSweep")) {
    g_ui.hbSweepDeg = fv;
  } else if (!strcmp(k, "hbThick")) {
    g_ui.hbThickness = (int)iv;
  } else if (!strcmp(k, "hbROut")) {
    g_ui.hbROut = (int)iv;
  } else if (!strcmp(k, "bootAnim")) {
    g_ui.bootAnim = iv != 0;
  } else if (!strcmp(k, "bootShowVer")) {
    g_ui.bootShowVer = iv != 0;
  } else if (!strcmp(k, "bootHoldMs")) {
    g_ui.bootHoldMs = (uint16_t)iv;
  } else if (!strcmp(k, "bootStepMs")) {
    g_ui.bootStepMs = (uint8_t)iv;
  } else if (!strcmp(k, "cutOffEn")) {
    g_ui.cutOffEn = iv != 0;
  } else if (!strcmp(k, "rpmCutOff")) {
    g_ui.rpmCutOff = (int)iv;
  } else if (!strcmp(k, "cutOffMs")) {
    g_ui.cutOffMs = (uint8_t)((iv < 40) ? 40 : ((iv > 250) ? 250 : iv));
  } else if (!strcmp(k, "fontId")) {
    return false;
  } else {
    return false;
  }
  return true;
}

bool serialUiApply(char *args) {
  bool changed = false;
  char *tok = strtok(args, " \t");
  while (tok != nullptr) {
    char *eqs = strchr(tok, '=');
    if (eqs != nullptr && eqs != tok) {
      *eqs = 0;
      if (setKey(tok, eqs + 1)) {
        changed = true;
      }
    }
    tok = strtok(nullptr, " \t");
  }
  if (changed) {
    g_remote.uiDirty = true;
  }
  return changed;
}

void serialUiPrint() {
  Serial.print(F("UI "));
  Serial.print(g_ui.rpmMax);
  Serial.print(' ');
  Serial.print(g_ui.rOut);
  Serial.print(' ');
  Serial.print(g_ui.thickness);
  Serial.print(' ');
  Serial.print(g_ui.szSpeed);
  Serial.print(' ');
  Serial.print(g_ui.rpmY);
  Serial.print(' ');
  Serial.println(g_ui.gearY);
}
