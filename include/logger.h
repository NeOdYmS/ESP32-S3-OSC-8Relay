#pragma once
#include <Arduino.h>
#include <cstdarg>

// Log levels
enum class LogLevel : uint8_t {
  DEBUG = 0,
  INFO = 1,
  WARN = 2,
  ERROR = 3
};

class Logger {
public:
  static Logger& instance() {
    static Logger _instance;
    return _instance;
  }

  void begin();
  
  void log(LogLevel level, const char* tag, const char* format, ...);
  void logOsc(const char* tag, const char* format, ...);
  void logNetwork(const char* tag, const char* format, ...);
  void logRelay(const char* tag, const char* format, ...);

private:
  Logger() = default;
  void _vlog(LogLevel level, const char* tag, const char* format, va_list args);
  const char* levelString(LogLevel level);
  const char* levelColor(LogLevel level);
};

// Convenient macros
#ifdef ENABLE_OSC_LOGGING
#define LOG_OSC(fmt, ...) Logger::instance().logOsc("OSC", fmt, ##__VA_ARGS__)
#else
#define LOG_OSC(fmt, ...)
#endif

#ifdef ENABLE_NETWORK_LOGGING
#define LOG_NET(fmt, ...) Logger::instance().logNetwork("NET", fmt, ##__VA_ARGS__)
#else
#define LOG_NET(fmt, ...)
#endif

#ifdef ENABLE_RELAY_LOGGING
#define LOG_RELAY(fmt, ...) Logger::instance().logRelay("RELAY", fmt, ##__VA_ARGS__)
#else
#define LOG_RELAY(fmt, ...)
#endif

#define LOG_INFO(tag, fmt, ...) Logger::instance().log(LogLevel::INFO, tag, fmt, ##__VA_ARGS__)
#define LOG_WARN(tag, fmt, ...) Logger::instance().log(LogLevel::WARN, tag, fmt, ##__VA_ARGS__)
#define LOG_ERROR(tag, fmt, ...) Logger::instance().log(LogLevel::ERROR, tag, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(tag, fmt, ...) Logger::instance().log(LogLevel::DEBUG, tag, fmt, ##__VA_ARGS__)
