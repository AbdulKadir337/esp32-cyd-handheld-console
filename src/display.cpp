#include "display.h"
#include "game_snake.h"
#include <TFT_eSPI.h>

static TFT_eSPI tft;

constexpr uint8_t DISPLAY_ROTATION = 1;   // landscape. Use 3 if the picture is upside down.

constexpr int SCREEN_W  = 320;
constexpr int SCREEN_H  = 240;
constexpr int CELL_SIZE = 12;
constexpr int FIELD_W   = GRID_WIDTH * CELL_SIZE;    // 312
constexpr int FIELD_H   = GRID_HEIGHT * CELL_SIZE;   // 216
constexpr int FIELD_X   = (SCREEN_W - FIELD_W) / 2;  // 4
constexpr int FIELD_Y   = SCREEN_H - FIELD_H - 4;    // 20 (HUD above it)

constexpr uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}
constexpr uint16_t COL_BG     = rgb565(6, 10, 14);
constexpr uint16_t COL_BORDER = rgb565(0, 150, 90);
constexpr uint16_t COL_BODY   = rgb565(0, 190, 90);
constexpr uint16_t COL_HEAD   = rgb565(150, 255, 180);
constexpr uint16_t COL_FOOD   = rgb565(255, 70, 70);
constexpr uint16_t COL_TEXT   = rgb565(220, 230, 235);
constexpr uint16_t COL_PANEL  = rgb565(16, 24, 32);

static void drawSnakeCell(const Cell &c, uint16_t color) {
  tft.fillRoundRect(FIELD_X + c.x * CELL_SIZE + 1, FIELD_Y + c.y * CELL_SIZE + 1,
                    CELL_SIZE - 2, CELL_SIZE - 2, 3, color);
}

static void eraseCell(const Cell &c) {
  tft.fillRect(FIELD_X + c.x * CELL_SIZE, FIELD_Y + c.y * CELL_SIZE, CELL_SIZE, CELL_SIZE, COL_BG);
}

static void drawFood() {
  tft.fillCircle(FIELD_X + food.x * CELL_SIZE + CELL_SIZE / 2,
                 FIELD_Y + food.y * CELL_SIZE + CELL_SIZE / 2, CELL_SIZE / 2 - 1, COL_FOOD);
}

static void drawHud() {
  char buf[24];
  tft.fillRect(0, 0, SCREEN_W, FIELD_Y - 3, COL_BG);
  tft.setTextFont(2);
  tft.setTextColor(COL_TEXT, COL_BG);

  snprintf(buf, sizeof(buf), "SCORE %u", score);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(buf, 6, 1);

  snprintf(buf, sizeof(buf), "BEST %u", score > bestScore ? score : bestScore);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(buf, SCREEN_W - 6, 1);
}

static void drawPanel(const char *title, const char *line2, const char *line3) {
  const int w = 220, h = 96;
  const int x = (SCREEN_W - w) / 2, y = (SCREEN_H - h) / 2;
  tft.fillRoundRect(x, y, w, h, 8, COL_PANEL);
  tft.drawRoundRect(x, y, w, h, 8, COL_BORDER);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(COL_TEXT, COL_PANEL);
  tft.setTextFont(4);
  tft.drawString(title, SCREEN_W / 2, y + 26);
  tft.setTextFont(2);
  tft.drawString(line2, SCREEN_W / 2, y + 56);
  tft.drawString(line3, SCREEN_W / 2, y + 76);
}

void setupDisplay() {
  tft.init();                       // also switches the backlight on (TFT_BL)
  tft.setRotation(DISPLAY_ROTATION);
  tft.fillScreen(COL_BG);
}

void drawGame() {
  tft.fillScreen(COL_BG);
  tft.drawRect(FIELD_X - 2, FIELD_Y - 2, FIELD_W + 4, FIELD_H + 4, COL_BORDER);
  tft.drawRect(FIELD_X - 1, FIELD_Y - 1, FIELD_W + 2, FIELD_H + 2, COL_BORDER);
  drawHud();
  for (uint16_t i = snakeLength - 1; i > 0; --i) drawSnakeCell(snake[i], COL_BODY);
  drawSnakeCell(snake[0], COL_HEAD);
  drawFood();
}

// Only touches the cells that changed, so there is no flicker and each step is fast.
void drawStep() {
  if (!ateFood) eraseCell(lastTail);
  if (snakeLength > 1) drawSnakeCell(snake[1], COL_BODY);   // old head becomes body
  drawSnakeCell(snake[0], COL_HEAD);
  if (ateFood) {
    drawFood();
    drawHud();
  }
}

void drawGameOver() {
  drawHud();   // refreshes BEST
  char buf[32];
  snprintf(buf, sizeof(buf), "Score: %u", score);
  drawPanel(gameWon ? "YOU WIN!" : "GAME OVER", buf, "Click or move stick");
}

void drawPaused() {
  drawPanel("PAUSED", "Click to resume", "");
}
