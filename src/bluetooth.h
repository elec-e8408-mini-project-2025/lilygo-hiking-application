#ifndef BLUETOOTH_H
#define BLUETOOTH_H
#include "BluetoothSerial.h"

#define BLUETOOTH_BUFFER_SIZE 256

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
  #error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

#define DEVICE_NAME "HIKING_WATCH"


void initBluetooth();


void handleBluetooth();














#endif