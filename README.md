# DevLab_Interface

Generic, zero-overhead bus contract for the DevLab ecosystem. It defines
one register-access API (`readRegister8/16/32`, `writeRegister8/16/32`,
`readBits...`) resolved once via CRTP, and lets any concrete transport
(I2C, SPI, ...) plug into it by implementing just two raw primitives:
`writeBytes()` and `readBytes()`.

Sensor drivers are written once against `DevLab_BusIO`, and work
unchanged on every bus that implements the contract.

---

## Features

- Generic bus contract (`DevLab_BusIO<Derived>`) resolved at compile time via CRTP — no vtable, no runtime dispatch cost
- Fixed-width register helpers (8/16/32-bit read/write) implemented once, shared by every bus
- Bit-field helpers (`readBits`, `readBits16`, `readBits32`)
- Concrete I2C transport (`DevLab_I2C_Interface`) built on `Wire`
- Per-device leading dummy-byte handling (e.g. Bosch BMI323) declared once at construction, discarded automatically by every helper
- Explicit SDA/SCL pin support on ESP32 and RP2040 (arduino-pico)
- Concrete bus classes are plain, non-template `.h`/`.cpp` pairs — only the contract itself is header-only

---

## Supported Interfaces

| Interface | Status |
|---|---|
| I2C | Implemented (`DevLab_I2C_Interface`) |
| SPI | Planned |

---

## Installation

### Manual Installation

1. Download or clone this repository
2. Copy the folder into your Arduino `libraries/` directory
3. Restart the Arduino IDE
4. Open `File -> Examples -> DevLab_Interface` to try the examples

---

## Quick Start Example

```cpp
#include <DevLab_I2C_Interface.h>

#define SDA_PIN 24
#define SCL_PIN 25

#define REG_WHO_AM_I 0x00

// Wire, I2C address, clock (Hz), leading dummy bytes on read (device-specific)
DevLab_I2C_Interface bus(Wire, 0x69, 400000, 0);

void setup() {
  Serial.begin(115200);

  bus.begin(SDA_PIN, SCL_PIN);

  uint8_t whoAmI;
  if (bus.getWhoAmI(REG_WHO_AM_I, whoAmI)) {
    Serial.print("WHO_AM_I: 0x");
    Serial.println(whoAmI, HEX);
  } else {
    Serial.println("No ACK from device.");
  }
}

void loop() {}
```

---

## Building a Sensor Driver on Top

A sensor driver only needs the contract, not the transport it ends up
running on:

```cpp
class MySensor {
public:
    template <typename Bus>
    bool begin(Bus &bus) {
        uint8_t chipId;
        return bus.readRegister8(REG_CHIP_ID, chipId) && chipId == EXPECTED_CHIP_ID;
    }
};
```

Any bus deriving from `DevLab_BusIO` — I2C today, SPI once implemented —
works with the same driver code unchanged.

---

## Examples

| Example | Description |
|---|---|
| `I2C/BusIO_WhoAmI` | Validates `getWhoAmI()` against a plain I2C device (MPU6050 by default) |
| `I2C/BusIO_Basic` | Exercises `DevLab_BusIO` + `DevLab_I2C_Interface` end to end (read/write/readback) against a Bosch BMI323, including its 2 leading dummy read bytes |
| `SPI/BMI323_SPI_BasicRead` | Reference integration: a sensor driver (`DevLab_BMI323`) built on the DevLab bus contract, read over SPI |
| `SPI/spi_interruptbmi323` | Reference integration: same driver, using its physical interrupt pins |

The SPI examples depend on the external `DevLab_BMI323` driver library
and demonstrate how a real sensor driver consumes this contract; the
SPI concrete bus class itself (`DevLab_SPI_Interface`) is not yet part
of this repository.

---

## Wiring Example (I2C)

| Device | MCU |
|---|---|
| SDA | SDA |
| SCL | SCL |
| VDD | 3.3V |
| GND | GND |

---

## Compatibility

| MCU Platform | Status |
|---|---|
| ESP32 | Tested |
| ESP32-C6 / C5 / H2 | Tested |
| RP2040 (arduino-pico) | Tested |
| Arduino-compatible boards | Compatible |

---

## Folder Structure

```text
unit_devlab_interface_library/
├── examples/
│   ├── I2C/
│   │   ├── BusIO_Basic/
│   │   └── BusIO_WhoAmI/
│   └── SPI/
│       ├── BMI323_SPI_BasicRead/
│       └── spi_interruptbmi323/
├── src/
│   ├── DevLab_BusIO.h
│   ├── DevLab_I2C_Interface.h
│   └── DevLab_I2C_Interface.cpp
├── library.properties
├── README.md
└── license.txt
```

---

## Version

| Parameter | Value |
|---|---|
| Library Name | DevLab_Interface |
| Version | 1.0.0 |
| Communication | I2C (SPI planned) |
| Architecture | Cross-platform |

---

## Author

UNIT Electronics - DevLab Ecosystem

---

## License

MIT License
