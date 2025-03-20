#ifndef DATA_H
#define DATA_H
#include <stdint.h>
#define GEO_MAX_SIZE 16

typedef struct timeStamp {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} timeStamp;

typedef struct geoData {
    double latitude[GEO_MAX_SIZE]; 
    double longitude[GEO_MAX_SIZE];
    int dataLen;
    timeStamp timestamp;
} geoData;

typedef struct tripData {
    int tripID;
    uint32_t stepCount;
    float avgSpeed;
    double distance;
    bool synced;
    timeStamp timestampStart;
    timeStamp timestampStop;
    int tag; // a integer tag controlled outside the clock. It gets cleared on new session.
} tripData;

typedef struct systemGlobals {
    float step_length;
    int currentTrip;
    int maxTrips;
    bool hasActiveSession;
    bool GPSavailable;
} systemGlobals;


#endif