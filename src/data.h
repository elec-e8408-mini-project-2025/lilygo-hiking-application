#ifndef DATA_H
#define DATA_H

#define GEO_MAX_SIZE 16

typedef struct {
    double latitude[GEO_MAX_SIZE]; 
    double longitude[GEO_MAX_SIZE]; 
    int dataLen;
    unsigned long timestamp;
} geoData;

typedef struct {
    int tripID;
    unsigned long timestampStart;
    unsigned long timestampStop;
    unsigned long stepCount;
    float avgSpeed;
    //geoData tripGeoData;
} tripData;










#endif