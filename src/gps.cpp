#ifndef ESP32_WROOM_32
#ifdef LILYGO_WATCH_2020_V2
#include "gps.h"
#include <LilyGoWatch.h>

TinyGPSPlus *gps = nullptr;
HardwareSerial *GNSS = NULL;

double lastLat = NULL;
double lastLon = NULL;

double avgLat = 0;
double avgLon = 0;
int avgCount = 0;

uint32_t last = 0;
uint32_t updateTimeout = 0;

bool Quectel_L76X_Probe()
{
    bool result = false;
    GNSS->write("$PCAS03,0,0,0,0,0,0,0,0,0,0,,,0,0*02\r\n");
    delay(5);
    // Get version information
    GNSS->write("$PCAS06,0*1B\r\n");
    uint32_t startTimeout = millis() + 500;
    while (millis() < startTimeout) {
        if (GNSS->available()) {
            String ver = GNSS->readStringUntil('\r');
            // Get module info , If the correct header is returned,
            // it can be determined that it is the MTK chip
            int index = ver.indexOf("$");
            if (index != -1) {
                ver = ver.substring(index);
                if (ver.startsWith("$GPTXT,01,01,02")) {
                    Serial.println("L76K GNSS init succeeded, using L76K GNSS Module");
                    result = true;
                }
            }
        }
    }
    // Initialize the L76K Chip, use GPS + GLONASS
    GNSS->write("$PCAS04,5*1C\r\n");
    delay(250);
    // only ask for RMC and GGA
    GNSS->write("$PCAS03,1,0,0,0,1,0,0,0,0,0,,,0,0*02\r\n");
    delay(250);
    // Switch to Vehicle Mode, since SoftRF enables Aviation < 2g
    GNSS->write("$PCAS11,3*1E\r\n");
    return result;
}

void initGPS(TTGOClass *ttgo) {

    //Open gps power
    ttgo->trunOnGPS();

    ttgo->gps_begin();

    GNSS = ttgo->hwSerial;
    gps = ttgo->gps;

    if (!Quectel_L76X_Probe()) 
    {
        Serial.print("Failed GPS setup");
    }


    last = millis();
}

void updateGPS() {
    while (GNSS->available()) {
        int r = GNSS->read();
        // Serial.write(r);
        gps->encode(r);
    }
}

double getLat() {
    return gps->location.lat();
}

double getLon() {
    return gps->location.lng();
}


bool isGPSavailable()
{
    return gps->location.isUpdated() && gps->location.isValid();
}

GPSPoint takeStep() {
    double lat = 0;
    double lon = 0;
    double dist = 0;
    if (isGPSavailable()) {
        lat = getLat();
        lon = getLon();
        if (lastLat == NULL || lastLon == NULL) {
            lastLat = lat;
            lastLon = lon;
        } else {
            dist = TinyGPSPlus::distanceBetween(lastLat, lastLon, lat, lon);
        }
    }
    
    GPSPoint point = {lat, lon, dist};
    return point;
}



void setRTCTime(PCF8563_Class *rtc) {
    Serial.println("updating GPS");
    uint8_t year = gps->date.year() - 8; // There is small offset in the year
    uint8_t month = gps->date.month();
    uint8_t day = gps->date.day();
    uint8_t hour = gps->time.hour();
    uint8_t minute = gps->time.minute();
    uint8_t second = gps->time.second();
    rtc->setDateTime(year, month, day, hour, minute, second);
}


void addValueToAverage()
{
    double lat = 0;
    double lon = 0;
    if (isGPSavailable()) {
        lat = getLat();
        lon = getLon();
        avgLat = avgLat + lat;
        avgLon = avgLon + lon;
        avgCount++;
    }
}

GPSPoint takeAverageStep()
{
    addValueToAverage();
    double lat = 0;
    double lon = 0;
    double dist = 0;
    lat = avgLat / avgCount;
    lon = avgLon / avgCount;
    GPSPoint point = {NULL, NULL, 0};
    if (avgCount > 0) {
        if (lastLat == NULL || lastLon == NULL) {
            lastLat = lat;
            lastLon = lon;
        } else {
            dist = TinyGPSPlus::distanceBetween(lastLat, lastLon, lat, lon);
        }
        point = {lat, lon, dist};
    }
    avgCount = 0;
    avgLat = 0;
    avgLon = 0;
    return point;
} 


#endif
#endif