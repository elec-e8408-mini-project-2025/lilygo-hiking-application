#include "./src/config.h"
#include "./src/data.h"
#include "./src/restful.h"
#include "./src/bluetooth.h"
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


// Serial debugging interface (Global Variables)
bool commandFinishedSer = 0;
int byteCountSer = 0;
char rxDataSer[MAX_RESTFUL_REQUEST_SIZE]; 


// Bluetooth (Global variables)
bool commandFinishedBT = 0;
int byteCountBT = 0;
char rxDataBT[MAX_RESTFUL_REQUEST_SIZE]; 

void setup() {
    // Touch Screen interface
    #ifndef ESP32_WROOM_32
    initInterface();
    #endif

    // Bluetooth interface
    initBluetooth();

    // Serial debugging interface
    Serial.begin(115200);
}

void loop() {
    
    // Touch Screen interface
    #ifndef ESP32_WROOM_32
    handleTasksInterface();
    #endif
   
    // Serial debugging interface
    if (Serial.available() > 0) {
        rxDataSer[byteCountSer] = Serial.read();
        Serial.print(rxDataSer[byteCountSer]);
        if(rxDataSer[byteCountSer] == 13)
        {
            Serial.println();
            commandFinishedSer = 1;
        }
        else
        {
            ++byteCountSer;
            if (byteCountSer == MAX_RESTFUL_REQUEST_SIZE)
            {
                byteCountSer = 0;
                commandFinishedSer = 0;
            }
        }
    }
    if(commandFinishedSer)
    {
        restfulPacket restfulData =  restfulHandlePacket(rxDataSer, &byteCountSer, trips);
        for (int i = 0; i < restfulData.responseLen; ++i)
        {
            Serial.print(restfulData.response[i]);
        }
        byteCountSer=0;
        commandFinishedSer = 0;
        Serial.println();
    }

    delay(5);          // Short delay to avoid overloading the processor
}

