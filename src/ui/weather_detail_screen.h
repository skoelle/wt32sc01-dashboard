// Copyright (c) 2026 Stefan Koelle - https://stefankoelle.de - MIT License
#pragma once
#include "ui/screen_base.h"

// Weather detail: scrollable hourly forecast list + back button.
Screen weatherDetailScreen_make();
void weatherDetailScreen_setNavigator(ScreenId (*nav)(ScreenId));
