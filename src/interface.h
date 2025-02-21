#ifndef INTERFACE_H
#define INTERFACE_H
#define LILYGO_WATCH_LVGL
#include <LilyGoWatch.h>

void createMainMenuView();

void createSessionView();

void createSettingsView();

void createPastSessionsView();

void initInterface(TTGOClass *ttgo);

void handleTasksInterface(TTGOClass *ttgo);

#endif