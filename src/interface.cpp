#ifndef ESP32_WROOM_32
#include "interface.h"

TTGOClass *ttgo;

// For toggling display state
bool displayOn = true;
bool irqPEK = false;

/*
 * Declare global variables for buttons and views
 * lv_obj_t: Create a base object (a rectangle)
 */
lv_obj_t *main_view, *settings_view, *session_view, *past_sessions_view;
lv_obj_t *settings_btn, *manual_sync_btn, *session_btn, *past_sessions_btn;
lv_obj_t *main_menu_btn1, *main_menu_btn2, *main_menu_btn3;

// GLobal style variables
// ref: https://docs.lvgl.io/8.0/overview/style.html
static lv_style_t cont_style;
static lv_style_t lbl_style_white;
static lv_style_t lbl_style_black;
static lv_style_t btn_style_blue;
static lv_style_t btn_style_red;

/*
 * Declare global variables for buttons and views
 * lv_obj_t: Create a base object (a rectangle)
 */
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
// Function to create the Main Menu view
void createMainMenuView()
{
    main_view = lv_obj_create(NULL, NULL); // Create a new object for the main screen

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
    session_view = lv_obj_create(NULL, NULL); // Create a new object for the main screen

    // Label for steps
    lv_obj_t *steps = lv_label_create(session_view, NULL);
    lv_label_set_text(steps, "STEPS.......0");
    lv_obj_align(steps, NULL, LV_ALIGN_CENTER, 0, -40);

    // Label for distance
    lv_obj_t *distance = lv_label_create(session_view, NULL);
    lv_label_set_text(distance, "DISTANCE....0");
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
    settings_view = lv_obj_create(NULL, NULL); // Create a new object for the settings screen

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
    past_sessions_view = lv_obj_create(NULL, NULL); // Create a new object for the main screen

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
void loopWakeUpFormTouchScreen()
{
    // Serial.println("loopWakeUpFormTouchScreen.BEGIN");
    if (irqPEK)
    {
        Serial.println("PEK pressed");
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
                Serial.println("Touchscreen turned off");
            }
            else
            {
                // Turn touchscreen back on
                ttgo->displayWakeup();
                ttgo->power->setPowerOutPut(AXP202_LDO2, true);
                Serial.println("Touchscreen turned on");
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
void setupToggleScreen()
{
    Serial.println("setupToggleScreen.BEGIN");
    pinMode(AXP202_INT, INPUT_PULLUP);
    attachInterrupt(AXP202_INT, []
                    { irqPEK = true; }, FALLING);
    //! Clear IRQ unprocessed  first
    ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ, true);
    ttgo->power->clearIRQ();

    pinMode(TOUCH_INT, INPUT);
    Serial.println("setupToggleScreen.END");
}

void initInterface()
{

    Serial.println("setup.BEGIN");

    // ref: https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library/blob/master/examples/LVGL/Lvgl_Button/Lvgl_Button.ino#L18
    ttgo = TTGOClass::getWatch(); // get an instance of TTGO class
    ttgo->begin();                // Initialize TTGO smartwatch hardware
    ttgo->openBL();               // Turn on the blacklight of the TTGO smartwatch display
    ttgo->lvgl_begin();           // Initialize LVGL graphics library for TTGO smartwatch

    // Initialize serial communication at given baud rate
    // ref: https://docs.arduino.cc/language-reference/en/functions/communication/serial/begin/
    Serial.begin(115200);

    init_global_styles();

    // Setup touch screen on and off toggling
    setupToggleScreen();

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

void handleTasksInterface()
{
    // toggle display on/off
    loopWakeUpFormTouchScreen();
    lv_task_handler(); // Handle LVGL tasks
}

#endif