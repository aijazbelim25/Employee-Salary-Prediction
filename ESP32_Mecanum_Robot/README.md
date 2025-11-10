# ESP32 Mecanum Robot Controller

This is a complete Arduino sketch for an ESP32 38-pin board that functions as a hybrid Wi-Fi and NRF24L01 receiver for a 4-wheel drive mecanum robot.

## Features

- **Dual Control Modes**: 
  - Primary: NRF24L01 wireless control with joystick
  - Secondary: Wi-Fi web interface with 10-button control
- **Intelligent Mode Switching**: Automatically switches to Wi-Fi control if NRF signal is lost for 3 seconds
- **Mecanum Drive Support**: Full mecanum wheel control with diagonal movement capability
- **Soft Start/Stop**: Prevents power supply brownouts with gradual motor speed ramping
- **Web Interface**: Mobile-friendly control panel accessible via Wi-Fi

## Hardware Requirements

### Components
- ESP32 Development Board (38-pin)
- NRF24L01 Module with 5V adapter
- 4x DC Motors (for mecanum wheels)
- 2x L298N Motor Drivers (or equivalent)
- Power supply (appropriate for motors and ESP32)

### Pin Connections

#### NRF24L01 Module
| NRF24L01 Pin | ESP32 Pin |
|--------------|-----------|
| SCK          | 18        |
| MISO         | 19        |
| MOSI         | 23        |
| CE           | 4         |
| CSN          | 5         |
| VCC          | 3.3V      |
| GND          | GND       |

#### Motor Drivers (L298N)

**Front-Right Motor** (PWM Channel 0)
- IN1 → GPIO 16
- IN2 → GPIO 17
- ENA → GPIO 13

**Back-Right Motor** (PWM Channel 1)
- IN1 → GPIO 32
- IN2 → GPIO 33
- ENB → GPIO 22

**Front-Left Motor** (PWM Channel 2)
- IN1 → GPIO 27
- IN2 → GPIO 26
- ENA → GPIO 14

**Back-Left Motor** (PWM Channel 3)
- IN1 → GPIO 12
- IN2 → GPIO 15
- ENB → GPIO 2

## Software Requirements

### Required Libraries

Install these libraries via Arduino IDE Library Manager:

1. **SPI** (Built-in)
2. **RF24** by TMRh20
   - Library Manager: Search "RF24"
   - Or: https://github.com/nRF24/RF24

3. **WiFi** (Built-in for ESP32)

4. **AsyncTCP** by me-no-dev
   - Download: https://github.com/me-no-dev/AsyncTCP
   - Install manually to Arduino/libraries/

5. **ESPAsyncWebServer** by me-no-dev
   - Download: https://github.com/me-no-dev/ESPAsyncWebServer
   - Install manually to Arduino/libraries/

6. **vector** (Part of C++ STL, included)

### Board Configuration

In Arduino IDE:
1. Go to **File → Preferences**
2. Add ESP32 board manager URL: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. Go to **Tools → Board → Board Manager**
4. Search "ESP32" and install "ESP32 by Espressif Systems"
5. Select **Tools → Board → ESP32 Dev Module**

## Usage

### Uploading the Sketch

1. Connect ESP32 to your computer via USB
2. Open `ESP32_Mecanum_Robot.ino` in Arduino IDE
3. Select the correct board and port in Tools menu
4. Click Upload

### NRF24L01 Control (Primary Mode)

The robot listens for radio packets on address `00001` with the following structure:

```cpp
struct RadioPacket {
  int joyX;  // Strafe (0-1023)
  int joyY;  // Forward/Back (0-1023)
  int pot;   // Rotation (0-1023)
}
```

**Mecanum Mixing Logic**:
- `joyX`: Strafe left/right (512 = center)
- `joyY`: Forward/backward (512 = center)
- `pot`: Rotation left/right (512 = center)

The robot will:
- Set `nrfActive = true` when receiving packets
- Process packets using mecanum drive mixing
- Allow diagonal movement through joystick combinations
- Apply dead band (100) and minimum speed (100) for smooth control

### Wi-Fi Control (Secondary Mode)

1. **Connect to Wi-Fi**:
   - SSID: `MyWiFiCar`
   - Password: `12345678`

2. **Access Web Interface**:
   - Open browser and navigate to: `http://192.168.4.1`
   - You'll see a control panel with 10 buttons

3. **Control Commands**:
   - **0**: Stop
   - **1**: Forward
   - **2**: Backward
   - **3**: Strafe Left
   - **4**: Strafe Right
   - **5**: Rotate Left
   - **6**: Rotate Right
   - **7**: Diagonal Forward-Left
   - **8**: Diagonal Forward-Right
   - **9**: Diagonal Backward-Left
   - **10**: Diagonal Backward-Right

**Important**: Wi-Fi commands are ignored when NRF24L01 is actively transmitting.

### Mode Switching

- **NRF → Wi-Fi**: If no NRF signal for 3 seconds, robot switches to Wi-Fi control and stops motors
- **Wi-Fi → NRF**: As soon as NRF signal is detected, it takes priority over Wi-Fi

## Configuration

You can customize these constants in the sketch:

```cpp
#define DEAD_BAND     100   // Joystick dead zone
#define MIN_SPEED     100   // Minimum motor speed
#define NRF_TIMEOUT   3000  // NRF timeout in milliseconds
#define PWM_FREQ      5000  // PWM frequency in Hz
```

Wi-Fi credentials:
```cpp
const char* ssid = "MyWiFiCar";
const char* password = "12345678";
```

## Troubleshooting

### NRF24L01 Not Working
- Verify all pin connections
- Ensure NRF24L01 has stable 3.3V power (use 5V adapter)
- Check for "NRF24L01 initialization failed!" in Serial Monitor
- Try adding a capacitor (10µF) across NRF24L01 power pins

### Motors Not Responding
- Check all motor driver connections
- Verify motor driver power supply
- Test individual motors manually
- Check Serial Monitor for command output

### Wi-Fi Connection Issues
- Ensure ESP32 is powered properly
- Check Wi-Fi credentials
- Look for AP IP address in Serial Monitor
- Try resetting ESP32

### Web Interface Not Loading
- Verify you're connected to "MyWiFiCar" network
- Navigate to exact IP shown in Serial Monitor
- Clear browser cache
- Try different browser

## Serial Monitor Output

Set baud rate to **115200** to see:
- Initialization status
- NRF packet data
- Wi-Fi connection info
- Command execution logs
- Error messages

## Safety Notes

⚠️ **Important Safety Information**:

1. **Power Supply**: Use adequate power supply for motors (typically 7-12V)
2. **Current Rating**: Ensure motor drivers can handle motor current
3. **Soft Start/Stop**: Built-in to prevent voltage drops and brownouts
4. **Emergency Stop**: Physical emergency stop button recommended
5. **Testing**: Test individual motors before full assembly
6. **Timeout**: 3-second NRF timeout prevents runaway robot

## License

This code is provided as-is for educational and hobbyist purposes.

## Credits

- RF24 Library by TMRh20
- ESPAsyncWebServer by me-no-dev
- AsyncTCP by me-no-dev
