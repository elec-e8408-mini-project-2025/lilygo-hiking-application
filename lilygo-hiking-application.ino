#include "./src/config.h"
#include "./src/data.h"
#include "./src/restful.h"
#include "./src/bluetooth.h"
#include "./src/serial.h"
#include "./src/net.h"
#include "./src/globals.h"
#ifndef ESP32_WROOM_32
// #include <LilyGoWatch.h>
#include "./src/interface.h"
#include "./src/accelerator.h"
#include "./src/step.h"
TTGOClass *ttgo;
PCF8563_Class *rtc;
#endif
#ifdef LILYGO_WATCH_2020_V2
#include "./src/gps.h"
GPSPoint gpsPoint;
#endif


// System data (Global Variables)
tripData trips[] = {
    {0, 0, 0},
    {1, 0, 0},
    {2, 0, 0},
    {3, 0, 0},
    {4, 0, 0},
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

// interval in milliseconds for getting gps data
const uint32_t gpsPollIntervalMs = 10000;
const uint32_t gpsPollRate = gpsPollIntervalMs / delayInMilliSeconds;

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
    #ifdef LILYGO_WATCH_2020_V2
    initGPS(ttgo);
    #endif
#endif

    // // Bluetooth interface
    // initBluetooth();

    // Serial debugging interface
    initSerial();
    //xTaskCreatePinnedToCore(loopTask, "LoopTask", 4096, NULL, 1, NULL, 1);  // Run on Core 1
}

void loop()
{
    #ifdef LILYGO_WATCH_2020_V2
    updateGPS();
    if (loopCounter % gpsPollRate == 0 || systemVariables.hasActiveSession) {
        gpsPoint = takeStep();
        trips[systemVariables.currentTrip].distance += gpsPoint.dist;
    }
    #endif

// Touch Screen interface
#ifndef ESP32_WROOM_32
    bool isRefreshSessionView = loopCounter % sessionViewRefreshRate == 0;
    
    if (loopCounter % displayRefreshRate == 0 || (isRefreshSessionView && systemVariables.hasActiveSession)) {
        
        interfaceEvent interfaceEvent = handleTasksInterface(ttgo, rtc, &trips[systemVariables.currentTrip], &systemVariables, isRefreshSessionView);

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
                    trips[systemVariables.currentTrip].timestampStop = createTimestampFromRTC(rtc->getDateTime());
                    Serial.print("Stop Timestamp: ");
                    writeSerialRTCDateObj(trips[systemVariables.currentTrip].timestampStop);
                    ++systemVariables.currentTrip;
                    systemVariables.currentTrip = systemVariables.currentTrip % (systemVariables.maxTrips); // If 5 it goes back to 0
                    resetAccelerator();
                } else {
                    trips[systemVariables.currentTrip].timestampStart = createTimestampFromRTC(rtc->getDateTime());

                    Serial.print("Start Timestamp: ");
                    writeSerialRTCDateObj(trips[systemVariables.currentTrip].timestampStart);

                }
                systemVariables.hasActiveSession = !systemVariables.hasActiveSession;
                break;
            case INTERFACE_SYNC_GPS_TIME:
                writeSerialString("SYNCING GPS TIME");
                Serial.print("Current RTC: ");
                writeSerialRTCTime();
                setRTCTime(rtc);
                Serial.print("RTC after: ");
                writeSerialRTCTime();
            case INTERFACE_DEBUG:
                // Outputs debug information
                writeSerialString(interfaceEvent.serialString);
                // writeSerialRTCTime(rtc);
                // writeSerialRTCTime(rtc->getDateTime());
                //Serial.println(rtc->formatDateTime());
                writeSerialRTCTime();
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
