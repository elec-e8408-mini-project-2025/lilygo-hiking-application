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
            addressOffset = 4; // GET <-4 characters
            break;
        case RESTFUL_POST:
            addressOffset = 8; // POST <- 8 characters
            break;
        default:
            return -1;
            break;
    };
    
    packet->addressData = data + addressOffset;
    if ( addressOffset > *dataLen )
    {
        packet->addressLen = 0;
    }
    else 
    {
        packet->addressLen = *dataLen - addressOffset;
    }

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
    char startTimeStamp[32] = "";
    char stopTimeStamp[32] = "";
    sprintf(startTimeStamp, "%u-%u-%u %u:%u:%u", trip.timestampStart.year, trip.timestampStart.month, trip.timestampStart.day, trip.timestampStart.hour, trip.timestampStart.minute, trip.timestampStart.second);
    sprintf(stopTimeStamp, "%u-%u-%u %u:%u:%u", trip.timestampStop.year, trip.timestampStop.month, trip.timestampStop.day, trip.timestampStop.hour, trip.timestampStop.minute, trip.timestampStop.second);
    packet->responseLen = sprintf(packet->response, RESTFUL_ONE_TRIP_VIEW, trip.tripID, trip.tripID, startTimeStamp, stopTimeStamp, trip.stepCount, trip.avgSpeed, trip.tag, trip.tripID);
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

int setResponseSuccessAction(restfulPacket * packet)
{
    packet->responseLen = sprintf(packet->response, "%s", RESTFUL_SUCCESS_ACTION);
    return 0;
}

int setTag(restfulPacket * packet, tripData *trips)
{

    int tripId = packet->addressData[packet->addressLen - 1 - 7] - '0';

    if ( tripId < MAX_CACHED_TRIPS)
    {
        ++trips[tripId].tag;
        setResponseSuccessAction(packet);
    }
    else
    {
        setResponseError(packet);
    }
    return 0;
}

int postAddressContent(restfulPacket * packet, tripData *trips)
{
    if (packet->type == RESTFUL_ERROR)
    {
        setResponseError(packet);
        return 0;
    }

    switch(packet->addressLen)
    {
        case sizeof(RESTFUL_TRIP_ACTION_SET_TAG) - 2:
            setTag(packet, trips);
            break;
        default:
            setResponseError(packet);
            break;
    }

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
    
    if (ret.addressLen == 0)
    {
        setResponseError(&ret);
        return ret;
    }
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