#ifndef ESP32_WROOM_32
#include "globals.h"
#include <LilyGoWatch.h>

timeStamp createTimestampFromRTC(RTC_Date date)
{
    uint16_t year = date.year;
    uint8_t month = date.month;
    uint8_t day = date.day;
    uint8_t hour = date.hour;
    uint8_t minute = date.minute;
    uint8_t second = date.second;
}

#endif