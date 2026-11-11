#include "demo.h"

#include <avr/pgmspace.h>
#include <string.h>

#include "ui_config.h"

struct DemoKey {
  uint16_t ms;
  uint16_t rpm;
  uint8_t speed;
  int8_t gear;
  uint8_t hb;
};

static const DemoKey kKeys[] PROGMEM = {
    {0, 1200, 0, 0, 0},
    {2000, 1200, 0, 0, 0},
    {2300, 7000, 40, 2, 0},
    {5000, 7000, 58, 2, 0},
    {5300, 9800, 90, 4, 1},
    {8000, 9800, 105, 5, 1},
    {8300, 1500, 20, 0, 0},
    {11000, 1200, 0, 0, 0},
};

static constexpr uint8_t kKeyCount =
    (uint8_t)(sizeof(kKeys) / sizeof(kKeys[0]));
static constexpr uint16_t kLoopMs = 11000;

static int g_rpm = 0;
static int g_speed = 0;
static int g_gear = 0;
static bool g_hb = false;
static uint32_t g_t0 = 0;

static DemoKey readKey(uint8_t i) {
  DemoKey k;
  memcpy_P(&k, &kKeys[i], sizeof(DemoKey));
  return k;
}

static int lerpInt(int a, int b, uint16_t num, uint16_t den) {
  if (den == 0) {
    return b;
  }
  return a + (int)(((long)(b - a) * num) / den);
}

void demoBegin() {
  g_t0 = millis();
  g_rpm = 1200;
  g_speed = 0;
  g_gear = 0;
  g_hb = false;
}

void demoUpdate() {
  const uint16_t t = (uint16_t)((millis() - g_t0) % kLoopMs);
  uint8_t i = 0;
  while (i + 1 < kKeyCount) {
    const DemoKey n = readKey((uint8_t)(i + 1));
    if (t < n.ms) {
      break;
    }
    i++;
  }
  const DemoKey a = readKey(i);
  if (i + 1 >= kKeyCount) {
    g_rpm = (int)a.rpm;
    g_speed = (int)a.speed;
    g_gear = a.gear;
    g_hb = a.hb != 0;
    return;
  }
  const DemoKey b = readKey((uint8_t)(i + 1));
  const uint16_t span = (uint16_t)(b.ms - a.ms);
  const uint16_t u = (uint16_t)(t - a.ms);
  g_rpm = lerpInt((int)a.rpm, (int)b.rpm, u, span);
  g_speed = lerpInt((int)a.speed, (int)b.speed, u, span);
  g_gear = b.gear;
  g_hb = b.hb != 0;
}

int demoSpeedKmh() { return g_speed; }
int demoRpm() { return g_rpm; }
int demoGear() { return g_gear; }
bool demoHighBeam() { return g_hb; }
