#ifndef ESP32_WROOM_32
#include "gps.h"

TinyGPSPlus *gps = nullptr;
HardwareSerial *GNSS = NULL;

double lastLat = NULL;
double lastLon = NULL;

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

    ttgo->begin();

    //Open gps power
    ttgo->trunOnGPS();

    ttgo->gps_begin();

    GNSS = ttgo->hwSerial;
    gps = ttgo->gps;

    if (!Quectel_L76X_Probe()) 
    {
        Serial.print("Failed GPS setup");
    }

    // Display on the screen, latitude and longitude, number of satellites, and date and time
    //eSpLoaction   = new TFT_eSprite(tft); // Sprite object for eSpLoaction
    //eSpDate   = new TFT_eSprite(tft); // Sprite object for eSpDate
    //eSpTime   = new TFT_eSprite(tft); // Sprite object for eSpTime
    //eSpSatellites   = new TFT_eSprite(tft); // Sprite object for eSpSatellites

    //eSpLoaction->createSprite(240, 48);
    //eSpLoaction->setTextFont(2);

    //eSpDate->createSprite(240, 48);
    //eSpDate->setTextFont(2);

    //eSpTime->createSprite(240, 48);
    //eSpTime->setTextFont(2);

    //eSpSatellites->createSprite(240, 48);
    //eSpSatellites->setTextFont(2);

    last = millis();
}

double getLat() {
    return gps->location.lat();
}

double getLon() {
    return gps->location.lng();
}

uint32_t getTime() {
    return gps->time.value();
}

uint32_t getDate() {
    return gps->date.value();
}

GPSPoint takeStep() {
    double lat = getLat();
    double lon = getLon();
    double dist = 0;
    if (lastLat == NULL || lastLon == NULL) {
        lastLat = lat;
        lastLon = lon;
    } else {
        dist = TinyGPSPlus::distanceBetween(lastLat, lastLon, lat, lon);
    }
    GPSPoint point = {lat, lon, dist};
    return point;
}

/*
uint8_t, uint8_t, uint8_t getTime() {
    uint8_t hour = gps->time.hour();
    uint8_t minute = gps->time.minute();
    uint8_t second = gps->time.second();
    return hours, minutes, seconds;
}

uint8_t, uint8_t, uint8_t getDate() {
    uint8_t year = gps->time.year();
    uint8_t month = gps->time.month();
    uint8_t day = gps->time.day();
    return hours, minutes, seconds;
}
*/


void setRTCTime(PCF8563_Class *rtc) {
    uint8_t year = gps->date.year();
    uint8_t month = gps->date.month();
    uint8_t day = gps->date.day();
    uint8_t hour = gps->time.hour();
    uint8_t minute = gps->time.minute();
    uint8_t second = gps->time.second();
    rtc->setDateTime(year, month, day, hour, minute, second);
}

#endif