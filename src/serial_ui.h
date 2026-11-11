#pragma once

#include <Arduino.h>

/** Parse "UI key=val ..." — returns true if any field changed. */
bool serialUiApply(char *args);

void serialUiPrint();
