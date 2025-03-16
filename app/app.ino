/*
 * GENERAL NOTES:
 * the buttons were based on TTGO example LVGL -> lvgl_button
 */
#include "config.h"

TTGOClass *ttgo;

// Step counter 
TFT_eSPI *tft;
BMA *sensor;
bool irqAcc = false;
bool irqPEK = false;
uint32_t stepCount = 0;
const float step_length = 0.8;
float avgSpeed = 0.0;

// session view is to be refreshed every second
unsigned long previousMillis = 0;
const unsigned long refreshInterval = 250;

// For toggling display state 
bool displayOn = true;

// Becomes true when start button is pressed in session view
// Becomes false when stop button is pressed in session view
bool hasActiveSession = false;


/*
 * Declare global variables for buttons and views
 * lv_obj_t: Create a base object (a rectangle)
 */
lv_obj_t *main_view, *settings_view, *session_view, *past_sessions_view;
lv_obj_t *settings_btn, *manual_sync_btn, *session_btn, *past_sessions_btn;
lv_obj_t *toggle_session_btn;
lv_obj_t *main_menu_btn1, *main_menu_btn2, *main_menu_btn3;


// GLobal style variables
// ref: https://docs.lvgl.io/8.0/overview/style.html
static lv_style_t cont_style;
static lv_style_t lbl_style_white;
static lv_style_t lbl_style_black;
static lv_style_t btn_style_blue;
static lv_style_t btn_style_red;


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
    lv_obj_t *stepsValue = lv_label_create(session_view, NULL);
    lv_obj_add_style(stepsValue, LV_OBJ_PART_MAIN, &lbl_style_white);
    char lblTextstepCount[32];  // Ensure the buffer is large enough
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
    lv_obj_t *distanceValue = lv_label_create(session_view, NULL);
    lv_obj_add_style(distanceValue, LV_OBJ_PART_MAIN, &lbl_style_white);
    char lblDistanceValue[32];  // Make sure buffer is large enough
    sprintf(lblDistanceValue, "%.2f", stepCount * step_length / 1000);
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
    lv_obj_t *avgSpeedValue = lv_label_create(session_view, NULL);
    lv_obj_add_style(avgSpeedValue, LV_OBJ_PART_MAIN, &lbl_style_white);
    char lblTextAvgSpeedValue[32];  // Ensure the buffer is large enough
    sprintf(lblTextAvgSpeedValue, "%.2f", avgSpeed);
    lv_label_set_text(avgSpeedValue, lblTextAvgSpeedValue);
    lv_obj_align(avgSpeedValue, session_view, LV_ALIGN_IN_TOP_RIGHT, -60, 60);

    
    // Button for starting a Session
    toggle_session_btn = lv_btn_create(session_view, NULL);
    lv_obj_set_event_cb(toggle_session_btn, event_handler); // Set event handler
    lv_obj_align(toggle_session_btn, NULL, LV_ALIGN_CENTER, 0, 10);

    lv_obj_t *toggle_session_lbl = lv_label_create(toggle_session_btn, NULL);
    
    if (hasActiveSession) {
      lv_label_set_text(toggle_session_lbl, "Stop");
      lv_obj_add_style(toggle_session_btn, LV_OBJ_PART_MAIN, &btn_style_red);
      lv_obj_add_style(toggle_session_lbl, LV_OBJ_PART_MAIN, &lbl_style_black);
    } else {
      lv_label_set_text(toggle_session_lbl, "Start");
      lv_obj_add_style(toggle_session_btn, LV_OBJ_PART_MAIN, &btn_style_blue);
      lv_obj_add_style(toggle_session_lbl, LV_OBJ_PART_MAIN, &lbl_style_white);
    }
    
    // Button for Main Menu
    main_menu_btn1 = lv_btn_create(session_view, NULL);
    lv_obj_set_event_cb(main_menu_btn1, event_handler); // Set event handler
    lv_obj_align(main_menu_btn1, NULL, LV_ALIGN_CENTER, 0, 60);

    lv_obj_t *main_menu_lbl = lv_label_create(main_menu_btn1, NULL);
    lv_label_set_text(main_menu_lbl, "Main Menu");
    lv_obj_add_style(main_menu_btn1, LV_OBJ_PART_MAIN, &btn_style_blue);
    lv_obj_add_style(main_menu_lbl, LV_OBJ_PART_MAIN, &lbl_style_white);

    
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
    lv_obj_align(settingsTitle, NULL, LV_ALIGN_CENTER, 0, -40);
    lv_obj_add_style(settingsTitle, LV_OBJ_PART_MAIN, &lbl_style_white);

    // Manual sync button
    manual_sync_btn = lv_btn_create(settings_view, NULL);
    lv_obj_set_event_cb(manual_sync_btn, event_handler); // Set event handler
    lv_obj_align(manual_sync_btn, settings_view, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *manual_sync_lbl = lv_label_create(manual_sync_btn, NULL);
    lv_label_set_text(manual_sync_lbl, "Manual sync");
    lv_obj_add_style(manual_sync_btn, LV_OBJ_PART_MAIN, &btn_style_blue);
    lv_obj_add_style(manual_sync_lbl, LV_OBJ_PART_MAIN, &lbl_style_white);

    // Button for Main Menu
    main_menu_btn2 = lv_btn_create(settings_view, NULL);
    lv_obj_set_event_cb(main_menu_btn2, event_handler); // Set event handler
    lv_obj_align(main_menu_btn2, settings_view, LV_ALIGN_CENTER, 0, 60);

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
    lv_obj_t *table_lbl = lv_label_create(past_sessions_view, NULL);
    lv_label_set_text(table_lbl, "TABLE HERE");
    lv_obj_align(table_lbl, past_sessions_view, LV_ALIGN_CENTER, 0, -40);
    lv_obj_add_style(table_lbl, LV_OBJ_PART_MAIN, &lbl_style_white);

    // Button for Main Menu
    main_menu_btn3 = lv_btn_create(past_sessions_view, NULL);
    lv_obj_set_event_cb(main_menu_btn3, event_handler); // Set event handler
    lv_obj_align(main_menu_btn3, past_sessions_view, LV_ALIGN_CENTER, 0, 60);

    lv_obj_t *main_menu_lbl = lv_label_create(main_menu_btn3, NULL);
    lv_label_set_text(main_menu_lbl, "Main Menu");
    lv_obj_add_style(main_menu_btn3, LV_OBJ_PART_MAIN, &btn_style_blue);
    lv_obj_add_style(main_menu_lbl, LV_OBJ_PART_MAIN, &lbl_style_white);
}

// Event handler function
static void event_handler(lv_obj_t *obj, lv_event_t event)
{
    // ref: https://docs.lvgl.io/8/overview/event.html?highlight=lv_event_clicked#events
    if (event == LV_EVENT_CLICKED)
    {
        Serial.println("Button clicked");

        if (obj == settings_btn)
        {
            lv_scr_load(settings_view); // Load the settings view
        }
        else if (obj == session_btn)
        {
            lv_obj_clean(session_view);
            createSessionView();
            lv_scr_load(session_view); // Load the session view
        }
        else if (obj == past_sessions_btn)
        {
            lv_scr_load(past_sessions_view); // load past sessions view
        }
        // TODO: This could perhaps be done more nicely?
        else if (obj == main_menu_btn1 || obj == main_menu_btn2 || obj == main_menu_btn3)
        {
            lv_scr_load(main_view); // load main menu view
        } 
        else if (obj == toggle_session_btn)
        {
          if (!hasActiveSession) {
            // Empty step count to prevent screen from rendering old count before reset
            stepCount = 0;
            // Reset counter
            sensor->resetStepCounter();
          }
          hasActiveSession = !hasActiveSession;
        }
    }
}

/*
 * Helper function that refreshes session view at predefined intervals
 * when session view is rendered
 */
void refreshSessionView() {
    
    // ref: https://docs.lvgl.io/7.11/get-started/quick-overview.html#widgets
    if (session_view == lv_scr_act()) {
      
      lv_obj_clean(session_view);
      createSessionView();
      lv_scr_load(session_view);
    }
    
    
}

/*
 * Sets up accelerator
 * Code taken from TTGO example stepCount
 */
void setupAccelerator() {
  Serial.println("setupAccelerator.BEGIN");
    sensor = ttgo->bma;

    // Accel parameter structure
    Acfg cfg;
    /*!
        Output data rate in Hz, Optional parameters:
            - BMA4_OUTPUT_DATA_RATE_0_78HZ
            - BMA4_OUTPUT_DATA_RATE_1_56HZ
            - BMA4_OUTPUT_DATA_RATE_3_12HZ
            - BMA4_OUTPUT_DATA_RATE_6_25HZ
            - BMA4_OUTPUT_DATA_RATE_12_5HZ
            - BMA4_OUTPUT_DATA_RATE_25HZ
            - BMA4_OUTPUT_DATA_RATE_50HZ
            - BMA4_OUTPUT_DATA_RATE_100HZ
            - BMA4_OUTPUT_DATA_RATE_200HZ
            - BMA4_OUTPUT_DATA_RATE_400HZ
            - BMA4_OUTPUT_DATA_RATE_800HZ
            - BMA4_OUTPUT_DATA_RATE_1600HZ
    */
    cfg.odr = BMA4_OUTPUT_DATA_RATE_100HZ;
    /*!
        G-range, Optional parameters:
            - BMA4_ACCEL_RANGE_2G
            - BMA4_ACCEL_RANGE_4G
            - BMA4_ACCEL_RANGE_8G
            - BMA4_ACCEL_RANGE_16G
    */
    cfg.range = BMA4_ACCEL_RANGE_2G;
    /*!
        Bandwidth parameter, determines filter configuration, Optional parameters:
            - BMA4_ACCEL_OSR4_AVG1
            - BMA4_ACCEL_OSR2_AVG2
            - BMA4_ACCEL_NORMAL_AVG4
            - BMA4_ACCEL_CIC_AVG8
            - BMA4_ACCEL_RES_AVG16
            - BMA4_ACCEL_RES_AVG32
            - BMA4_ACCEL_RES_AVG64
            - BMA4_ACCEL_RES_AVG128
    */
    cfg.bandwidth = BMA4_ACCEL_NORMAL_AVG4;

    /*! Filter performance mode , Optional parameters:
        - BMA4_CIC_AVG_MODE
        - BMA4_CONTINUOUS_MODE
    */
    cfg.perf_mode = BMA4_CONTINUOUS_MODE;

    // Configure the BMA423 accelerometer
    sensor->accelConfig(cfg);

    // Enable BMA423 accelerometer
    // Warning : Need to use steps, you must first enable the accelerometer
    // Warning : Need to use steps, you must first enable the accelerometer
    // Warning : Need to use steps, you must first enable the accelerometer
    sensor->enableAccel();

    pinMode(BMA423_INT1, INPUT);
    attachInterrupt(BMA423_INT1, [] {
        // Set interrupt to set irq value to 1
        irqAcc = 1;
    }, RISING); //It must be a rising edge

    // Enable BMA423 step count feature
    sensor->enableFeature(BMA423_STEP_CNTR, true);

    // Reset steps
    sensor->resetStepCounter();

    // Turn on step interrupt
    sensor->enableStepCountInterrupt();
    Serial.println("setupAccelerator.END");
}

/*
 * Sets up toggle screen ON/OFF
 * Code taken from TTGO example WakeUpFormTouchScreen
 */
void setupToggleScreen() {
  Serial.println("setupToggleScreen.BEGIN");
  pinMode(AXP202_INT, INPUT_PULLUP);
    attachInterrupt(AXP202_INT, [] {
        irqPEK = true;
    }, FALLING);
    //!Clear IRQ unprocessed  first
    ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ, true);
    ttgo->power->clearIRQ();

    pinMode(TOUCH_INT, INPUT);
    Serial.println("setupToggleScreen.END");

}

/*
 * Configures the app
 */
void setup()
{
    Serial.println("setup.BEGIN");
    // Initialize serial communication at given baud rate
    // ref: https://docs.arduino.cc/language-reference/en/functions/communication/serial/begin/
    Serial.begin(115200);

    // ref: https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library/blob/master/examples/LVGL/Lvgl_Button/Lvgl_Button.ino#L18
    ttgo = TTGOClass::getWatch(); // get an instance of TTGO class
    ttgo->begin();                // Initialize TTGO smartwatch hardware
    ttgo->openBL();               // Turn on the blacklight of the TTGO smartwatch display
    ttgo->lvgl_begin();           // Initialize LVGL graphics library for TTGO smartwatch  

    init_global_styles();
    
    // Setup touch screen on and off toggling
    setupToggleScreen();
    // setup up accelerator functionalities 
    setupAccelerator();

    // Initialize views
    createMainMenuView();
    createSettingsView();
    createSessionView();
    createPastSessionsView();

    // Load the initial screen (Main Menu)
    // ref: https://docs.lvgl.io/8/overview/display.html
    lv_scr_load(main_view);
    Serial.println("setup.END");
}

/*
 * PEK button used to turn touch screen on and off
 * applied based on TTGO example WakeUpFormTouchScreen
 */
void loopWakeUpFormTouchScreen() {
  
  if (irqPEK) {
    Serial.println("PEK pressed");
    irqPEK = false;
    ttgo->power->readIRQ();

    if (ttgo->power->isPEKShortPressIRQ()) {
        // Clean power chip IRQ status
        ttgo->power->clearIRQ();

        /*
            Please see the attached references for AXP202 Power domain considerations
            REFS: 
              - https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library/issues/41
              - https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library/blob/master/docs/watch_2020_v3.md
            */
        if (displayOn) {
            // Turn touchscreen off
            ttgo->displaySleep();
            ttgo->power->setPowerOutPut(AXP202_LDO2, false);
            Serial.println("Touchscreen turned off");
        } else {
            // Turn touchscreen back on
            ttgo->displayWakeup();
            ttgo->power->setPowerOutPut(AXP202_LDO2, true);
            Serial.println("Touchscreen turned on");
        }

        // Toggle display state
        displayOn = !displayOn;
    }

    ttgo->power->clearIRQ();
    
    
  }
    
}

/*
 * loop handler for accelerator
 * Code taken from TTGO example stepCount
 */
void loopAccelerator() {
  
  if (irqAcc && hasActiveSession) {
        Serial.println("Accelerator interrupt");
        irqAcc = 0;
        bool  rlst;
        do {
            // Read the BMA423 interrupt status,
            // need to wait for it to return to true before continuing
            rlst =  sensor->readInterrupt();
        } while (!rlst);

        // Check if it is a step interrupt
        if (sensor->isStepCounter()) {
            // Get step data from register
            stepCount = sensor->getCounter();
        }
        // Print step count to serial
        Serial.print("Step Count: ");
        Serial.println(stepCount);
        
    }
    

}


/*
 * This function sets the global style configuration for the application
 * ref: https://docs.lvgl.io/8.0/overview/style.html
 * NOTE: not all functionalities of LVGL are supported. For hands-on
 * example see: LilyGO T-Watch exaple lvgl->LilyGoGui
 */
void init_global_styles() {
    // container style
    lv_style_init(&cont_style);
    lv_style_init(&cont_style);
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
 * Main logic loop:
 *  - calls LVGL task handler
 *  - sets delay for looping
 */
void loop()
{
    
    lv_task_handler(); // Handle LVGL tasks

    loopAccelerator();

    loopWakeUpFormTouchScreen();
    

    // Refresh the session view every second
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= refreshInterval) {
      previousMillis = currentMillis;
      refreshSessionView();
    }

    delay(20);          // Short delay to avoid overloading the processor
    
}
