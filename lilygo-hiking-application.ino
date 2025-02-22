#include "./src/config.h"
#include "./src/data.h"
#include "./src/restful.h"
#include "./src/bluetooth.h"
#include "./src/serial.h"
#include "./src/net.h"
#ifndef ESP32_WROOM_32
#include "./src/interface.h"
#include "./src/accelerator.h"
#include "./src/step.h"
TTGOClass *ttgo;
#endif


// System data (Global Variables)
tripData trips[] = {
    {0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {2, 0, 0, 0, 0},
    {3, 0, 0, 0, 0},
    {4, 0, 0, 0, 0},
};


// Global loop counter
long loopCounter = 0;
// hard coded constant for loop delay
const uint32_t delayInMilliSeconds = 5;
// interval in milliseconds for handling interface related tasks
const uint32_t displayRefreshIntervalMs = 20;
const uint32_t displayRefreshRate = displayRefreshIntervalMs / delayInMilliSeconds;

systemGlobals systemVariables = {0.8, 0, sizeof(trips)  / sizeof(trips[0]), false};

void setup()
{
// Touch Screen interface
#ifndef ESP32_WROOM_32
    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    initInterface(ttgo);
    initAccelerator(ttgo);
#endif

    // // Bluetooth interface
    // initBluetooth();

    // Serial debugging interface
    initSerial();
}

void loop()
{

// Touch Screen interface
#ifndef ESP32_WROOM_32
    // TODO: calculate from delay like: 20 ms / delay % == 0 -> MUST return int
    if (loopCounter % 4 == 0) {
        interfaceEvent interfaceEvent = handleTasksInterface(ttgo, &trips[systemVariables.currentTrip], &systemVariables);
        Serial.print("Handling resultData. Interface event is: ");
        Serial.println(interfaceEvent.event);

        if (interfaceEvent.event != INTERFACE_IDLE) {
            writeSerialString(interfaceEvent.serialString);
        }
        switch (interfaceEvent.event)
        {
            case INTERFACE_TOGGLE_SESSION:
                // stopping session
                writeSerialString("TOGGLE SESSION PRESSED!");
                if (systemVariables.hasActiveSession)
                {
                    Serial.print("Current trip: ");
                    Serial.println(systemVariables.currentTrip);
                    Serial.print("Step count: ");
                    Serial.println(trips[systemVariables.currentTrip].stepCount);
                    writeSerialString("Stopping session!");
                    ++systemVariables.currentTrip;
                    systemVariables.currentTrip = systemVariables.currentTrip % (systemVariables.maxTrips); // If 5 it goes back to 0
                    resetAccelerator();
                }
                systemVariables.hasActiveSession = !systemVariables.hasActiveSession;
                break;
            case INTERFACE_IDLE:
                break;
            default:
                writeSerialString("INTERFACE EVENT: not implemented");
                break;
        }
    }

    if (systemVariables.hasActiveSession)
    {
        trips[systemVariables.currentTrip].stepCount =  handleTasksAccelerator();
        
        // TODO: add computation for avgSpeed
        // trpis[systemVariables.currentTrip].avgSpeed 
    }
#endif

    restfulPacket restfulData;
    // Serial debugging interface
    serialBuffer serialData = handleSerialByte();
    if (serialData.status == SER_READY)
    {
        restfulData = restfulHandlePacket(serialData.buf, &serialData.bufLen, trips);
        writeSerial(restfulData.response, restfulData.responseLen);
    }

    // // Bluetooth interface
    // bluetoothBuffer bluetoothData = handleBluetoothByte();
    // if (bluetoothData.status == BT_READY)
    // {
    //     restfulData = restfulHandlePacket(bluetoothData.buf, &bluetoothData.bufLen, trips);
    //     writeBluetooth(restfulData.response, restfulData.responseLen);
    // }

    // Short delay to avoid overloading the processor
    ++loopCounter;
    delay(delayInMilliSeconds);
}
