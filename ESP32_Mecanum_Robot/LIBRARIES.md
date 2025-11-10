# Library Dependencies for ESP32 Mecanum Robot

This file lists all required Arduino libraries for the ESP32 Mecanum Robot Controller.

## Installation Instructions

### Method 1: Arduino Library Manager (Recommended for RF24)

1. Open Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries**
3. Search for and install:
   - **RF24** by TMRh20 (version 1.4.0 or later)

### Method 2: Manual Installation (Required for AsyncTCP and ESPAsyncWebServer)

1. **AsyncTCP Library**
   - Download: https://github.com/me-no-dev/AsyncTCP
   - Click "Code" → "Download ZIP"
   - Extract to: `Arduino/libraries/AsyncTCP/`

2. **ESPAsyncWebServer Library**
   - Download: https://github.com/me-no-dev/ESPAsyncWebServer
   - Click "Code" → "Download ZIP"
   - Extract to: `Arduino/libraries/ESPAsyncWebServer/`

3. Restart Arduino IDE after installation

## Library List

### Built-in Libraries (No installation needed)
- **SPI.h** - SPI communication for NRF24L01
- **WiFi.h** - ESP32 Wi-Fi functionality
- **vector** - C++ Standard Template Library

### External Libraries (Require installation)

| Library | Author | Version | Source |
|---------|--------|---------|--------|
| RF24 | TMRh20 | 1.4.0+ | https://github.com/nRF24/RF24 |
| AsyncTCP | me-no-dev | Latest | https://github.com/me-no-dev/AsyncTCP |
| ESPAsyncWebServer | me-no-dev | Latest | https://github.com/me-no-dev/ESPAsyncWebServer |

## Verification

After installing all libraries:

1. Open `ESP32_Mecanum_Robot.ino`
2. Click **Verify** (checkmark icon)
3. If compilation succeeds, all libraries are correctly installed
4. If errors occur, check that:
   - ESP32 board support is installed
   - All libraries are in the correct folders
   - Arduino IDE has been restarted

## ESP32 Board Support

If not already installed:

1. Go to **File → Preferences**
2. In "Additional Board Manager URLs", add:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Go to **Tools → Board → Board Manager**
4. Search "ESP32"
5. Install "ESP32 by Espressif Systems" (version 2.0.0 or later)

## Troubleshooting

### "RF24.h: No such file or directory"
- Install RF24 library via Library Manager

### "AsyncTCP.h: No such file or directory"
- Download and manually install AsyncTCP library

### "ESPAsyncWebServer.h: No such file or directory"
- Download and manually install ESPAsyncWebServer library

### "WiFi.h: No such file or directory"
- Install ESP32 board support package

### Compilation errors with AsyncTCP
- Ensure you're using AsyncTCP (for ESP32), NOT ESPAsyncTCP (for ESP8266)
- Verify board is set to "ESP32 Dev Module" not ESP8266

## Version Compatibility

Tested with:
- Arduino IDE 1.8.19 or later (or Arduino IDE 2.x)
- ESP32 Board Package 2.0.0 or later
- RF24 Library 1.4.0 or later

## Additional Resources

- RF24 Documentation: http://nRF24.github.io/RF24/
- ESP32 Documentation: https://docs.espressif.com/projects/arduino-esp32/
- ESPAsyncWebServer Examples: https://github.com/me-no-dev/ESPAsyncWebServer/tree/master/examples
