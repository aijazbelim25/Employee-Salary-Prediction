# Quick Start Guide - ESP32 Mecanum Robot

## 5-Minute Setup

### 1. Install Arduino IDE & Libraries
```
1. Download Arduino IDE from https://www.arduino.cc/en/software
2. Install ESP32 board support (see LIBRARIES.md)
3. Install required libraries:
   - RF24 (via Library Manager)
   - AsyncTCP (manual install)
   - ESPAsyncWebServer (manual install)
```

### 2. Upload Sketch
```
1. Connect ESP32 via USB
2. Open ESP32_Mecanum_Robot.ino
3. Select Tools → Board → ESP32 Dev Module
4. Select correct COM port
5. Click Upload
```

### 3. Wire Hardware
```
Follow WIRING.md for complete wiring diagram
Key connections:
- NRF24L01 → ESP32 (CE=4, CSN=5, use 3.3V!)
- Motors → L298N drivers
- L298N IN/EN pins → ESP32 GPIOs (see pin table)
```

### 4. Test
```
1. Open Serial Monitor (115200 baud)
2. Look for "Ready!" message
3. Connect phone to "MyWiFiCar" Wi-Fi (password: 12345678)
4. Navigate to http://192.168.4.1
5. Test controls
```

## Control Summary

### NRF24L01 Control (Primary)
- Transmit RadioPacket{joyX, joyY, pot} to address "00001"
- Values: 0-1023 (512 = center)
- joyX: strafe, joyY: forward/back, pot: rotation
- Overrides Wi-Fi control when active

### Wi-Fi Control (Secondary)
- Connect to "MyWiFiCar" network
- Open http://192.168.4.1 in browser
- Use on-screen buttons
- Active only when NRF signal lost (>3 seconds)

## Button Layout

```
[DiagFL] [Forward] [DiagFR]
[Left]   [STOP]    [Right]
[DiagBL] [Back]    [DiagBR]
[Rotate Left] [Rotate Right]
```

## Common Issues

| Problem | Solution |
|---------|----------|
| NRF not detected | Check 3.3V power, add capacitor |
| Motors don't run | Remove L298N ENA/ENB jumpers |
| Can't connect Wi-Fi | Check SSID/password, restart ESP32 |
| Web page won't load | Verify IP in Serial Monitor |
| Motors run backwards | Swap motor wires or change code |

## Pin Quick Reference

| Component | ESP32 Pins |
|-----------|------------|
| NRF24L01 | CE=4, CSN=5, SCK=18, MISO=19, MOSI=23 |
| Front-Right Motor | IN1=16, IN2=17, ENA=13 |
| Back-Right Motor | IN1=32, IN2=33, ENB=22 |
| Front-Left Motor | IN1=27, IN2=26, ENA=14 |
| Back-Left Motor | IN1=12, IN2=15, ENB=2 |

## Serial Monitor Commands

Watch for:
- "NRF24L01 initialized successfully" - NRF working
- "AP IP address: 192.168.4.1" - Wi-Fi ready
- "Web server started" - Web interface ready
- "NRF: X=512 Y=512 Pot=512" - Receiving NRF data
- "NRF signal lost" - Timeout, switched to Wi-Fi
- "WiFi: Forward" etc. - Wi-Fi commands executing

## Troubleshooting Steps

1. **Check Serial Monitor** (115200 baud)
   - Look for error messages
   - Verify all components initialized

2. **Test Power**
   - ESP32 has stable 5V
   - Motors have 7-12V
   - All grounds connected

3. **Test Components Individually**
   - Upload simple NRF24L01 test sketch
   - Test motors with simple L298N sketch
   - Test Wi-Fi with basic AP example

4. **Verify Wiring**
   - Double-check all pin assignments
   - Look for loose connections
   - Ensure no short circuits

## Performance Tips

- Use quality power supply (2A+ for motors)
- Keep NRF24L01 wires short (<10cm)
- Add heat sinks to motor drivers if getting hot
- Use PWM frequency 5000Hz (already set)
- Adjust DEAD_BAND if joystick drift occurs

## Safety Checklist

- [ ] Common ground between ESP32 and motor drivers
- [ ] NRF24L01 connected to 3.3V (NOT 5V)
- [ ] Motor power supply adequate (7-12V, 2A+)
- [ ] No loose wires near moving parts
- [ ] Emergency stop method available
- [ ] Tested in safe area first

## Next Steps

1. Calibrate joystick center (adjust DEAD_BAND)
2. Tune MIN_SPEED for your motors
3. Adjust motor directions if needed
4. Add battery voltage monitoring
5. Implement custom movement patterns
6. Add sensors (ultrasonic, line following, etc.)

## Getting Help

- Check README.md for detailed information
- Review WIRING.md for connection diagrams
- See LIBRARIES.md for library installation
- Open issue on GitHub if problems persist

## Default Settings

```cpp
Wi-Fi SSID: "MyWiFiCar"
Password: "12345678"
Web Interface: http://192.168.4.1
Serial Baud: 115200
NRF Address: "00001"
PWM Frequency: 5000 Hz
Dead Band: 100
Min Speed: 100
NRF Timeout: 3000 ms (3 seconds)
```

---

**Ready to Roll!** 🤖

Power on, connect, and drive your mecanum robot!
