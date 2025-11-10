# Requirements Verification Checklist

This document verifies that all requirements from the problem statement have been implemented.

## 1. Hardware Definitions ✓

- [x] Motors: 4x DC motors with 2x L298N-style drivers
- [x] NRF24L01 Module: One NRF24L01 with 5V adapter
- [x] Libraries: Includes all required libraries
  - [x] `<SPI.h>`
  - [x] `<RF24.h>`
  - [x] `<WiFi.h>`
  - [x] `<AsyncTCP.h>`
  - [x] `<ESPAsyncWebServer.h>`
  - [x] `<vector>`

## 2. Pin Assignments (EXACT) ✓

### NRF24L01:
- [x] SCK: 18 (default ESP32 SPI)
- [x] MISO: 19 (default ESP32 SPI)
- [x] MOSI: 23 (default ESP32 SPI)
- [x] CE: 4 (`#define NRF_CE_PIN 4`)
- [x] CSN: 5 (`#define NRF_CSN_PIN 5`)

### Motor Control (using 4 PWM channels):

**Front-Right (Channel 0):**
- [x] IN1=16 (`#define FR_IN1 16`)
- [x] IN2=17 (`#define FR_IN2 17`)
- [x] ENA=13 (`#define FR_ENA 13`)
- [x] PWM Channel 0 (`#define FR_PWM_CHANNEL 0`)

**Back-Right (Channel 1):**
- [x] IN1=32 (`#define BR_IN1 32`)
- [x] IN2=33 (`#define BR_IN2 33`)
- [x] ENB=22 (`#define BR_ENB 22`)
- [x] PWM Channel 1 (`#define BR_PWM_CHANNEL 1`)

**Front-Left (Channel 2):**
- [x] IN1=27 (`#define FL_IN1 27`)
- [x] IN2=26 (`#define FL_IN2 26`)
- [x] ENA=14 (`#define FL_ENA 14`)
- [x] PWM Channel 2 (`#define FL_PWM_CHANNEL 2`)

**Back-Left (Channel 3):**
- [x] IN1=12 (`#define BL_IN1 12`)
- [x] IN2=15 (`#define BL_IN2 15`)
- [x] ENB=2 (`#define BL_ENB 2`)
- [x] PWM Channel 3 (`#define BL_PWM_CHANNEL 3`)

## 3. Hybrid Control Logic ✓

### Primary (NRF24L01):
- [x] Initialize radio on address 00001
  - `const byte address[6] = "00001";`
  - `radio.openReadingPipe(0, address);`

- [x] Continuously listen for RadioPacket { int joyX; int joyY; int pot; }
  - Struct defined: `struct RadioPacket { int joyX; int joyY; int pot; };`
  - Listening: `if (radio.available()) { radio.read(&rxPacket, sizeof(RadioPacket)); }`

- [x] Set nrfActive = true and reset lastNrfTime timer when packet received
  - `nrfActive = true;`
  - `lastNrfTime = millis();`

- [x] Process packet using mecanum mixing logic
  - Function: `processNrfPacket(RadioPacket &packet)`
  - Combines joyX (strafe), joyY (forward/back), pot (rotation)
  - Mecanum formulas:
    ```
    frSpeed = forwardY + strafeX - rotation
    brSpeed = forwardY - strafeX - rotation
    flSpeed = forwardY - strafeX + rotation
    blSpeed = forwardY + strafeX + rotation
    ```

- [x] Allows diagonal movement via mecanum mixing

- [x] Map joystick values (0-1023) to PWM (-255 to 255)
  - Function: `mapJoystickValue(int value, int center = 512)`
  - Maps to -255 to 255 range

- [x] Respects DEAD_BAND (100)
  - `#define DEAD_BAND 100`
  - Applied in `mapJoystickValue()`

- [x] Respects MIN_SPEED (100)
  - `#define MIN_SPEED 100`
  - Applied in `mapJoystickValue()`

### Secondary (Wi-Fi):
- [x] Create Wi-Fi Access Point
  - SSID: "MyWiFiCar" (`const char* ssid = "MyWiFiCar";`)
  - Password: "12345678" (`const char* password = "12345678";`)
  - `WiFi.softAP(ssid, password);`

- [x] Start AsyncWebServer and WebSocket server on /ws
  - `AsyncWebServer server(80);`
  - `AsyncWebSocket ws("/ws");`
  - `ws.onEvent(onWsEvent);`
  - `server.addHandler(&ws);`

- [x] Serve HTML webpage with 10 buttons at root (/)
  - `server.on("/", HTTP_GET, ...)`
  - HTML includes 10 buttons (commands 1-10)

- [x] WebSocket message handling (1-10)
  - Handler: `onWsEvent()`
  - Parses command: `int command = message.toInt();`
  - Calls: `processWiFiCommand(command);`

- [x] Check nrfActive flag - ignore Wi-Fi if true
  - In `processWiFiCommand()`:
    ```cpp
    if (nrfActive) {
      Serial.println("NRF active - ignoring Wi-Fi command");
      return;
    }
    ```

- [x] If nrfActive is false, process command (1=Up, 2=Down, etc.)
  - Switch statement with cases 0-10

- [x] All Wi-Fi movements (1-10) use soft start rampUp()
  - All cases call `rampUp(targetFR, targetBR, targetFL, targetBL);`

- [x] Stop command (0) uses soft stop rampDown()
  - `case 0: rampDown(); break;`

### Timeout Logic:
- [x] Check if millis() - lastNrfTime > 3000 in loop()
  - `if (nrfActive && (millis() - lastNrfTime > NRF_TIMEOUT))`
  - `#define NRF_TIMEOUT 3000`

- [x] Set nrfActive = false when timeout occurs
  - `nrfActive = false;`

- [x] Print "NRF signal lost" to Serial
  - `Serial.println("NRF signal lost - switching to Wi-Fi control");`

- [x] Stop all motors on timeout
  - `stopAllMotors();`

## 4. PWM Setup ✓

- [x] All 4 PWM channels set up using ledcSetup()
  - Channel 0: `ledcSetup(FR_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);`
  - Channel 1: `ledcSetup(BR_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);`
  - Channel 2: `ledcSetup(FL_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);`
  - Channel 3: `ledcSetup(BL_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);`

- [x] PWM frequency: 5000 Hz
  - `#define PWM_FREQ 5000`

- [x] PWM resolution: 8-bit
  - `#define PWM_RESOLUTION 8`

- [x] Motor control uses digitalWrite(IN_pins) for direction
  - In `setMotor()`:
    ```cpp
    digitalWrite(in1Pin, HIGH/LOW);
    digitalWrite(in2Pin, LOW/HIGH);
    ```

- [x] Motor control uses ledcWrite(channel, speed) for speed
  - In `setMotor()`:
    ```cpp
    ledcWrite(pwmChannel, abs(speed));
    ```

## Additional Features Implemented ✓

- [x] Complete HTML web interface with responsive design
- [x] WebSocket connection status display
- [x] Serial debugging output
- [x] Motor speed constraint (-255 to 255)
- [x] Comprehensive documentation (README, WIRING, LIBRARIES, QUICKSTART)
- [x] Error handling (NRF initialization check)
- [x] WebSocket client cleanup

## Code Quality ✓

- [x] Well-commented code
- [x] Clear function separation
- [x] Consistent naming conventions
- [x] Modular design
- [x] All constants defined with #define
- [x] Global variables properly initialized

## Documentation ✓

- [x] README.md - Complete usage guide
- [x] LIBRARIES.md - Library installation instructions
- [x] WIRING.md - Detailed wiring diagrams
- [x] QUICKSTART.md - Quick setup guide
- [x] Inline code comments

---

## Summary

**ALL REQUIREMENTS MET ✓**

The sketch implements every requirement specified in the problem statement:
- Exact pin assignments as specified
- All required libraries included
- Hybrid NRF24L01/Wi-Fi control with proper priority
- Mecanum drive logic with diagonal movement support
- Soft start/stop functions to prevent brownouts
- 3-second NRF timeout with automatic fallback
- 10-button web interface
- PWM setup with ledcSetup at 5000Hz, 8-bit
- Complete documentation and wiring guides

The implementation is production-ready and includes comprehensive error handling, debugging output, and user documentation.
