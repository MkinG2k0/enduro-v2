/**
 * Wheel speed via PC817 isolation module channel 2 (non-blocking).
 *
 * Motorcycle Janxin M178A-005-1P (ASSUMED — not yet confirmed):
 *   pin 9 (yellow) → IN2
 *   pin 1          → GND (input side of module)
 * Arduino:
 *   V2 / OUT2 → SPEED_PIN (INPUT_PULLUP)
 *   GND (output side) → Arduino GND
 *
 * Pro Micro ATmega32U4: D1 = INT3 (external interrupt OK).
 * D0 remains RPM. No conflict with I2C (D2/D3) or TFT.
 */
#pragma once

#include <Arduino.h>

/* ===== SPEED configuration (calibrate here) ===== */
#define SPEED_PIN 1

/**
 * AJERRA S8 RL300: колёса 21/18, перед 80/100-21, зад 120/90-18
 * (или 120/100-18 — зависит от комплектации).
 *
 * SPEED считаем по ПЕРЕДНЕМУ колесу (типичный датчик у M178):
 *   D = 21×25.4 + 2×(80×1.00) = 693.4 mm
 *   C = π×D ≈ 2.178 m
 *
 * Если скорость кратно врёт — крути SPEED_PULSES_PER_WHEEL_REV
 * (сколько импульсов датчика на 1 оборот колеса).
 */
const float WHEEL_CIRCUMFERENCE_M = 2.178f;
const float SPEED_PULSES_PER_WHEEL_REV = 1.0f;

const float SPEED_FILTER_ALPHA = 0.25f;
const uint32_t SPEED_SAMPLE_INTERVAL_MS = 500;
const uint32_t SPEED_TIMEOUT_MS = 1500;

/**
 * Debounce / glitch reject (µs).
 * 200 km/h @ 2.10 m, 1 ppr → ~26.5 Hz → period ~38 ms
 * 200 km/h @ 6 ppr        → ~159 Hz  → period ~6.3 ms
 * 500 µs rejects short noise without blocking realistic wheel rates.
 */
const uint32_t SPEED_MIN_PULSE_US = 500;

void setupSpeed();
void updateSpeed();

float getSpeedKmh();
float getRawSpeedKmh();
float getSpeedFrequency();
uint32_t getSpeedPulseCount();
uint32_t getSpeedTotalPulses();
uint32_t getSpeedLastPulseAgeMs();
