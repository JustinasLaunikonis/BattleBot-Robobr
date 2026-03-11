#include <Adafruit_NeoPixel.h>

// ===== NEOPIXELS =====
const int PIN_NEO = 12;
const int NUM_PIXELS = 4;

Adafruit_NeoPixel pixels(NUM_PIXELS, PIN_NEO, NEO_RGB + NEO_KHZ800);

// ===== SENSOR PINS =====
const int NUM_SENSORS = 8;
const int SENSOR_PINS[NUM_SENSORS] = {A0, A1, A2, A3, A4, A5, A6, A7};

// ===== MOTOR CONTROL PINS =====
const int MOTOR_LEFT_FORWARD = 11;
const int MOTOR_LEFT_BACK = 10;
const int MOTOR_RIGHT_BACK = 9;
const int MOTOR_RIGHT_FORWARD = 6;

// ===== THRESHOLDS AND SPEEDS =====
const int BLACK_THRESHOLD = 800;
const int SPEED_FULL = 255;           //default 255
const int SPEED_SLIGHT_CORRECT = 170; //default 170
const int SPEED_HARD_CORRECT = 60;    //default 60
const int SPEED_TANK_SHARP = 230;     //default 230
const int SPEED_SEARCH = 255;         //default 180
const int SPEED_STRAIGHT_LOST = 160;  //default 120

// -1 = last turned right
// +1 = last turned left
// 0 = initial state
int lastTurnDir = 0;

int sensorValues[NUM_SENSORS] = {0};

void setup() {
  pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
  pinMode(MOTOR_LEFT_BACK, OUTPUT);
  pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_BACK, OUTPUT);

  pixels.begin();
  pixels.setBrightness(50);

  pixels.show();
}

void loop() {
  int i;
  for (i = 0; i < NUM_SENSORS; i++) {
    sensorValues[i] = analogRead(SENSOR_PINS[i]);
  }

  if (sensorValues[3] > BLACK_THRESHOLD && sensorValues[4] > BLACK_THRESHOLD) {
    // Perfectly centered. Full speed forward
    driveForward(SPEED_FULL, SPEED_FULL);
    lastTurnDir = 0;

  } else if (sensorValues[3] > BLACK_THRESHOLD) {
    // Slight right. Both wheels moving, left faster
    driveForward(SPEED_FULL, SPEED_SLIGHT_CORRECT);
    lastTurnDir = -1;

  } else if (sensorValues[4] > BLACK_THRESHOLD) {
    // Slight left. Both wheels moving, right faster
    driveForward(SPEED_SLIGHT_CORRECT, SPEED_FULL);
    lastTurnDir = 1;
    

  } else if (sensorValues[2] > BLACK_THRESHOLD) {
    // Moderate right. Tighter differential
    driveForward(SPEED_FULL, SPEED_HARD_CORRECT);
    lastTurnDir = -1;

  } else if (sensorValues[5] > BLACK_THRESHOLD) {
    // Moderate left. Tighter differential
    driveForward(SPEED_HARD_CORRECT, SPEED_FULL);
    lastTurnDir = 1;

  } else if (sensorValues[1] > BLACK_THRESHOLD) {
    // Sharp right. Tank turn (left fwd, right rev)
    tankTurnRight(SPEED_TANK_SHARP);
    lastTurnDir = -1;

  } else if (sensorValues[6] > BLACK_THRESHOLD) {
    // Sharp left. Tank turn (right fwd, left rev)
    tankTurnLeft(SPEED_TANK_SHARP);
    lastTurnDir = 1;

  } else if (sensorValues[0] > BLACK_THRESHOLD) {
    // Very sharp right
    tankTurnRight(SPEED_FULL);
    lastTurnDir = -1;

  } else if (sensorValues[7] > BLACK_THRESHOLD) {
    // Very sharp left
    tankTurnLeft(SPEED_FULL);
    lastTurnDir = 1;
        
  } else {
    // Line completely lost continue turning in last known direction
    if (lastTurnDir < 0) {
      tankTurnRight(SPEED_SEARCH);
    } else if (lastTurnDir > 0) {
      tankTurnLeft(SPEED_SEARCH);
    } else {
      driveForward(SPEED_STRAIGHT_LOST, SPEED_STRAIGHT_LOST);
    }
  }

  // Update indicator lights based on turn direction
  if (lastTurnDir < 0) {
    rightSignalLights();
  } else if (lastTurnDir > 0) {
    leftSignalLights();
  } else {
    lightsOff();
  }
}

void driveForward(int leftSpeed, int rightSpeed) {
  analogWrite(MOTOR_LEFT_FORWARD, leftSpeed);
  analogWrite(MOTOR_LEFT_BACK, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, rightSpeed);
  analogWrite(MOTOR_RIGHT_BACK, 0);
}

void tankTurnRight(int speed) {
  analogWrite(MOTOR_LEFT_FORWARD, speed);
  analogWrite(MOTOR_LEFT_BACK, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, 0);
  analogWrite(MOTOR_RIGHT_BACK, speed);
}

void tankTurnLeft(int speed) {
  analogWrite(MOTOR_LEFT_FORWARD, 0);
  analogWrite(MOTOR_LEFT_BACK, speed);
  analogWrite(MOTOR_RIGHT_FORWARD, speed);
  analogWrite(MOTOR_RIGHT_BACK, 0);
}

void stopMotors() {
  analogWrite(MOTOR_LEFT_FORWARD, 0);
  analogWrite(MOTOR_LEFT_BACK, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, 0);
  analogWrite(MOTOR_RIGHT_BACK, 0);
}

void leftSignalLights() {
  pixels.clear();
  pixels.setPixelColor(0, pixels.Color(255, 172, 28));    // front-left
  pixels.setPixelColor(3, pixels.Color(255, 172, 28));    // back-left
  pixels.show();
}

void rightSignalLights() {
  pixels.clear();
  pixels.setPixelColor(1, pixels.Color(255, 172, 28));    // front-right
  pixels.setPixelColor(2, pixels.Color(255, 172, 28));    // back-right
  pixels.show();
}

void lightsOff() {
  pixels.clear();
  pixels.show();
}