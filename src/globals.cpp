#ifndef ESP32_WROOM_32
#include "globals.h"
#include <LilyGoWatch.h>

timeStamp createTimestampFromRTC(RTC_Date date)
{
    timeStamp stamp;
    stamp.year = date.year;
    stamp.month = date.month;
    stamp.day = date.day;
    stamp.hour = date.hour;
    stamp.minute = date.minute;
    stamp.second = date.second;
    return stamp;
}

float getTimeDifference(timeStamp start, timeStamp stop)
{
    float hoursPassedInSeconds = (start.hour - stop.hour) * 3600;
    float minutesPassedInSeconds = (start.minute - stop.minute) * 60;
    float secondsPassed = (start.second - stop.second) + minutesPassedInSeconds + hoursPassedInSeconds;
    return secondsPassed;
}

#endif