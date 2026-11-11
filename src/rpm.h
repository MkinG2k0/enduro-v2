/**
 * Engine RPM via 8-ch PC817 isolation module (non-blocking).
 *
 * Motorcycle Janxin M178A-005-1P (channel 1):
 *   pin 16 → IN1
 *   pin 1  → GND (input side of module)
 * Arduino:
 *   V1 / OUT1 → RPM_PIN (INPUT_PULLUP)
 *   GND (output side) → Arduino GND
 *
 * Pro Micro layout (FW 1.5+):
 *   D0 = RPM (INT2), D1 = SPEED (INT3)
 *   D2..D8 = gears, D9 = high beam — bike harness on right edge
 *   TFT control on A0..A2 (not D7..D9)
 */
#pragma once

#include <Arduino.h>

/* ===== RPM configuration (calibrate here) ===== */
#define RPM_PIN 0

const float RPM_PULSES_PER_REV = 1.0f;
/** EMA rise — keep some smoothing so the arc does not jitter. */
const float RPM_FILTER_ALPHA = 0.35f;
/** EMA fall — faster so releasing throttle drains the ring promptly. */
const float RPM_FILTER_ALPHA_DOWN = 0.65f;
const uint32_t RPM_SAMPLE_INTERVAL_MS = 250;
const uint32_t RPM_TIMEOUT_MS = 750;

/**
 * Debounce / glitch reject (µs).
 * 15000 RPM @ 1 ppr → 250 Hz → period 4000 µs
 * 15000 RPM @ 2 ppr → 500 Hz → period 2000 µs
 * 1000 µs is safe for ≤2 pulses/rev up to 15k RPM.
 * (At 4 ppr / 15k RPM period=1000 µs — would need a lower value.)
 */
const uint32_t RPM_MIN_PULSE_US = 1000;

void setupRPM();
void updateRPM();

float getRPM();
float getRawRPM();
float getRPMFrequency();
uint32_t getRPMPulseCount();
uint32_t getRPMLastPulseAgeMs();
