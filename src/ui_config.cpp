#include "ui_config.h"

#include <EEPROM.h>

static const UiCfg kUiDefaults = {
    12000, 5000, 8000, 1000, 110, 10, 220.0f, 260.0f, 3.5f, 2.0f,
    /* Classic GFX 5x7 + opaque draw (no erase flicker) */
    false, false, true, true, true, true, true, 6, 2, 4, 2, 42, 168,
    -90.0f, 40.0f, 110, 7,
    true, true, 800, 12,
    false, 8000, 120,
};

UiCfg g_ui = kUiDefaults;

static constexpr int kEepromBase = 0;
static constexpr uint16_t kEepromMagic = 0xE201;
static constexpr uint8_t kEepromVer = 5;

struct EepromUiHdr {
  uint16_t magic;
  uint8_t ver;
  uint8_t reserved;
};

static uint16_t crc16(const uint8_t *data, size_t len) {
  uint16_t crc = 0xFFFF;
  for (size_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (uint8_t b = 0; b < 8; b++) {
      crc = (crc & 1) ? (crc >> 1) ^ 0xA001 : (crc >> 1);
    }
  }
  return crc;
}

void uiCfgReset() { g_ui = kUiDefaults; }

bool uiCfgLoad() {
  EepromUiHdr hdr;
  EEPROM.get(kEepromBase, hdr);
  if (hdr.magic != kEepromMagic || hdr.ver != kEepromVer) {
    return false;
  }
  const int cfgAddr = kEepromBase + (int)sizeof(hdr);
  UiCfg cfg;
  EEPROM.get(cfgAddr, cfg);
  uint16_t stored = 0;
  EEPROM.get(cfgAddr + (int)sizeof(cfg), stored);
  const uint16_t expect =
      crc16(reinterpret_cast<const uint8_t *>(&cfg), sizeof(cfg));
  if (stored != expect) {
    return false;
  }
  g_ui = cfg;
  return true;
}

bool uiCfgSave() {
  EepromUiHdr hdr = {kEepromMagic, kEepromVer, 0};
  EEPROM.put(kEepromBase, hdr);
  const int cfgAddr = kEepromBase + (int)sizeof(hdr);
  EEPROM.put(cfgAddr, g_ui);
  const uint16_t sum =
      crc16(reinterpret_cast<const uint8_t *>(&g_ui), sizeof(g_ui));
  EEPROM.put(cfgAddr + (int)sizeof(g_ui), sum);
  return true;
}

void uiCfgErase() {
  EepromUiHdr hdr = {0, 0, 0};
  EEPROM.put(kEepromBase, hdr);
}
