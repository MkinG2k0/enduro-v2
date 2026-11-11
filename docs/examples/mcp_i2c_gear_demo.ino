/**
 * Standalone reference sketch: I2C + MCP23017 gear/high-beam.
 *
 * NOT built by PlatformIO src/ firmware — copy ideas into main.cpp or
 * build as a separate env if needed.
 *
 * Pro Micro 5V/16 MHz wiring:
 *   D0  = RPM (PC817) — leave alone / do not use for I2C
 *   D1  = SPEED (future interrupt)
 *   D2  = I2C SDA → CHMCU-2317
 *   D3  = I2C SCL → CHMCU-2317
 *   TFT pins unchanged (D7/D8/D9/D10/D15/D16)
 *
 * MCP23017 / CHMCU-2317:
 *   VCC=5V, GND=GND, SDA=D2, SCL=D3, A0=A1=A2=GND
 *   RESET → 5V via 10k (or direct to 5V)
 *   A0 = Neutral; A2..A7 = Gear 1..6 (switch to bike GND; see HARDWARE.md)
 *   High Beam: PC817 → Arduino D4 — NEVER onto MCP (12 V)
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>

#include "mcp_inputs.h"
#include "high_beam.h"
#include "rpm.h"

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 2000) {
    /* Pro Micro USB CDC */
  }

  setupRPM();
  setupHighBeam();

  Wire.begin(); // D2/D3 on ATmega32U4
  Wire.setClock(400000);

  const bool mcpOk = setupMcpInputs();

  Serial.println(F("I2C + MCP23017 gear demo"));
  Serial.print(F("MCP23017 @0x20: "));
  Serial.println(mcpOk ? F("OK") : F("FAIL"));
  Serial.print(F("HighBeam pin D"));
  Serial.println(HIGH_BEAM_PIN);
}

void loop() {
  updateRPM();

  const int gear = readGear();
  const bool hb = isHighBeamOn();
  const int gearUi = g_mcpInputs.readGearStable();

  Serial.print(F("Gear raw="));
  if (gear < 0) {
    Serial.print(F("ERR"));
  } else if (gear == 0) {
    Serial.print(F("N"));
  } else {
    Serial.print(gear);
  }

  Serial.print(F("  stable="));
  if (gearUi < 0) {
    Serial.print(F("-"));
  } else if (gearUi == 0) {
    Serial.print(F("N"));
  } else {
    Serial.print(gearUi);
  }

  Serial.print(F("  HighBeam="));
  Serial.print(hb ? F("ON") : F("OFF"));

  Serial.print(F("  RPM="));
  Serial.println((int)(getRPM() + 0.5f));

  delay(200);
}
