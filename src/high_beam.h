/**
 * High beam via PC817 — NOT through MCP23017 / CHMCU-2317.
 * M178 pin 4 is ~12–14 V; MCP GPIO is 5 V only.
 *
 *   M178 pin 4 → PC817 INx
 *   M178 pin 1 (GND) → PC817 GND_in
 *   PC817 OUTx → HIGH_BEAM_PIN (INPUT_PULLUP)
 *   PC817 GND_out → Arduino GND
 *
 * ON → opto pulls pin LOW.
 * D9 sits right after gear block D2..D8.
 */
#pragma once

#include <Arduino.h>

/** After gears D2..D8; TFT control is on A0..A2. */
#define HIGH_BEAM_PIN 9

static constexpr uint16_t HB_DEBOUNCE_MS = 40;

inline void setupHighBeam() {
  pinMode(HIGH_BEAM_PIN, INPUT_PULLUP);
}

inline bool isHighBeamOn() {
  static bool stable = false;
  static bool lastRaw = false;
  static uint32_t changedMs = 0;

  const bool raw = digitalRead(HIGH_BEAM_PIN) == LOW;
  const uint32_t now = millis();
  if (raw != lastRaw) {
    lastRaw = raw;
    changedMs = now;
  }
  if ((now - changedMs) >= HB_DEBOUNCE_MS) {
    stable = lastRaw;
  }
  return stable;
}
