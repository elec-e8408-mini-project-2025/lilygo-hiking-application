#ifndef ESP32_WROOM_32
#include "globals.h"

// Step counter
TFT_eSPI *tft;
BMA *sensor;
bool irqAcc = false;

uint32_t stepCount = 0;
const float step_length = 0.8;
float avgSpeed = 0.0;

// Becomes true when start button is pressed in session view
// Becomes false when stop button is pressed in session view
bool hasActiveSession = false;

#endif