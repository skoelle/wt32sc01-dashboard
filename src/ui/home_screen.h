// Copyright (c) 2026 Stefan Koelle - https://stefankoelle.de - MIT License
#pragma once
#include "ui/screen_base.h"

// Home screen: three touch tiles (weather big, calendar big w/ 2 events,
// mvg small). Tap navigates to the respective detail screen.

// Navigation callback type: called with the target ScreenId, returns the
// actually-shown ScreenId (so main.cpp can drive transitions + inactivity).
using Navigator = ScreenId (*)(ScreenId);

// Build the home screen object (does not create widgets until show()).
Screen homeScreen_make();

// Set the navigation callback (called by main.cpp once at startup).
void homeScreen_setNavigator(Navigator nav);

extern Screen homeScreen;
