#include "./src/config.h"
#include "./src/data.h"
#include "./src/restful.h"
#include "./src/bluetooth.h"
#include "./src/serial.h"
#include "./src/net.h"
#ifndef ESP32_WROOM_32
#include "./src/interface.h"
#include "./src/step.h"
#endif

// System data (Global Variables)
tripData trips[] = {
        {0,0,0,0,0},
        {1,0,0,0,0},
        {2,0,0,0,0},
        {3,0,0,0,0},
        {4,0,0,0,0},    
};

void setup() {
    // Touch Screen interface
    #ifndef ESP32_WROOM_32
    initInterface();
    #endif

    // Bluetooth interface
    initBluetooth();

    // Serial debugging interface
    initSerial();
}

void loop() {
    
    // Touch Screen interface
    #ifndef ESP32_WROOM_32
    handleTasksInterface();
    #endif
   
    // Serial debugging interface
    serialBuffer serialData = handleSerialByte();
    if (serialData.status == SER_READY)
    {
        restfulPacket restfulData =  restfulHandlePacket(serialData.buf, &serialData.bufLen, trips);
        writeSerial(restfulData.response, restfulData.responseLen);
    }


    // Bluetooth interface



    delay(5);          // Short delay to avoid overloading the processor
}

