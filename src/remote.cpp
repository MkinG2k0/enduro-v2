#include "remote.h"

#include "demo.h"
#include "gear_inputs.h"
#include "high_beam.h"
#include "rpm.h"
#include "source_mode.h"
#include "speed.h"
#include "ui_config.h"

RemoteState g_remote = {};

static bool g_sensorsOk = false;

static void enterDemo() {
  g_remote.demo = true;
  g_remote.live = true;
  g_remote.hold = false;
  demoBegin();
  g_remote.speed = demoSpeedKmh();
  g_remote.rpm = demoRpm();
  g_remote.gear = demoGear();
  g_remote.hb = demoHighBeam();
}

static void enterProd() {
  g_remote.demo = false;
  g_remote.live = false;
  g_remote.hold = false;
}

static void applyAuto() {
  g_remote.bike = isBikeConnected();
  g_remote.usb = isUsbConnected();
  if (g_remote.bike) {
    if (g_remote.demo || (!g_remote.hold && g_remote.live)) {
      enterProd();
    }
    g_remote.manual = false;
    return;
  }
  if (g_remote.manual) {
    return;
  }
  if (g_remote.usb) {
    if (!g_remote.demo) {
      enterDemo();
    }
  } else if (g_remote.demo || g_remote.live) {
    enterProd();
  }
}

void remoteBegin() {
  g_remote = {};
  setupRPM();
  setupSpeed();
  setupHighBeam();
  g_sensorsOk = setupGearInputs();
  demoBegin();
  applyAuto();
}

void remoteSetDemo(bool on) {
  g_remote.manual = true;
  if (on) {
    enterDemo();
    g_remote.manual = true;
  } else {
    g_remote.manual = false;
    applyAuto();
  }
}

void remoteSetLive(bool on) {
  g_remote.manual = on;
  if (on) {
    g_remote.live = true;
    g_remote.demo = false;
    g_remote.hold = true;
  } else {
    g_remote.hold = false;
    applyAuto();
  }
}

void remoteSetAuto() {
  g_remote.manual = false;
  g_remote.hold = false;
  applyAuto();
}

void remoteHoldValues(int speed, int rpm, int gear, bool hb) {
  if (speed < 0) {
    speed = 0;
  }
  if (speed > 999) {
    speed = 999;
  }
  if (rpm < 0) {
    rpm = 0;
  }
  if (rpm > UI_RPM_MAX) {
    rpm = UI_RPM_MAX;
  }
  g_remote.manual = true;
  g_remote.live = true;
  g_remote.hold = true;
  g_remote.demo = false;
  g_remote.speed = speed;
  g_remote.rpm = rpm;
  g_remote.gear = gear;
  g_remote.hb = hb;
}

void remoteTick() {
  applyAuto();

  if (g_remote.demo && !g_remote.hold) {
    demoUpdate();
    g_remote.speed = demoSpeedKmh();
    g_remote.rpm = demoRpm();
    g_remote.gear = demoGear();
    g_remote.hb = demoHighBeam();
    return;
  }
  if (g_remote.live || g_remote.hold) {
    return;
  }
  updateRPM();
  updateSpeed();
}

void remoteRead(int &speed, int &rpm, int &gear, bool &hb) {
  if (g_remote.demo || g_remote.live || g_remote.hold) {
    speed = g_remote.speed;
    rpm = g_remote.rpm;
    gear = g_remote.gear;
    hb = g_remote.hb;
    return;
  }
  speed = (int)(getSpeedKmh() + 0.5f);
  rpm = (int)(getRPM() + 0.5f);
  gear = g_sensorsOk ? g_gearInputs.readGearStable() : -1;
  hb = isHighBeamOn();
}
