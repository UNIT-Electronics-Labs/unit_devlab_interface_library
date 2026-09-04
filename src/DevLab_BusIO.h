/*
  DevLab_BusIO.h

  Generic bus contract for the DevLab ecosystem, resolved at compile
  time via CRTP (no vtable, no runtime dispatch cost).

  A concrete bus (I2C, SPI, ...) derives as:

    class DevLab_I2C_Interface : public DevLab_BusIO<DevLab_I2C_Interface> {
    public:
        bool begin();
        bool writeBytes(uint8_t reg, const uint8_t *data, size_t len);
        bool readBytes(uint8_t reg, uint8_t *data, size_t len);
    };

  and only implements those two raw primitives — the fixed width
  helpers (8/16/32 bit read/write) are resolved once here, on top of
  writeBytes/readBytes, and inherited by every bus for free.

  The concrete bus class itself is NOT a template, so it keeps a plain
  .h/.cpp pair; only this contract is header-only.

  Organization:
  UNIT Electronics - DevLab Ecosystem

  Author: 
  Jonathan Mejorado Lopez

  License:
  MIT License
*/

#ifndef DEVLAB_BUSIO_H
#define DEVLAB_BUSIO_H

#include <Arduino.h>

template <typename Derived>

class DevLab_BusIO {
public:
    // Lifecycle: opens/configures the underlying transport.
    bool begin() {
        return static_cast<Derived*>(this)->begin();
    }

    // Raw primitives, forwarded to the concrete bus implementation.
    bool writeBytes(uint8_t reg, const uint8_t *data, size_t len) {
        return static_cast<Derived*>(this)->writeBytes(reg, data, len);
    }

    bool readBytes(uint8_t reg, uint8_t *data, size_t len) {
        return static_cast<Derived*>(this)->readBytes(reg, data, len);
    }

    // Fixed-width helpers, built on writeBytes/readBytes. Byte order is
    // little-endian (LSB first) to match typical sensor register layout.
    bool writeRegister8(uint8_t reg, uint8_t value) {
        return writeBytes(reg, &value, 1);
    }

    bool readRegister8(uint8_t reg, uint8_t &value) {
        return readBytes(reg, &value, 1);
    }

    bool writeRegister16(uint8_t reg, uint16_t value) {
        uint8_t buf[2] = {
            (uint8_t)(value & 0xFF),
            (uint8_t)((value >> 8) & 0xFF)
        };
        return writeBytes(reg, buf, 2);
    }

    bool readRegister16(uint8_t reg, uint16_t &value) {
        uint8_t buf[2];
        if (!readBytes(reg, buf, 2)) return false;
        value = (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
        return true;
    }

    bool writeRegister32(uint8_t reg, uint32_t value) {
        uint8_t buf[4] = {
            (uint8_t)(value & 0xFF),
            (uint8_t)((value >> 8) & 0xFF),
            (uint8_t)((value >> 16) & 0xFF),
            (uint8_t)((value >> 24) & 0xFF)
        };
        return writeBytes(reg, buf, 4);
    }

    bool readRegister32(uint8_t reg, uint32_t &value) {
        uint8_t buf[4];
        if (!readBytes(reg, buf, 4)) return false;
        value = (uint32_t)buf[0]
              | ((uint32_t)buf[1] << 8)
              | ((uint32_t)buf[2] << 16)
              | ((uint32_t)buf[3] << 24);
        return true;
    }

    bool readBits(uint8_t reg, uint8_t pos, uint8_t len, uint8_t &value){
        uint8_t buf;
        if (!readBytes(reg, &buf, 1)) return false;
        value = (buf >> pos) & ((1 << len) - 1);
        return true;
    }

    bool readBits16(uint8_t reg, uint8_t pos, uint8_t len, uint16_t &value){
        uint16_t buf;
        if (!readRegister16(reg, buf)) return false;
        value = (buf >> pos) & ((1 << len) - 1);
        return true;
    }

    bool readBits32(uint8_t reg, uint8_t pos, uint8_t len, uint32_t &value){
        uint32_t buf;
        if (!readRegister32(reg, buf)) return false;
        value = (buf >> pos) & ((1 << len) - 1);
        return true;
    }
protected:
    DevLab_BusIO() {}
};

#endif // DEVLAB_BUSIO_H
