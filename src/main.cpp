/**
 * Enduro — rider cluster
 *
 * USB (no bike harness) → DEMO. Bike gear/N present → production signals.
 * Serial: HELP / DEMO / LIVE / SET / UI / AUTO.
 */

#include <Adafruit_GC9A01A.h>
#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <SPI.h>

#include "remote.h"
#include "serial_cmd.h"
#include "ui.h"
#include "ui_config.h"

static constexpr int PIN_TFT_CS = 10;
static constexpr int PIN_TFT_DC = A0;
static constexpr int PIN_TFT_RST = A1;
static constexpr int PIN_TFT_BLK = A2;

Adafruit_GC9A01A tft(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);

static uint32_t lastUiMs = 0;
static uint32_t lastLogMs = 0;
static bool uiArcBusy = false;

static void logStatus(int speed, int rpm, int gear, bool hb) {
  Serial.print(speed);
  Serial.print(' ');
  Serial.print(rpm);
  Serial.print(' ');
  Serial.print(gear);
  Serial.print(' ');
  Serial.print(hb ? 1 : 0);
  Serial.print(' ');
  Serial.print(g_remote.bike ? 1 : 0);
  Serial.print(' ');
  Serial.println(g_remote.usb ? 1 : 0);
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_TFT_BLK, OUTPUT);
  digitalWrite(PIN_TFT_BLK, LOW);

  SPI.begin();
  tft.begin();
  tft.setSPISpeed(40000000);
  tft.setRotation(0);

  remoteBegin();
  uiCfgLoad();
  Serial.println(F(ENDURO_FW_VERSION));

  uiBegin(tft);
  uiBootAnim(tft, PIN_TFT_BLK);

  int speed = 0;
  int rpm = 0;
  int gear = 0;
  bool hb = false;
  remoteRead(speed, rpm, gear, hb);
  uiUpdate(speed, rpm, gear, hb, true);
}

void loop() {
  serialCmdPoll();
  remoteTick();

  const uint32_t now = millis();
  int speed = 0;
  int rpm = 0;
  int gear = 0;
  bool hb = false;
  remoteRead(speed, rpm, gear, hb);

  if (g_remote.uiDirty) {
    g_remote.uiDirty = false;
    uiReload();
    uiArcBusy = uiUpdate(speed, rpm, gear, hb, true);
    lastUiMs = now;
  } else {
    /* Cutoff needs tight polling or the strobe freezes as a solid fill. */
    const uint16_t uiMs = uiArcBusy ? 20 : 80;
    if (now - lastUiMs >= uiMs) {
      lastUiMs = now;
      uiArcBusy = uiUpdate(speed, rpm, gear, hb, false);
    }
  }

  if (now - lastLogMs >= 500) {
    lastLogMs = now;
    logStatus(speed, rpm, gear, hb);
  }
}
