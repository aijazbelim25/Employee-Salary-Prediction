# Arduino Library Requirements

This file lists all required libraries for the wireless robotics project.

## Installation Methods

### Method 1: Arduino IDE Library Manager (Recommended)
1. Open Arduino IDE
2. Go to: Sketch → Include Library → Manage Libraries
3. Search for each library name below
4. Click Install

### Method 2: Manual Installation
Download from GitHub and place in Arduino/libraries folder

---

## Transmitter Libraries (Arduino Pro Mini)

### 1. RF24
- **Author**: TMRh20
- **Version**: 1.4.0 or higher
- **Purpose**: NRF24L01 wireless communication
- **Library Manager Name**: "RF24 by TMRh20"
- **GitHub**: https://github.com/nRF24/RF24
- **License**: GPL-2.0

**Installation via Library Manager:**
```
Search: RF24
Install: RF24 by TMRh20
```

### 2. SPI
- **Source**: Built-in with Arduino IDE
- **Purpose**: SPI communication for NRF24L01
- **No installation required**

---

## Receiver Libraries (ESP32)

### 1. RF24
- **Author**: TMRh20
- **Version**: 1.4.0 or higher
- **Purpose**: NRF24L01 wireless communication
- **Library Manager Name**: "RF24 by TMRh20"
- **GitHub**: https://github.com/nRF24/RF24
- **License**: GPL-2.0

**Installation via Library Manager:**
```
Search: RF24
Install: RF24 by TMRh20
```

### 2. ESP32Servo
- **Author**: Kevin Harrington
- **Version**: 0.13.0 or higher
- **Purpose**: Servo motor control on ESP32
- **Library Manager Name**: "ESP32Servo by Kevin Harrington"
- **GitHub**: https://github.com/madhephaestus/ESP32Servo
- **License**: LGPL-3.0

**Installation via Library Manager:**
```
Search: ESP32Servo
Install: ESP32Servo by Kevin Harrington
```

### 3. SPI
- **Source**: Built-in with Arduino IDE
- **Purpose**: SPI communication for NRF24L01
- **No installation required**

---

## Board Support Packages

### ESP32 Board Support
**Required for**: Receiver (ESP32-WROOM-32)

**Installation Steps:**
1. Open Arduino IDE
2. Go to: File → Preferences
3. In "Additional Board Manager URLs" field, add:
   ```
   https://dl.espressif.com/dl/package_esp32_index.json
   ```
4. Click OK
5. Go to: Tools → Board → Boards Manager
6. Search for: "esp32"
7. Install: "esp32 by Espressif Systems"
8. Version: 2.0.0 or higher recommended

**Board Selection:**
- Tools → Board → ESP32 Arduino → ESP32 Dev Module

### Arduino AVR Boards
**Required for**: Transmitter (Arduino Pro Mini)

**Installation:**
- Usually pre-installed with Arduino IDE
- If missing: Tools → Board → Boards Manager → Search "Arduino AVR Boards"

**Board Selection:**
- Tools → Board → Arduino AVR Boards → Arduino Pro or Pro Mini
- Tools → Processor → ATmega328P (5V, 16MHz)

---

## Library Version Compatibility

### Tested Versions
| Library      | Minimum Version | Tested Version | Status |
|-------------|----------------|----------------|--------|
| RF24        | 1.4.0          | 1.4.8          | ✅ Working |
| ESP32Servo  | 0.13.0         | 0.13.0         | ✅ Working |
| ESP32 Core  | 2.0.0          | 2.0.11         | ✅ Working |

### Known Issues
- **RF24 < 1.4.0**: May have ESP32 SPI compatibility issues
- **ESP32Servo < 0.11.0**: Limited PWM channel support
- **ESP32 Core < 2.0.0**: GPIO matrix configuration changes

---

## Verification

### Verify Transmitter Libraries
Upload this test sketch to verify RF24 installation:
```cpp
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

void setup() {
  Serial.begin(115200);
  RF24 radio(9, 10);
  if (radio.begin()) {
    Serial.println("RF24 library working!");
  } else {
    Serial.println("RF24 library ERROR!");
  }
}

void loop() {}
```

### Verify Receiver Libraries
Upload this test sketch to verify ESP32Servo installation:
```cpp
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <ESP32Servo.h>

void setup() {
  Serial.begin(115200);
  Servo testServo;
  testServo.attach(2);
  testServo.write(90);
  Serial.println("ESP32Servo library working!");
  
  SPI.begin(18, 19, 23);
  RF24 radio(4, 5);
  if (radio.begin()) {
    Serial.println("RF24 library working!");
  } else {
    Serial.println("RF24 library ERROR!");
  }
}

void loop() {}
```

---

## Troubleshooting Library Issues

### "Library not found" error
- Restart Arduino IDE after installation
- Check library installed in correct folder: 
  - Windows: `Documents/Arduino/libraries/`
  - Mac: `~/Documents/Arduino/libraries/`
  - Linux: `~/Arduino/libraries/`

### "Multiple libraries found" warning
- Check for duplicate installations
- Remove older versions
- Keep only one copy of each library

### Compilation errors with RF24
- Update to latest version via Library Manager
- Check ESP32 board support is installed
- Verify SPI pins match your configuration

### Compilation errors with ESP32Servo
- Ensure ESP32 board package is installed
- Update ESP32 board package to latest version
- Check servo pin assignments (avoid GPIO0, GPIO1)

---

## Alternative Libraries (Not Recommended)

These alternatives exist but are NOT used in this project:

### For RF24:
- ❌ Mirf library (outdated)
- ❌ ManiacBug RF24 (deprecated)
- ✅ Use: RF24 by TMRh20 (actively maintained)

### For ESP32 Servos:
- ❌ Standard Arduino Servo (not ESP32 compatible)
- ❌ ServoESP32 (limited functionality)
- ✅ Use: ESP32Servo by Kevin Harrington (ESP32 optimized)

---

## License Information

This project uses the following open-source libraries:

- **RF24**: GPL-2.0 License
- **ESP32Servo**: LGPL-3.0 License
- **ESP32 Arduino Core**: LGPL-2.1 License

Ensure compliance with these licenses if redistributing this project.

---

## Support and Updates

### Getting Help
- RF24 Library: https://github.com/nRF24/RF24/issues
- ESP32Servo: https://github.com/madhephaestus/ESP32Servo/issues
- ESP32 Arduino: https://github.com/espressif/arduino-esp32/issues

### Checking for Updates
Regularly check for library updates via Arduino IDE Library Manager to get:
- Bug fixes
- Performance improvements
- New features
- Security patches

**Recommended**: Check for updates monthly
