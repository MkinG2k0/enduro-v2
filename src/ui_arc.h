/**
 * RPM arc — integer wedges, incremental updates (no full redraw jitter).
 */
#pragma once

#include <Adafruit_GC9A01A.h>
#include <Arduino.h>

#include "ui_config.h"

int arcDivRpm();
uint8_t arcKCount();
float arcStepDeg();
uint16_t arcGapSteps();
uint16_t arcSectionSteps();
float arcGapSweep();
float arcSectionSweep();
uint16_t arcWedgeCount();
uint16_t arcWedgesForRpm(int rpm);
uint16_t arcColorForRpm(int rpm);

void arcPolar(int r, float deg, int16_t &x, int16_t &y);
void arcFillSpanAt(Adafruit_GC9A01A &tft, int rOut, int rIn, float degStart,
                   float degEnd, uint16_t color);
void arcFillSpan(Adafruit_GC9A01A &tft, float degStart, float degEnd,
                 uint16_t color);
void arcClearGaps(Adafruit_GC9A01A &tft);
void arcDrawSectionTicks(Adafruit_GC9A01A &tft);
/** Full track + fill + gaps (chrome / force). */
void arcDrawGauge(Adafruit_GC9A01A &tft, int rpm);
/** Shift-light: recolor filled wedges white/red. */
void arcDrawCutoff(Adafruit_GC9A01A &tft, int rpm, bool bright);
/**
 * Paint wedges from→to; at most maxWedges per call (keeps UI snappy on
 * enduro-like RPM snaps). Returns the filled-wedge count after this step.
 */
uint16_t arcUpdateWedges(Adafruit_GC9A01A &tft, uint16_t fromW, uint16_t toW,
                         uint16_t maxWedges);
