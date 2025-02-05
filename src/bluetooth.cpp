#include "bluetooth.h"


// Made from the example: https://github.com/espressif/arduino-esp32/blob/master/libraries/BluetoothSerial/examples/SerialToSerialBT/SerialToSerialBT.ino

// Bluetooth (Global variables)
String device_name = "Hiking_Watch";
BluetoothSerial SerialBT;
String deviceName = DEVICE_NAME;
bool commandFinishedBT = 0;
int byteCountBT = 0;
char rxDataBT[BLUETOOTH_BUFFER_SIZE]; 

void initBluetooth()
{
    bool connected;
    SerialBT.begin(deviceName);

}

void handleBluetoothByte()
{

}