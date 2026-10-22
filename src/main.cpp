#include <Arduino.h>
#include "controls.h"
#include "display.h"
#include "game_snake.h"

constexpr unsigned long RESTART_LOCKOUT_MS = 800;   // ignore the stick right after dying

static void restart() {
  startGame();
  drawGame();
}

void setup() {
  Serial.begin(115200);
  setupControls();
  setupDisplay();
  randomSeed(esp_random());
  restart();
}

void loop() {
  // Click: pause / resume, or restart after game over.
  if (buttonPressed()) {
    if (gameOver) {
      restart();
    } else {
      paused = !paused;
      if (paused) {
        drawPaused();
      } else {
        lastStep = millis();
        drawGame();
      }
    }
    return;
  }

  Direction requested;
  const bool moved = readRequestedDirection(requested);

  if (gameOver) {
    if (moved && millis() - gameOverAt > RESTART_LOCKOUT_MS) restart();
    delay(10);
    return;
  }
  if (paused) {
    delay(10);
    return;
  }

  if (moved && !isOpposite(requested, direction)) queuedDirection = requested;

  if (millis() - lastStep >= stepInterval()) {
    lastStep = millis();
    moveSnake();
    if (gameOver) drawGameOver();
    else          drawStep();
  }

  delay(5);
}

