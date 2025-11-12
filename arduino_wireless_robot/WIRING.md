# Hardware Wiring Reference

Quick visual reference for pin connections.

## Transmitter Connections (Arduino Pro Mini)

### Power Distribution
```
7.4V LiPo Battery
    |
    v
[L7805 Voltage Regulator]
    |
    v
   5V ----+---- Arduino Pro Mini (RAW or VCC)
          |
          +---- NRF24 Adapter (provides 3.3V to NRF24)
          |
          +---- Left Joystick VCC
          |
          +---- Right Joystick VCC
          |
          +---- Potentiometer 1 (Right pin)
          |
          +---- Potentiometer 2 (Right pin)

All GND together
```

### NRF24L01 Module
```
Pro Mini    NRF24 (via adapter)
--------    -------------------
  GND   <->  GND
  5V    <->  VCC (adapter has 3.3V reg)
  Pin 9 <->  CE
  Pin 10<->  CSN
  Pin 11<->  MOSI
  Pin 12<->  MISO
  Pin 13<->  SCK
```

### Left Joystick (Drive Control)
```
Joystick    Pro Mini
--------    --------
  GND   <->  GND
  +5V   <->  5V
  VRx   <->  A0 (X-axis)
  VRy   <->  A1 (Y-axis)
  SW    <->  Not connected
```

### Right Joystick (Arm Base/Shoulder)
```
Joystick    Pro Mini
--------    --------
  GND   <->  GND
  +5V   <->  5V
  VRx   <->  A2 (X-axis)
  VRy   <->  A3 (Y-axis)
  SW    <->  Not connected
```

### Potentiometers (Elbow/Wrist)
```
Pot 1 (Elbow):
  Left terminal   <-> GND
  Center (wiper)  <-> A6
  Right terminal  <-> 5V

Pot 2 (Wrist):
  Left terminal   <-> GND
  Center (wiper)  <-> A7
  Right terminal  <-> 5V
```

### Buttons and Switches (Active LOW)
```
Button 1 (Wrist Rotate CW):
  Terminal 1  <-> Pin 2
  Terminal 2  <-> GND

Button 2 (Wrist Rotate CCW):
  Terminal 1  <-> Pin 3
  Terminal 2  <-> GND

Switch 1 (Gripper Close):
  Terminal 1  <-> Pin 4
  Terminal 2  <-> GND

Switch 2 (Gripper Open):
  Terminal 1  <-> Pin 5
  Terminal 2  <-> GND

Switch 3 (EMERGENCY STOP):
  Terminal 1  <-> Pin 6
  Terminal 2  <-> GND
  
NOTE: No pull-up resistors needed - code uses INPUT_PULLUP
```

---

## Receiver Connections (ESP32-WROOM-32)

### Power Distribution
```
7.4V LiPo Battery
    |
    +---> [LM2596 Buck Converter] (set to 6V)
    |          |
    |          v
    |      6V Output ---+--- L298N #1 (VMS)
    |                   |
    |                   +--- L298N #2 (VMS)
    |                   |
    |                   +--- Servo 1 VCC (Red wire)
    |                   +--- Servo 2 VCC
    |                   +--- Servo 3 VCC
    |                   +--- Servo 4 VCC
    |                   +--- Servo 5 VCC
    |
    v
5V USB Power Bank
    |
    v
   5V ----+---- ESP32 (5V pin)
          |
          +---- NRF24 Adapter VCC

All GND together (CRITICAL!)
```

### NRF24L01 Module
```
ESP32       NRF24 (via adapter)      Note
-----       -------------------      ----
 GND    <->  GND                     Common ground
 3.3V   <->  VCC                     Or use 5V with adapter
GPIO4   <->  CE                      Chip Enable
GPIO5   <->  CSN                     Chip Select Not
GPIO18  <->  SCK                     SPI Clock
GPIO19  <->  MISO                    Master In Slave Out
GPIO23  <->  MOSI                    Master Out Slave In
```

### L298N Motor Driver #1 (Front Motors)

#### Power
```
L298N Pin       Connection
---------       ----------
VMS        <->  6V (from LM2596)
GND        <->  Common GND
5V         <->  Leave disconnected
ENA        <->  Jumper ON
ENB        <->  Jumper ON
```

#### Control (Front Left Motor)
```
L298N Pin       ESP32
---------       -----
IN1        <->  GPIO33
IN2        <->  GPIO15
OUT1       <->  Motor + (Red)
OUT2       <->  Motor - (Black)
```

#### Control (Front Right Motor)
```
L298N Pin       ESP32
---------       -----
IN3        <->  GPIO27
IN4        <->  GPIO14
OUT3       <->  Motor + (Red)
OUT4       <->  Motor - (Black)
```

### L298N Motor Driver #2 (Rear Motors)

#### Power
```
L298N Pin       Connection
---------       ----------
VMS        <->  6V (from LM2596)
GND        <->  Common GND
5V         <->  Leave disconnected
ENA        <->  Jumper ON
ENB        <->  Jumper ON
```

#### Control (Rear Left Motor)
```
L298N Pin       ESP32
---------       -----
IN1        <->  GPIO13
IN2        <->  GPIO12
OUT1       <->  Motor + (Red)
OUT2       <->  Motor - (Black)
```

#### Control (Rear Right Motor)
```
L298N Pin       ESP32
---------       -----
IN3        <->  GPIO32
IN4        <->  GPIO26
OUT3       <->  Motor + (Red)
OUT4       <->  Motor - (Black)
```

### Servos (Robotic Arm)

#### Servo 1 - Base Rotation
```
Servo Wire      Connection
----------      ----------
Brown/Black <-> GND
Red         <-> 6V (from LM2596)
Orange      <-> GPIO2
```

#### Servo 2 - Shoulder
```
Servo Wire      Connection
----------      ----------
Brown/Black <-> GND
Red         <-> 6V (from LM2596)
Orange      <-> GPIO16
```

#### Servo 3 - Elbow
```
Servo Wire      Connection
----------      ----------
Brown/Black <-> GND
Red         <-> 6V (from LM2596)
Orange      <-> GPIO21
```

#### Servo 4 - Wrist Rotation
```
Servo Wire      Connection
----------      ----------
Brown/Black <-> GND
Red         <-> 6V (from LM2596)
Orange      <-> GPIO22
```

#### Servo 5 - Gripper
```
Servo Wire      Connection
----------      ----------
Brown/Black <-> GND
Red         <-> 6V (from LM2596)
Orange      <-> GPIO17
```

---

## Pin Assignment Summary

### Transmitter (Arduino Pro Mini)
| Pin  | Function              | Type   |
|------|-----------------------|--------|
| A0   | Left Joystick X       | Analog |
| A1   | Left Joystick Y       | Analog |
| A2   | Right Joystick X      | Analog |
| A3   | Right Joystick Y      | Analog |
| A6   | Potentiometer 1       | Analog |
| A7   | Potentiometer 2       | Analog |
| 2    | Button Rotate CW      | Digital (INPUT_PULLUP) |
| 3    | Button Rotate CCW     | Digital (INPUT_PULLUP) |
| 4    | Switch Gripper Close  | Digital (INPUT_PULLUP) |
| 5    | Switch Gripper Open   | Digital (INPUT_PULLUP) |
| 6    | Switch E-Stop         | Digital (INPUT_PULLUP) |
| 9    | NRF24 CE              | Digital |
| 10   | NRF24 CSN             | Digital |
| 11   | NRF24 MOSI            | SPI |
| 12   | NRF24 MISO            | SPI |
| 13   | NRF24 SCK             | SPI |

### Receiver (ESP32-WROOM-32)
| GPIO | Function              | Type   |
|------|-----------------------|--------|
| 2    | Servo Base            | PWM Servo |
| 4    | NRF24 CE              | Digital |
| 5    | NRF24 CSN             | Digital |
| 12   | Motor RL IN2          | PWM Motor |
| 13   | Motor RL IN1          | PWM Motor |
| 14   | Motor FR IN2          | PWM Motor |
| 15   | Motor FL IN2          | PWM Motor |
| 16   | Servo Shoulder        | PWM Servo |
| 17   | Servo Gripper         | PWM Servo |
| 18   | NRF24 SCK             | SPI |
| 19   | NRF24 MISO            | SPI |
| 21   | Servo Elbow           | PWM Servo |
| 22   | Servo Wrist           | PWM Servo |
| 23   | NRF24 MOSI            | SPI |
| 26   | Motor RR IN2          | PWM Motor |
| 27   | Motor FR IN1          | PWM Motor |
| 32   | Motor RR IN1          | PWM Motor |
| 33   | Motor FL IN1          | PWM Motor |

**AVOIDED:** GPIO0, GPIO1 (boot/serial conflicts)

---

## PWM Channel Assignments (ESP32 Receiver)

The ESP32 uses LEDC PWM channels for motor control:

| Channel | Motor Pin    | GPIO |
|---------|--------------|------|
| 0       | FL IN1       | 33   |
| 1       | FL IN2       | 15   |
| 2       | FR IN1       | 27   |
| 3       | FR IN2       | 14   |
| 4       | RL IN1       | 13   |
| 5       | RL IN2       | 12   |
| 6       | RR IN1       | 32   |
| 7       | RR IN2       | 26   |

Note: Servos use separate PWM mechanism via ESP32Servo library.

---

## Common Wiring Mistakes to Avoid

### Transmitter
❌ **DON'T:** Connect 5V directly to NRF24 VCC  
✅ **DO:** Use adapter with 3.3V regulator

❌ **DON'T:** Forget pull-up resistors on buttons  
✅ **DO:** Not needed - code uses INPUT_PULLUP

❌ **DON'T:** Mix up joystick X and Y axes  
✅ **DO:** Test each axis separately, swap in code if needed

### Receiver
❌ **DON'T:** Use GPIO0 or GPIO1  
✅ **DO:** These are used for boot/serial - avoid them

❌ **DON'T:** Connect motor power to ESP32 VIN  
✅ **DO:** Keep motor power separate, use LM2596 to regulate

❌ **DON'T:** Forget common ground  
✅ **DO:** All grounds MUST connect together

❌ **DON'T:** Remove L298N ENA/ENB jumpers  
✅ **DO:** Keep jumpers ON for full-speed operation

❌ **DON'T:** Power servos from ESP32 pins  
✅ **DO:** Use external 6V supply from LM2596

---

## Testing Checklist

### Power-On Test (No movement)
- [ ] Transmitter: LED on Arduino Pro Mini lights up
- [ ] Transmitter: NRF24 module powered (check with multimeter: 3.3V)
- [ ] Receiver: ESP32 powered, boot successful
- [ ] Receiver: NRF24 module powered (check with multimeter: 3.3V)
- [ ] All grounds connected together

### Communication Test
- [ ] Open Serial Monitor on transmitter (115200 baud)
- [ ] See "TX: OK" messages
- [ ] Open Serial Monitor on receiver (115200 baud)
- [ ] See "RX OK" messages
- [ ] Move joystick - see values change in transmitter log

### Motor Test
- [ ] E-Stop OFF (switch not active)
- [ ] Move left joystick forward - motors spin
- [ ] Move left joystick backward - motors reverse
- [ ] Move left joystick left - robot turns left
- [ ] Move left joystick right - robot turns right

### Servo Test
- [ ] Move right joystick - base and shoulder servos move
- [ ] Turn potentiometers - elbow and wrist servos move
- [ ] Press buttons - wrist fine-tunes
- [ ] Activate switches - gripper opens/closes

### Emergency Stop Test
- [ ] Activate E-Stop switch
- [ ] All motors stop immediately
- [ ] Serial shows "EMERGENCY STOP ACTIVATED"
- [ ] Deactivate E-Stop
- [ ] Normal operation resumes

---

## Voltage Reference

| Component        | Voltage | Current (typ) | Notes |
|-----------------|---------|---------------|-------|
| Arduino Pro Mini| 5V      | 50mA          | Via L7805 |
| ESP32           | 5V      | 200mA         | From USB bank |
| NRF24L01        | 3.3V    | 10-15mA       | Via adapter reg |
| DC Motors (each)| 6V      | 200-500mA     | Via LM2596 |
| Servos (each)   | 6V      | 100-500mA     | Via LM2596 |
| Total (Motors)  | 6V      | 2-4A          | 4 motors peak |
| Total (Servos)  | 6V      | 1-2A          | 5 servos peak |

**Recommended Power Supply:**
- 7.4V 2S LiPo, 2000mAh minimum
- For heavy load: 3000-5000mAh
- Check battery voltage regularly (alarm at 7.0V)

---

## Additional Notes

### Wire Gauge Recommendations
- **Power (battery to regulators):** 18 AWG
- **Motor power:** 20 AWG
- **Servo power rail:** 20 AWG
- **Signal wires:** 22-24 AWG
- **Grounds:** 18-20 AWG (carry high current)

### Capacitor Recommendations
- **NRF24 modules:** 10-100µF electrolytic + 0.1µF ceramic
- **Motor drivers:** 470µF electrolytic on VMS
- **Servo power rail:** 1000µF electrolytic
- **All near the component they protect**

### Connector Types
- **Battery:** XT60 or Deans
- **Motors:** JST or solder direct
- **Servos:** Standard 3-pin servo connector
- **Between boards:** DuPont connectors or JST-XH

This wiring reference should be printed and kept handy during assembly!
