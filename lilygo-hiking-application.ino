#include "./src/config.h"
#include "./src/data.h"
#include "./src/restful.h"
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


// Serial debugging (Global Variables)
bool commandFinished = 0;
int byteCount = 0;
char rxDataSerial[MAX_RESTFUL_REQUEST_SIZE]; 


void setup() {
    // Touch Screen interface
    #ifndef ESP32_WROOM_32
    initInterface();
    #endif

    // Serial debugging 
    Serial.begin(115200);
}

void loop() {
    
    // Touch Screen interface
    #ifndef ESP32_WROOM_32
    handleTasksInterface();
    #endif
   
    // Serial debugging 
    if (Serial.available() > 0) {
        rxDataSerial[byteCount] = Serial.read();
        Serial.print(rxDataSerial[byteCount]);
        if(rxDataSerial[byteCount] == 13)
        {
            Serial.println();
            commandFinished = 1;
        }
        else
        {
            ++byteCount;
            if (byteCount == MAX_RESTFUL_REQUEST_SIZE)
            {
                byteCount = 0;
                commandFinished = 0;
            }
        }
    }
    if(commandFinished)
    {
        restfulPacket restfulData =  restfulHandlePacket(rxDataSerial, &byteCount, trips);
        for (int i = 0; i < restfulData.responseLen; ++i)
        {
            Serial.print(restfulData.response[i]);
        }
        byteCount=0;
        commandFinished = 0;
        Serial.println();
    }
    
    delay(5);          // Short delay to avoid overloading the processor
}

