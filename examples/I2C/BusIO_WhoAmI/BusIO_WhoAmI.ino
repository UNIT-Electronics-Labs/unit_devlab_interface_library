/*
  DevLab_BusIO - getWhoAmI() Validation Example

  Description:
  Exercises DevLab_I2C_Interface::getWhoAmI() against a plain I2C
  device (no leading dummy bytes, unlike the BMI323). Defaults below
  target the MPU6050, but every value is a #define so you can point
  this at whatever sensor you have on the bench.

  To test a different device, change:
    - I2C_ADDRESS       (7-bit I2C address)
    - WHO_AM_I_REG       (the register that holds the identity byte)
    - EXPECTED_WHO_AM_I  (the value the datasheet says to expect)

  Connections (adjust to your board/wiring):
  SDA -> GPIO 24
  SCL -> GPIO 25

  Organization:
  UNIT Electronics - DevLab Ecosystem

  License:
  MIT License
*/

#include <DevLab_I2C_Interface.h>

#define SDA_PIN 24
#define SCL_PIN 25

#define I2C_ADDRESS        0x68  // MPU6050 default address (AD0 low)
#define WHO_AM_I_REG        0x75  // MPU6050 WHO_AM_I register
#define EXPECTED_WHO_AM_I   0x68  // Value the MPU6050 datasheet specifies

DevLab_I2C_Interface bus(Wire, I2C_ADDRESS, 400000);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("==================================================");
  Serial.println(" DevLab_BusIO - getWhoAmI() Validation");
  Serial.println("==================================================");

  bus.begin(SDA_PIN, SCL_PIN);
  delay(50);

  uint8_t whoAmI;
  if (bus.getWhoAmI(WHO_AM_I_REG, whoAmI)) {
    Serial.print("WHO_AM_I: 0x");
    Serial.print(whoAmI, HEX);
    Serial.print("  (expected 0x");
    Serial.print(EXPECTED_WHO_AM_I, HEX);
    Serial.println(")");

    if (whoAmI == EXPECTED_WHO_AM_I) {
      Serial.println("[PASS] Device identity matches.");
    } else {
      Serial.println("[FAIL] Unexpected value - wrong device, address, or register.");
    }
  } else {
    Serial.println("[FAIL] getWhoAmI() returned false - no ACK from device.");
  }

  Serial.println("==================================================");
}

void loop() {
  // Nothing to do here - this sketch only validates getWhoAmI() on setup().
}
