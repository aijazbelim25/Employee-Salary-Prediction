# Troubleshooting Guide

Comprehensive troubleshooting for common issues.

## Table of Contents
1. [Communication Issues](#communication-issues)
2. [Motor Problems](#motor-problems)
3. [Servo Problems](#servo-problems)
4. [Power Issues](#power-issues)
5. [Control Issues](#control-issues)
6. [Emergency Stop Issues](#emergency-stop-issues)

---

## Communication Issues

### Issue: "TX: FAIL" on Transmitter

**Symptoms:**
- Serial Monitor shows "TX: FAIL" instead of "TX: OK"
- Receiver not receiving data

**Possible Causes & Solutions:**

1. **NRF24L01 not properly powered**
   - Check: Measure 3.3V on NRF24 VCC pin
   - Fix: Use adapter with voltage regulator
   - Fix: Add 10-100µF capacitor across VCC/GND

2. **Poor connections**
   - Check: All 8 NRF24 wires connected
   - Fix: Resolder or reconnect wires
   - Fix: Use short wires (<10cm for breadboard)

3. **Wrong pins in code**
   - Check: CE=9, CSN=10 for Pro Mini
   - Fix: Verify pin assignments match wiring

4. **NRF24 module defective**
   - Check: Try different NRF24 module
   - Check: Antenna properly attached

5. **Interference**
   - Fix: Move away from WiFi routers
   - Fix: Add metal shield around NRF24
   - Fix: Use shielded cables for power

### Issue: "Warning: No data received" on Receiver

**Symptoms:**
- Receiver Serial Monitor shows timeout warning
- No control response

**Possible Causes & Solutions:**

1. **Transmitter not transmitting**
   - Check: Transmitter Serial shows "TX: OK"
   - Fix: Resolve transmitter issues first

2. **Address mismatch**
   - Check: Both use address "00001"
   - Fix: Verify both sketches have identical address

3. **Out of range**
   - Check: Distance between TX and RX
   - Fix: Move closer (<5m for PA_LOW)
   - Fix: Change to PA_HIGH in both codes

4. **Receiver NRF24 not initialized**
   - Check: Boot message shows "NRF24L01 initialized successfully"
   - Fix: Check SPI pin configuration (18, 19, 23)
   - Fix: Ensure SPI.begin() called before radio.begin()

### Issue: Intermittent Communication

**Symptoms:**
- Works sometimes, fails other times
- "TX: OK" and "TX: FAIL" alternating

**Possible Causes & Solutions:**

1. **Weak power supply**
   - Check: Voltage drops when NRF24 transmits
   - Fix: Add large capacitor (100-470µF) near NRF24
   - Fix: Use dedicated 3.3V regulator

2. **Loose connections**
   - Check: Wiggle wires while monitoring Serial
   - Fix: Solder all connections
   - Fix: Use proper connectors

3. **Interference**
   - Check: Other 2.4GHz devices nearby
   - Fix: Change physical location
   - Fix: Add shielding

---

## Motor Problems

### Issue: Motors Don't Run

**Symptoms:**
- Joystick moves but motors don't respond
- No sound from motors

**Possible Causes & Solutions:**

1. **Emergency Stop Active**
   - Check: Serial shows "EMERGENCY STOP ACTIVATED"
   - Fix: Turn off E-Stop switch (Pin 6)
   - Check: Switch should read HIGH when inactive

2. **L298N not powered**
   - Check: 6V on VMS terminal
   - Fix: Verify LM2596 output voltage
   - Fix: Check battery charged (>7.0V)

3. **ENA/ENB jumpers missing**
   - Check: Jumpers on L298N ENA and ENB pins
   - Fix: Install jumpers (enables motors)

4. **Wrong GPIO pins**
   - Check: Pin assignments match code
   - Fix: Verify all 8 motor control pins

5. **Motor wiring**
   - Check: Motors connected to OUT1-OUT4
   - Fix: Check polarity (swap if reversed)

6. **Common ground missing**
   - Check: All grounds connected together
   - Fix: Connect ESP32 GND to L298N GND

### Issue: Motors Run Backward

**Symptoms:**
- Forward joystick makes robot go backward
- Left makes robot turn right

**Solutions:**

1. **Individual motor reversed**
   - Fix: Swap the two motor wires
   - OR: Swap IN1/IN2 pins in code for that motor

2. **Left/Right swapped**
   - Fix: Swap left and right motor channel assignments in code

### Issue: Motors Run at Full Speed Only

**Symptoms:**
- No speed control with joystick
- Motors are ON or OFF only

**Possible Causes & Solutions:**

1. **PWM not working**
   - Check: LEDC channels configured in setup()
   - Fix: Verify setupMotorPWM() is called
   - Fix: Check frequency is 5000Hz, resolution is 8-bit

2. **Joystick deadzone too large**
   - Check: JOYSTICK_DEADZONE value
   - Fix: Reduce from 100 to 50

### Issue: Motors Drift When Joystick Centered

**Symptoms:**
- Robot moves slowly when joystick released
- Serial shows small motor values (10-30)

**Solutions:**

1. **Increase deadzone**
   - Fix: Change JOYSTICK_DEADZONE to 150
   - Test: Check Serial Monitor for motor values near 0

2. **Joystick not centered**
   - Check: Joystick reads ~512 when centered
   - Fix: Adjust in code or replace joystick

---

## Servo Problems

### Issue: Servos Jitter/Twitch

**Symptoms:**
- Servos vibrate when stationary
- Erratic servo movement

**Possible Causes & Solutions:**

1. **Insufficient power**
   - Check: Voltage on servo power rail
   - Fix: Use separate power supply for servos
   - Fix: Increase current capacity (5A+ recommended)

2. **Power supply noise**
   - Fix: Add large capacitor (1000µF) on servo power rail
   - Fix: Separate motor and servo power supplies
   - Fix: Use low-ESR capacitors

3. **PWM conflict**
   - Check: Servos not on GPIO0, GPIO1
   - Fix: Verify ESP32Servo library version
   - Fix: Ensure LEDC channels don't conflict

4. **Signal noise**
   - Fix: Use shielded cable for servo signals
   - Fix: Keep signal wires away from motor wires
   - Fix: Add 0.1µF capacitor near servo connector

### Issue: Servos Don't Move

**Symptoms:**
- Servo attached but no movement
- No servo sound (should buzz slightly)

**Possible Causes & Solutions:**

1. **Not powered**
   - Check: 6V on servo red wire
   - Fix: Verify LM2596 connected to servo power rail

2. **Signal not connected**
   - Check: Orange/yellow wire to correct GPIO
   - Fix: Verify pin assignments

3. **ESP32Servo not initialized**
   - Check: Boot message confirms servos attached
   - Fix: Ensure ESP32Servo library installed

4. **Servo defective**
   - Test: Connect to known working system
   - Fix: Replace servo

### Issue: Servo Moves Opposite Direction

**Symptoms:**
- Increasing position moves servo backward
- Joystick up makes shoulder go down

**Solutions:**

1. **Invert control in code**
   ```cpp
   // For rate-based (joystick):
   basePos -= rate;  // Change += to -=
   
   // For incremental (pot):
   elbowPos -= movement;  // Change += to -=
   ```

### Issue: Servo Reaches Limit and Buzzes

**Symptoms:**
- Servo makes grinding noise
- Servo gets hot
- Mechanical binding

**Solutions:**

1. **Adjust limits in code**
   ```cpp
   // Reduce range to avoid mechanical limits
   basePos = constrain(basePos, 20, 160);  // Instead of 0-180
   ```

2. **Center servo mechanically**
   - Remove servo horn
   - Set servo to 90° in code
   - Reattach horn at neutral position

---

## Power Issues

### Issue: ESP32 Keeps Rebooting

**Symptoms:**
- ESP32 resets during operation
- "Brownout detector" messages
- Boot loop

**Possible Causes & Solutions:**

1. **Insufficient current**
   - Check: USB power bank current rating
   - Fix: Use 2A+ power bank
   - Fix: Don't power ESP32 from motor supply

2. **Voltage drop**
   - Check: Measure ESP32 5V pin under load
   - Fix: Thicker power wires
   - Fix: Add 470µF capacitor near ESP32

3. **Ground loop**
   - Check: All grounds connected together
   - Fix: Use star grounding topology

### Issue: Battery Drains Quickly

**Symptoms:**
- <10 minutes runtime
- Battery gets hot

**Possible Causes & Solutions:**

1. **Battery too small**
   - Check: mAh rating of battery
   - Fix: Use 2000mAh+ battery
   - Calculation: 4 motors × 300mA = 1200mA typical

2. **Motors stalling**
   - Check: Motors free to rotate
   - Fix: Reduce mechanical load
   - Fix: Use lower voltage (5V instead of 6V)

3. **Short circuit**
   - Check: Motor wires not touching
   - Fix: Insulate all connections
   - Check: Measure current draw (should be <3A idle)

### Issue: Voltage Regulator Gets Hot

**Symptoms:**
- L7805 or LM2596 very hot to touch
- Thermal shutdown

**Solutions:**

1. **Add heatsink**
   - Fix: Attach heatsink to regulator
   - Fix: Add cooling airflow

2. **Reduce input voltage**
   - Fix: Use 7.4V battery instead of higher voltage

3. **Reduce load**
   - Fix: Use separate regulators for different subsystems

---

## Control Issues

### Issue: Joystick Not Responding

**Symptoms:**
- Moving joystick doesn't change values
- Serial shows constant 0 or 1023

**Possible Causes & Solutions:**

1. **Not powered**
   - Check: 5V on joystick VCC pin
   - Fix: Verify power connections

2. **Wrong analog pin**
   - Check: VRx/VRy connected to correct pins
   - Fix: Swap X and Y if needed

3. **Deadzone too large**
   - Check: JOYSTICK_DEADZONE value
   - Fix: Reduce to 50 for testing

4. **Joystick defective**
   - Test: Measure voltage on VRx/VRy (should be 0-5V)
   - Fix: Replace joystick

### Issue: Potentiometer Not Working

**Symptoms:**
- Turning pot doesn't move servo
- Servo jumps randomly

**Possible Causes & Solutions:**

1. **Wiring wrong**
   - Check: Center pin to analog input
   - Check: Outer pins to GND and 5V
   - Fix: Correct wiring

2. **Deadzone too large**
   - Check: POT_DEADZONE value
   - Fix: Reduce to 2 for testing

3. **Sensitivity too low**
   - Check: POT_SENSITIVITY value
   - Fix: Increase to 0.3 for more movement

4. **First read issue**
   - Check: Move pot after startup
   - Fix: Turn pot significantly on first use

### Issue: Buttons Don't Work

**Symptoms:**
- Pressing button has no effect
- Serial shows button always pressed (0)

**Possible Causes & Solutions:**

1. **Wiring reversed**
   - Check: Button wired between pin and GND
   - Fix: Don't connect to VCC

2. **No INPUT_PULLUP**
   - Check: pinMode uses INPUT_PULLUP
   - Fix: Verify code configuration

3. **Button defective**
   - Test: Use multimeter in continuity mode
   - Fix: Replace button

4. **Logic inverted**
   - Remember: 0 = pressed, 1 = released (active LOW)

---

## Emergency Stop Issues

### Issue: E-Stop Won't Release

**Symptoms:**
- Serial continuously shows "EMERGENCY STOP ACTIVATED"
- Motors never run

**Possible Causes & Solutions:**

1. **Switch stuck**
   - Check: Manually toggle switch
   - Test: Measure pin 6 voltage (should be HIGH when off)

2. **Wiring issue**
   - Check: Switch between pin 6 and GND
   - Fix: Verify not stuck to GND

3. **Logic inverted**
   - Check: Code expects 1 (HIGH) for normal operation
   - Fix: Switch should be open (not pressed) normally

### Issue: E-Stop Doesn't Stop Motors

**Symptoms:**
- Activating E-Stop doesn't stop robot
- Motors keep running

**Possible Causes & Solutions:**

1. **Code not checking E-Stop**
   - Check: E-Stop check at start of loop
   - Fix: Verify return statement after stop command

2. **Communication lost**
   - Check: Data still being received
   - Fix: Implement timeout on receiver

3. **Switch not working**
   - Test: Serial Monitor shows switch state
   - Fix: Replace switch

---

## Serial Monitor Issues

### Issue: Garbage Characters on Serial Monitor

**Symptoms:**
- Unreadable characters
- Random symbols instead of text

**Solutions:**

1. **Wrong baud rate**
   - Fix: Set Serial Monitor to 115200 baud
   - Check: Both TX and RX use same baud rate

2. **Timing issue**
   - Fix: Add delay(1000) before Serial.begin()
   - Fix: Add while(!Serial); after Serial.begin()

### Issue: No Serial Output

**Symptoms:**
- Serial Monitor blank
- No text appears

**Possible Causes & Solutions:**

1. **Wrong COM port**
   - Check: Select correct port in Tools menu
   - Fix: Disconnect/reconnect USB

2. **Serial not initialized**
   - Check: Serial.begin(115200) in setup()
   - Fix: Upload code again

3. **USB cable issue**
   - Test: Try different USB cable (data capable)
   - Check: Cable not charge-only

---

## Advanced Troubleshooting

### Using Serial Monitor for Debugging

**Transmitter Debug Values:**
```
TX: OK | LJ(512,512) RJ(512,512) P1:512 P2:512
```
- LJ should change when moving left joystick (0-1023)
- RJ should change when moving right joystick (0-1023)
- P1/P2 should change when turning pots (0-1023)

**Receiver Debug Values:**
```
RX OK | Motors L:0 R:0 | Servos B:90 S:90 E:90 W:90 G:90
```
- Motors should show -255 to +255
- Servos should show 0 to 180
- Values should change smoothly

### Voltage Testing Points

**Transmitter:**
- Battery: 7.4V (or current battery voltage)
- L7805 Output: 5.0V (±0.1V)
- Pro Mini VCC: 5.0V
- NRF24 VCC: 3.3V
- Joystick VCC: 5.0V

**Receiver:**
- Battery: 7.4V
- LM2596 Output: 6.0V (±0.2V)
- L298N VMS: 6.0V
- Servo Power: 6.0V
- ESP32 5V Pin: 5.0V
- NRF24 VCC: 3.3V

### Current Draw Testing

**Measure at battery:**
- Idle (no motors): 200-500mA
- Motors running: 1-3A
- Peak (all motors + servos): 4-6A

If significantly higher, check for:
- Short circuits
- Mechanical binding
- Defective components

---

## Getting Additional Help

If problems persist after trying these solutions:

1. **Document the issue:**
   - Take photos of wiring
   - Copy Serial Monitor output
   - Note when problem occurs

2. **Check basics:**
   - All power connections secure
   - All grounds connected together
   - Code uploaded successfully
   - Correct board/port selected

3. **Isolate the problem:**
   - Test transmitter alone
   - Test receiver alone
   - Test subsystems separately

4. **Measure voltages:**
   - Use multimeter at key test points
   - Check under load

5. **Community support:**
   - Arduino forums
   - ESP32 forums
   - NRF24 community

Remember: Most issues are wiring or power-related. Double-check connections first!
