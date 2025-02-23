#ifndef DATA_H
#define DATA_H
#include <stdint.h>
#define GEO_MAX_SIZE 16

typedef struct {
    double latitude[GEO_MAX_SIZE]; 
    double longitude[GEO_MAX_SIZE]; 
    double distance;
    int dataLen;
    unsigned long timestamp;
} geoData;

typedef struct {
    int tripID;
    unsigned long timestampStart;
    unsigned long timestampStop;
    uint32_t stepCount;
    float avgSpeed;
    geoData tripGeoData;
} tripData;

typedef struct {
    float step_length;
    int currentTrip;
    int maxTrips;
    bool hasActiveSession;
} systemGlobals;








#endif