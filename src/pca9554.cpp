#include "pca9554.h"
#include "logger.h"

bool PCA9554::begin(TwoWire& w, int sda, int scl, uint32_t freq_hz) {
  Serial.printf("[PCA9554] Starting init: SDA=%d SCL=%d FREQ=%u addr=0x%02X\n", sda, scl, freq_hz, _addr);
  
  _wire = &w;
  _wire->begin(sda, scl, freq_hz);
  delay(100);
  Serial.println("[PCA9554] I2C bus initialized");

  // Test communication
  Serial.println("[PCA9554] Testing communication by reading register 0x03...");
  uint8_t val;
  if (!readReg(0x03, val)) {
    Serial.println("[PCA9554] ❌ FAILED to read register 0x03 (config)");
    return false;
  }
  Serial.printf("[PCA9554] ✅ Config register read: 0x%02X\n", val);

  // Configure as outputs: set Configuration register (0x03) to 0x00 (all outputs)
  Serial.println("[PCA9554] Configuring all pins as outputs (reg 0x03 = 0x00)...");
  if (!writeReg(0x03, 0x00)) {
    Serial.println("[PCA9554] ❌ FAILED to write config register");
    return false;
  }
  Serial.println("[PCA9554] ✅ Config written successfully");
  
  delay(10);
  
  // Initialize output register (0x01) to 0x00 (all OFF)
  Serial.println("[PCA9554] Setting all outputs to LOW (reg 0x01 = 0x00)...");
  if (!writeReg(0x01, 0x00)) {
    Serial.println("[PCA9554] ❌ FAILED to write output register");
    return false;
  }
  Serial.println("[PCA9554] ✅ Outputs initialized to 0x00");

  _ready = true;
  Serial.println("[PCA9554] 🎉 Initialization complete!\n");
  return true;
}

bool PCA9554::writeAll(bool ch[8]) {
  if (!_ready) return false;

  uint8_t val = 0;
  for (int i = 0; i < 8; i++) {
    if (ch[i]) val |= (1 << i);
  }

  LOG_RELAY("PCA9554", "writeAll mask=0x%02X", val);
  return writeReg(0x01, val);
}

bool PCA9554::writeChannel(uint8_t ch, bool on) {
  if (!_ready || ch >= 8) return false;

  if (on) {
    _out |= (1 << ch);
  } else {
    _out &= ~(1 << ch);
  }

  LOG_RELAY("PCA9554", "ch=%d %s mask=0x%02X", ch, on ? "ON" : "OFF", _out);
  return writeReg(0x01, _out);
}

bool PCA9554::readOutputs(bool out[8]) const {
  if (!_ready) return false;

  for (int i = 0; i < 8; i++) {
    out[i] = (_out & (1 << i)) != 0;
  }
  return true;
}

bool PCA9554::writeReg(uint8_t reg, uint8_t value) {
  if (!_wire) {
    Serial.println("[PCA9554] writeReg: _wire is NULL!");
    return false;
  }

  _wire->beginTransmission(_addr);
  _wire->write(reg);
  _wire->write(value);
  int err = _wire->endTransmission();

  if (err != 0) {
    LOG_ERROR("PCA9554", "writeReg(0x%02X, 0x%02X) FAIL err=%d", reg, value, err);
    return false;
  }

  if (reg == 0x01) _out = value;
  return true;
}

bool PCA9554::readReg(uint8_t reg, uint8_t& value) {
  if (!_wire) {
    Serial.println("[PCA9554] readReg: _wire is NULL!");
    return false;
  }

  _wire->beginTransmission(_addr);
  _wire->write(reg);
  int err = _wire->endTransmission();
  if (err != 0) {
    Serial.printf("[PCA9554] readReg(0x%02X): beginTx failed, err=%d\n", reg, err);
    return false;
  }

  int n = _wire->requestFrom((int)_addr, 1);
  if (n != 1) {
    Serial.printf("[PCA9554] readReg(0x%02X): requestFrom got %d bytes (expected 1)\n", reg, n);
    return false;
  }

  value = _wire->read();
  Serial.printf("[PCA9554] readReg(0x%02X) -> 0x%02X OK\n", reg, value);
  return true;
}
