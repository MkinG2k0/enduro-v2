/**
 * Gear / Neutral on Pro Micro GPIO (no MCP23017).
 * Active LOW: switch → bike GND (common with Arduino GND).
 *
 * One ribbon on digital row D2..D8:
 *   D2=1, D3=2, D4=3, D5=4, D6=5, D7=6, D8=N
 *   (M178 pin10→D2 … pin15→D7, Neutral pin2→D8)
 *
 * TFT DC/RES/BLK on A0/A1/A2. High beam on D9.
 * Do NOT put +12 V on these pins.
 */
#pragma once

#include <Arduino.h>

static constexpr uint8_t GEAR_PIN_1 = 2;
static constexpr uint8_t GEAR_PIN_2 = 3;
static constexpr uint8_t GEAR_PIN_3 = 4;
static constexpr uint8_t GEAR_PIN_4 = 5;
static constexpr uint8_t GEAR_PIN_5 = 6;
static constexpr uint8_t GEAR_PIN_6 = 7;
static constexpr uint8_t GEAR_PIN_N = 8;

static constexpr uint8_t GEAR_PINS[7] = {
    GEAR_PIN_N, GEAR_PIN_1, GEAR_PIN_2, GEAR_PIN_3,
    GEAR_PIN_4, GEAR_PIN_5, GEAR_PIN_6};

class GearInputs {
public:
  bool begin() {
    for (uint8_t i = 0; i < 7; i++) {
      pinMode(GEAR_PINS[i], INPUT_PULLUP);
    }
    ready_ = true;
    lastRaw_ = -1;
    stable_ = -1;
    held_ = -1;
    rawChangedMs_ = millis();
    invalidSinceMs_ = 0;
    return true;
  }

  bool isReady() const { return ready_; }

  /** Bit0=N … bit6=Gear6. 1 = line active (LOW). */
  uint8_t readMask() const {
    uint8_t m = 0;
    for (uint8_t i = 0; i < 7; i++) {
      if (digitalRead(GEAR_PINS[i]) == LOW) {
        m |= (uint8_t)(1u << i);
      }
    }
    return m;
  }

  /** 0 = Neutral, 1..6 = gear, -1 = none / multi / not ready */
  int readGear() const {
    if (!ready_) {
      return -1;
    }
    int active = -1;
    uint8_t count = 0;
    for (uint8_t i = 0; i < 7; i++) {
      if (digitalRead(GEAR_PINS[i]) == LOW) {
        count++;
        active = (int)i;
      }
    }
    if (count != 1) {
      return -1;
    }
    return active;
  }

  int readGearStable(uint16_t debounceMs = 30, uint16_t holdMs = 250) {
    const int raw = readGear();
    const uint32_t now = millis();

    if (raw != lastRaw_) {
      lastRaw_ = raw;
      rawChangedMs_ = now;
    }
    if ((now - rawChangedMs_) < debounceMs) {
      return stable_;
    }
    if (raw >= 0) {
      stable_ = raw;
      held_ = raw;
      invalidSinceMs_ = 0;
      return stable_;
    }
    if (held_ >= 0) {
      if (invalidSinceMs_ == 0) {
        invalidSinceMs_ = now;
      }
      if ((now - invalidSinceMs_) < holdMs) {
        return held_;
      }
    }
    stable_ = -1;
    return -1;
  }

private:
  bool ready_ = false;
  int lastRaw_ = -1;
  int stable_ = -1;
  int held_ = -1;
  uint32_t rawChangedMs_ = 0;
  uint32_t invalidSinceMs_ = 0;
};

extern GearInputs g_gearInputs;

inline bool setupGearInputs() { return g_gearInputs.begin(); }
inline int readGear() { return g_gearInputs.readGear(); }
