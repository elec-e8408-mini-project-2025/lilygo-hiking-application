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

    // Bluetooth interface
    initBluetooth();

    // Serial debugging interface
    initSerial();
}

void loop()
{

// Touch Screen interface
#ifndef ESP32_WROOM_32
    interfaceEvent interfaceEvent = handleTasksInterface(ttgo, &trips[systemVariables.currentTrip], &systemVariables);
    writeSerialString(interfaceEvent.serialString);
    switch (interfaceEvent.event)
    {
        case INTERFACE_TOGGLE_SESSION:
            // stopping session
            if (systemVariables.hasActiveSession)
            {
                ++systemVariables.currentTrip;
                systemVariables.currentTrip = systemVariables.currentTrip % (systemVariables.maxTrips); // If 5 it goes back to 0
            }
            systemVariables.hasActiveSession = !systemVariables.hasActiveSession;
            break;
        default:
            writeSerialString("INTERFACE EVENT: not implemented");
            break;
    }
    if (systemVariables.hasActiveSession)
    {
        trips[systemVariables.currentTrip].stepCount =  handleTasksAccelerator();
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

    // Bluetooth interface
    bluetoothBuffer bluetoothData = handleBluetoothByte();
    if (bluetoothData.status == BT_READY)
    {
        restfulData = restfulHandlePacket(bluetoothData.buf, &bluetoothData.bufLen, trips);
        writeBluetooth(restfulData.response, restfulData.responseLen);
    }

    // Short delay to avoid overloading the processor
    delay(5);
}
