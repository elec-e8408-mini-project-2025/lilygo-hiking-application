#ifndef CONFIG_H
#define CONFIG_H
// => Hardware select
////// HIKING APP DEFINED DURING COMPILATION /////////////////////////////////////////////////////////////////////////
// #define LILYGO_WATCH_2019_WITH_TOUCH     // To use T-Watch2019 with touchscreen, please uncomment this line      //
// #define LILYGO_WATCH_2019_NO_TOUCH          // To use T-Watch2019 Not touchscreen , please uncomment this line   //
// #define LILYGO_WATCH_2020_V1             //To use T-Watch2020, please uncomment this line                        //
// #define LILYGO_WATCH_2020_V2             //To use T-Watch2020 V2, please uncomment this line                     //
// #define LILYGO_WATCH_2020_V3             //To use T-Watch2020 V3, please uncomment this line                     //
// #define ESP32_WROOM_32                                                                                           //
// #define DEBUG                                                                                                    //
// NOT SUPPORT ...                                                                                                  //
//// #define LILYGO_WATCH_BLOCK                                                                                     //
// NOT SUPPORT ...                                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// gcc precompiler: https://gcc.gnu.org/onlinedocs/cpp/
// => Function select
#ifndef ESP32_WROOM_32
#define LILYGO_WATCH_LVGL                   //To use LVGL, you need to enable the macro LVGL
#include <LilyGoWatch.h>
#endif

#endif