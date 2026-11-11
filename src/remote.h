/**
 * Runtime source: auto (USB→demo / bike→sensors) or manual LIVE/DEMO.
 */
#pragma once

#include <Arduino.h>

struct RemoteState {
  bool demo;
  bool live;
  bool hold;
  bool manual; /* Serial DEMO/LIVE/SET — until AUTO or bike (clears demo) */
  bool uiDirty;
  bool bike;
  bool usb;
  int speed;
  int rpm;
  int gear;
  bool hb;
};

extern RemoteState g_remote;

void remoteBegin();
void remoteSetDemo(bool on);
void remoteSetLive(bool on);
void remoteSetAuto();
void remoteHoldValues(int speed, int rpm, int gear, bool hb);
void remoteTick();
void remoteRead(int &speed, int &rpm, int &gear, bool &hb);
