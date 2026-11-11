/**
 * On-device DEMO: synthetic RPM/speed/gear/HB.
 * Auto: USB without bike harness. Manual: Serial `DEMO 1`.
 */
#pragma once

#include <Arduino.h>

void demoBegin();
void demoUpdate();

int demoSpeedKmh();
int demoRpm();
int demoGear();
bool demoHighBeam();
