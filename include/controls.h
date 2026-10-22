#pragma once
#include <Arduino.h>
#include "game_snake.h"

void setupControls();
bool readRequestedDirection(Direction &requested);
bool buttonPressed();   // true once per click (debounced)
