#include "restful.h"

restfulAddressView mainView;

restfulAddressView tripsView;

int initRestful(tripData *trips, int size)
{
    mainView.address = RESTFUL_MAIN_VIEW;
    char *fixedAddresses[] = FIXED_ADDRESSES;
    mainView.otherAddresses = fixedAddresses;
    mainView.dataCount = FIXED_ADDRESSES_COUNT;

    char tripAddressesTmp[MAX_CACHED_TRIPS][sizeof(RESTFUL_INDIVIDUAL_TRIP_VIEW)];
    char sampleTripAddress[] = RESTFUL_INDIVIDUAL_TRIP_VIEW; 
    for (int i = 0; i < MAX_CACHED_TRIPS; ++i)
    {
        for (int i2 = 0; i2 < sizeof(RESTFUL_INDIVIDUAL_TRIP_VIEW); ++i2)
        {
            tripAddressesTmp[i][i2] = sampleTripAddress[i2];
        }
        tripAddressesTmp[i][sizeof(RESTFUL_INDIVIDUAL_TRIP_VIEW) - 2] = (char) i + 48; 
    }
    
    char *tripAddresses[MAX_CACHED_TRIPS];
    char **tripAddressesPointer = tripAddresses;
    for (int i = 0; i < MAX_CACHED_TRIPS; ++i)
    {
        tripAddresses[i] = tripAddressesTmp[i];
    }
    tripsView.otherAddresses = tripAddressesPointer;
    tripsView.address = RESTFUL_TRIPS_VIEW;
    tripsView.dataCount = MAX_CACHED_TRIPS;
    return 0;
}


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


restfulPacket restfulHandlePacket(char *data, const int *dataLen)
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
            break;
        case RESTFUL_POST:
            break;
        default:
            break;
    }

    return ret;

}