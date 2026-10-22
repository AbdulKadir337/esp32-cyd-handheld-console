#pragma once
#include <Arduino.h>

void setupDisplay();
void drawGame();       // full redraw: new game, resume from pause
void drawStep();       // incremental redraw after a normal moveSnake()
void drawGameOver();
void drawPaused();
