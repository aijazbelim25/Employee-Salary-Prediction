# Wireless Robotics Control System

Complete Arduino code for a two-part wireless robotics system using NRF24L01 modules.

## System Overview

This project implements a wireless control system for a 4-wheel drive robot with a 5-axis robotic arm:

- **Transmitter**: Arduino Pro Mini (5V, 16MHz) with NRF24L01 wireless module
- **Receiver**: ESP32-WROOM-32 (38-pin) with NRF24L01 wireless module
- **Communication**: 20Hz (50ms intervals) at 250KBPS data rate
- **Range**: Short to medium (PA_LOW power level)

## Hardware Components

### Transmitter Side (Remote Controller)
- **Microcontroller**: Arduino Pro Mini (5V, 16MHz)
- **Wireless Module**: NRF24L01 with adapter
- **Inputs**:
  - 2x Analog Joysticks (Left for driving, Right for arm control)
  - 2x Potentiometers (10K ohm, for elbow/wrist incremental control)
  - 2x Push Buttons (momentary, for wrist fine-tuning)
  - 3x Toggle Switches (gripper control + emergency stop)

### Receiver Side (Robot)
- **Microcontroller**: ESP32-WROOM-32 (38-pin)
- **Wireless Module**: NRF24L01 with adapter
- **Motors**: 4x DC BO Motors controlled via 2x L298N motor drivers
- **Servos**: 5x Standard hobby servos (SG90 or similar)
  - Base rotation
  - Shoulder
  - Elbow
  - Wrist rotation
  - Gripper

## Complete Pinout Reference

### Transmitter (Arduino Pro Mini)

#### NRF24L01 Connections
```
NRF24L01    Arduino Pro Mini
--------    ----------------
VCC    -->  3.3V (from adapter)
GND    -->  GND
CE     -->  Pin 9
CSN    -->  Pin 10
SCK    -->  Pin 13
MOSI   -->  Pin 11
MISO   -->  Pin 12
```

#### Analog Inputs
```
Component         Pin
---------         ---
Left Joystick X   A0
Left Joystick Y   A1
Right Joystick X  A2
Right Joystick Y  A3
Potentiometer 1   A6 (Elbow control)
Potentiometer 2   A7 (Wrist control)
```

#### Digital Inputs (All with INPUT_PULLUP)
```
Component                Pin
---------                ---
Button 1 (Rotate CW)     2
Button 2 (Rotate CCW)    3
Switch 1 (Gripper Close) 4
Switch 2 (Gripper Open)  5
Switch 3 (Emergency Stop) 6
```

### Receiver (ESP32-WROOM-32)

#### NRF24L01 Connections
```
NRF24L01    ESP32
--------    -----
VCC    -->  3.3V
GND    -->  GND
CE     -->  GPIO4
CSN    -->  GPIO5
SCK    -->  GPIO18
MISO   -->  GPIO19
MOSI   -->  GPIO23
```

#### Motor Control (L298N Drivers)
```
Motor             ESP32 Pin (IN1)  ESP32 Pin (IN2)
-----             ---------------  ---------------
Front Left        GPIO33           GPIO15
Front Right       GPIO27           GPIO14
Rear Left         GPIO13           GPIO12
Rear Right        GPIO32           GPIO26
```

#### Servo Control
```
Servo       Function           ESP32 Pin
-----       --------           ---------
Servo 1     Base Rotation      GPIO2
Servo 2     Shoulder           GPIO16
Servo 3     Elbow              GPIO21
Servo 4     Wrist Rotation     GPIO22
Servo 5     Gripper            GPIO17
```

## Power System

### Transmitter Power
```
7.4V LiPo --> L7805 Voltage Regulator --> 5V
                                          |
                                          +--> Arduino Pro Mini VCC
                                          +--> NRF24L01 Adapter (provides 3.3V)
                                          +--> Joysticks VCC
                                          +--> Potentiometers VCC
```

### Receiver Power
```
Motors & Servos:
7.4V LiPo --> LM2596 (adjusted to 6V) --> L298N VMS + Servo Power Rails

Logic:
5V USB Power Bank --> ESP32 5V Pin
                  --> NRF24L01 Adapter (provides 3.3V)

Important: All grounds must be connected together (common ground)
```

## Control Mapping

### Drive System (Left Joystick)
- **Forward/Backward**: Left joystick Y-axis
- **Left/Right Turn**: Left joystick X-axis
- **Control Style**: Tank-style mixing
- **Deadzone**: 100 units (prevents drift when centered)
- **Speed Range**: -255 to +255 (PWM)

### Robotic Arm Control

#### Rate-Based Control (Right Joystick)
- **Base Rotation**: Right joystick X-axis
  - Continuous movement while joystick held
  - Speed: 0.8 degrees per loop cycle
- **Shoulder**: Right joystick Y-axis
  - Continuous movement while joystick held
  - Speed: 0.8 degrees per loop cycle

#### Incremental Control (Potentiometers)
- **Elbow** (Pot 1):
  - Turn potentiometer → servo moves proportionally → stops
  - Sensitivity: 0.15x pot movement
  - Deadzone: 5 units
- **Wrist Rotation** (Pot 2):
  - Same behavior as Pot 1
  - Allows precise positioning

#### Fine-Tune Control (Buttons)
- **Button on Pin 2**: Rotate wrist CW (1° per press)
- **Button on Pin 3**: Rotate wrist CCW (1° per press)
- Hold for continuous movement

#### Gripper Control (Switches)
- **Switch on Pin 4**: Close gripper (hold to close, 2°/cycle)
- **Switch on Pin 5**: Open gripper (hold to open, 2°/cycle)

### Emergency Stop
- **Switch on Pin 6**: Emergency stop
- **Behavior**: Immediately stops all motors
- **Priority**: Highest - checked before any other control

## Software Requirements

### Arduino Libraries Required

#### For Transmitter (Arduino Pro Mini):
```
- RF24 by TMRh20 (NRF24L01 library)
- SPI (included with Arduino IDE)
```

#### For Receiver (ESP32):
```
- RF24 by TMRh20 (NRF24L01 library)
- ESP32Servo by Kevin Harrington
- SPI (included with Arduino IDE)
```

### Installation via Arduino IDE
1. Open Arduino IDE
2. Go to Sketch → Include Library → Manage Libraries
3. Search for "RF24" and install "RF24 by TMRh20"
4. Search for "ESP32Servo" and install (for receiver only)
5. Install ESP32 board support (File → Preferences → Additional Board URLs):
   ```
   https://dl.espressif.com/dl/package_esp32_index.json
   ```

## Uploading Instructions

### Transmitter (Arduino Pro Mini)
1. Connect FTDI adapter to Arduino Pro Mini
2. Select: Tools → Board → Arduino Pro or Pro Mini
3. Select: Tools → Processor → ATmega328P (5V, 16MHz)
4. Select correct COM port
5. Upload `transmitter.ino`
6. Open Serial Monitor (115200 baud) to verify operation

### Receiver (ESP32)
1. Connect ESP32 to computer via USB
2. Select: Tools → Board → ESP32 Dev Module
3. Select correct COM port
4. Upload `receiver.ino`
5. Open Serial Monitor (115200 baud) to verify operation

## Serial Debug Output

### Transmitter Output Example
```
TX: OK | LJ(512,512) RJ(512,512) P1:512 P2:512 | BTN:-- --- | SW:----- ---- ------
TX: OK | LJ(712,820) RJ(450,512) P1:520 P2:498 | BTN:CW --- | SW:CLOSE ---- ------
TX: FAIL | LJ(512,512) RJ(512,512) P1:512 P2:512 | BTN:-- CCW | SW:----- OPEN E-STOP
```

### Receiver Output Example
```
RX OK | Motors L:150 R:150 | Servos B:90 S:90 E:90 W:90 G:90
RX OK | Motors L:0 R:200 | Servos B:95 S:88 E:92 W:91 G:95
*** EMERGENCY STOP ACTIVATED ***
```

## Tuning Parameters

Located in receiver code, adjust these for your specific setup:

```cpp
const int JOYSTICK_DEADZONE = 100;        // Joystick center deadzone
const int POT_DEADZONE = 5;               // Potentiometer noise filter
const float POT_SENSITIVITY = 0.15;       // Pot movement scaling
const float SERVO_RATE_SPEED = 0.8;       // Joystick servo speed (deg/cycle)
const float BUTTON_FINE_TUNE_STEP = 1.0;  // Button step size (degrees)
const float GRIPPER_SWITCH_SPEED = 2.0;   // Gripper open/close speed
```

## Troubleshooting

### NRF24L01 Connection Issues
- **Check power**: NRF24 requires clean 3.3V (use adapter with regulator)
- **Add capacitor**: 10-100µF across VCC/GND close to module
- **Check wiring**: Verify all 8 connections
- **Reduce power**: Use PA_LOW setting for testing
- **Check antennas**: Ensure antenna is properly attached

### Motor Not Running
- **L298N jumpers**: Verify ENA/ENB jumpers are ON
- **Power supply**: Check 6V regulated supply to VMS
- **Common ground**: Ensure all grounds connected
- **GPIO pins**: Verify ESP32 pins not conflicting

### Servos Jittering
- **Power supply**: Servos need stable 6V with adequate current
- **Separate power**: Use separate supply from motors if possible
- **Capacitors**: Add 100-470µF capacitor to servo power rail
- **Initial positions**: Ensure servos start at mechanical center (90°)

### Serial Output Shows "FAIL"
- **NRF24 power**: Check 3.3V supply is stable
- **Range**: Move transmitter closer to receiver
- **Interference**: Move away from WiFi routers, metal objects
- **Address match**: Verify address "00001" in both sketches

### Emergency Stop Won't Deactivate
- **Switch wiring**: Verify switch connected with INPUT_PULLUP
- **Logic level**: Check switch in OFF position reads HIGH (1)
- **Debouncing**: May need to toggle switch several times

## Safety Warnings

⚠️ **IMPORTANT SAFETY CONSIDERATIONS**

1. **Emergency Stop**: Always test E-Stop before operating robot
2. **Power**: Never connect 7.4V directly to Arduino or ESP32
3. **Common Ground**: Always connect all ground points together
4. **Current Draw**: Ensure power supplies can handle peak current
5. **Workspace**: Clear area before operating robot
6. **Testing**: Test each subsystem independently before integration

## Technical Specifications

### Communication Protocol
- **Frequency**: 2.4GHz
- **Data Rate**: 250KBPS
- **Power**: PA_LOW (short range, low power)
- **Address**: "00001"
- **Packet Size**: sizeof(ControlData) = ~20 bytes
- **Update Rate**: 20Hz (50ms intervals)

### Data Structure
```cpp
struct ControlData {
  int leftJoyX;           // 0-1023
  int leftJoyY;           // 0-1023
  int rightJoyX;          // 0-1023
  int rightJoyY;          // 0-1023
  int pot1;               // 0-1023
  int pot2;               // 0-1023
  bool buttonRotateCW;    // 0=pressed, 1=released
  bool buttonRotateCCW;   // 0=pressed, 1=released
  bool switchGripperClose;// 0=active, 1=inactive
  bool switchGripperOpen; // 0=active, 1=inactive
  bool switchEmergencyStop;// 0=STOP, 1=normal
};
```

## Wiring Diagrams

### Joystick Wiring
```
Joystick Pin    Connection
------------    ----------
GND        -->  GND
+5V        -->  5V
VRx        -->  Analog Pin (A0-A3)
VRy        -->  Analog Pin (A0-A3)
SW         -->  Not used
```

### Potentiometer Wiring
```
Pot Pin    Connection
-------    ----------
Pin 1  -->  GND
Pin 2  -->  Analog Pin (A6 or A7)
Pin 3  -->  5V
```

### Button/Switch Wiring (INPUT_PULLUP)
```
One terminal  -->  Digital Pin (2-6)
Other terminal --> GND
(No external resistor needed - using INPUT_PULLUP)
```

### L298N Motor Driver Setup
```
L298N Pin     Connection
---------     ----------
VMS      -->  6V from LM2596
GND      -->  Common Ground
5V       -->  Leave disconnected (jumper removed if using internal regulator)
ENA      -->  Jumper ON (or PWM if needed)
IN1      -->  ESP32 GPIO
IN2      -->  ESP32 GPIO
OUT1     -->  Motor +
OUT2     -->  Motor -
ENB      -->  Jumper ON
IN3      -->  ESP32 GPIO
IN4      -->  ESP32 GPIO
OUT3     -->  Motor +
OUT4     -->  Motor -
```

## License

This code is provided as-is for educational and hobbyist purposes.

## Support

For issues, questions, or improvements:
1. Check the troubleshooting section
2. Verify all wiring connections
3. Test components individually
4. Check serial monitor output for error messages

## Version History

- **v1.0** - Initial release
  - Complete transmitter and receiver code
  - Tank-style drive control
  - Rate-based and incremental servo control
  - Emergency stop functionality
  - Comprehensive serial debugging

## Credits

- RF24 Library by TMRh20
- ESP32Servo Library by Kevin Harrington
- Arduino and ESP32 communities

---

**Note**: This is a complete, production-ready implementation. Test thoroughly before deploying in any critical application.
