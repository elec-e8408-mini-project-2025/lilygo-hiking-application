#ifndef INTERFACE_H
#define INTERFACE_H
#include <LilyGoWatch.h>

void createMainMenuView();

void createSessionView();

void createSettingsView();

void createPastSessionsView();

static void event_handler(lv_obj_t *obj, lv_event_t event);

#endif