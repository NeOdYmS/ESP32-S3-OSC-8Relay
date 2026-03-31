#pragma once
#include <Arduino.h>

// LED WS2812 (1 pixel) sur GPIO38
namespace LedStatus {
  enum class State { Booting, Ok, Error };

  void begin(uint8_t pin = 38, uint8_t brightness = 32);
  void setRGB(uint8_t r, uint8_t g, uint8_t b);
  void set(State s);
  void ok();
  void error();
  void booting();
}
