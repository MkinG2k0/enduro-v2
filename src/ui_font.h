/**
 * Font binding from UI_FONT_ID in ui_config.h
 */
#pragma once

#include <Adafruit_GC9A01A.h>

#include "ui_config.h"

#if UI_FONT_ID == 1
#include <Fonts/Picopixel.h>
#elif UI_FONT_ID == 2
#include <Fonts/FreeMonoBold9pt7b.h>
#elif UI_FONT_ID == 3
#include <Fonts/FreeMonoBold12pt7b.h>
#elif UI_FONT_ID == 4
#include <Fonts/FreeSansBold9pt7b.h>
#elif UI_FONT_ID == 5
#include <Fonts/FreeSansBold12pt7b.h>
#endif

inline const GFXfont *uiFontPtr() {
#if UI_FONT_ID == 1
  return &Picopixel;
#elif UI_FONT_ID == 2
  return &FreeMonoBold9pt7b;
#elif UI_FONT_ID == 3
  return &FreeMonoBold12pt7b;
#elif UI_FONT_ID == 4
  return &FreeSansBold9pt7b;
#elif UI_FONT_ID == 5
  return &FreeSansBold12pt7b;
#else
  return nullptr;
#endif
}

inline void uiDrawTextCentered(Adafruit_GC9A01A &tft, const char *text,
                               int anchorY, uint8_t size, uint16_t color,
                               bool vCenter, int16_t *outX = nullptr,
                               int16_t *outY = nullptr, uint16_t *outW = nullptr,
                               uint16_t *outH = nullptr) {
  const GFXfont *font = uiFontPtr();
  tft.setFont(font);
  tft.setTextSize(size);
  if (font == nullptr) {
    tft.setTextColor(color, COL_BG);
  } else {
    tft.setTextColor(color);
  }
  int16_t x1 = 0;
  int16_t y1 = 0;
  uint16_t w = 0;
  uint16_t h = 0;
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  const int x = UI_CX - (int)w / 2;
  int y;
  if (font == nullptr) {
    y = vCenter ? (anchorY - (int)h / 2) : anchorY;
  } else {
    y = vCenter ? (anchorY - (int)h / 2 - y1) : (anchorY - y1);
  }
  tft.setCursor(x, y);
  tft.print(text);
  if (outX != nullptr) {
    *outX = (int16_t)(x + x1);
  }
  if (outY != nullptr) {
    *outY = (int16_t)(y + y1);
  }
  if (outW != nullptr) {
    *outW = w;
  }
  if (outH != nullptr) {
    *outH = h;
  }
}
