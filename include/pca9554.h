#pragma once
#include <Arduino.h>
#include <Wire.h>

class PCA9554 {
public:
  explicit PCA9554(uint8_t i2c_addr = 0x20) : _addr(i2c_addr) {}

  bool begin(TwoWire& w = Wire, int sda = 42, int scl = 41, uint32_t freq_hz = 100000);
  bool writeAll(bool ch[8]);           // logical ON/OFF per channel
  bool writeChannel(uint8_t ch, bool on);
  bool readOutputs(bool out[8]) const;

private:
  bool writeReg(uint8_t reg, uint8_t value);
  bool readReg(uint8_t reg, uint8_t& value);

  TwoWire* _wire = nullptr;
  uint8_t _addr;
  uint8_t _out = 0x00;                 // cached output register
  bool _ready = false;
};
