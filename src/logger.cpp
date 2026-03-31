#include "logger.h"
#include <time.h>

void Logger::begin() {
  Serial.begin(115200);
  delay(1000);
}

const char* Logger::levelString(LogLevel level) {
  switch (level) {
    case LogLevel::DEBUG: return "DEBUG";
    case LogLevel::INFO:  return "INFO ";
    case LogLevel::WARN:  return "WARN ";
    case LogLevel::ERROR: return "ERROR";
    default: return "?????";
  }
}

const char* Logger::levelColor(LogLevel level) {
  switch (level) {
    case LogLevel::DEBUG: return "\033[36m"; // Cyan
    case LogLevel::INFO:  return "\033[32m"; // Green
    case LogLevel::WARN:  return "\033[33m"; // Yellow
    case LogLevel::ERROR: return "\033[31m"; // Red
    default: return "\033[0m";
  }
}

void Logger::_vlog(LogLevel level, const char* tag, const char* format, va_list args) {
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  
  char timebuf[16];
  strftime(timebuf, sizeof(timebuf), "%H:%M:%S", timeinfo);
  
  char logbuf[512];
  vsnprintf(logbuf, sizeof(logbuf), format, args);
  
  Serial.printf("%s [%s] [%-15s] %s\033[0m\n", 
    levelColor(level),
    timebuf,
    tag,
    logbuf
  );
}

void Logger::log(LogLevel level, const char* tag, const char* format, ...) {
  va_list args;
  va_start(args, format);
  _vlog(level, tag, format, args);
  va_end(args);
}

void Logger::logOsc(const char* tag, const char* format, ...) {
  va_list args;
  va_start(args, format);
  _vlog(LogLevel::DEBUG, tag, format, args);
  va_end(args);
}

void Logger::logNetwork(const char* tag, const char* format, ...) {
  va_list args;
  va_start(args, format);
  _vlog(LogLevel::INFO, tag, format, args);
  va_end(args);
}

void Logger::logRelay(const char* tag, const char* format, ...) {
  va_list args;
  va_start(args, format);
  _vlog(LogLevel::INFO, tag, format, args);
  va_end(args);
}
