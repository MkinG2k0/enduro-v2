/**
 * Production rider UI for GC9A01 240×240.
 */
#pragma once

#include <Adafruit_GC9A01A.h>

#include "ui_config.h"

void uiBegin(Adafruit_GC9A01A &tft);
void uiDrawChrome();
void uiBootAnim(Adafruit_GC9A01A &tft, uint8_t blkPin);
/** Returns true if the arc still needs more frames to catch target RPM. */
bool uiUpdate(int speedKmh, int rpm, int gear, bool hb, bool force);
/** Full chrome redraw after live UI config change. */
void uiReload();
