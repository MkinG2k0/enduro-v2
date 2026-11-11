#include "speed.h"

volatile uint32_t speedPulseCount = 0;
volatile uint32_t speedLastPulseUs = 0;
volatile uint32_t speedLastPulseMs = 0;

static uint32_t speedSampleStartMs = 0;
static float speedFrequencyHz = 0.0f;
static float speedRawKmh = 0.0f;
static float speedFilteredKmh = 0.0f;
static uint32_t speedWindowPulses = 0;
static uint32_t speedTotalPulses = 0;
static uint32_t speedLastPulseAgeMs = 0;

static void speedISR() {
  const uint32_t nowUs = micros();
  if (speedLastPulseUs != 0 &&
      (nowUs - speedLastPulseUs) < SPEED_MIN_PULSE_US) {
    return;
  }
  speedLastPulseUs = nowUs;
  speedLastPulseMs = millis();
  speedPulseCount++;
}

void setupSpeed() {
  pinMode(SPEED_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SPEED_PIN), speedISR, FALLING);

  const uint32_t now = millis();
  noInterrupts();
  speedPulseCount = 0;
  speedLastPulseUs = 0;
  speedLastPulseMs = now;
  interrupts();

  speedSampleStartMs = now;
  speedFrequencyHz = 0.0f;
  speedRawKmh = 0.0f;
  speedFilteredKmh = 0.0f;
  speedWindowPulses = 0;
  speedTotalPulses = 0;
  speedLastPulseAgeMs = 0;
}

void updateSpeed() {
  const uint32_t now = millis();
  if ((now - speedSampleStartMs) < SPEED_SAMPLE_INTERVAL_MS) {
    return;
  }

  const uint32_t elapsedMs = now - speedSampleStartMs;
  speedSampleStartMs = now;

  uint32_t count;
  uint32_t lastPulseMs;
  noInterrupts();
  count = speedPulseCount;
  speedPulseCount = 0;
  lastPulseMs = speedLastPulseMs;
  interrupts();

  speedWindowPulses = count;
  speedTotalPulses += count;
  speedLastPulseAgeMs = now - lastPulseMs;

  if (count > 0 && elapsedMs > 0) {
    const float elapsedSeconds = elapsedMs / 1000.0f;
    speedFrequencyHz = count / elapsedSeconds;
    speedRawKmh = speedFrequencyHz * WHEEL_CIRCUMFERENCE_M * 3.6f /
                  SPEED_PULSES_PER_WHEEL_REV;
    speedFilteredKmh = speedFilteredKmh * (1.0f - SPEED_FILTER_ALPHA) +
                       speedRawKmh * SPEED_FILTER_ALPHA;
  } else if (speedLastPulseAgeMs >= SPEED_TIMEOUT_MS) {
    speedFrequencyHz = 0.0f;
    speedRawKmh = 0.0f;
    /* raw=0 through same filter so standing vehicle decays to 0. */
    speedFilteredKmh =
        speedFilteredKmh * (1.0f - SPEED_FILTER_ALPHA);
    if (speedFilteredKmh < 0.05f) {
      speedFilteredKmh = 0.0f;
    }
  }

}

float getSpeedKmh() { return speedFilteredKmh; }

float getRawSpeedKmh() { return speedRawKmh; }

float getSpeedFrequency() { return speedFrequencyHz; }

uint32_t getSpeedPulseCount() { return speedWindowPulses; }

uint32_t getSpeedTotalPulses() { return speedTotalPulses; }

uint32_t getSpeedLastPulseAgeMs() { return speedLastPulseAgeMs; }
