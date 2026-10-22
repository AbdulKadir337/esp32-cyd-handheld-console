#include "controls.h"

// CYD wiring (KiCad schematic rev B)
constexpr int JOY_X_GPIO   = 35;  // VRx - P3,  ADC1, input-only
constexpr int JOY_Y_GPIO   = 27;  // VRy - CN1, ADC2 (analogRead only works with WiFi off)
constexpr int JOY_BTN_GPIO = 22;  // SW  - P3 / CN1, INPUT_PULLUP, pressed = LOW

// Orientation. These defaults reproduce your OLED build (VRy = left/right, VRx = up/down).
// If the snake turns the wrong way, flip INVERT_H / INVERT_V; if the axes are
// swapped, flip SWAP_AXES.
constexpr bool JOY_SWAP_AXES = true;
constexpr bool JOY_INVERT_H  = true;
constexpr bool JOY_INVERT_V  = false;

constexpr int JOY_DEADZONE = 700;      // of 0..4095
constexpr bool DEBUG_JOY   = false;    // print raw X/Y to Serial

static int joyXCenter = 1910;          // fallbacks from the OLED build
static int joyYCenter = 1922;

static int readAveraged(int pin, int samples = 32) {
  long sum = 0;
  for (int i = 0; i < samples; ++i) {
    sum += analogRead(pin);
    delay(2);
  }
  return static_cast<int>(sum / samples);
}

void setupControls() {
  pinMode(JOY_BTN_GPIO, INPUT_PULLUP);
  analogReadResolution(12);

  // Calibrate the centre at power-up (keep the stick untouched). If the reading
  // is implausible, keep the fallback values.
  const int cx = readAveraged(JOY_X_GPIO);
  const int cy = readAveraged(JOY_Y_GPIO);
  if (cx > 1200 && cx < 2900) joyXCenter = cx;
  if (cy > 1200 && cy < 2900) joyYCenter = cy;
  Serial.printf("Joystick centre: X=%d Y=%d\n", joyXCenter, joyYCenter);
}

bool readRequestedDirection(Direction &requested) {
  const int x = analogRead(JOY_X_GPIO) - joyXCenter;
  const int y = analogRead(JOY_Y_GPIO) - joyYCenter;

  if (DEBUG_JOY) Serial.printf("X=%d Y=%d\n", x, y);

  int h = JOY_SWAP_AXES ? y : x;   // positive h = RIGHT
  int v = JOY_SWAP_AXES ? x : y;   // positive v = DOWN
  if (JOY_INVERT_H) h = -h;
  if (JOY_INVERT_V) v = -v;

  if (abs(h) < JOY_DEADZONE && abs(v) < JOY_DEADZONE) return false;

  // Follow whichever axis is pushed further, so diagonals pick one direction.
  if (abs(h) >= abs(v)) requested = (h > 0) ? RIGHT : LEFT;
  else                  requested = (v > 0) ? DOWN : UP;
  return true;
}

bool buttonPressed() {
  static bool lastLevel = HIGH;              // idle level with the pull-up
  static unsigned long lastChange = 0;

  const bool level = digitalRead(JOY_BTN_GPIO);
  if (level != lastLevel && millis() - lastChange > 30) {
    lastChange = millis();
    lastLevel = level;
    return level == LOW;                     // only the press, not the release
  }
  return false;
}
