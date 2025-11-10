# Wiring Diagram - ESP32 Mecanum Robot

This document provides detailed wiring instructions for the ESP32 Mecanum Robot Controller.

## Component Overview

- 1x ESP32 Development Board (38-pin)
- 1x NRF24L01+ Wireless Module with 5V adapter
- 4x DC Motors (for mecanum wheels)
- 2x L298N Dual H-Bridge Motor Drivers
- 1x Power Supply for Motors (7-12V, 2A+ recommended)
- 1x Power Supply for ESP32 (5V via USB or VIN)
- Jumper wires
- Optional: 10µF capacitor for NRF24L01

## Power Distribution

### ESP32 Power
- **Option 1**: USB cable (5V from computer)
- **Option 2**: External 5V regulator → ESP32 5V pin
- **Option 3**: 7-12V battery → ESP32 VIN pin (uses onboard regulator)

### Motor Driver Power
- Connect motor power supply (7-12V) to both L298N 12V inputs
- Connect motor power GND to both L298N GND
- **Important**: Connect ESP32 GND to motor driver GND (common ground)

### NRF24L01 Power
- **VCC**: ESP32 3.3V (NRF24L01 is 3.3V device)
- **GND**: ESP32 GND
- If using 5V adapter module, connect adapter VCC to ESP32 5V
- **Tip**: Add 10µF capacitor between VCC and GND close to NRF24L01

## Detailed Pin Connections

### NRF24L01 Module Connections

```
NRF24L01 Pin    →    ESP32 Pin    (Function)
----------------------------------------
VCC             →    3.3V          (Power - 3.3V only!)
GND             →    GND           (Ground)
CE              →    GPIO 4        (Chip Enable)
CSN             →    GPIO 5        (Chip Select Not)
SCK             →    GPIO 18       (SPI Clock)
MOSI            →    GPIO 23       (Master Out Slave In)
MISO            →    GPIO 19       (Master In Slave Out)
IRQ             →    Not Connected (Optional interrupt)
```

**Notes**:
- Do NOT connect 5V to NRF24L01 VCC - it will damage the module
- If using NRF24L01 with PA+LNA (long range), use 5V adapter module
- Keep wires short (< 10cm) to reduce noise

### Motor Driver #1 (Right Side Motors)

**Front-Right Motor Connections**:
```
L298N Pin       →    ESP32 Pin    (Function)
----------------------------------------
IN1             →    GPIO 16       (Direction control)
IN2             →    GPIO 17       (Direction control)
ENA             →    GPIO 13       (Speed control via PWM)
OUT1            →    Motor FR +    (Motor terminal)
OUT2            →    Motor FR -    (Motor terminal)
```

**Back-Right Motor Connections**:
```
L298N Pin       →    ESP32 Pin    (Function)
----------------------------------------
IN3             →    GPIO 32       (Direction control)
IN4             →    GPIO 33       (Direction control)
ENB             →    GPIO 22       (Speed control via PWM)
OUT3            →    Motor BR +    (Motor terminal)
OUT4            →    Motor BR -    (Motor terminal)
```

### Motor Driver #2 (Left Side Motors)

**Front-Left Motor Connections**:
```
L298N Pin       →    ESP32 Pin    (Function)
----------------------------------------
IN1             →    GPIO 27       (Direction control)
IN2             →    GPIO 26       (Direction control)
ENA             →    GPIO 14       (Speed control via PWM)
OUT1            →    Motor FL +    (Motor terminal)
OUT2            →    Motor FL -    (Motor terminal)
```

**Back-Left Motor Connections**:
```
L298N Pin       →    ESP32 Pin    (Function)
----------------------------------------
IN3             →    GPIO 12       (Direction control)
IN4             →    GPIO 15       (Direction control)
ENB             →    GPIO 2        (Speed control via PWM)
OUT3            →    Motor BL +    (Motor terminal)
OUT4            →    Motor BL -    (Motor terminal)
```

### L298N Driver Setup

For each L298N driver:
1. Remove jumpers from ENA and ENB (allows PWM speed control)
2. Connect 12V power supply to 12V input
3. Connect GND to both ESP32 GND and power supply GND
4. 5V output pin can power ESP32 if using 7-12V input (optional)

## Connection Checklist

Before powering on:

- [ ] All GND connections are common (ESP32, drivers, power supplies)
- [ ] NRF24L01 connected to 3.3V (NOT 5V)
- [ ] All motor driver IN pins connected to correct ESP32 GPIOs
- [ ] All motor driver EN pins connected to correct ESP32 GPIOs
- [ ] Motor power supply connected to both L298N 12V inputs
- [ ] No short circuits between power and ground
- [ ] Capacitor added to NRF24L01 power pins (recommended)

## Pin Summary Table

| ESP32 GPIO | Function | Connected To |
|------------|----------|--------------|
| 2 | BL ENB (PWM Ch 3) | L298N #2 ENB |
| 4 | NRF CE | NRF24L01 CE |
| 5 | NRF CSN | NRF24L01 CSN |
| 12 | BL IN1 | L298N #2 IN3 |
| 13 | FR ENA (PWM Ch 0) | L298N #1 ENA |
| 14 | FL ENA (PWM Ch 2) | L298N #2 ENA |
| 15 | BL IN2 | L298N #2 IN4 |
| 16 | FR IN1 | L298N #1 IN1 |
| 17 | FR IN2 | L298N #1 IN2 |
| 18 | SPI SCK | NRF24L01 SCK |
| 19 | SPI MISO | NRF24L01 MISO |
| 22 | BR ENB (PWM Ch 1) | L298N #1 ENB |
| 23 | SPI MOSI | NRF24L01 MOSI |
| 26 | FL IN2 | L298N #2 IN2 |
| 27 | FL IN1 | L298N #2 IN1 |
| 32 | BR IN1 | L298N #1 IN3 |
| 33 | BR IN2 | L298N #1 IN4 |
| 3.3V | NRF Power | NRF24L01 VCC |
| GND | Common Ground | All GND pins |

## Motor Orientation

For proper mecanum drive operation:

```
Front of Robot
     ↑
    FL  FR     FR = Front Right
    ╱╲  ╱╲     FL = Front Left
   ╱  ╲╱  ╲    BR = Back Right
  │   ╱╲   │   BL = Back Left
  │  ╱  ╲  │
   ╲╱    ╲╱
    BL  BR
```

- Front-Right wheel: rollers point ╱ (45° forward-right)
- Front-Left wheel: rollers point ╲ (45° forward-left)
- Back-Right wheel: rollers point ╲ (45° backward-left)
- Back-Left wheel: rollers point ╱ (45° backward-right)

## Troubleshooting Wiring

### NRF24L01 Issues
- **Not detected**: Check VCC (must be 3.3V), GND, CE, CSN connections
- **Intermittent**: Add capacitor, shorten wires, check for loose connections
- **No communication**: Verify SPI pins (SCK=18, MISO=19, MOSI=23)

### Motor Issues
- **Not spinning**: Check power supply, verify L298N ENA/ENB jumpers removed
- **Wrong direction**: Swap motor wires or swap IN1/IN2 in code
- **Weak/slow**: Check power supply voltage and current capacity
- **Only some work**: Verify each motor's IN and EN pin connections

### Power Issues
- **ESP32 resets**: Motor draws too much current, use separate power supplies
- **NRF24L01 fails**: Add capacitor, ensure stable 3.3V supply
- **Brownouts**: Use soft start/stop functions (already implemented)

## Safety Recommendations

⚠️ **Before First Power-On**:
1. Double-check all connections against this guide
2. Verify no short circuits with multimeter
3. Test each motor individually first
4. Start with lower voltage (6-7V) for initial testing
5. Keep emergency power disconnect accessible

## Optional Additions

### LED Indicators
- Pin 25: Status LED (shows when NRF active)
- Pin 21: Wi-Fi LED (shows when Wi-Fi connected)

### Physical Emergency Stop
- Add a switch between motor power supply and L298N drivers
- Allows immediate motor shutdown without touching ESP32

### Voltage Monitoring
- Use ESP32 ADC pins to monitor battery voltage
- Add low voltage warning in code

## Additional Resources

- ESP32 Pinout: https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
- L298N Datasheet: http://www.st.com/content/ccc/resource/technical/document/datasheet/82/cc/3f/39/0a/29/4d/f0/CD00000240.pdf/files/CD00000240.pdf
- NRF24L01 Guide: https://lastminuteengineers.com/nrf24l01-arduino-wireless-communication/
