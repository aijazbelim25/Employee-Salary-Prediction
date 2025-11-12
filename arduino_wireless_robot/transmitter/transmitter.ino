/*
 * WIRELESS ROBOTICS TRANSMITTER
 * Arduino Pro Mini (5V, 16MHz) + NRF24L01
 * 
 * Controls: 2 Joysticks, 2 Potentiometers, 2 Buttons, 3 Toggle Switches
 * Transmits control data at 20Hz (50ms intervals) to receiver
 */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// ===== NRF24 CONFIGURATION =====
#define CE_PIN 9
#define CSN_PIN 10
RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

// ===== PIN DEFINITIONS =====
// Analog Inputs
#define LEFT_JOYSTICK_X   A0
#define LEFT_JOYSTICK_Y   A1
#define RIGHT_JOYSTICK_X  A2
#define RIGHT_JOYSTICK_Y  A3
#define POTENTIOMETER_1   A6
#define POTENTIOMETER_2   A7

// Digital Inputs (All use INPUT_PULLUP - Active LOW)
#define BUTTON_ROTATE_CW   2
#define BUTTON_ROTATE_CCW  3
#define SWITCH_GRIPPER_CLOSE  4
#define SWITCH_GRIPPER_OPEN   5
#define SWITCH_EMERGENCY_STOP 6

// ===== DATA STRUCTURE =====
struct ControlData {
  // Joystick values (0-1023)
  int leftJoyX;
  int leftJoyY;
  int rightJoyX;
  int rightJoyY;
  
  // Potentiometer values (0-1023)
  int pot1;
  int pot2;
  
  // Button states (0=pressed, 1=not pressed due to INPUT_PULLUP)
  bool buttonRotateCW;
  bool buttonRotateCCW;
  
  // Switch states (0=active, 1=inactive due to INPUT_PULLUP)
  bool switchGripperClose;
  bool switchGripperOpen;
  bool switchEmergencyStop;
};

ControlData controlData;

// ===== TIMING =====
unsigned long lastTransmit = 0;
const unsigned long TRANSMIT_INTERVAL = 50; // 50ms = 20Hz

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  Serial.println("===== WIRELESS ROBOTICS TRANSMITTER =====");
  Serial.println("Arduino Pro Mini + NRF24L01");
  
  // Configure analog input pins
  pinMode(LEFT_JOYSTICK_X, INPUT);
  pinMode(LEFT_JOYSTICK_Y, INPUT);
  pinMode(RIGHT_JOYSTICK_X, INPUT);
  pinMode(RIGHT_JOYSTICK_Y, INPUT);
  pinMode(POTENTIOMETER_1, INPUT);
  pinMode(POTENTIOMETER_2, INPUT);
  
  // Configure digital input pins with pullup resistors
  pinMode(BUTTON_ROTATE_CW, INPUT_PULLUP);
  pinMode(BUTTON_ROTATE_CCW, INPUT_PULLUP);
  pinMode(SWITCH_GRIPPER_CLOSE, INPUT_PULLUP);
  pinMode(SWITCH_GRIPPER_OPEN, INPUT_PULLUP);
  pinMode(SWITCH_EMERGENCY_STOP, INPUT_PULLUP);
  
  // Initialize NRF24L01
  if (!radio.begin()) {
    Serial.println("ERROR: NRF24L01 initialization failed!");
    while (1); // Halt if radio init fails
  }
  
  // Configure NRF24 settings
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);      // Low power for short range
  radio.setDataRate(RF24_250KBPS);    // 250kbps data rate
  radio.stopListening();               // Set as transmitter
  
  Serial.println("NRF24L01 initialized successfully");
  Serial.println("Transmitter ready!");
  Serial.println("----------------------------------------");
}

void loop() {
  // Check if it's time to transmit
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastTransmit >= TRANSMIT_INTERVAL) {
    lastTransmit = currentMillis;
    
    // Read all analog inputs
    controlData.leftJoyX = analogRead(LEFT_JOYSTICK_X);
    controlData.leftJoyY = analogRead(LEFT_JOYSTICK_Y);
    controlData.rightJoyX = analogRead(RIGHT_JOYSTICK_X);
    controlData.rightJoyY = analogRead(RIGHT_JOYSTICK_Y);
    controlData.pot1 = analogRead(POTENTIOMETER_1);
    controlData.pot2 = analogRead(POTENTIOMETER_2);
    
    // Read all digital inputs (Active LOW - 0=pressed/active)
    controlData.buttonRotateCW = digitalRead(BUTTON_ROTATE_CW);
    controlData.buttonRotateCCW = digitalRead(BUTTON_ROTATE_CCW);
    controlData.switchGripperClose = digitalRead(SWITCH_GRIPPER_CLOSE);
    controlData.switchGripperOpen = digitalRead(SWITCH_GRIPPER_OPEN);
    controlData.switchEmergencyStop = digitalRead(SWITCH_EMERGENCY_STOP);
    
    // Transmit data
    bool success = radio.write(&controlData, sizeof(controlData));
    
    // Debug output
    Serial.print("TX: ");
    Serial.print(success ? "OK " : "FAIL ");
    Serial.print("| LJ(");
    Serial.print(controlData.leftJoyX);
    Serial.print(",");
    Serial.print(controlData.leftJoyY);
    Serial.print(") RJ(");
    Serial.print(controlData.rightJoyX);
    Serial.print(",");
    Serial.print(controlData.rightJoyY);
    Serial.print(") P1:");
    Serial.print(controlData.pot1);
    Serial.print(" P2:");
    Serial.print(controlData.pot2);
    Serial.print(" | BTN:");
    Serial.print(!controlData.buttonRotateCW ? "CW " : "-- ");
    Serial.print(!controlData.buttonRotateCCW ? "CCW " : "--- ");
    Serial.print("| SW:");
    Serial.print(!controlData.switchGripperClose ? "CLOSE " : "----- ");
    Serial.print(!controlData.switchGripperOpen ? "OPEN " : "---- ");
    Serial.print(!controlData.switchEmergencyStop ? "E-STOP" : "------");
    Serial.println();
  }
}
