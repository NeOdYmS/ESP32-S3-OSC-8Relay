#pragma once
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// Wrapper pour mutex thread-safe
class Mutex {
public:
  Mutex() : _handle(xSemaphoreCreateMutex()) {}
  ~Mutex() { if (_handle) vSemaphoreDelete(_handle); }

  bool lock(uint32_t timeoutMs = portMAX_DELAY) {
    return _handle && xSemaphoreTake(_handle, pdMS_TO_TICKS(timeoutMs)) == pdTRUE;
  }

  void unlock() {
    if (_handle) xSemaphoreGive(_handle);
  }

private:
  SemaphoreHandle_t _handle;
};

// RAII lock guard
class LockGuard {
public:
  explicit LockGuard(Mutex& mutex, uint32_t timeoutMs = portMAX_DELAY) 
    : _mutex(mutex), _locked(mutex.lock(timeoutMs)) {}
  
  ~LockGuard() {
    if (_locked) _mutex.unlock();
  }

  bool isLocked() const { return _locked; }

private:
  Mutex& _mutex;
  bool _locked;
};
