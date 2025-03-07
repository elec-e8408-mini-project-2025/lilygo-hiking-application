#ifndef RESTFUL_H
#define RESTFUL_H
#include "config.h"
#include "data.h"
#include <cstdio>

#define MAX_RESTFUL_RESPONSE_SIZE 1024 
#define MAX_RESTFUL_REQUEST_SIZE MAX_RESTFUL_RESPONSE_SIZE/4

// Path: /
#define RESTFUL_MAIN_VIEW_PATH "/"
#define RESTFUL_MAIN_VIEW "{\
\"Context\":\"/\", \
\"Description\":\"List of paths of different functionalities\",\
\"Paths\":[{\"Path\":\"/tripdata\",\"Description\":\"List of different trips and their paths.\"}, {\"Path\":\"/error\",\"Description\":\"General error information.\"}]\
}\n"

// Path: /trips
#define RESTFUL_TRIPS_VIEW_PATH "/tripdata"
#define RESTFUL_TRIPS_VIEW "{\
\"Context\":\"/tripdata\",\
\"Description\":\"List of different trips and their paths.\",\
\"Paths\":[%s]\
}\n"
#define RESTFUL_TRIPS_VIEW_PATHS_LIST_ENTRY "\"/tripdata/%i\""

// Path: /trips/%i
#define RESTFUL_ONE_TRIP_VIEW_PATH "/tripdata/%i"
#define RESTFUL_ONE_TRIP_VIEW "{\
\"Context\":\"/tripdata/%i\",\
\"Description\":\"Individual trip data overview.\",\
\"ID\":%i,\
\"StartTimestamp\":\"%s\",\
\"EndTimestamp\":\"%s\",\
\"Steps\":%i,\
\"AvgSpeed\":\"%f\",\
\"GeoLocationPath\":null\
}\n"

// Path: /tripdata/%i/GeoData
#define RESTFUL_GEODATA_VIEW_PATH "/tripdata/%i/GeoData"

// Path: /error
#define RESTFUL_ERROR_PATH "/error"
#define RESTFUL_ERROR_VIEW "{\
\"Context\":\"/error\",\
\"Description\":\"Unable to respond to request.\"\
}\n"

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