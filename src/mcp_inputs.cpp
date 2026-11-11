#include "mcp_inputs.h"

McpDashboardInputs g_mcpInputs;

bool McpDashboardInputs::begin(uint8_t i2cAddr, TwoWire *wire) {
  ready_ = false;
  if (!mcp_.begin_I2C(i2cAddr, wire)) {
    return false;
  }

  // GPA0..GPA7: active-LOW inputs with MCP internal ~100 kΩ pull-ups
  for (uint8_t pin = 0; pin < 8; pin++) {
    mcp_.pinMode(pin, INPUT_PULLUP);
  }

  // GPB0..GPB7: prepare as pull-up inputs for future turn signals / buttons
  for (uint8_t pin = 8; pin < 16; pin++) {
    mcp_.pinMode(pin, INPUT_PULLUP);
  }

  ready_ = true;
  lastRawGear_ = -1;
  stableGear_ = -1;
  heldGear_ = -1;
  rawChangedMs_ = millis();
  invalidSinceMs_ = 0;
  return true;
}

uint8_t McpDashboardInputs::readPortA() {
  if (!ready_) {
    return 0xFF;
  }
  return mcp_.readGPIOA();
}

int McpDashboardInputs::readGear() {
  if (!ready_) {
    return -1;
  }

  // Active LOW: bit clear = that line pulled to GND
  // A0=N, A2..A7=gears 1..6 (A1 spare — ignored here)
  const uint8_t portA = mcp_.readGPIOA();

  int active = -1;
  uint8_t count = 0;

  if ((portA & (1u << MCP_PIN_NEUTRAL)) == 0) {
    count++;
    active = 0;
  }

  for (uint8_t gear = 1; gear <= 6; gear++) {
    const uint8_t pin = (uint8_t)(MCP_PIN_GEAR1 + (gear - 1)); // A2..A7
    if ((portA & (1u << pin)) == 0) {
      count++;
      active = (int)gear;
    }
  }

  if (count != 1) {
    return -1; // none or multi-active / wiring fault
  }
  return active;
}

int McpDashboardInputs::readGearStable(uint16_t debounceMs, uint16_t holdMs) {
  const int raw = readGear();
  const uint32_t now = millis();

  if (raw != lastRawGear_) {
    lastRawGear_ = raw;
    rawChangedMs_ = now;
  }

  if ((now - rawChangedMs_) < debounceMs) {
    return stableGear_;
  }

  if (raw >= 0) {
    stableGear_ = raw;
    heldGear_ = raw;
    invalidSinceMs_ = 0;
    return stableGear_;
  }

  // Invalid / open during shift: keep last valid briefly
  if (heldGear_ >= 0) {
    if (invalidSinceMs_ == 0) {
      invalidSinceMs_ = now;
    }
    if ((now - invalidSinceMs_) < holdMs) {
      return heldGear_;
    }
  }

  stableGear_ = -1;
  return -1;
}

bool setupMcpInputs() { return g_mcpInputs.begin(); }

int readGear() { return g_mcpInputs.readGear(); }
