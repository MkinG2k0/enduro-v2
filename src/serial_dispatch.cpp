#include "serial_dispatch.h"

#include <Arduino.h>
#include <string.h>

#include "remote.h"
#include "serial_ui.h"
#include "ui_config.h"

static bool isOn(const char *s) {
  return s[0] == '1' || strcasecmp(s, "ON") == 0;
}

static bool isOff(const char *s) {
  return s[0] == '0' || strcasecmp(s, "OFF") == 0;
}

static int parseGear(const char *s) {
  return (s[0] == 'N' || s[0] == 'n') ? 0 : atoi(s);
}

static void ok() { Serial.println(F("OK")); }

static void handleSet(char *args) {
  int speed = g_remote.speed;
  int rpm = g_remote.rpm;
  int gear = g_remote.gear;
  bool hb = g_remote.hb;
  char *tok = strtok(args, " \t");
  while (tok != nullptr) {
    char *eq = strchr(tok, '=');
    if (eq != nullptr && eq != tok) {
      *eq = 0;
      const char *v = eq + 1;
      if (tok[0] == 'S' || tok[0] == 's') {
        speed = atoi(v);
      } else if (tok[0] == 'R' || tok[0] == 'r') {
        rpm = atoi(v);
      } else if (tok[0] == 'G' || tok[0] == 'g') {
        gear = parseGear(v);
      } else if (strcasecmp(tok, "HB") == 0) {
        hb = isOn(v);
      }
    }
    tok = strtok(nullptr, " \t");
  }
  remoteHoldValues(speed, rpm, gear, hb);
  ok();
}

void serialDispatchLine(char *line) {
  while (*line == ' ' || *line == '\t') {
    line++;
  }
  if (*line == 0) {
    return;
  }
  char *args = line;
  while (*args && *args != ' ' && *args != '\t') {
    args++;
  }
  if (*args) {
    *args++ = 0;
    while (*args == ' ' || *args == '\t') {
      args++;
    }
  }

  if (strcasecmp(line, "HELP") == 0) {
    Serial.println(F("HELP PING VER ST DEMO LIVE SET UI"));
  } else if (strcasecmp(line, "PING") == 0) {
    Serial.println(F("PONG"));
  } else if (strcasecmp(line, "VER") == 0) {
    Serial.println(F(ENDURO_FW_VERSION));
  } else if (strcasecmp(line, "STATUS") == 0 || strcasecmp(line, "ST") == 0) {
    Serial.print(F("ST d="));
    Serial.print(g_remote.demo);
    Serial.print(F(" b="));
    Serial.print(g_remote.bike);
    Serial.print(F(" u="));
    Serial.print(g_remote.usb);
    Serial.print(' ');
    Serial.print(g_remote.speed);
    Serial.print(' ');
    Serial.print(g_remote.rpm);
    Serial.print(' ');
    Serial.print(g_remote.gear);
    Serial.print(' ');
    Serial.println(g_remote.hb);
  } else if (strcasecmp(line, "AUTO") == 0) {
    remoteSetAuto();
    ok();
  } else if (strcasecmp(line, "DEMO") == 0) {
    if (isOn(args)) {
      remoteSetDemo(true);
      ok();
    } else if (isOff(args)) {
      remoteSetDemo(false);
      ok();
    } else {
      Serial.println(F("ERR"));
    }
  } else if (strcasecmp(line, "LIVE") == 0) {
    if (isOn(args)) {
      remoteSetLive(true);
      ok();
    } else if (isOff(args)) {
      remoteSetLive(false);
      ok();
    } else {
      Serial.println(F("ERR"));
    }
  } else if (strcasecmp(line, "SPEED") == 0) {
    remoteHoldValues(atoi(args), g_remote.rpm, g_remote.gear, g_remote.hb);
    ok();
  } else if (strcasecmp(line, "RPM") == 0) {
    remoteHoldValues(g_remote.speed, atoi(args), g_remote.gear, g_remote.hb);
    ok();
  } else if (strcasecmp(line, "GEAR") == 0) {
    remoteHoldValues(g_remote.speed, g_remote.rpm, parseGear(args),
                     g_remote.hb);
    ok();
  } else if (strcasecmp(line, "HB") == 0) {
    remoteHoldValues(g_remote.speed, g_remote.rpm, g_remote.gear, isOn(args));
    ok();
  } else if (strcasecmp(line, "SET") == 0) {
    handleSet(args);
  } else if (strcasecmp(line, "UI") == 0) {
    if (strcasecmp(args, "GET") == 0 || *args == 0) {
      serialUiPrint();
    } else if (strcasecmp(args, "RESET") == 0) {
      uiCfgReset();
      uiCfgErase();
      g_remote.uiDirty = true;
      ok();
    } else if (strcasecmp(args, "SAVE") == 0) {
      uiCfgSave();
      ok();
    } else if (strcasecmp(args, "LOAD") == 0) {
      if (uiCfgLoad()) {
        g_remote.uiDirty = true;
        ok();
      } else {
        Serial.println(F("ERR"));
      }
    } else if (serialUiApply(args)) {
      ok();
    }
  } else {
    Serial.println(F("ERR"));
  }
}
