#pragma once
#include <Arduino.h>

class Watchdog {
public:
  static Watchdog& instance() {
    static Watchdog _instance;
    return _instance;
  }

  void begin(uint32_t timeoutMs = 10000);
  void feed();
  void stop();
  bool isEnabled() const { return _enabled; }

private:
  Watchdog() = default;
  bool _enabled = false;
  uint32_t _timeout = 10000;
  unsigned long _lastFeed = 0;
};

#define WATCHDOG Watchdog::instance()
