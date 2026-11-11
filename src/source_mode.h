/**
 * Auto source: bike harness → production signals; USB bench → DEMO.
 *
 * Bike = any gear/N line LOW (D2..D8, shared GND with M178).
 * USB  = VBUS on ATmega32U4.
 */
#pragma once

#include <Arduino.h>
#include <avr/io.h>

#include "gear_inputs.h"

inline bool isUsbConnected() { return (USBSTA & (1 << VBUS)) != 0; }

inline bool isBikeConnected() {
  for (uint8_t i = 0; i < 7; i++) {
    if (digitalRead(GEAR_PINS[i]) == LOW) {
      return true;
    }
  }
  return false;
}
