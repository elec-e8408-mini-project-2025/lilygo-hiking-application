#ifndef GPS_H
#define GPS_H
#include <LilyGoWatch.h>


void initGPS(TTGOClass *ttgo);

double getLatitude();

double getLongitude();

uint32_t getTime();

uint32_t getDate();

double distanceBetween(double lat1, double long1, double lat2, double long2);

#endif