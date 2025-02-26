#ifndef GPS_H
#define GPS_H
#include <LilyGoWatch.h>
#include <string>

typedef struct {
    double lat;
    double lon;
    double dist;
} GPSPoint;

void initGPS(TTGOClass *ttgo);

double getLatitude();

double getLongitude();

uint32_t getTime();

uint32_t getDate();

// returns a struct with latitude, longitude, distance from last step
// gets given the datastructure and puts the nxt data into it
GPSPoint takeStep();

void setRTCTime(PCF8563_Class *rtc);

#endif