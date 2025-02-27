#ifndef GPS_H
#define GPS_H
#include <string>

class TTGOClass;
class PCF8563_Class;

typedef struct {
    double lat;
    double lon;
    double dist;
} GPSPoint;

void initGPS(TTGOClass *ttgo);

void updateGPS();

double getLatitude();

double getLongitude();

// returns a struct with latitude, longitude, distance from last step
// gets given the datastructure and puts the nxt data into it
GPSPoint takeStep();

void setRTCTime(PCF8563_Class *rtc);

#endif