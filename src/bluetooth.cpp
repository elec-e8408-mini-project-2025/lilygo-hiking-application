#include "bluetooth.h"


// Made from the example: https://github.com/espressif/arduino-esp32/blob/master/libraries/BluetoothSerial/examples/SerialToSerialBT/SerialToSerialBT.ino

// Bluetooth (Global variables)
String device_name = "Hiking_Watch";
String deviceName = DEVICE_NAME;

BluetoothSerial SerialBT;
bool commandFinishedBT = 0;
int byteCountBT = 0;
char rxDataBT[BLUETOOTH_BUFFER_SIZE]; 

void initBluetooth()
{
    bool connected;
    SerialBT.begin(deviceName);

}

bluetoothBuffer handleBluetoothByte()
{
    bluetoothBuffer ret = {
        0,
        0,
        BT_NOT_READY
    };

    if (SerialBT.available() > 0) 
    {
        
        rxDataBT[byteCountBT] = (char) SerialBT.read();

        if(rxDataBT[byteCountBT] < 32)
        {
            commandFinishedBT = 1;
        }
        else
        {
            ++byteCountBT;
            if (byteCountBT == BLUETOOTH_BUFFER_SIZE)
            {
                byteCountBT = 0;
                commandFinishedBT = 0;
            }
        }
    }
    if(commandFinishedBT)
    {
        // restfulPacket restfulData =  restfulHandlePacket(rxDataSer, &byteCountSer, trips);
        ret.buf = rxDataBT;
        ret.bufLen = byteCountBT;
        if(ret.bufLen)
        {
            ret.status = BT_READY;
        }
        byteCountBT=0;
        commandFinishedBT = 0;

    }
    return ret;
}

void writeBluetooth(char * data, int dataLen)
{
    SerialBT.write((const uint8_t*) data, dataLen);
    byteCountBT=0;
    commandFinishedBT = 0;
}