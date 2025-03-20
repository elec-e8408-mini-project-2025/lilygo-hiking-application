#include "serial.h"
#include <LilyGoWatch.h>
#include "data.h"
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

void writeSerialString(char * data)
{
    Serial.println(data);
}

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

void writeSerialRTCTime()
{
    if (rtc == nullptr) {
        Serial.println("RTC object is null!");
        return;
    }

    Wire.beginTransmission(0x00);  // Dummy transmission
    Wire.endTransmission();
    Serial.println(rtc->formatDateTime(5));
}

void writeSerialRTCDateObj(RTC_Date date)
{
    Serial.print(date.year);
    Serial.print("-");
    Serial.print(date.month);
    Serial.print("-");
    Serial.print(date.day);
    Serial.print(" / ");
    Serial.print(date.hour);
    Serial.print(":");
    Serial.print(date.minute);
    Serial.print(":");
    Serial.println(date.second);

}

void writeSerialRTCDateObj(timeStamp date)
{
    Serial.print(date.year);
    Serial.print("-");
    Serial.print(date.month);
    Serial.print("-");
    Serial.print(date.day);
    Serial.print(" / ");
    Serial.print(date.hour);
    Serial.print(":");
    Serial.print(date.minute);
    Serial.print(":");
    Serial.println(date.second);

}