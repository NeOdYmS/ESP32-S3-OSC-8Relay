#include "watchdog.h"
#include "logger.h"
#include <esp_task_wdt.h>

void Watchdog::begin(uint32_t timeoutMs) {
  #ifdef WATCHDOG_ENABLED
  _timeout = timeoutMs;
  _lastFeed = millis();
  _enabled = true;
  
  // Configure ESP32 watchdog
  esp_task_wdt_init(timeoutMs / 1000, true);
  esp_task_wdt_add(NULL); // Add current task to WDT
  
  LOG_INFO("WATCHDOG", "Initialized with %lu ms timeout", timeoutMs);
  #else
  LOG_INFO("WATCHDOG", "Disabled (WATCHDOG_ENABLED not defined)");
  #endif
}

void Watchdog::feed() {
  if (!_enabled) return;
  
  _lastFeed = millis();
  
  #ifdef WATCHDOG_ENABLED
  esp_task_wdt_reset();
  #endif
}

void Watchdog::stop() {
  #ifdef WATCHDOG_ENABLED
  if (_enabled) {
    esp_task_wdt_delete(NULL);
    _enabled = false;
    LOG_INFO("WATCHDOG", "Stopped");
  }
  #endif
}
