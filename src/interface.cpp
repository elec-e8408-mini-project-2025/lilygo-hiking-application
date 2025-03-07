#ifndef ESP32_WROOM_32
#include "interface.h"
#include <LilyGoWatch.h>
#include "globals.h"
#include "data.h"
#include <string>
// #include "globals.h"


// For toggling display state
bool displayOn = true;
bool irqPEK = false;
bool GPSavailable = false;

uint32_t stepCount = 0;
float step_length = 0.76;
float avgSpeed = 0.0;
float distance = 0.0;
timeStamp sessionStartTime;
timeStamp currentTime;
bool hasActiveSession = false;
uint32_t nOfTrips = 0;


tripData *pastTrips; 


interfaceEvent returnData = {INTERFACE_IDLE, ""};

/*
 * Declare global variables for buttons and views
 * lv_obj_t: Create a base object (a rectangle)
 */
lv_obj_t *main_view, *settings_view, *session_view, *past_sessions_view;
lv_obj_t *settings_btn, *manual_sync_btn, *session_btn, *past_sessions_btn, *sync_clock_btn;
lv_obj_t *main_menu_btn1, *main_menu_btn2, *main_menu_btn3;
lv_obj_t *past_sessions_data;

// GLobal style variables
// ref: https://docs.lvgl.io/8.0/overview/style.html
static lv_style_t cont_style;
static lv_style_t lbl_style_white;
static lv_style_t lbl_style_black;
static lv_style_t btn_style_blue;
static lv_style_t btn_style_red;

// Session view labels containing updateable values
lv_obj_t *distanceValue, *stepsValue, *avgSpeedValue, *toggle_session_lbl, *toggle_session_btn;

// Event handler function
static void event_handler(lv_obj_t *obj, lv_event_t event)
{
    // ref: https://docs.lvgl.io/8/overview/event.html?highlight=lv_event_clicked#events
    if (event == LV_EVENT_CLICKED)
    {
        //Serial.print("Button clicked: ");returnData
        returnData.event = INTERFACE_DEBUG;

        if (obj == settings_btn)
        {
            //Serial.println("Settings");
            returnData.serialString = "Button click: Settings";
            lv_scr_load(settings_view); // Load the settings view
        }
        else if (obj == session_btn)
        {
            //Serial.println("Session");
            returnData.serialString = "Button click: Session";
            lv_obj_clean(session_view);
            createSessionView();
            lv_scr_load(session_view); // Load the session view
        }
        else if (obj == past_sessions_btn)
        {
            //Serial.println("Past Sessions");
            returnData.serialString = "Button click: Past Sessions";
            updatePastSessionData();
            lv_scr_load(past_sessions_view); // load past sessions view
        }
        // TODO: This could perhaps be done more nicely?
        else if (obj == main_menu_btn1 || obj == main_menu_btn2 || obj == main_menu_btn3)
        {
            //Serial.println("Main view");
            returnData.serialString = "Button click: Main view";
            lv_scr_load(main_view); // load main menu view
        }
        else if (obj == toggle_session_btn)
        {
            returnData.serialString = "Toggle Session";
            returnData.event = INTERFACE_TOGGLE_SESSION;
            //Serial.println("Toggle Session");

            // Empty step count to prevent screen from rendering old count before reset
            stepCount = 0;
            avgSpeed = 0.0;
            distance = 0.0;
            // hasActiveSession = !hasActiveSession;
        }
        else if (obj == sync_clock_btn)
        {
            returnData.event = INTERFACE_SYNC_GPS_TIME;
        }
    }
}

void updatePastSessionData() {
    
    Serial.print("Updating past session data, size of past trips: ");
    Serial.println(nOfTrips);
    uint32_t takenTripsCounter = 0;
    
    // Using Arduino String library
    String data = "";
    
    for (int i = 0; i < nOfTrips; i++) {
        if (pastTrips[i].stepCount == 0) {
            continue;
        }
        takenTripsCounter++;
        float distance = pastTrips[i].distance;
        float speed = pastTrips[i].avgSpeed;
        int id = pastTrips[i].tripID;
        // TODO: as a future refactor this String formation could be made more efficient
        Serial.print("Id: ");
        Serial.print(id);
        Serial.print(", distance: ");
        Serial.print(distance);
        Serial.print(", speed: ");
        Serial.println(speed);
        data += String("25-02-28: " + String(distance, 1) + " km " + String(speed, 1) + " km/h\n");
    }

    // Serial.println(data);
    // Each table cell has 12 characters 

    if (data.length() > 0) {
        Serial.print("Refreshing past sessions text label. Trips taken: ");
        Serial.println(takenTripsCounter);
        String header = "PAST HIKING SESSIONS: \n";
        header += data;
        lv_label_set_text(past_sessions_data, header.c_str());
    }
    
}

// Function to create the Main Menu view
void createMainMenuView()
{

    main_view = lv_cont_create(NULL, NULL);
    lv_obj_set_size(main_view, 240, 240);
    lv_obj_add_style(main_view, LV_OBJ_PART_MAIN, &cont_style);

    // Label for title
    lv_obj_t *main_view_title = lv_label_create(main_view, NULL);
    lv_label_set_text(main_view_title, "HIKING APP");
    lv_obj_align(main_view_title, main_view, LV_ALIGN_CENTER, 0, -80);
    lv_obj_add_style(main_view_title, LV_OBJ_PART_MAIN, &lbl_style_white);

    // Button for new session
    session_btn = lv_btn_create(main_view, NULL);
    lv_obj_set_event_cb(session_btn, event_handler); // Set event handler
    lv_obj_align(session_btn, main_view, LV_ALIGN_CENTER, 0, -50);

    lv_obj_t *session_lbl = lv_label_create(session_btn, NULL);
    lv_label_set_text(session_lbl, "NEW SESSION");
    lv_obj_add_style(session_btn, LV_OBJ_PART_MAIN, &btn_style_blue);
    lv_obj_add_style(session_lbl, LV_OBJ_PART_MAIN, &lbl_style_white);

    // Button for past sessions
    past_sessions_btn = lv_btn_create(main_view, NULL);
    lv_obj_set_event_cb(past_sessions_btn, event_handler); // Set event handler
    lv_obj_align(past_sessions_btn, main_view, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *past_sessions_lbl = lv_label_create(past_sessions_btn, NULL);
    lv_label_set_text(past_sessions_lbl, "PAST SESSIONS");
    lv_obj_add_style(past_sessions_btn, LV_OBJ_PART_MAIN, &btn_style_blue);
    lv_obj_add_style(past_sessions_lbl, LV_OBJ_PART_MAIN, &lbl_style_white);

    // Button for Settings
    settings_btn = lv_btn_create(main_view, NULL);
    lv_obj_set_event_cb(settings_btn, event_handler); // Set event handler
    lv_obj_align(settings_btn, main_view, LV_ALIGN_CENTER, 0, 50);

    lv_obj_t *settings_lbl = lv_label_create(settings_btn, NULL);
    lv_label_set_text(settings_lbl, "SETTINGS");
    lv_obj_add_style(settings_btn, LV_OBJ_PART_MAIN, &btn_style_blue);
    lv_obj_add_style(settings_lbl, LV_OBJ_PART_MAIN, &lbl_style_white);
}

// Function to create Session view
void createSessionView()
{

    session_view = lv_cont_create(NULL, NULL);
    lv_obj_set_size(session_view, 240, 240);
    lv_obj_add_style(session_view, LV_OBJ_PART_MAIN, &cont_style);

    // Label for steps title
    lv_obj_t *stepsTitle = lv_label_create(session_view, NULL);
    lv_obj_add_style(stepsTitle, LV_OBJ_PART_MAIN, &lbl_style_white);
    lv_label_set_text(stepsTitle, "STEPS");
    lv_obj_align(stepsTitle, session_view, LV_ALIGN_IN_TOP_LEFT, 20, 20);

    // Label for steps value
    stepsValue = lv_label_create(session_view, NULL);
    lv_obj_add_style(stepsValue, LV_OBJ_PART_MAIN, &lbl_style_white);
    char lblTextstepCount[32]; // Ensure the buffer is large enough
    sprintf(lblTextstepCount, "%u", stepCount);
    lv_label_set_text(stepsValue, lblTextstepCount);
    lv_obj_align(stepsValue, session_view, LV_ALIGN_IN_TOP_RIGHT, -60, 20);

    // Label for distance title
    lv_obj_t *distanceTitle = lv_label_create(session_view, NULL);
    lv_obj_add_style(distanceTitle, LV_OBJ_PART_MAIN, &lbl_style_white);
    lv_label_set_text(distanceTitle, "DISTANCE");
    lv_obj_align(distanceTitle, session_view, LV_ALIGN_IN_TOP_LEFT, 20, 40);

    // Label for distance unit
    lv_obj_t *distanceUnit = lv_label_create(session_view, NULL);
    lv_obj_add_style(distanceUnit, LV_OBJ_PART_MAIN, &lbl_style_white);
    lv_label_set_text(distanceUnit, "km");
    lv_obj_align(distanceUnit, session_view, LV_ALIGN_IN_TOP_LEFT, 190, 40);

    // Label for distance value
    distanceValue = lv_label_create(session_view, NULL);
    lv_obj_add_style(distanceValue, LV_OBJ_PART_MAIN, &lbl_style_white);
    char lblDistanceValue[32]; // Make sure buffer is large enough
    sprintf(lblDistanceValue, "%.2f", distance);
    lv_label_set_text(distanceValue, lblDistanceValue);
    lv_obj_align(distanceValue, session_view, LV_ALIGN_IN_TOP_RIGHT, -60, 40);

    // Label for avg.speed title
    lv_obj_t *avgSpeedTitle = lv_label_create(session_view, NULL);
    lv_obj_add_style(avgSpeedTitle, LV_OBJ_PART_MAIN, &lbl_style_white);
    lv_label_set_text(avgSpeedTitle, "AVG.SPEED");
    lv_obj_align(avgSpeedTitle, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 60);

    // Label for avg.speed unit
    lv_obj_t *avgSpeedUnit = lv_label_create(session_view, NULL);
    lv_obj_add_style(avgSpeedUnit, LV_OBJ_PART_MAIN, &lbl_style_white);
    lv_label_set_text(avgSpeedUnit, "km/h");
    lv_obj_align(avgSpeedUnit, session_view, LV_ALIGN_IN_TOP_LEFT, 190, 60);

    // Label for steps value
    avgSpeedValue = lv_label_create(session_view, NULL);
    lv_obj_add_style(avgSpeedValue, LV_OBJ_PART_MAIN, &lbl_style_white);
    char lblTextAvgSpeedValue[32]; // Ensure the buffer is large enough
    sprintf(lblTextAvgSpeedValue, "%.2f", avgSpeed);
    lv_label_set_text(avgSpeedValue, lblTextAvgSpeedValue);
    lv_obj_align(avgSpeedValue, session_view, LV_ALIGN_IN_TOP_RIGHT, -60, 60);

    // Button for starting a Session
    toggle_session_btn = lv_btn_create(session_view, NULL);
    lv_obj_set_event_cb(toggle_session_btn, event_handler); // Set event handler
    lv_obj_align(toggle_session_btn, NULL, LV_ALIGN_CENTER, 0, 10);

    toggle_session_lbl = lv_label_create(toggle_session_btn, NULL);
    lv_label_set_text(toggle_session_lbl, "Start");
    lv_obj_add_style(toggle_session_btn, LV_OBJ_PART_MAIN, &btn_style_blue);
    lv_obj_add_style(toggle_session_lbl, LV_OBJ_PART_MAIN, &lbl_style_white);

    // Button for Main Menu
    main_menu_btn1 = lv_btn_create(session_view, NULL);
    lv_obj_set_event_cb(main_menu_btn1, event_handler); // Set event handler
    lv_obj_align(main_menu_btn1, NULL, LV_ALIGN_CENTER, 0, 60);

    lv_obj_t *main_menu_lbl = lv_label_create(main_menu_btn1, NULL);
    lv_label_set_text(main_menu_lbl, "Main Menu");
    lv_obj_add_style(main_menu_btn1, LV_OBJ_PART_MAIN, &btn_style_blue);
    lv_obj_add_style(main_menu_lbl, LV_OBJ_PART_MAIN, &lbl_style_white);
}

/*
 * Helper function that refreshes session view at predefined intervals
 * when session view is rendered
 */
void refreshSessionView()
{
    // Serial.println("refreshSesssionView.BEGIN");
    // ref: https://docs.lvgl.io/7.11/get-started/quick-overview.html#widgets
    if (session_view == lv_scr_act())
    {
        // Serial.println("refreshSessionView.cleanAndLoadSessionView");
        returnData.event = INTERFACE_DEBUG;
        returnData.serialString = "Refreshing sessiong view";
        // Update stepCount value
        // Update distance value
        char lblDistanceValue[32]; // Make sure buffer is large enough
        sprintf(lblDistanceValue, "%.2f", distance);
        lv_label_set_text(distanceValue, lblDistanceValue);
        // update step value
        char lblTextstepCount[32]; // Ensure the buffer is large enough
        sprintf(lblTextstepCount, "%u", stepCount);
        lv_label_set_text(stepsValue, lblTextstepCount);

        // Serial.println("avg speed update");

        char lblTextAvgSpeedValue[32]; // Ensure the buffer is large enough
        sprintf(lblTextAvgSpeedValue, "%.2f", avgSpeed);
        lv_label_set_text(avgSpeedValue, lblTextAvgSpeedValue);

        // Serial.print("Distance: ");
        // Serial.print(distance);
        // Serial.print(", Seconds passed: ");
        // Serial.print(secondsPassed);
        // Serial.print(" Speed: ");
        // Serial.println(avgSpeed);
        // Serial.println("only button to be updated");
    }
        
    if (hasActiveSession)
    {
        lv_label_set_text(toggle_session_lbl, "Stop");
        lv_obj_add_style(toggle_session_btn, LV_OBJ_PART_MAIN, &btn_style_red);
        lv_obj_add_style(toggle_session_lbl, LV_OBJ_PART_MAIN, &lbl_style_black);
    }
    else
    {
        lv_label_set_text(toggle_session_lbl, "Start");
        lv_obj_add_style(toggle_session_btn, LV_OBJ_PART_MAIN, &btn_style_blue);
        lv_obj_add_style(toggle_session_lbl, LV_OBJ_PART_MAIN, &lbl_style_white);
    }
    // Serial.println("refreshSesssionView.END");
}

// Function to create Settings view
void createSettingsView()
{

    settings_view = lv_cont_create(NULL, NULL);
    lv_obj_set_size(settings_view, 240, 240);
    lv_obj_add_style(settings_view, LV_OBJ_PART_MAIN, &cont_style);

    // Label for Settings
    lv_obj_t *settingsTitle = lv_label_create(settings_view, NULL);
    lv_label_set_text(settingsTitle, "Change settings here");
    lv_obj_align(settingsTitle, NULL, LV_ALIGN_CENTER, 0, -80);
    lv_obj_add_style(settingsTitle, LV_OBJ_PART_MAIN, &lbl_style_white);

    // Manual sync button
    manual_sync_btn = lv_btn_create(settings_view, NULL);
    lv_obj_set_event_cb(manual_sync_btn, event_handler); // Set event handler
    lv_obj_align(manual_sync_btn, settings_view, LV_ALIGN_CENTER, 0, -50);

    lv_obj_t *manual_sync_lbl = lv_label_create(manual_sync_btn, NULL);
    lv_label_set_text(manual_sync_lbl, "Manual sync");
    lv_obj_add_style(manual_sync_btn, LV_OBJ_PART_MAIN, &btn_style_blue);
    lv_obj_add_style(manual_sync_lbl, LV_OBJ_PART_MAIN, &lbl_style_white);

    // Sync clock button
    sync_clock_btn = lv_btn_create(settings_view, NULL);
    lv_obj_set_event_cb(sync_clock_btn, event_handler); // Set event handler
    lv_obj_align(sync_clock_btn, settings_view, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *sync_clock_lbl = lv_label_create(sync_clock_btn, NULL);
    lv_label_set_text(sync_clock_lbl, "Sync Time");
    lv_obj_add_style(sync_clock_btn, LV_OBJ_PART_MAIN, &btn_style_blue);
    lv_obj_add_style(sync_clock_lbl, LV_OBJ_PART_MAIN, &lbl_style_white);

    // Button for Main Menu
    main_menu_btn2 = lv_btn_create(settings_view, NULL);
    lv_obj_set_event_cb(main_menu_btn2, event_handler); // Set event handler
    lv_obj_align(main_menu_btn2, settings_view, LV_ALIGN_CENTER, 0, 50);

    lv_obj_t *main_menu_lbl = lv_label_create(main_menu_btn2, NULL);
    lv_label_set_text(main_menu_lbl, "Main Menu");
    lv_obj_add_style(main_menu_btn2, LV_OBJ_PART_MAIN, &btn_style_blue);
    lv_obj_add_style(main_menu_lbl, LV_OBJ_PART_MAIN, &lbl_style_white);
}

// Function to create Past Sessions View
void createPastSessionsView()
{
    past_sessions_view = lv_cont_create(NULL, NULL);
    lv_obj_set_size(past_sessions_view, 240, 240);
    lv_obj_add_style(past_sessions_view, LV_OBJ_PART_MAIN, &cont_style);

    // Label for steps
    past_sessions_data = lv_label_create(past_sessions_view, NULL);
    lv_label_set_text(past_sessions_data, "NO HIKING SESSIONS TO SHOW.\n TAKE YOUR WATCH ON A HIKE!");
    lv_obj_align(past_sessions_data, past_sessions_view, LV_ALIGN_CENTER, 0, -60);
    lv_obj_add_style(past_sessions_data, LV_OBJ_PART_MAIN, &lbl_style_white);

    // Button for Main Menu
    main_menu_btn3 = lv_btn_create(past_sessions_view, NULL);
    lv_obj_set_event_cb(main_menu_btn3, event_handler); // Set event handler
    lv_obj_align(main_menu_btn3, past_sessions_view, LV_ALIGN_CENTER, 0, 60);

    lv_obj_t *main_menu_lbl = lv_label_create(main_menu_btn3, NULL);
    lv_label_set_text(main_menu_lbl, "Main Menu");
    lv_obj_add_style(main_menu_btn3, LV_OBJ_PART_MAIN, &btn_style_blue);
    lv_obj_add_style(main_menu_lbl, LV_OBJ_PART_MAIN, &lbl_style_white);
}

/*
 * This function sets the global style configuration for the application
 * ref: https://docs.lvgl.io/8.0/overview/style.html
 * NOTE: not all functionalities of LVGL are supported. For hands-on
 * example see: LilyGO T-Watch exaple lvgl->LilyGoGui
 */
void init_global_styles()
{
    // container style
    lv_style_init(&cont_style);
    lv_style_init(&cont_style);
    // NOTE: Supported fonts are listed in lv_conf.h
    lv_style_set_text_font(&cont_style, LV_STATE_DEFAULT, &lv_font_montserrat_14);
    lv_style_set_radius(&cont_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_bg_color(&cont_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_bg_opa(&cont_style, LV_OBJ_PART_MAIN, LV_OPA_COVER);
    lv_style_set_border_width(&cont_style, LV_OBJ_PART_MAIN, 0);

    // label style white
    lv_style_init(&lbl_style_white);
    lv_style_set_text_color(&lbl_style_white, LV_STATE_DEFAULT, LV_COLOR_WHITE);

    // label style white
    lv_style_init(&lbl_style_black);
    lv_style_set_text_color(&lbl_style_black, LV_STATE_DEFAULT, LV_COLOR_BLACK);

    // button style blue
    lv_style_init(&btn_style_blue);
    lv_style_set_bg_color(&btn_style_blue, LV_OBJ_PART_MAIN, LV_COLOR_BLUE);
    lv_style_set_bg_opa(&btn_style_blue, LV_OBJ_PART_MAIN, LV_OPA_COVER);
    lv_style_set_border_width(&btn_style_blue, LV_OBJ_PART_MAIN, 0);

    // button style red
    lv_style_init(&btn_style_red);
    lv_style_set_bg_color(&btn_style_red, LV_OBJ_PART_MAIN, LV_COLOR_RED);
    lv_style_set_bg_opa(&btn_style_red, LV_OBJ_PART_MAIN, LV_OPA_COVER);
    lv_style_set_border_width(&btn_style_red, LV_OBJ_PART_MAIN, 0);
}

/*
 * PEK button used to turn touch screen on and off
 * applied based on TTGO example WakeUpFormTouchScreen
 */
void loopWakeUpFormTouchScreen(TTGOClass *ttgo)
{
    // Serial.println("loopWakeUpFormTouchScreen.BEGIN");
    if (irqPEK)
    {
        //Serial.println("PEK pressed");
        irqPEK = false;
        ttgo->power->readIRQ();

        if (ttgo->power->isPEKShortPressIRQ())
        {
            // Clean power chip IRQ status
            ttgo->power->clearIRQ();

            /*
                Please see the attached references for AXP202 Power domain considerations
                REFS:
                  - https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library/issues/41
                  - https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library/blob/master/docs/watch_2020_v3.md
                */
            if (displayOn)
            {
                // Turn touchscreen off
                ttgo->displaySleep();
                ttgo->power->setPowerOutPut(AXP202_LDO2, false);
                //Serial.println("Touchscreen turned off");
            }
            else
            {
                // Turn touchscreen back on
                ttgo->displayWakeup();
                ttgo->power->setPowerOutPut(AXP202_LDO2, true);
                //Serial.println("Touchscreen turned on");
            }

            // Toggle display state
            displayOn = !displayOn;
        }

        ttgo->power->clearIRQ();

        // Serial.println("loopWakeUpFormTouchScreen.END");
    }
}

/*
 * Sets up toggle screen ON/OFF
 * Code taken from TTGO example WakeUpFormTouchScreen
 */
void setupToggleScreen(TTGOClass *ttgo)
{
    //Serial.println("setupToggleScreen.BEGIN");
    pinMode(AXP202_INT, INPUT_PULLUP);
    attachInterrupt(AXP202_INT, []
                    { irqPEK = true; }, FALLING);
    //! Clear IRQ unprocessed  first
    ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ, true);
    ttgo->power->clearIRQ();

    pinMode(TOUCH_INT, INPUT);
    //Serial.println("setupToggleScreen.END");
}

void initInterface(TTGOClass *ttgo)
{

    //Serial.println("setup.BEGIN");

    // ref: https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library/blob/master/examples/LVGL/Lvgl_Button/Lvgl_Button.ino#L18
    // ttgo = TTGOClass::getWatch(); // get an instance of TTGO class
                    // Initialize TTGO smartwatch hardware
    ttgo->openBL();               // Turn on the blacklight of the TTGO smartwatch display
    ttgo->lvgl_begin();           // Initialize LVGL graphics library for TTGO smartwatch

    // Initialize serial communication at given baud rate
    // ref: https://docs.arduino.cc/language-reference/en/functions/communication/serial/begin/
    //Serial.begin(115200);

    init_global_styles();

    // Setup touch screen on and off toggling
    setupToggleScreen(ttgo);

    // Initialize views
    createMainMenuView();
    createSettingsView();
    createSessionView();
    createPastSessionsView();

    // Load the initial screen (Main Menu)
    // ref: https://docs.lvgl.io/8/overview/display.html
    lv_scr_load(main_view);
    //Serial.println("setup.END");
}


/*
 * Handles interface related tasks in set intervals:
 * @param *ttgo pointer to TTGOClass
 * @param *trip pointer to trip struct
 * @param *systemVariables pointer to global variables shared by multiple libraries
 * @param refreshSessionView indicates if sessionView is to be refreshed 
 * @return returnData an interface event that is handled in main ino-file
 */
interfaceEvent handleTasksInterface(TTGOClass *ttgo, tripData * trip, systemGlobals * systemVariables, bool isRefreshSessionView, tripData * trips)
{
    returnData.serialString = "";
    returnData.event = INTERFACE_IDLE;
    
    hasActiveSession = systemVariables->hasActiveSession;
    stepCount = trip->stepCount;
    avgSpeed = trip->avgSpeed;
    distance = trip->distance;
    step_length = systemVariables->step_length;
    sessionStartTime = trip->timestampStart;
    pastTrips = trips;
    nOfTrips = systemVariables->maxTrips;

    lv_task_handler(); // Handle LVGL tasks
    
    // toggle display on/off
    loopWakeUpFormTouchScreen(ttgo);

    // refresh session view
    if (isRefreshSessionView) {
        refreshSessionView();
    }
    

    

    // char steps[32]; // Ensure the buffer is large enough
    // sprintf(steps, "%u", stepCount);
    // Serial.println(steps)

    return returnData;
}

#endif