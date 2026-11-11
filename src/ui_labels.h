#pragma once

#include <Adafruit_GC9A01A.h>
#include <Arduino.h>

struct TextSlot {
  int16_t x;
  int16_t y;
  uint16_t w;
  uint16_t h;
  bool valid;
};

void uiLabelsReset();
void uiPaintSpeed(Adafruit_GC9A01A &tft, int speedKmh);
void uiPaintRpmNum(Adafruit_GC9A01A &tft, int rpm);
void uiPaintGear(Adafruit_GC9A01A &tft, int gear);
void uiPaintHb(Adafruit_GC9A01A &tft, bool hb);
void uiPaintLabels(Adafruit_GC9A01A &tft, int speedKmh, int rpmShown, int gear,
                   bool hb);
