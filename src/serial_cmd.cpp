#include "serial_cmd.h"

#include <Arduino.h>

#include "serial_dispatch.h"

static constexpr uint8_t kLineMax = 160;
static char g_line[kLineMax];
static uint8_t g_len = 0;

void serialCmdPoll() {
  while (Serial.available() > 0) {
    const char c = (char)Serial.read();
    if (c == '\r') {
      continue;
    }
    if (c == '\n') {
      g_line[g_len] = 0;
      g_len = 0;
      serialDispatchLine(g_line);
      continue;
    }
    if (g_len + 1 < kLineMax) {
      g_line[g_len++] = c;
    } else {
      g_len = 0;
      Serial.println(F("ERR LINE"));
    }
  }
}
