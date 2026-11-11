#include "rpm.h"

volatile uint32_t rpmPulseCount = 0;
volatile uint32_t rpmLastPulseUs = 0;
volatile uint32_t rpmLastPulseMs = 0;

static uint32_t rpmSampleStartMs = 0;
static float rpmFrequencyHz = 0.0f;
static float rpmRaw = 0.0f;
static float rpmFiltered = 0.0f;
static uint32_t rpmWindowPulses = 0;
static uint32_t rpmLastPulseAgeMs = 0;

static void rpmISR() {
  const uint32_t nowUs = micros();
  if (rpmLastPulseUs != 0 && (nowUs - rpmLastPulseUs) < RPM_MIN_PULSE_US) {
    return;
  }
  rpmLastPulseUs = nowUs;
  rpmLastPulseMs = millis();
  rpmPulseCount++;
}

void setupRPM() {
  pinMode(RPM_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RPM_PIN), rpmISR, FALLING);

  const uint32_t now = millis();
  noInterrupts();
  rpmPulseCount = 0;
  rpmLastPulseUs = 0;
  rpmLastPulseMs = now;
  interrupts();

  rpmSampleStartMs = now;
  rpmFrequencyHz = 0.0f;
  rpmRaw = 0.0f;
  rpmFiltered = 0.0f;
  rpmWindowPulses = 0;
}

void updateRPM() {
  const uint32_t now = millis();
  if ((now - rpmSampleStartMs) < RPM_SAMPLE_INTERVAL_MS) {
    return;
  }

  const uint32_t elapsedMs = now - rpmSampleStartMs;
  rpmSampleStartMs = now;

  uint32_t count;
  uint32_t lastPulseMs;
  noInterrupts();
  count = rpmPulseCount;
  rpmPulseCount = 0;
  lastPulseMs = rpmLastPulseMs;
  interrupts();

  rpmWindowPulses = count;
  rpmLastPulseAgeMs = now - lastPulseMs;

  if (count > 0 && elapsedMs > 0) {
    const float elapsedSeconds = elapsedMs / 1000.0f;
    rpmFrequencyHz = count / elapsedSeconds;
    rpmRaw = rpmFrequencyHz * 60.0f / RPM_PULSES_PER_REV;
    const float alpha =
        (rpmRaw < rpmFiltered) ? RPM_FILTER_ALPHA_DOWN : RPM_FILTER_ALPHA;
    rpmFiltered = rpmFiltered * (1.0f - alpha) + rpmRaw * alpha;
  } else if (rpmLastPulseAgeMs >= RPM_TIMEOUT_MS) {
    rpmFrequencyHz = 0.0f;
    rpmRaw = 0.0f;
    rpmFiltered = 0.0f;
  } else {
    /* No pulses this window — decay toward 0 instead of freezing. */
    rpmFrequencyHz = 0.0f;
    rpmRaw = 0.0f;
    rpmFiltered *= (1.0f - RPM_FILTER_ALPHA_DOWN);
    if (rpmFiltered < 50.0f) {
      rpmFiltered = 0.0f;
    }
  }

}

float getRPM() { return rpmFiltered; }

float getRawRPM() { return rpmRaw; }

float getRPMFrequency() { return rpmFrequencyHz; }

uint32_t getRPMPulseCount() { return rpmWindowPulses; }

uint32_t getRPMLastPulseAgeMs() { return rpmLastPulseAgeMs; }
