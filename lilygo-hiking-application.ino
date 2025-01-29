#include "./src/config.h"
#include "./src/data.h"
#include "./src/restful.h"
#include "./src/net.h"
#ifndef ESP32_WROOM_32
#include "./src/interface.h"
#include "./src/step.h"
#endif

void setup() {
    Serial.begin(115200);

    #ifndef ESP32_WROOM_32
    initInterface();
    #endif
}

void loop() {
    #ifndef ESP32_WROOM_32
    handleTasksInterface();
    #endif
    delay(5);          // Short delay to avoid overloading the processor
}

