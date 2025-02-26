#ifndef INTERFACE_H
#define INTERFACE_H
#define LILYGO_WATCH_LVGL
#include <LilyGoWatch.h>
#include <stdint.h>
#include "data.h"

typedef enum {
    INTERFACE_TOGGLE_SESSION,
    INTERFACE_PAUSE_SESSION,
    INTERFACE_DISABLE_BLUETOOTH,
    INTERFACE_ENABLE_BLUETOOTH,
    INTERFACE_IDLE,
    INTERFACE_DEBUG,
    INTERFACE_SYNC_GPS_TIME
} interfaceEventType;

typedef struct {
    interfaceEventType event;
    char *serialString; // MUST BE \0 terminated
} interfaceEvent;

void createMainMenuView();

void createSessionView();

void createSettingsView();

void createPastSessionsView();

void initInterface(TTGOClass *ttgo);

interfaceEvent handleTasksInterface(TTGOClass *ttgo, tripData * trip, systemGlobals * systemVariables, bool isRefreshSessionView);

#endif