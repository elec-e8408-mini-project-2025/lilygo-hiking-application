#ifndef ACCELERATOR_H
#define ACCELERATOR_H
#include <stdint.h>

class TTGOClass;

void initAccelerator(TTGOClass *ttgo);

uint32_t handleTasksAccelerator();

void resetAccelerator();


#endif