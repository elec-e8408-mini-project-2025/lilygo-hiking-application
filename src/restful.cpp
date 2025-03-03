#include "restful.h"
#include "data.h"

httpType restfulParseType(const char* data, const int *dataLen)
{
    httpType ret;
    if (*dataLen < 1)
    {
        ret = RESTFUL_ERROR;
        return ret;
    }
    switch (data[0])
    {
        case 'G':
            ret = RESTFUL_GET;
            break;
        case 'P':
            ret = RESTFUL_POST;
            break;
        default:
            ret = RESTFUL_ERROR;
            break;
    } 
    return ret;
} 

int restfulParseAddress(restfulPacket * packet, char * data, const int * dataLen)
{
    char address[21] = "";
    int addressOffset = 0;
    bool addresspart; 

    switch (packet->type)
    {
        case RESTFUL_GET:
            addressOffset = 4;
            break;
        case RESTFUL_POST:
            addressOffset = 5;
            break;
        default:
            return -1;
            break;
    };
    
    packet->addressData = data + addressOffset;
    packet->addressLen = *dataLen - addressOffset;

    return 0;
}

int parseRestfulPacket(restfulPacket * packet, char * data, const int * dataLen)
{
    packet->type = restfulParseType(data,dataLen);
    restfulParseAddress(packet, data, dataLen);
    return 0;
}

int setResponseError(restfulPacket * packet)
{
    packet->responseLen = sprintf(packet->response, "%s", RESTFUL_ERROR_VIEW);
    return 0;
}

int setResponseMainView(restfulPacket * packet)
{
    packet->responseLen = sprintf(packet->response, "%s", RESTFUL_MAIN_VIEW);
    return 0;
}

int setResponseTrips(restfulPacket * packet)
{
    char msgArray[sizeof(RESTFUL_TRIPS_VIEW_PATHS_LIST_ENTRY) * MAX_CACHED_TRIPS];
    char *p = &msgArray[0];
    for (int i = 0; i < MAX_CACHED_TRIPS; ++i)
    {
        if (i != 0)
        {
            p[0] = ',';
            ++p; 
        }
        int offset = sprintf(p, RESTFUL_TRIPS_VIEW_PATHS_LIST_ENTRY, i);
        p+=offset;
    }
    packet->responseLen = sprintf(packet->response, RESTFUL_TRIPS_VIEW, msgArray);
    return 0;
}

int setResponseOneTrip(restfulPacket * packet, tripData *trips)
{
    int tripId = packet->addressData[packet->addressLen - 1] - '0';
    tripData trip = trips[tripId];
    packet->responseLen = sprintf(packet->response, RESTFUL_ONE_TRIP_VIEW, trip.tripID, trip.tripID, trip.timestampStart, trip.timestampStop, trip.stepCount, trip.avgSpeed);
    return 0;
}

int getAddressContent(restfulPacket * packet, tripData *trips)
{
    if (packet->type == RESTFUL_ERROR)
    {
        setResponseError(packet);
        return 0;
    }

    switch(packet->addressLen)
    {
        case sizeof(RESTFUL_MAIN_VIEW_PATH) - 1:
            setResponseMainView(packet);
            break;
        
        case sizeof(RESTFUL_TRIPS_VIEW_PATH) - 1:
            setResponseTrips(packet);
            break;
        
        case sizeof(RESTFUL_ONE_TRIP_VIEW_PATH) - 2:
            setResponseOneTrip(packet, trips);
            break;

        default:
            setResponseError(packet);
            break;
    }

    
    return 0;
}

int postAddressContent(restfulPacket * packet, tripData *trips)
{
    packet->type = RESTFUL_ERROR; // NOT YET IMPLEMENTED
    return 0;
}

restfulPacket restfulHandlePacket(char *data, const int *dataLen, tripData *trips)
{
    restfulPacket ret = {
        0,
        0,
        "",
        0,
        0,
        0,
        RESTFUL_ERROR
    };

    parseRestfulPacket(&ret, data, dataLen);

    switch (ret.type)
    {
        case RESTFUL_GET:
            getAddressContent(&ret, trips);
            break;
        case RESTFUL_POST:
            postAddressContent(&ret, trips);
            break;
        default:
            setResponseError(&ret);
            break;
    }

    return ret;

}