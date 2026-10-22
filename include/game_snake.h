#pragma once
#include <Arduino.h>

// Playfield: 26 x 18 cells of 12 px = 312 x 216 px on the 320 x 240 CYD screen.
constexpr uint8_t  GRID_WIDTH  = 26;
constexpr uint8_t  GRID_HEIGHT = 18;
constexpr uint16_t MAX_SNAKE   = GRID_WIDTH * GRID_HEIGHT;

constexpr unsigned long START_STEP_MS        = 180;  // ms per move at score 0
constexpr unsigned long MIN_STEP_MS          = 70;   // fastest the snake gets
constexpr unsigned long SPEEDUP_MS_PER_POINT = 4;

enum Direction : uint8_t { UP, DOWN, LEFT, RIGHT };

struct Cell {
  uint8_t x;
  uint8_t y;
};

extern Cell snake[MAX_SNAKE];
extern Cell food;
extern Cell lastTail;          // cell the tail just left (only valid if !ateFood)
extern bool ateFood;           // true if the last move ate the food
extern uint16_t snakeLength;
extern uint16_t score;
extern uint16_t bestScore;
extern Direction direction;
extern Direction queuedDirection;
extern bool gameOver;
extern bool gameWon;           // snake filled the whole grid
extern bool paused;
extern unsigned long lastStep;
extern unsigned long gameOverAt;

bool sameCell(const Cell &a, const Cell &b);
bool isOpposite(Direction a, Direction b);
void startGame();
void moveSnake();
unsigned long stepInterval();
