/*
 * WIRELESS ROBOTICS RECEIVER
 * ESP32-WROOM-32 (38-pin) + NRF24L01
 * 
 * Controls: 4x DC Motors (via 2x L298N), 5x Servos (robotic arm)
 * Receives control data from transmitter at 20Hz
 */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <ESP32Servo.h>

// ===== NRF24 CONFIGURATION =====
#define CE_PIN 4
#define CSN_PIN 5
// ESP32 SPI pins: SCK=18, MISO=19, MOSI=23
RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

// ===== MOTOR CONTROL PINS =====
// Front Left Motor
#define MOTOR_FL_IN1  33
#define MOTOR_FL_IN2  15
// Front Right Motor
#define MOTOR_FR_IN1  27
#define MOTOR_FR_IN2  14
// Rear Left Motor
#define MOTOR_RL_IN1  13
#define MOTOR_RL_IN2  12
// Rear Right Motor
#define MOTOR_RR_IN1  32
#define MOTOR_RR_IN2  26

// ===== SERVO CONTROL PINS =====
#define SERVO_BASE      2   // Base rotation
#define SERVO_SHOULDER  16  // Shoulder
#define SERVO_ELBOW     21  // Elbow
#define SERVO_WRIST     22  // Wrist rotation
#define SERVO_GRIPPER   17  // Gripper

// ===== SERVO OBJECTS =====
Servo servoBase;
Servo servoShoulder;
Servo servoElbow;
Servo servoWrist;
Servo servoGripper;

// ===== DATA STRUCTURE (must match transmitter) =====
struct ControlData {
  int leftJoyX;
  int leftJoyY;
  int rightJoyX;
  int rightJoyY;
  int pot1;
  int pot2;
  bool buttonRotateCW;
  bool buttonRotateCCW;
  bool switchGripperClose;
  bool switchGripperOpen;
  bool switchEmergencyStop;
};

ControlData controlData;

// ===== SERVO POSITION TRACKING =====
float basePos = 90.0;
float shoulderPos = 90.0;
float elbowPos = 90.0;
float wristPos = 90.0;
float gripperPos = 90.0;

// ===== POTENTIOMETER TRACKING (for incremental control) =====
int lastPot1 = 512;
int lastPot2 = 512;
bool firstRead = true;

// ===== TUNING PARAMETERS =====
const int JOYSTICK_DEADZONE = 100;
const int POT_DEADZONE = 5;
const float POT_SENSITIVITY = 0.15;
const float SERVO_RATE_SPEED = 0.8;
const float BUTTON_FINE_TUNE_STEP = 1.0;
const float GRIPPER_SWITCH_SPEED = 2.0;

// ===== MOTOR CONTROL FUNCTION =====
void setMotor(int in1Channel, int in2Channel, int speed) {
  // speed range: -255 to +255
  // positive = forward, negative = reverse, 0 = stop
  
  if (speed > 0) {
    // Forward
    ledcWrite(in1Channel, speed);
    ledcWrite(in2Channel, 0);
  } else if (speed < 0) {
    // Reverse
    ledcWrite(in1Channel, 0);
    ledcWrite(in2Channel, -speed);
  } else {
    // Stop
    ledcWrite(in1Channel, 0);
    ledcWrite(in2Channel, 0);
  }
}

// ===== SETUP PWM CHANNELS FOR MOTORS =====
void setupMotorPWM() {
  // Configure LEDC PWM channels for motor control
  // Using 8 channels (0-7) for 8 motor pins
  
  // Front Left Motor
  ledcSetup(0, 5000, 8); // Channel 0, 5kHz, 8-bit resolution
  ledcAttachPin(MOTOR_FL_IN1, 0);
  ledcSetup(1, 5000, 8);
  ledcAttachPin(MOTOR_FL_IN2, 1);
  
  // Front Right Motor
  ledcSetup(2, 5000, 8);
  ledcAttachPin(MOTOR_FR_IN1, 2);
  ledcSetup(3, 5000, 8);
  ledcAttachPin(MOTOR_FR_IN2, 3);
  
  // Rear Left Motor
  ledcSetup(4, 5000, 8);
  ledcAttachPin(MOTOR_RL_IN1, 4);
  ledcSetup(5, 5000, 8);
  ledcAttachPin(MOTOR_RL_IN2, 5);
  
  // Rear Right Motor
  ledcSetup(6, 5000, 8);
  ledcAttachPin(MOTOR_RR_IN1, 6);
  ledcSetup(7, 5000, 8);
  ledcAttachPin(MOTOR_RR_IN2, 7);
}

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  Serial.println("===== WIRELESS ROBOTICS RECEIVER =====");
  Serial.println("ESP32-WROOM-32 + NRF24L01");
  
  // Setup motor PWM channels
  setupMotorPWM();
  Serial.println("Motor PWM channels configured");
  
  // Initialize servos
  servoBase.attach(SERVO_BASE);
  servoShoulder.attach(SERVO_SHOULDER);
  servoElbow.attach(SERVO_ELBOW);
  servoWrist.attach(SERVO_WRIST);
  servoGripper.attach(SERVO_GRIPPER);
  
  // Set servos to initial positions
  servoBase.write((int)basePos);
  servoShoulder.write((int)shoulderPos);
  servoElbow.write((int)elbowPos);
  servoWrist.write((int)wristPos);
  servoGripper.write((int)gripperPos);
  
  Serial.println("Servos initialized to center positions (90°)");
  
  // Initialize NRF24L01 with custom SPI pins for ESP32
  SPI.begin(18, 19, 23); // SCK, MISO, MOSI
  
  if (!radio.begin()) {
    Serial.println("ERROR: NRF24L01 initialization failed!");
    while (1); // Halt if radio init fails
  }
  
  // Configure NRF24 settings
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.startListening(); // Set as receiver
  
  Serial.println("NRF24L01 initialized successfully");
  
  // Initialize controlData with safe default values
  // This prevents E-Stop from triggering before first data reception
  controlData.leftJoyX = 512;
  controlData.leftJoyY = 512;
  controlData.rightJoyX = 512;
  controlData.rightJoyY = 512;
  controlData.pot1 = 512;
  controlData.pot2 = 512;
  controlData.buttonRotateCW = true;       // Not pressed (INPUT_PULLUP = HIGH when released)
  controlData.buttonRotateCCW = true;      // Not pressed
  controlData.switchGripperClose = true;   // Not active
  controlData.switchGripperOpen = true;    // Not active
  controlData.switchEmergencyStop = true;  // Not active (CRITICAL - prevents false E-Stop)
  
  Serial.println("Control data initialized with safe defaults");
  Serial.println("Receiver ready - waiting for data...");
  Serial.println("----------------------------------------");
}

void loop() {
  // Check for incoming data
  if (radio.available()) {
    // Read the data
    radio.read(&controlData, sizeof(controlData));
    
    // ===== EMERGENCY STOP CHECK =====
    // Active LOW logic: switchEmergencyStop == false (0) means E-Stop switch is PRESSED/ACTIVE
    // Normal operation: switchEmergencyStop == true (1) when switch is not pressed
    if (!controlData.switchEmergencyStop) {
      // E-STOP ACTIVE - Stop all motors immediately
      setMotor(0, 1, 0);  // Front Left
      setMotor(2, 3, 0);  // Front Right
      setMotor(4, 5, 0);  // Rear Left
      setMotor(6, 7, 0);  // Rear Right
      
      Serial.println("*** EMERGENCY STOP ACTIVATED ***");
      return; // Exit loop immediately
    }
    
    // ===== DRIVE SYSTEM (Tank-style with Left Joystick) =====
    // Map joystick to -255 to +255 with deadzone
    int joyX = controlData.leftJoyX - 512; // -512 to +511
    int joyY = controlData.leftJoyY - 512; // -512 to +511
    
    // Apply deadzone
    if (abs(joyX) < JOYSTICK_DEADZONE) joyX = 0;
    if (abs(joyY) < JOYSTICK_DEADZONE) joyY = 0;
    
    // Scale to motor range (-255 to +255)
    int forward = map(joyY, -512, 511, -255, 255);
    int turn = map(joyX, -512, 511, -255, 255);
    
    // Tank-style mixing
    int leftSpeed = constrain(forward + turn, -255, 255);
    int rightSpeed = constrain(forward - turn, -255, 255);
    
    // Apply to motors
    setMotor(0, 1, leftSpeed);   // Front Left
    setMotor(4, 5, leftSpeed);   // Rear Left
    setMotor(2, 3, rightSpeed);  // Front Right
    setMotor(6, 7, rightSpeed);  // Rear Right
    
    // ===== ARM CONTROL - BASE & SHOULDER (Rate-based with Right Joystick) =====
    int rightJoyX = controlData.rightJoyX - 512;
    int rightJoyY = controlData.rightJoyY - 512;
    
    // Apply deadzone
    if (abs(rightJoyX) < JOYSTICK_DEADZONE) rightJoyX = 0;
    if (abs(rightJoyY) < JOYSTICK_DEADZONE) rightJoyY = 0;
    
    // Base servo (X-axis) - rate-based control
    if (rightJoyX != 0) {
      float rate = (rightJoyX / 512.0) * SERVO_RATE_SPEED;
      basePos += rate;
      basePos = constrain(basePos, 0, 180);
      servoBase.write((int)basePos);
    }
    
    // Shoulder servo (Y-axis) - rate-based control
    if (rightJoyY != 0) {
      float rate = (rightJoyY / 512.0) * SERVO_RATE_SPEED;
      shoulderPos += rate;
      shoulderPos = constrain(shoulderPos, 0, 180);
      servoShoulder.write((int)shoulderPos);
    }
    
    // ===== ARM CONTROL - ELBOW (Incremental with Pot 1) =====
    if (firstRead) {
      // On first read, just store the value without moving
      lastPot1 = controlData.pot1;
    } else {
      // Calculate change in potentiometer value
      int pot1Delta = controlData.pot1 - lastPot1;
      
      // Only move if change exceeds deadzone
      if (abs(pot1Delta) > POT_DEADZONE) {
        // Apply scaled movement
        float movement = pot1Delta * POT_SENSITIVITY;
        elbowPos += movement;
        elbowPos = constrain(elbowPos, 0, 180);
        servoElbow.write((int)elbowPos);
        
        // Update last value
        lastPot1 = controlData.pot1;
      }
    }
    
    // ===== ARM CONTROL - WRIST ROTATION (Incremental with Pot 2) =====
    if (firstRead) {
      lastPot2 = controlData.pot2;
    } else {
      int pot2Delta = controlData.pot2 - lastPot2;
      
      if (abs(pot2Delta) > POT_DEADZONE) {
        float movement = pot2Delta * POT_SENSITIVITY;
        wristPos += movement;
        wristPos = constrain(wristPos, 0, 180);
        servoWrist.write((int)wristPos);
        
        lastPot2 = controlData.pot2;
      }
    }
    
    // ===== WRIST FINE-TUNE (Buttons) =====
    // Active LOW: button == 0 means pressed
    if (!controlData.buttonRotateCW) {
      wristPos += BUTTON_FINE_TUNE_STEP;
      wristPos = constrain(wristPos, 0, 180);
      servoWrist.write((int)wristPos);
    }
    
    if (!controlData.buttonRotateCCW) {
      wristPos -= BUTTON_FINE_TUNE_STEP;
      wristPos = constrain(wristPos, 0, 180);
      servoWrist.write((int)wristPos);
    }
    
    // ===== GRIPPER CONTROL (Switches) =====
    // Active LOW: switch == 0 means active
    if (!controlData.switchGripperClose) {
      // Close gripper
      gripperPos += GRIPPER_SWITCH_SPEED;
      gripperPos = constrain(gripperPos, 0, 180);
      servoGripper.write((int)gripperPos);
    } else if (!controlData.switchGripperOpen) {
      // Open gripper
      gripperPos -= GRIPPER_SWITCH_SPEED;
      gripperPos = constrain(gripperPos, 0, 180);
      servoGripper.write((int)gripperPos);
    }
    
    // Clear first read flag
    if (firstRead) firstRead = false;
    
    // ===== DEBUG OUTPUT =====
    Serial.print("RX OK | Motors L:");
    Serial.print(leftSpeed);
    Serial.print(" R:");
    Serial.print(rightSpeed);
    Serial.print(" | Servos B:");
    Serial.print((int)basePos);
    Serial.print(" S:");
    Serial.print((int)shoulderPos);
    Serial.print(" E:");
    Serial.print((int)elbowPos);
    Serial.print(" W:");
    Serial.print((int)wristPos);
    Serial.print(" G:");
    Serial.print((int)gripperPos);
    Serial.println();
    
  } else {
    // No data received - could add timeout handling here
    static unsigned long lastDataTime = millis();
    static bool noDataWarning = false;
    
    if (millis() - lastDataTime > 1000 && !noDataWarning) {
      Serial.println("Warning: No data received for 1 second");
      noDataWarning = true;
    }
    
    if (radio.available()) {
      lastDataTime = millis();
      noDataWarning = false;
    }
  }
  
  delay(10); // Small delay for stability
}
