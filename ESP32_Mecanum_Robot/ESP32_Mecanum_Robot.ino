/*
 * ESP32 Mecanum Robot Controller
 * Hybrid Wi-Fi and NRF24L01 Receiver for 4-Wheel Drive Mecanum Robot
 * 
 * Hardware:
 * - ESP32 38-pin board
 * - 4x DC motors with 2x L298N drivers
 * - NRF24L01 module with 5V adapter
 */

#include <SPI.h>
#include <RF24.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <vector>

// ========== PIN DEFINITIONS ==========
// NRF24L01 Pins
#define NRF_CE_PIN   4
#define NRF_CSN_PIN  5
// SPI Pins (default ESP32 SPI pins)
// SCK: 18, MISO: 19, MOSI: 23

// Motor Control Pins - Front Right
#define FR_IN1  16
#define FR_IN2  17
#define FR_ENA  13
#define FR_PWM_CHANNEL  0

// Motor Control Pins - Back Right
#define BR_IN1  32
#define BR_IN2  33
#define BR_ENB  22
#define BR_PWM_CHANNEL  1

// Motor Control Pins - Front Left
#define FL_IN1  27
#define FL_IN2  26
#define FL_ENA  14
#define FL_PWM_CHANNEL  2

// Motor Control Pins - Back Left
#define BL_IN1  12
#define BL_IN2  15
#define BL_ENB  2
#define BL_PWM_CHANNEL  3

// ========== CONSTANTS ==========
#define PWM_FREQ      5000
#define PWM_RESOLUTION  8
#define DEAD_BAND     100
#define MIN_SPEED     100
#define NRF_TIMEOUT   3000  // 3 seconds

// Wi-Fi credentials
const char* ssid = "MyWiFiCar";
const char* password = "12345678";

// ========== GLOBAL OBJECTS ==========
RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// NRF24L01 Radio Address
const byte address[6] = "00001";

// Radio Packet Structure
struct RadioPacket {
  int joyX;  // Strafe (0-1023)
  int joyY;  // Forward/Back (0-1023)
  int pot;   // Rotation (0-1023)
};

// ========== GLOBAL VARIABLES ==========
RadioPacket rxPacket;
bool nrfActive = false;
unsigned long lastNrfTime = 0;

// ========== HTML WEB PAGE ==========
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 Mecanum Robot Control</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      margin: 0;
      padding: 20px;
      background-color: #f0f0f0;
    }
    h1 {
      color: #333;
    }
    .status {
      margin: 20px 0;
      padding: 10px;
      background-color: #fff;
      border-radius: 5px;
    }
    .control-grid {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      gap: 10px;
      max-width: 400px;
      margin: 20px auto;
    }
    .control-btn {
      padding: 30px;
      font-size: 18px;
      font-weight: bold;
      border: none;
      border-radius: 8px;
      cursor: pointer;
      color: white;
      background-color: #4CAF50;
      transition: all 0.3s;
    }
    .control-btn:active {
      transform: scale(0.95);
      background-color: #45a049;
    }
    .stop-btn {
      background-color: #f44336;
    }
    .stop-btn:active {
      background-color: #da190b;
    }
  </style>
</head>
<body>
  <h1>ESP32 Mecanum Robot</h1>
  <div class="status">
    <p>Status: <span id="status">Disconnected</span></p>
  </div>
  
  <div class="control-grid">
    <button class="control-btn" onmousedown="sendCmd(7)" onmouseup="sendCmd(0)">↖<br>DiagFL</button>
    <button class="control-btn" onmousedown="sendCmd(1)" onmouseup="sendCmd(0)">↑<br>Forward</button>
    <button class="control-btn" onmousedown="sendCmd(8)" onmouseup="sendCmd(0)">↗<br>DiagFR</button>
    
    <button class="control-btn" onmousedown="sendCmd(3)" onmouseup="sendCmd(0)">←<br>Left</button>
    <button class="control-btn stop-btn" onmousedown="sendCmd(0)">STOP</button>
    <button class="control-btn" onmousedown="sendCmd(4)" onmouseup="sendCmd(0)">→<br>Right</button>
    
    <button class="control-btn" onmousedown="sendCmd(9)" onmouseup="sendCmd(0)">↙<br>DiagBL</button>
    <button class="control-btn" onmousedown="sendCmd(2)" onmouseup="sendCmd(0)">↓<br>Backward</button>
    <button class="control-btn" onmousedown="sendCmd(10)" onmouseup="sendCmd(0)">↘<br>DiagBR</button>
  </div>
  
  <div class="control-grid" style="grid-template-columns: repeat(2, 1fr);">
    <button class="control-btn" onmousedown="sendCmd(5)" onmouseup="sendCmd(0)">↺<br>Rot Left</button>
    <button class="control-btn" onmousedown="sendCmd(6)" onmouseup="sendCmd(0)">↻<br>Rot Right</button>
  </div>

  <script>
    var gateway = `ws://${window.location.hostname}/ws`;
    var websocket;
    
    function initWebSocket() {
      console.log('Trying to open a WebSocket connection...');
      websocket = new WebSocket(gateway);
      websocket.onopen = onOpen;
      websocket.onclose = onClose;
      websocket.onmessage = onMessage;
    }
    
    function onOpen(event) {
      console.log('Connection opened');
      document.getElementById('status').innerHTML = 'Connected';
      document.getElementById('status').style.color = 'green';
    }
    
    function onClose(event) {
      console.log('Connection closed');
      document.getElementById('status').innerHTML = 'Disconnected';
      document.getElementById('status').style.color = 'red';
      setTimeout(initWebSocket, 2000);
    }
    
    function onMessage(event) {
      console.log(event.data);
    }
    
    function sendCmd(cmd) {
      if (websocket.readyState === WebSocket.OPEN) {
        websocket.send(String(cmd));
        console.log('Sent command: ' + cmd);
      }
    }
    
    window.addEventListener('load', initWebSocket);
  </script>
</body>
</html>
)rawliteral";

// ========== MOTOR CONTROL FUNCTIONS ==========

void setupMotorPins() {
  // Front Right Motor
  pinMode(FR_IN1, OUTPUT);
  pinMode(FR_IN2, OUTPUT);
  ledcSetup(FR_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(FR_ENA, FR_PWM_CHANNEL);
  
  // Back Right Motor
  pinMode(BR_IN1, OUTPUT);
  pinMode(BR_IN2, OUTPUT);
  ledcSetup(BR_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(BR_ENB, BR_PWM_CHANNEL);
  
  // Front Left Motor
  pinMode(FL_IN1, OUTPUT);
  pinMode(FL_IN2, OUTPUT);
  ledcSetup(FL_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(FL_ENA, FL_PWM_CHANNEL);
  
  // Back Left Motor
  pinMode(BL_IN1, OUTPUT);
  pinMode(BL_IN2, OUTPUT);
  ledcSetup(BL_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(BL_ENB, BL_PWM_CHANNEL);
  
  // Initialize all motors to stop
  stopAllMotors();
}

void setMotor(int in1Pin, int in2Pin, int pwmChannel, int speed) {
  if (speed > 0) {
    digitalWrite(in1Pin, HIGH);
    digitalWrite(in2Pin, LOW);
    ledcWrite(pwmChannel, abs(speed));
  } else if (speed < 0) {
    digitalWrite(in1Pin, LOW);
    digitalWrite(in2Pin, HIGH);
    ledcWrite(pwmChannel, abs(speed));
  } else {
    digitalWrite(in1Pin, LOW);
    digitalWrite(in2Pin, LOW);
    ledcWrite(pwmChannel, 0);
  }
}

void setMotorSpeeds(int frSpeed, int brSpeed, int flSpeed, int blSpeed) {
  setMotor(FR_IN1, FR_IN2, FR_PWM_CHANNEL, frSpeed);
  setMotor(BR_IN1, BR_IN2, BR_PWM_CHANNEL, brSpeed);
  setMotor(FL_IN1, FL_IN2, FL_PWM_CHANNEL, flSpeed);
  setMotor(BL_IN1, BL_IN2, BL_PWM_CHANNEL, blSpeed);
}

void stopAllMotors() {
  setMotorSpeeds(0, 0, 0, 0);
}

// Soft start function - gradually ramp up motor speeds
void rampUp(int targetFR, int targetBR, int targetFL, int targetBL) {
  const int steps = 10;
  const int delayMs = 20;
  
  for (int i = 1; i <= steps; i++) {
    int fr = (targetFR * i) / steps;
    int br = (targetBR * i) / steps;
    int fl = (targetFL * i) / steps;
    int bl = (targetBL * i) / steps;
    setMotorSpeeds(fr, br, fl, bl);
    delay(delayMs);
  }
}

// Soft stop function - gradually ramp down motor speeds
void rampDown() {
  // Get current speeds (simplified - assumes motors are at full speed)
  const int steps = 10;
  const int delayMs = 20;
  
  for (int i = steps - 1; i >= 0; i--) {
    // Gradually reduce to zero
    // Note: In a real implementation, you'd track current speeds
    // For simplicity, we'll just apply decreasing PWM values
    int reducedSpeed = (200 * i) / steps;  // Assuming 200 as typical speed
    ledcWrite(FR_PWM_CHANNEL, reducedSpeed);
    ledcWrite(BR_PWM_CHANNEL, reducedSpeed);
    ledcWrite(FL_PWM_CHANNEL, reducedSpeed);
    ledcWrite(BL_PWM_CHANNEL, reducedSpeed);
    delay(delayMs);
  }
  
  stopAllMotors();
}

// ========== MECANUM DRIVE LOGIC ==========

int mapJoystickValue(int value, int center = 512) {
  int offset = value - center;
  
  // Apply dead band
  if (abs(offset) < DEAD_BAND) {
    return 0;
  }
  
  // Map to -255 to 255 range
  int mapped = map(abs(offset), DEAD_BAND, 512, MIN_SPEED, 255);
  
  // Constrain and apply sign
  mapped = constrain(mapped, MIN_SPEED, 255);
  return (offset > 0) ? mapped : -mapped;
}

void processNrfPacket(RadioPacket &packet) {
  // Map joystick values to motor control values
  int strafeX = mapJoystickValue(packet.joyX, 512);  // Strafe left/right
  int forwardY = mapJoystickValue(packet.joyY, 512); // Forward/back
  int rotation = mapJoystickValue(packet.pot, 512);  // Rotation
  
  // Mecanum drive mixing logic
  // FR: forward + strafe - rotation
  // BR: forward - strafe - rotation
  // FL: forward - strafe + rotation
  // BL: forward + strafe + rotation
  
  int frSpeed = forwardY + strafeX - rotation;
  int brSpeed = forwardY - strafeX - rotation;
  int flSpeed = forwardY - strafeX + rotation;
  int blSpeed = forwardY + strafeX + rotation;
  
  // Constrain to PWM range
  frSpeed = constrain(frSpeed, -255, 255);
  brSpeed = constrain(brSpeed, -255, 255);
  flSpeed = constrain(flSpeed, -255, 255);
  blSpeed = constrain(blSpeed, -255, 255);
  
  // Apply to motors
  setMotorSpeeds(frSpeed, brSpeed, flSpeed, blSpeed);
}

// ========== WI-FI CONTROL FUNCTIONS ==========

void processWiFiCommand(int command) {
  // Check if NRF is active - if so, ignore Wi-Fi commands
  if (nrfActive) {
    Serial.println("NRF active - ignoring Wi-Fi command");
    return;
  }
  
  const int speed = 200;  // Standard speed for Wi-Fi control
  
  switch(command) {
    case 0:  // Stop
      Serial.println("WiFi: Stop");
      rampDown();
      break;
      
    case 1:  // Forward
      Serial.println("WiFi: Forward");
      rampUp(speed, speed, speed, speed);
      break;
      
    case 2:  // Backward
      Serial.println("WiFi: Backward");
      rampUp(-speed, -speed, -speed, -speed);
      break;
      
    case 3:  // Strafe Left
      Serial.println("WiFi: Strafe Left");
      rampUp(-speed, speed, speed, -speed);
      break;
      
    case 4:  // Strafe Right
      Serial.println("WiFi: Strafe Right");
      rampUp(speed, -speed, -speed, speed);
      break;
      
    case 5:  // Rotate Left
      Serial.println("WiFi: Rotate Left");
      rampUp(-speed, -speed, speed, speed);
      break;
      
    case 6:  // Rotate Right
      Serial.println("WiFi: Rotate Right");
      rampUp(speed, speed, -speed, -speed);
      break;
      
    case 7:  // Diagonal Forward-Left
      Serial.println("WiFi: Diagonal Forward-Left");
      rampUp(0, speed, speed, 0);
      break;
      
    case 8:  // Diagonal Forward-Right
      Serial.println("WiFi: Diagonal Forward-Right");
      rampUp(speed, 0, 0, speed);
      break;
      
    case 9:  // Diagonal Backward-Left
      Serial.println("WiFi: Diagonal Backward-Left");
      rampUp(-speed, 0, 0, -speed);
      break;
      
    case 10:  // Diagonal Backward-Right
      Serial.println("WiFi: Diagonal Backward-Right");
      rampUp(0, -speed, -speed, 0);
      break;
      
    default:
      Serial.println("WiFi: Unknown command");
      break;
  }
}

// ========== WEBSOCKET HANDLERS ==========

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("WebSocket client #%u connected from %s\n", 
                  client->id(), client->remoteIP().toString().c_str());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      data[len] = 0;  // Null terminate
      String message = (char*)data;
      int command = message.toInt();
      
      Serial.printf("WebSocket command received: %d\n", command);
      processWiFiCommand(command);
    }
  }
}

// ========== SETUP FUNCTION ==========

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nESP32 Mecanum Robot Controller Starting...");
  
  // Setup motor control
  Serial.println("Initializing motor pins and PWM...");
  setupMotorPins();
  
  // Setup NRF24L01
  Serial.println("Initializing NRF24L01...");
  if (!radio.begin()) {
    Serial.println("ERROR: NRF24L01 initialization failed!");
  } else {
    Serial.println("NRF24L01 initialized successfully");
    radio.openReadingPipe(0, address);
    radio.setPALevel(RF24_PA_LOW);
    radio.setDataRate(RF24_250KBPS);
    radio.startListening();
    Serial.println("NRF24L01 listening on address: 00001");
  }
  
  // Setup Wi-Fi Access Point
  Serial.println("Setting up Wi-Fi Access Point...");
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  // Setup WebSocket
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  
  // Setup web server routes
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });
  
  // Start server
  server.begin();
  Serial.println("Web server started");
  Serial.println("Ready! Connect to Wi-Fi: MyWiFiCar / 12345678");
  Serial.println("========================================\n");
}

// ========== MAIN LOOP ==========

void loop() {
  // Check for NRF24L01 data
  if (radio.available()) {
    radio.read(&rxPacket, sizeof(RadioPacket));
    
    // Set NRF active flag and update timer
    nrfActive = true;
    lastNrfTime = millis();
    
    // Process the packet
    processNrfPacket(rxPacket);
    
    // Debug output
    Serial.printf("NRF: X=%d Y=%d Pot=%d\n", 
                  rxPacket.joyX, rxPacket.joyY, rxPacket.pot);
  }
  
  // Check for NRF timeout
  if (nrfActive && (millis() - lastNrfTime > NRF_TIMEOUT)) {
    Serial.println("NRF signal lost - switching to Wi-Fi control");
    nrfActive = false;
    stopAllMotors();
  }
  
  // Clean up WebSocket clients
  ws.cleanupClients();
  
  // Small delay to prevent watchdog issues
  delay(10);
}
