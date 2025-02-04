#ifndef RESTFUL_H
#define RESTFUL_H
#include "config.h"
#include "data.h"
#include <cstdio>

#define MAX_RESTFUL_RESPONSE_SIZE 1024 

// Path: /
#define RESTFUL_MAIN_VIEW_PATH "/"
#define RESTFUL_MAIN_VIEW "{\
\"Context\":\"/\", \
\"Description\":\"List of paths of different functionalities\",\
\"Paths\":[{\"Path\":\"/trips\",\"Description\":\"List of different trips and their paths.\"}]\
}"

// Path: /trips
#define RESTFUL_TRIPS_VIEW_PATH "/trips"
#define RESTFUL_TRIPS_VIEW "{\
\"Context\":\"/trips\",\
\"Description\":\"List of different trips and their paths.\",\
\"Paths\":[%s]\
}"
#define RESTFUL_TRIPS_VIEW_PATHS_LIST_ENTRY "\"/trips/%i\""

// Path: /trips/%i
#define RESTFUL_ONE_TRIP_VIEW_PATH "/trips/%i"
#define RESTFUL_ONE_TRIP_VIEW "{\
\"Context\":\"/trips/%i\",\
\"Description\":\"Individual trip data overview.\",\
\"ID\":%i,\
\"StartTimestamp\":\"%d\",\
\"EndTimestamp\":\"%d\",\
\"Steps\":%i,\
\"AvgSpeed\":\"%d\",\
\"GeoLocationPath\":null\
}"

// Path: /trips/%i/GeoData
#define RESTFUL_GEODATA_VIEW_PATH "/trips/%i/GeoData"

// Path: /error
#define RESTFUL_ERROR_PATH "/error"
#define RESTFUL_ERROR_VIEW "{\
\"Context\":\"error\",\
\"Description\":\"Unable to respond to request.\"\
}"

enum restfulTripStatus{
    SYNCED,
    NOT_SYNCED,
    ERROR
};

typedef struct {
    tripData * trips;
    restfulTripStatus status;
} restfulTripData;

#ifndef ESP32_WROOM_32
// PLACE HOLDER NOT YET IMPLEMENTED
typedef struct {
    geoData * data;
    restfulTripStatus status;
} restfulGeolocation;
#endif

typedef enum {
    RESTFUL_GET,
    RESTFUL_POST,
    RESTFUL_ERROR
} httpType;

typedef struct {
    char *addressData;
    int addressLen;
    char postData[32];
    int postDataLen;
    char response[MAX_RESTFUL_RESPONSE_SIZE];
    int responseLen;
    httpType type;
} restfulPacket;

restfulPacket restfulHandlePacket(char *data, const int *dataLen, tripData *trips);
























#endif