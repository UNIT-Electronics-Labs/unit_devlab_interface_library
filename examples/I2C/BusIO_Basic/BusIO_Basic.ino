/*
  DevLab_BusIO - I2C Basic Validation Example

  Description:
  Exercises DevLab_BusIO + DevLab_I2C_Interface directly, with no
  sensor driver on top, against a Bosch BMI323 as the test device.

  This validates:
    - DevLab_I2C_Interface::begin()
    - DevLab_I2C_Interface::getWhoAmI() / DevLab_BusIO::readRegister16()
      (fixed-width helpers, unaware of any device quirk)
    - DevLab_BusIO::writeRegister16() (fixed-width helper)

  Note: the BMI323 prepends 2 dummy bytes to every I2C read. That is a
  device-specific quirk, not generic I2C behavior - it is declared
  ONCE via the `readDummyBytes` constructor argument below, and every
  helper (getWhoAmI, readRegister8/16/32) discards it automatically.
  No byte-offset math needed at the call site.

  Connections:
  BMI323 -> MCU

  SDA   -> GPIO 24
  SCL   -> GPIO 25
  VDD   -> 3.3V
  VDDIO -> 3.3V
  GND   -> GND
  CSB   -> 3.3V
  SDO   -> 3.3V (I2C address 0x69)

  Organization:
  UNIT Electronics - DevLab Ecosystem

  Author: 
  Jonathan Mejorado Lopez
  
  License:
  MIT License
*/

#include <DevLab_I2C_Interface.h>

#define SDA_PIN 24
#define SCL_PIN 25

#define REG_CHIP_ID    0x00
#define REG_ACC_CONF   0x20
#define BMI323_CHIP_ID 0x43

// Last argument (2) declares the BMI323's 2 leading dummy bytes once,
// here - not at every call site.
DevLab_I2C_Interface bus(Wire, 0x69, 400000, 2);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("==================================================");
  Serial.println(" DevLab_BusIO - I2C Basic Validation");
  Serial.println("==================================================");

  bus.begin(SDA_PIN, SCL_PIN);
  delay(50);

  // --- Test 1: getWhoAmI() - dummy bytes handled transparently ---
  uint8_t chipId;
  if (bus.getWhoAmI(REG_CHIP_ID, chipId)) {
    Serial.print("[getWhoAmI] CHIP_ID: 0x");
    Serial.print(chipId, HEX);
    Serial.print("  (expected 0x");
    Serial.print(BMI323_CHIP_ID, HEX);
    Serial.println(")");

    if (chipId == BMI323_CHIP_ID) {
      Serial.println("[PASS] getWhoAmI() works, CHIP_ID matches.");
    } else {
      Serial.println("[FAIL] CHIP_ID mismatch - check wiring/address.");
    }
  } else {
    Serial.println("[FAIL] getWhoAmI() returned false - no ACK from device.");
  }

  // --- Test 2: writeRegister16() helper (no dummy bytes on write) ---
  bool writeOk = bus.writeRegister16(REG_ACC_CONF, 0x708B);
  Serial.print("[writeRegister16] ACC_CONF write: ");
  Serial.println(writeOk ? "[PASS]" : "[FAIL]");

  // --- Test 3: readRegister16() - dummy bytes handled transparently ---
  uint16_t confValue;
  if (bus.readRegister16(REG_ACC_CONF, confValue)) {
    Serial.print("[readRegister16] ACC_CONF readback: 0x");
    Serial.println(confValue, HEX);

    if (confValue == 0x708B) {
      Serial.println("[PASS] Write/readback round-trip matches.");
    } else {
      Serial.println("[FAIL] Readback does not match what was written.");
    }
  } else {
    Serial.println("[FAIL] readRegister16() returned false on ACC_CONF.");
  }

  Serial.println("==================================================");
}

void loop() {
  // Nothing to do here - this sketch only validates BusIO on setup().
}
