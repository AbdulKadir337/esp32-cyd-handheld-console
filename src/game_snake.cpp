#include "game_snake.h"

Cell snake[MAX_SNAKE];
Cell food;
Cell lastTail;
bool ateFood;
uint16_t snakeLength;
uint16_t score;
uint16_t bestScore = 0;
Direction direction;
Direction queuedDirection;
bool gameOver;
bool gameWon;
bool paused;
unsigned long lastStep;
unsigned long gameOverAt;

bool sameCell(const Cell &a, const Cell &b) { return a.x == b.x && a.y == b.y; }

bool isOpposite(Direction a, Direction b) {
  return (a == UP && b == DOWN) || (a == DOWN && b == UP) ||
         (a == LEFT && b == RIGHT) || (a == RIGHT && b == LEFT);
}

static bool snakeContains(const Cell &cell, uint16_t count) {
  for (uint16_t i = 0; i < count; ++i) {
    if (sameCell(snake[i], cell)) return true;
  }
  return false;
}

static void placeFood() {
  do {
    food.x = random(GRID_WIDTH);
    food.y = random(GRID_HEIGHT);
  } while (snakeContains(food, snakeLength));
}

static void endGame(bool won) {
  gameOver = true;
  gameWon = won;
  gameOverAt = millis();
  if (score > bestScore) bestScore = score;
}

void startGame() {
  snakeLength = 4;
  score = 0;
  direction = RIGHT;
  queuedDirection = RIGHT;
  gameOver = false;
  gameWon = false;
  paused = false;
  ateFood = false;

  const uint8_t startX = GRID_WIDTH / 2;
  const uint8_t startY = GRID_HEIGHT / 2;
  for (uint16_t i = 0; i < snakeLength; ++i) {
    snake[i] = {static_cast<uint8_t>(startX - i), startY};
  }

  placeFood();
  lastStep = millis();
}

// Speed ramps up with the score; clamped so it can never underflow.
unsigned long stepInterval() {
  const unsigned long speedup = score * SPEEDUP_MS_PER_POINT;
  if (speedup >= START_STEP_MS - MIN_STEP_MS) return MIN_STEP_MS;
  return START_STEP_MS - speedup;
}

void moveSnake() {
  direction = queuedDirection;
  Cell next = snake[0];

  if (direction == UP) {
    if (next.y == 0) { endGame(false); return; }
    --next.y;
  } else if (direction == DOWN) {
    if (next.y == GRID_HEIGHT - 1) { endGame(false); return; }
    ++next.y;
  } else if (direction == LEFT) {
    if (next.x == 0) { endGame(false); return; }
    --next.x;
  } else {
    if (next.x == GRID_WIDTH - 1) { endGame(false); return; }
    ++next.x;
  }

  const bool eating = sameCell(next, food);
  // The tail moves away this turn unless we are eating, so it is not a collision.
  const uint16_t collisionCount = eating ? snakeLength : snakeLength - 1;

  if (snakeContains(next, collisionCount)) {
    endGame(false);
    return;
  }

  ateFood = eating;
  lastTail = snake[snakeLength - 1];

  if (eating && snakeLength < MAX_SNAKE) ++snakeLength;

  for (uint16_t i = snakeLength - 1; i > 0; --i) {
    snake[i] = snake[i - 1];
  }
  snake[0] = next;

  if (eating) {
    ++score;
    if (snakeLength >= MAX_SNAKE) {   // grid is full: nowhere left to put food
      endGame(true);
    } else {
      placeFood();
    }
  }
}
