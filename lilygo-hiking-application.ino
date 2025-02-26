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
PCF8563_Class *rtc;
#endif
#ifdef LILYGO_WATCH_2020_V2
#include "./src/gps.h"
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
// to ensure optimal 
const uint32_t delayInMilliSeconds = 5;

// interval in milliseconds for handling interface related tasks
const uint32_t displayRefreshIntervalMs = 20;
const uint32_t displayRefreshRate = displayRefreshIntervalMs / delayInMilliSeconds;

// interval in milliseconds for updating session view data
const uint32_t sessionViewRefreshIntervalMs = 250;
const uint32_t sessionViewRefreshRate = sessionViewRefreshIntervalMs / delayInMilliSeconds;

systemGlobals systemVariables = {0.8, 0, sizeof(trips)  / sizeof(trips[0]), false};

void setup()
{
// Touch Screen interface
#ifndef ESP32_WROOM_32
    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    
    rtc = ttgo->rtc;
    // Use compile time
    rtc->check();

    initInterface(ttgo);
    initAccelerator(ttgo);
    initGPS(ttgo);
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
    bool isRefreshSessionView = loopCounter % sessionViewRefreshRate == 0;
    
    if (loopCounter % displayRefreshRate == 0 || isRefreshSessionView) {
        

        interfaceEvent interfaceEvent = handleTasksInterface(ttgo, &trips[systemVariables.currentTrip], &systemVariables, isRefreshSessionView);
        
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
                    trips[systemVariables.currentTrip].timestampStop = millis();
                    ++systemVariables.currentTrip;
                    systemVariables.currentTrip = systemVariables.currentTrip % (systemVariables.maxTrips); // If 5 it goes back to 0
                    resetAccelerator();
                } else {
                    trips[systemVariables.currentTrip].timestampStart = millis();

                    Serial.print("Timestamp: ");
                    Serial.println(trips[systemVariables.currentTrip].timestampStart);

                }
                systemVariables.hasActiveSession = !systemVariables.hasActiveSession;
                break;
            case INTERFACE_SYNC_GPS_TIME:
                writeSerialString("SYNCING GPS TIME");
                // Serial.print("Current RTC");
                // Serial.println(rtc->formatDateTime(PCF_TIMEFORMAT_YYYY_MM_DD_H_M_S));
                //Serial.print("Coming date");
                //Serial.println(getDateTime());
                setRTCTime(rtc);
                // Serial.print("RTC after");
                // Serial.println(rtc->formatDateTime(PCF_TIMEFORMAT_YYYY_MM_DD_H_M_S));
            case INTERFACE_DEBUG:
                // Outputs debug information
                writeSerialString(interfaceEvent.serialString);
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
