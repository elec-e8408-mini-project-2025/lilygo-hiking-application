#ifndef GLOBALS_H
#define GLOBALS_H
#include <LilyGoWatch.h>

extern TFT_eSPI *tft;
extern BMA *sensor;
extern bool irqAcc;

extern uint32_t stepCount;
extern const float step_length;
extern float avgSpeed;

extern bool hasActiveSession;

#endif