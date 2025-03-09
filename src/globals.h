#ifndef GLOBALS_H
#define GLOBALS_H
#include "data.h"

class RTC_Date;

timeStamp createTimestampFromRTC(RTC_Date date);

// returns the time in seconds that has passed between the two
// values
float getTimeDifference(timeStamp start, timeStamp stop);

#endif