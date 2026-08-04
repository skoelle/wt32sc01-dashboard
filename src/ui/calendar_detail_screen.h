// Copyright (c) 2026 Stefan Koelle - https://stefankoelle.de - MIT License
#pragma once
#include "ui/screen_base.h"

// Calendar detail: scrollable list of all 10 events + back button.
Screen calendarDetailScreen_make();
void calendarDetailScreen_setNavigator(ScreenId (*nav)(ScreenId));
