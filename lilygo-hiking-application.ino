#include "./src/config.h"
#include "./src/data.h"
#include "./src/restful.h"
#include "./src/bluetooth.h"
#include "./src/serial.h"
#include "./src/net.h"
#include "./src/globals.h"
#ifndef ESP32_WROOM_32
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
    {0, 0, 0, 0, false, 0},
    {1, 0, 0, 0, false, 0},
    {2, 0, 0, 0, false, 0},
    {3, 0, 0, 0, false, 0},
    {4, 0, 0, 0, false, 0}
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

const uint32_t gpsAveragePointsInInterval = 5;
const uint32_t gpsAveragePollRate = gpsPollRate / gpsAveragePointsInInterval;

systemGlobals systemVariables = {0.8, 0, sizeof(trips)  / sizeof(trips[0]), false, false};
// int tripId = 0;


/*
 * Initializes LilyGO T-Watch Hiking application
 */
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
    initBluetooth();

    // Serial debugging interface
    initSerial();
    
}


/*
 * Main loop of the LilyGO T-Watch application
 */
void loop()
{


// Touch Screen interface
#ifndef ESP32_WROOM_32

    if (systemVariables.hasActiveSession)
    {
        trips[systemVariables.currentTrip].stepCount =  handleTasksAccelerator();
        
        // For LilyGO T-Watch V2 GPS functionalities are used
        #ifdef LILYGO_WATCH_2020_V2
        updateGPS();

        if (loopCounter % gpsAveragePollRate == 0) {
            addValueToAverage();
        }
        if (loopCounter % gpsPollRate == 0) {
            gpspoint = takeAverageStep();
            trips[systemVariables.currentTrip].distance += gpspoint.dist / 1000;
            date = rtc->getDateTime();
            timeStamp timeNow = createTimestampFromRTC(date);

            float secondsPassed = getTimeDifference(trips[systemVariables.currentTrip].timestampStart, timeNow);
            if (secondsPassed < 1) {
                trips[systemVariables.currentTrip].avgSpeed = 0;
            } else {
                trips[systemVariables.currentTrip].avgSpeed = trips[systemVariables.currentTrip].distance / (secondsPassed / 3600);
            }
        }
        
        #endif
        #ifndef LILYGO_WATCH_2020_V2
        
        trips[systemVariables.currentTrip].distance = trips[systemVariables.currentTrip].stepCount * systemVariables.step_length / 1000;
        date = rtc->getDateTime();
        timeStamp timeNow = createTimestampFromRTC(date);

        float secondsPassed = getTimeDifference(trips[systemVariables.currentTrip].timestampStart, timeNow);
        if (secondsPassed < 1) {
            trips[systemVariables.currentTrip].avgSpeed = 0;
        } else {
            trips[systemVariables.currentTrip].avgSpeed = trips[systemVariables.currentTrip].distance / (secondsPassed / 3600);
        }
        #endif

    }

    bool isRefreshSessionView = loopCounter % sessionViewRefreshRate == 0;
    
    /*
     * For efficiency the frequency of display render rate 
     * is lower than the frequency of the main loop
     */ 
    if (loopCounter % displayRefreshRate == 0 || (isRefreshSessionView && systemVariables.hasActiveSession)) {

        tripData * prevTrip = &trips[ (systemVariables.currentTrip + 4) % systemVariables.maxTrips]; // pointer to previous trip
        interfaceEvent interface;
        if (systemVariables.hasActiveSession) // if active session show current session
        {
            interface = handleTasksInterface(ttgo, &trips[systemVariables.currentTrip], &systemVariables, isRefreshSessionView, trips);
        }
        else // if no active session show previous session
        {
            interface = handleTasksInterface(ttgo, prevTrip, &systemVariables, isRefreshSessionView, trips);
        }
        
        switch (interface.event)
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
                    date = rtc->getDateTime();
                    trips[systemVariables.currentTrip].timestampStop = createTimestampFromRTC(date);
                    Serial.print("Stop Timestamp: ");
                    writeSerialRTCDateObj(trips[systemVariables.currentTrip].timestampStop);
                    systemVariables.currentTrip = (systemVariables.currentTrip + 1) % (systemVariables.maxTrips);
                    resetAccelerator();
                } else {
                    resetAccelerator(); 
                    trips[systemVariables.currentTrip] = {systemVariables.currentTrip, 0, 0, 0, false, 0};
                    date = rtc->getDateTime();
                    trips[systemVariables.currentTrip].timestampStart = createTimestampFromRTC(date);

                    Serial.print("Start Timestamp: ");
                    writeSerialRTCDateObj(trips[systemVariables.currentTrip].timestampStart);

                }
                systemVariables.hasActiveSession = !systemVariables.hasActiveSession;
                
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
    bluetoothBuffer bluetoothData = handleBluetoothByte();
    if (bluetoothData.status == BT_READY)
    {
        if (systemVariables.hasActiveSession) // ignore bluetooth requests if session is active to avoid reading active data
        {
            writeBluetooth("{}\n",3);
        }
        else
        {
            restfulData = restfulHandlePacket(bluetoothData.buf, &bluetoothData.bufLen, trips);
            writeBluetooth(restfulData.response, restfulData.responseLen);
        }
    }

    // Short delay to avoid overloading the processor
    ++loopCounter;
    delay(delayInMilliSeconds);
}
