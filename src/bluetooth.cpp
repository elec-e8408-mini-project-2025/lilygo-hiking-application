#include "bluetooth.h"


// Made from the example: https://github.com/espressif/arduino-esp32/blob/master/libraries/BluetoothSerial/examples/SerialToSerialBT/SerialToSerialBT.ino

String device_name = "Hiking_Watch";

BluetoothSerial SerialBT;

String deviceName = DEVICE_NAME;

void initBluetooth()
{
    bool connected;
    SerialBT.begin(deviceName);

}

void handleBluetooth()
{

}