# Quick Start Guide - Wireless Robotics System

## Step-by-Step Setup Instructions

### Phase 1: Component Preparation (30 minutes)

#### Transmitter Assembly
1. **Arduino Pro Mini Setup**
   - Solder header pins if not pre-installed
   - Prepare FTDI adapter for programming
   - Test power connection (5V via L7805)

2. **NRF24L01 Adapter**
   - Use adapter board with 3.3V regulator
   - Add 10µF capacitor across VCC/GND on adapter
   - Verify adapter provides 3.3V output

3. **Control Components**
   - Test all joysticks with multimeter (0-5V sweep)
   - Test potentiometers (0-5V sweep)
   - Verify buttons/switches functionality

#### Receiver Assembly
1. **ESP32 Setup**
   - Verify ESP32 is genuine (check chip marking)
   - Test USB connection and Arduino IDE detection
   - Install ESP32 board definitions

2. **Motor Drivers (L298N)**
   - Verify ENA/ENB jumpers are ON
   - Connect heat sinks if driving high current
   - Test voltage regulator output (should be ~5V)

3. **Servos**
   - Test each servo individually
   - Verify they center at 90° signal
   - Label each servo for arm position

### Phase 2: Software Setup (20 minutes)

#### Install Required Libraries
1. **Open Arduino IDE**
2. **Install RF24 Library**
   - Sketch → Include Library → Manage Libraries
   - Search: "RF24"
   - Install: "RF24 by TMRh20"

3. **Install ESP32Servo Library**
   - In Library Manager
   - Search: "ESP32Servo"
   - Install: "ESP32Servo by Kevin Harrington"

4. **Install ESP32 Board Support**
   - File → Preferences
   - Additional Board Manager URLs:
     ```
     https://dl.espressif.com/dl/package_esp32_index.json
     ```
   - Tools → Board → Boards Manager
   - Search: "esp32"
   - Install: "esp32 by Espressif Systems"

#### Upload Code
1. **Upload Transmitter Code**
   - Open: `transmitter/transmitter.ino`
   - Tools → Board → Arduino Pro or Pro Mini
   - Tools → Processor → ATmega328P (5V, 16MHz)
   - Select COM port
   - Upload
   - Open Serial Monitor (115200 baud)

2. **Upload Receiver Code**
   - Open: `receiver/receiver.ino`
   - Tools → Board → ESP32 Dev Module
   - Select COM port
   - Upload
   - Open Serial Monitor (115200 baud)

### Phase 3: Hardware Assembly (60 minutes)

#### Transmitter Wiring

**NRF24L01 to Arduino Pro Mini:**
```
NRF24  --> Arduino
VCC    --> 3.3V (from adapter)
GND    --> GND
CE     --> 9
CSN    --> 10
SCK    --> 13
MOSI   --> 11
MISO   --> 12
```

**Left Joystick (Drive Control):**
```
VCC --> 5V
GND --> GND
VRx --> A0
VRy --> A1
```

**Right Joystick (Arm Control):**
```
VCC --> 5V
GND --> GND
VRx --> A2
VRy --> A3
```

**Potentiometers:**
```
Pot 1: Left → GND, Center → A6, Right → 5V
Pot 2: Left → GND, Center → A7, Right → 5V
```

**Buttons (Active LOW):**
```
Button 1 (CW):  One pin → Pin 2, Other pin → GND
Button 2 (CCW): One pin → Pin 3, Other pin → GND
```

**Switches (Active LOW):**
```
Switch 1 (Close):  One pin → Pin 4, Other pin → GND
Switch 2 (Open):   One pin → Pin 5, Other pin → GND
Switch 3 (E-Stop): One pin → Pin 6, Other pin → GND
```

**Power:**
```
7.4V LiPo → L7805 → 5V → Arduino Pro Mini RAW/VCC
                      → Joysticks VCC
                      → Potentiometers
```

#### Receiver Wiring

**NRF24L01 to ESP32:**
```
NRF24  --> ESP32
VCC    --> 3.3V
GND    --> GND
CE     --> GPIO4
CSN    --> GPIO5
SCK    --> GPIO18
MISO   --> GPIO19
MOSI   --> GPIO23
```

**Motors to L298N (Driver 1):**
```
Motor FL: OUT1, OUT2
Motor FR: OUT3, OUT4

L298N → ESP32
IN1 (FL) → GPIO33
IN2 (FL) → GPIO15
IN3 (FR) → GPIO27
IN4 (FR) → GPIO14
```

**Motors to L298N (Driver 2):**
```
Motor RL: OUT1, OUT2
Motor RR: OUT3, OUT4

L298N → ESP32
IN1 (RL) → GPIO13
IN2 (RL) → GPIO12
IN3 (RR) → GPIO32
IN4 (RR) → GPIO26
```

**Servos to ESP32:**
```
Servo         Pin
-----         ----
Base      → GPIO2   (Orange/Yellow wire)
Shoulder  → GPIO16
Elbow     → GPIO21
Wrist     → GPIO22
Gripper   → GPIO17

All servos:
  Brown/Black → GND
  Red         → 6V (from LM2596)
  Orange      → Signal (GPIO pin above)
```

**Power Distribution:**
```
7.4V LiPo → LM2596 (set to 6V) → L298N VMS (both drivers)
                                → Servo power rail (VCC for all servos)

5V USB Bank → ESP32 5V pin
            → NRF24 adapter VCC (or use 3.3V pin)

CRITICAL: Connect ALL grounds together!
```

### Phase 4: Testing (45 minutes)

#### Test 1: Communication (10 min)
1. Power on transmitter
2. Power on receiver
3. Check Serial Monitors
4. Transmitter should show "TX: OK"
5. Receiver should show "RX OK"
6. If "TX: FAIL", check NRF24 connections

#### Test 2: Emergency Stop (5 min)
1. Activate E-Stop switch
2. Receiver should print "*** EMERGENCY STOP ACTIVATED ***"
3. All motors should be stopped
4. Deactivate E-Stop
5. Normal operation should resume

#### Test 3: Drive System (10 min)
1. Keep E-Stop deactivated
2. Move left joystick forward
3. Watch Serial Monitor: Motors L and R should show positive values
4. Move joystick backward: negative values
5. Move joystick left/right: different L/R values
6. Center joystick: motors should stop (values near 0)

#### Test 4: Arm Control - Joystick (10 min)
1. Move right joystick left/right
2. Base servo should rotate
3. Move right joystick up/down
4. Shoulder servo should move
5. Servos should stop when joystick centered

#### Test 5: Arm Control - Potentiometers (10 min)
1. Turn Pot 1 slowly
2. Elbow servo should move and stop
3. Turn Pot 1 back: servo moves opposite direction
4. Turn Pot 2: Wrist servo moves
5. Rapid pot changes should result in larger movements

#### Test 6: Fine Controls (5 min)
1. Press Button 1 (CW): wrist rotates slowly
2. Press Button 2 (CCW): wrist rotates opposite
3. Activate Switch 1 (Close): gripper closes
4. Activate Switch 2 (Open): gripper opens

### Phase 5: Calibration (15 minutes)

#### Motor Direction Check
If motors run backward:
- Swap the two wires on that motor
- OR swap IN1/IN2 pins in code

#### Servo Direction/Range
1. Check each servo mechanical range
2. Adjust min/max if servos bind:
   ```cpp
   servoBase.write(constrain(basePos, 10, 170));  // Adjust limits
   ```

#### Deadzone Tuning
If joysticks drift:
1. Center joysticks
2. Check Serial Monitor values
3. Adjust JOYSTICK_DEADZONE in receiver code:
   ```cpp
   const int JOYSTICK_DEADZONE = 150;  // Increase if drifting
   ```

#### Sensitivity Tuning
Adjust in receiver code as needed:
```cpp
const float POT_SENSITIVITY = 0.20;      // Higher = more sensitive pots
const float SERVO_RATE_SPEED = 1.2;      // Higher = faster joystick servos
const float BUTTON_FINE_TUNE_STEP = 0.5; // Lower = finer button control
const float GRIPPER_SWITCH_SPEED = 3.0;  // Higher = faster gripper
```

### Phase 6: Final Assembly (30 minutes)

#### Transmitter Housing
1. Mount components in controller box
2. Secure joysticks with nuts
3. Add labels to switches
4. Ensure easy access to E-Stop
5. Mount NRF24 antenna externally

#### Receiver Robot Assembly
1. Mount ESP32 securely on robot chassis
2. Position NRF24 antenna away from motors
3. Route motor wires cleanly
4. Secure servo wires
5. Ensure power connections are solid
6. Add power switch for safety

### Troubleshooting Guide

#### "TX: FAIL" Messages
- **Check**: NRF24 power (must be 3.3V, stable)
- **Add**: 10-100µF capacitor on NRF24 VCC/GND
- **Verify**: All 8 NRF24 connections
- **Try**: Reduce transmit power distance

#### "Warning: No data received"
- **Check**: Both NRF24 modules powered
- **Verify**: Same address "00001" in both codes
- **Test**: Move devices closer together
- **Check**: Antennas properly attached

#### Motors Don't Move
- **Verify**: E-Stop is OFF (Serial shows normal operation)
- **Check**: L298N ENA/ENB jumpers ON
- **Test**: 6V on L298N VMS terminal
- **Verify**: Motor power supply adequate (2A+ per motor)
- **Check**: All grounds connected together

#### Servos Jitter/Twitch
- **Add**: Large capacitor (470-1000µF) on servo power rail
- **Check**: Servo power supply current rating (5A+ for 5 servos)
- **Separate**: Servo power from motor power if possible
- **Test**: Disconnect motors, test servos alone

#### Joystick Drift
- **Increase**: JOYSTICK_DEADZONE value
- **Check**: Joystick quality (cheap ones drift more)
- **Verify**: 5V supply is stable
- **Consider**: Adding filtering capacitors

#### Potentiometer Jumpy
- **Increase**: POT_DEADZONE value
- **Decrease**: POT_SENSITIVITY value
- **Check**: Good quality potentiometers
- **Add**: 0.1µF capacitor across pot terminals

### Safety Checklist

Before each operation:
- ✅ E-Stop switch easily accessible
- ✅ All power connections secure
- ✅ No loose wires near moving parts
- ✅ Clear workspace around robot
- ✅ Battery voltage checked (>7.0V recommended)
- ✅ NRF24 communication verified
- ✅ Motor directions correct
- ✅ Servo ranges verified (no binding)
- ✅ E-Stop tested and working

### Performance Tips

1. **Battery Management**
   - Charge LiPo before voltage drops below 6.8V
   - Use battery alarm/checker
   - Never over-discharge (can damage battery)

2. **Range Improvement**
   - Keep antennas vertical
   - Avoid metal obstacles between TX/RX
   - Position NRF24 away from motors/servos
   - Use PA_HIGH for longer range (change in code)

3. **Reliability**
   - Add capacitors on all power rails
   - Use quality wire (22-24 AWG for signals, 18-20 AWG for power)
   - Solder all connections (no loose breadboard wires)
   - Add hot glue to secure connectors

4. **Maintenance**
   - Check all screws before each use
   - Inspect wires for damage
   - Clean dust from electronics
   - Lubricate servo gears periodically
   - Keep spare batteries charged

### Next Steps

Once basic operation is verified:
1. Tune sensitivity parameters to your preference
2. Add additional features (speed modes, servo presets, etc.)
3. Improve power management (voltage monitoring)
4. Add status LEDs for visual feedback
5. Consider adding FPV camera for remote viewing

---

**Congratulations!** You now have a fully functional wireless robotics control system.

Remember: Always test E-Stop before operating!
