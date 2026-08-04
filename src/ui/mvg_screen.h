// Copyright (c) 2026 Stefan Koelle - https://stefankoelle.de - MIT License
#pragma once
#include "ui/screen_base.h"

// MVG departures: scrollable list of all departures + back button.
Screen mvgScreen_make();
void mvgScreen_setNavigator(ScreenId (*nav)(ScreenId));
