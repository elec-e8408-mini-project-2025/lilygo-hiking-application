#include "serial.h"
bool commandFinishedSer = 0;
int byteCountSer = 0;
char rxDataSer[SERIAL_BUFFER_SIZE]; 

void initSerial()
{
    Serial.begin(115200);
};

// Read a byte per call and append to a buffer.
// If the byte is CARRIAGE FEED = 13 then handle packet as a restful packet 
serialBuffer handleSerialByte()
{
    serialBuffer ret = {
        0,
        0,
        SER_NOT_READY
    };

    if (Serial.available() > 0) {
        rxDataSer[byteCountSer] = Serial.read();
        Serial.print(rxDataSer[byteCountSer]);
        if(rxDataSer[byteCountSer] < 32)
        {
            commandFinishedSer = 1;
        }
        else
        {
            ++byteCountSer;
            if (byteCountSer == SERIAL_BUFFER_SIZE)
            {
                byteCountSer = 0;
                commandFinishedSer = 0;
            }
        }
    }
    if(commandFinishedSer)
    {
        // restfulPacket restfulData =  restfulHandlePacket(rxDataSer, &byteCountSer, trips);
        ret.buf = rxDataSer;
        ret.bufLen = byteCountSer;
        if(ret.bufLen)
        {
            ret.status = SER_READY;
        }
        byteCountSer=0;
        commandFinishedSer = 0;

    }
    return ret;
};


void writeSerial(char * data, int dataLen)
{
    for (int i = 0; i < dataLen; ++i)
    {
        Serial.print(data[i]);
    }
    byteCountSer=0;
    commandFinishedSer = 0;
    Serial.println();
}