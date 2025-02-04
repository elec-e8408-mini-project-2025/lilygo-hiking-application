#ifndef RESTFUL_H
#define RESTFUL_H
#include "config.h"
#include "data.h"

#define RESTFUL_MAIN_VIEW "/"
#define RESTFUL_TRIPS_VIEW "/trips"
#define RESTFUL_INDIVIDUAL_TRIP_VIEW "/trips/0"

#ifndef ESP32_WROOM_32
#define RESTFUL_TRIPS_VIEW_GEOLOCATION "/trips/x/geolocation"
#endif

#define FIXED_ADDRESSES { \
                    RESTFUL_MAIN_VIEW, \
                    RESTFUL_TRIPS_VIEW \
                    }
#define FIXED_ADDRESSES_COUNT 2


typedef struct {
    char * address;
    char ** otherAddresses;
    int dataCount;
} restfulAddressView;

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
    char *response;
    int responseLen;
    httpType type;
} restfulPacket;


int initRestful(tripData *trips, int size);

restfulPacket restfulHandlePacket(char *data, const int *dataLen);
























#endif