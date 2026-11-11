/**
 * Discrete dashboard inputs via MCP23017 (I2C).
 *
 * I2C on Pro Micro:
 *   SDA = D2, SCL = D3
 * Address 0x20 when A0/A1/A2 are tied to GND.
 *
 * GPA mapping (active LOW with internal pull-ups) — CHMCU-2317 silk A0..A7:
 *   A0 / GPA0 Neutral (M178 pin 2)
 *   A1 / GPA1 spare (do not feed 12 V)
 *   A2 / GPA2 Gear 1  (M178 pin 10, pink)
 *   A3 / GPA3 Gear 2  (M178 pin 11)
 *   A4 / GPA4 Gear 3  (M178 pin 12)
 *   A5 / GPA5 Gear 4  (M178 pin 13)
 *   A6 / GPA6 Gear 5  (M178 pin 14)
 *   A7 / GPA7 Gear 6  (M178 pin 15)
 *
 * Do NOT feed 12 V into MCP pins. High beam (M178 pin 4) goes
 * PC817 → Arduino D4 — see high_beam.h. Never onto A0..A7 / B0..B7.
 *
 * RPM = D0; SPEED = D1; HighBeam = D4 — not on MCP23017.
 */
#pragma once

#include <Arduino.h>
#include <Adafruit_MCP23X17.h>

/** I2C address with A0=A1=A2=GND */
static constexpr uint8_t MCP23017_I2C_ADDR = 0x20;

/** Adafruit pin numbers: 0..7 = GPA0..GPA7 (= silk A0..A7), 8..15 = GPB0..GPB7 */
static constexpr uint8_t MCP_PIN_NEUTRAL = 0;  // A0
static constexpr uint8_t MCP_PIN_SPARE_A1 = 1; // A1 — spare / future switch→GND only
static constexpr uint8_t MCP_PIN_GEAR1 = 2;    // A2
static constexpr uint8_t MCP_PIN_GEAR2 = 3;    // A3
static constexpr uint8_t MCP_PIN_GEAR3 = 4;    // A4
static constexpr uint8_t MCP_PIN_GEAR4 = 5;    // A5
static constexpr uint8_t MCP_PIN_GEAR5 = 6;    // A6
static constexpr uint8_t MCP_PIN_GEAR6 = 7;    // A7

/**
 * Future GPB map (documented, not wired yet).
 * Only switch→GND or PC817 *output* side — never raw +12 V.
 *   GPB0 Left turn
 *   GPB1 Right turn
 *   GPB2..GPB7 spare / buttons
 */
static constexpr uint8_t MCP_PIN_TURN_L = 8;  // GPB0
static constexpr uint8_t MCP_PIN_TURN_R = 9;  // GPB1

class McpDashboardInputs {
public:
  /**
   * Init MCP23017 on the given Wire bus.
   * Call after Wire.begin(). Returns false if chip not found.
   */
  bool begin(uint8_t i2cAddr = MCP23017_I2C_ADDR, TwoWire *wire = &Wire);

  bool isReady() const { return ready_; }

  /**
   * Raw gear decode: A0=N, A2..A7=gears 1..6.
   *   0  = Neutral
   *   1..6 = gear
   *  -1 = none active, multi-active, or MCP not ready
   */
  int readGear();

  /**
   * Debounced gear for UI: ignores brief open during shifts
   * and holds last valid gear for holdMs after -1.
   */
  int readGearStable(uint16_t debounceMs = 30, uint16_t holdMs = 250);

  /** Port A byte (GPA0=bit0 … GPA7=bit7). 0xFF on error. */
  uint8_t readPortA();

  Adafruit_MCP23X17 &chip() { return mcp_; }

private:
  Adafruit_MCP23X17 mcp_;
  bool ready_ = false;

  int lastRawGear_ = -1;
  int stableGear_ = -1;
  int heldGear_ = -1;
  uint32_t rawChangedMs_ = 0;
  uint32_t invalidSinceMs_ = 0;
};

/** Global helper wrappers (same API as requested). */
extern McpDashboardInputs g_mcpInputs;

bool setupMcpInputs();
int readGear();
