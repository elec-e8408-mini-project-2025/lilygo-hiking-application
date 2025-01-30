/*
 * GENERAL NOTES:
 * - the buttons were based on TTGO example LVGL -> lvgl_button
 */
#include "config.h"

TTGOClass *ttgo;

// Step counter 
TFT_eSPI *tft;
BMA *sensor;
bool irq = false;
uint32_t stepCount = 0;
const float step_length = 0.8;

// session view is to be refreshed every second
unsigned long previousMillis = 0;
const unsigned long refreshInterval = 1000;

/*
 * Declare global variables for buttons and views
 * lv_obj_t: Create a base object (a rectangle)
 */
lv_obj_t *main_view, *settings_view, *session_view, *past_sessions_view;
lv_obj_t *settings_btn, *manual_sync_btn, *session_btn, *past_sessions_btn;
lv_obj_t *main_menu_btn1, *main_menu_btn2, *main_menu_btn3;

// Function to create the Main Menu view
void createMainMenuView()
{
    main_view = lv_obj_create(NULL, NULL); // Create a new object for the main view

    // Label for title
    lv_obj_t *main_view_title = lv_label_create(main_view, NULL);
    lv_label_set_text(main_view_title, "HIKING APP");
    lv_obj_align(main_view_title, NULL, LV_ALIGN_CENTER, 0, -80);

    // Button for new session
    session_btn = lv_btn_create(main_view, NULL);
    lv_obj_set_event_cb(session_btn, event_handler); // Set event handler
    lv_obj_align(session_btn, NULL, LV_ALIGN_CENTER, 0, -50);

    lv_obj_t *session_lbl = lv_label_create(session_btn, NULL);
    lv_label_set_text(session_lbl, "NEW SESSION");

    // Button for past sessions
    past_sessions_btn = lv_btn_create(main_view, NULL);
    lv_obj_set_event_cb(past_sessions_btn, event_handler); // Set event handler
    lv_obj_align(past_sessions_btn, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *past_sessions_lbl = lv_label_create(past_sessions_btn, NULL);
    lv_label_set_text(past_sessions_lbl, "PAST SESSIONS");

    // Button for Settings
    settings_btn = lv_btn_create(main_view, NULL);
    lv_obj_set_event_cb(settings_btn, event_handler); // Set event handler
    lv_obj_align(settings_btn, NULL, LV_ALIGN_CENTER, 0, 50);

    lv_obj_t *settings_lbl = lv_label_create(settings_btn, NULL);
    lv_label_set_text(settings_lbl, "SETTINGS");
}

// Function to create Session view
void createSessionView()
{
    session_view = lv_obj_create(NULL, NULL); // Create a new object for the session view

    // Label for steps
    lv_obj_t *steps = lv_label_create(session_view, NULL);
    char lblTextstepCount[32];  // Ensure the buffer is large enough
    sprintf(lblTextstepCount, "STEPS.......%u", stepCount);
    lv_label_set_text(steps, lblTextstepCount);
    lv_obj_align(steps, NULL, LV_ALIGN_CENTER, 0, -40);

    // Label for distance
    lv_obj_t *distance = lv_label_create(session_view, NULL);
    char lblTextDistance[32];  // Make sure buffer is large enough
    sprintf(lblTextDistance, "DISTANCE....%.2f", stepCount * step_length);
    lv_label_set_text(distance, lblTextDistance);
    lv_obj_align(distance, NULL, LV_ALIGN_CENTER, 0, -20);

    // Label for avg.speed
    lv_obj_t *avg_speed = lv_label_create(session_view, NULL);
    lv_label_set_text(avg_speed, "AVG.SPEED...0");
    lv_obj_align(avg_speed, NULL, LV_ALIGN_CENTER, 0, 0);

    // Button for Main Menu
    main_menu_btn1 = lv_btn_create(session_view, NULL);
    lv_obj_set_event_cb(main_menu_btn1, event_handler); // Set event handler
    lv_obj_align(main_menu_btn1, NULL, LV_ALIGN_CENTER, 0, 60);

    lv_obj_t *main_menu_lbl = lv_label_create(main_menu_btn1, NULL);
    lv_label_set_text(main_menu_lbl, "Main Menu");
}

// Function to create Settings view
void createSettingsView()
{
    settings_view = lv_obj_create(NULL, NULL); // Create a new object for the settings view

    // Label for Settings
    lv_obj_t *label = lv_label_create(settings_view, NULL);
    lv_label_set_text(label, "Change settings here");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, -40);

    // Manual sync button
    manual_sync_btn = lv_btn_create(settings_view, NULL);
    lv_obj_set_event_cb(manual_sync_btn, event_handler); // Set event handler
    lv_obj_align(manual_sync_btn, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *label1 = lv_label_create(manual_sync_btn, NULL);
    lv_label_set_text(label1, "Manual sync");

    // Button for Main Menu
    main_menu_btn2 = lv_btn_create(settings_view, NULL);
    lv_obj_set_event_cb(main_menu_btn2, event_handler); // Set event handler
    lv_obj_align(main_menu_btn2, NULL, LV_ALIGN_CENTER, 0, 60);

    lv_obj_t *main_menu_lbl = lv_label_create(main_menu_btn2, NULL);
    lv_label_set_text(main_menu_lbl, "Main Menu");
}

// Function to create Past Sessions View
void createPastSessionsView()
{
    past_sessions_view = lv_obj_create(NULL, NULL); // Create a new object for the past sessions view

    // Label for steps
    lv_obj_t *table_lbl = lv_label_create(past_sessions_view, NULL);
    lv_label_set_text(table_lbl, "TABLE HERE");
    lv_obj_align(table_lbl, NULL, LV_ALIGN_CENTER, 0, -40);

    // Button for Main Menu
    main_menu_btn3 = lv_btn_create(past_sessions_view, NULL);
    lv_obj_set_event_cb(main_menu_btn3, event_handler); // Set event handler
    lv_obj_align(main_menu_btn3, NULL, LV_ALIGN_CENTER, 0, 60);

    lv_obj_t *main_menu_lbl = lv_label_create(main_menu_btn3, NULL);
    lv_label_set_text(main_menu_lbl, "Main Menu");
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
 * Configures the app
 */
void setup()
{
    // Initialize serial communication at given baud rate
    // ref: https://docs.arduino.cc/language-reference/en/functions/communication/serial/begin/
    Serial.begin(115200);

    // ref: https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library/blob/master/examples/LVGL/Lvgl_Button/Lvgl_Button.ino#L18
    ttgo = TTGOClass::getWatch(); // get an instance of TTGO class
    ttgo->begin();                // Initialize TTGO smartwatch hardware
    ttgo->openBL();               // Turn on the blacklight of the TTGO smartwatch display
    ttgo->lvgl_begin();           // Initialize LVGL graphics library for TTGO smartwatch

    sensor = ttgo->bma;

    // This code segment is from the TTGO example project stepCount
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
        irq = 1;
    }, RISING); //It must be a rising edge

    // Enable BMA423 step count feature
    sensor->enableFeature(BMA423_STEP_CNTR, true);

    // Reset steps
    sensor->resetStepCounter();

    // Turn on step interrupt
    sensor->enableStepCountInterrupt();

    // Initialize views
    createMainMenuView();
    createSettingsView();
    createSessionView();
    createPastSessionsView();

    // Load the initial screen (Main Menu)
    // ref: https://docs.lvgl.io/8/overview/display.html
    lv_scr_load(main_view);
}

/*
 * Main logic loop:
 *  - calls LVGL task handler
 *  - sets delay for looping
 */

void loop()
{
    lv_task_handler(); // Handle LVGL tasks
    
    if (irq) {
        Serial.println("Interrupt handler handling interrupt");
        irq = 0;
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

    // Refresh the session view every second
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= refreshInterval) {
      previousMillis = currentMillis;
      refreshSessionView();
    }

    delay(20);          // Short delay to avoid overloading the processor
}
