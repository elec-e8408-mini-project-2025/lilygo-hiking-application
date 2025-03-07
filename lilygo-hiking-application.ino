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
RTC_Date date;
#endif
#ifdef LILYGO_WATCH_2020_V2
#include "./src/gps.h"
GPSPoint gpspoint;
#endif


// System data (Global Variables)
tripData trips[] = {
    {0, 0, 0, 0, false},
    {1, 0, 0, 0, false},
    {2, 0, 0, 0, false},
    {3, 0, 0, 0, false},
    {4, 0, 0, 0, false},
    {5, 0, 0, 0, false},
};

tripData *trip = &trips[0];

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

const uint32_t gpsAveragePointsInInterval = 5;
const uint32_t gpsAveragePollRate = gpsPollRate / gpsAveragePointsInInterval;

systemGlobals systemVariables = {0.8, 0, sizeof(trips)  / sizeof(trips[0]), false, false};
int tripId = 0;

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


// Touch Screen interface
#ifndef ESP32_WROOM_32

    if (systemVariables.hasActiveSession)
    {
        trip->stepCount =  handleTasksAccelerator();
        #ifdef LILYGO_WATCH_2020_V2
        updateGPS();
        // systemVariables.GPSavailable = isGPSavailable();
        if (loopCounter % gpsAveragePollRate == 0) {
            addValueToAverage();
        }
        if (loopCounter % gpsPollRate == 0) {
            gpspoint = takeAverageStep();
            trip->distance += gpspoint.dist / 1000;
            date = rtc->getDateTime();
            timeStamp timeNow = createTimestampFromRTC(date);

            float secondsPassed = getTimeDifference(trips[systemVariables.currentTrip].timestampStart, timeNow);
            if (secondsPassed < 1) {
                trip->avgSpeed = 0;
            } else {
                trip->avgSpeed = trip->distance / (secondsPassed / 3600);
            }
        }
        
        #endif
        #ifndef LILYGO_WATCH_2020_V2
        trip->distance = trip->stepCount * systemVariables.step_length / 1000;
        date = rtc->getDateTime();
        timeStamp timeNow = createTimestampFromRTC(date);

        float secondsPassed = getTimeDifference(trips[systemVariables.currentTrip].timestampStart, timeNow);
        if (secondsPassed < 1) {
            trip->avgSpeed = 0;
        } else {
            trip->avgSpeed = trip->distance / (secondsPassed / 3600);
        }
        #endif
        // Serial.print("Id: ");
        // Serial.print(trip->tripID);
        // Serial.print(", distance: ");
        // Serial.print(trip->distance);
        // Serial.print(", speed: ");
        // Serial.print(trip->avgSpeed);
        // Serial.print(", steps: ");
        // Serial.print(trip->stepCount);
        // Serial.print(", seconds: ");
        // Serial.println(secondsPassed);


    }

    bool isRefreshSessionView = loopCounter % sessionViewRefreshRate == 0;
    
    if (loopCounter % displayRefreshRate == 0 || (isRefreshSessionView && systemVariables.hasActiveSession)) {

        interfaceEvent interfaceEvent = handleTasksInterface(ttgo, trip, &systemVariables, isRefreshSessionView, trips);
        
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
                    Serial.println(trip->stepCount);
                    writeSerialString("Stopping session!");
                    date = rtc->getDateTime();
                    trip->timestampStop = createTimestampFromRTC(date);
                    Serial.print("Stop Timestamp: ");
                    writeSerialRTCDateObj(trip->timestampStop);
                    ++tripId;
                    int nextTrip = (systemVariables.currentTrip + 1) % (systemVariables.maxTrips); // If 5 it goes back to 0
                    trips[nextTrip] = {tripId, 0, 0, 0, false};
                    resetAccelerator();
                } else {
                    resetAccelerator();
                    ++systemVariables.currentTrip;
                    systemVariables.currentTrip = systemVariables.currentTrip % (systemVariables.maxTrips);
                    trip = &trips[systemVariables.currentTrip];
                    date = rtc->getDateTime();
                    trip->timestampStart = createTimestampFromRTC(date);

                    Serial.print("Start Timestamp: ");
                    writeSerialRTCDateObj(trip->timestampStart);

                }
                systemVariables.hasActiveSession = !systemVariables.hasActiveSession;
                // Serial.println(systemVariables.hasActiveSession);
                break;
            case INTERFACE_SYNC_GPS_TIME:
                writeSerialString("SYNCING GPS TIME");
                Serial.print("Current RTC: ");
                writeSerialRTCTime();
                #ifdef LILYGO_WATCH_2020_V2
                setRTCTime(rtc);
                #endif
                Serial.print("RTC after: ");
                writeSerialRTCTime();
            case INTERFACE_DEBUG:
                // Outputs debug information
                // writeSerialString(interfaceEvent.serialString);
                // writeSerialRTCTime(rtc);
                // writeSerialRTCTime(rtc->getDateTime());
                //Serial.println(rtc->formatDateTime());
                // writeSerialRTCTime();
                case INTERFACE_IDLE:
                break;
            default:
                writeSerialString("INTERFACE EVENT: not implemented");
                break;
        }
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
