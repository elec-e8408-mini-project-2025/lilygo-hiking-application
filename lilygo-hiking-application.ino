#include "./src/config.h"
#include "./src/data.h"
#include "./src/restful.h"

#ifndef ESP32_WROOM_32
#include "./src/twatch/interface.h"
#include "./src/twatch/step.h"
TTGOClass *ttgo;
#include <LilyGoWatch.h>
#endif



void setup() {
    Serial.begin(115200);

    #ifndef ESP32_WROOM_32
    // ref: https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library/blob/master/examples/LVGL/Lvgl_Button/Lvgl_Button.ino#L18
    ttgo = TTGOClass::getWatch(); // get an instance of TTGO class
    ttgo->begin();                // Initialize TTGO smartwatch hardware
    ttgo->openBL();               // Turn on the blacklight of the TTGO smartwatch display
    ttgo->lvgl_begin();           // Initialize LVGL graphics library for TTGO smartwatch

    // Initialize views
    createMainMenuView();
    createSettingsView();
    createSessionView();
    createPastSessionsView();

    // Load the initial screen (Main Menu)
    // ref: https://docs.lvgl.io/8/overview/display.html
    lv_scr_load(main_view);
    #endif
}

void loop() {
    #ifndef ESP32_WROOM_32
    lv_task_handler(); // Handle LVGL tasks
    #endif
    delay(5);          // Short delay to avoid overloading the processor
}

