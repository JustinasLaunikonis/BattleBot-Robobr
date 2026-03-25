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

// ===== ULTRASONIC SENSOR =====
const int ULTRASONIC_TRIG = 7;
const int ULTRASONIC_ECHO = 8;
const int OBSTACLE_DISTANCE_CM = 12;

// ===== GRIPPER SERVO =====
const int SERVO_PIN = 4;
const int SERVO_CLOSED_PULSE = 1000;
const int SERVO_OPEN_PULSE = 1500;

// ===== THRESHOLDS AND SPEEDS =====
const int BLACK_THRESHOLD = 800;      // 1:00         // 0:54   // 0:52
const int SPEED_FULL = 255;           //default 255   // 255    // 255
const int SPEED_SLIGHT_CORRECT = 210; //default 170   // 210    // 210
const int SPEED_HARD_CORRECT = 120;   //default 60   // 60     // 120
const int SPEED_TANK_SHARP = 230;     //default 230   // 230    // 230
const int SPEED_SEARCH = 255;         //default 180   // 180    // 255
const int SPEED_STRAIGHT_LOST = 200;  //default 120   // 120    // 200
const unsigned long FINISH_BLACK_HOLD_MS = 50;
const unsigned long FINISH_FORWARD_MS = 500;
const unsigned long FINISH_BACKWARD_MS = 750;
const int FINISH_SEQUENCE_SPEED = SPEED_FULL;

// -1 = last turned right
// +1 = last turned left
// 0 = initial state
int lastTurnDir = 0;

int sensorValues[NUM_SENSORS] = {0};
unsigned long allBlackStartMs = 0;

void setup() {
  pinMode(SERVO_PIN, OUTPUT);

  pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
  pinMode(MOTOR_LEFT_BACK, OUTPUT);
  pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_BACK, OUTPUT);
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);
  
  pixels.begin();
  pixels.setBrightness(50);
  pixels.show();

  // Start with gripper closed.
  servoWrite(SERVO_CLOSED_PULSE);
}

void loop() {
  // Keep applying closed pulse so the gripper stays shut.
  servoWrite(SERVO_CLOSED_PULSE);

  for (int i = 0; i < NUM_SENSORS; i++) {
    sensorValues[i] = analogRead(SENSOR_PINS[i]);
  }

  // Finish condition: black square detected by all sensors.
  if (areAllSensorsBlack()) {
    if (allBlackStartMs == 0) {
      allBlackStartMs = millis();
    } else if (millis() - allBlackStartMs >= FINISH_BLACK_HOLD_MS) {
      finishRace();
    }
  } else {
    allBlackStartMs = 0;
  }

  if (isObstacleDetected()) {
    avoidObject();
    return;
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

void driveBackward(int leftSpeed, int rightSpeed) {
  analogWrite(MOTOR_LEFT_FORWARD, 0);
  analogWrite(MOTOR_LEFT_BACK, leftSpeed);
  analogWrite(MOTOR_RIGHT_FORWARD, 0);
  analogWrite(MOTOR_RIGHT_BACK, rightSpeed);
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

bool areAllSensorsBlack() {
  for (int i = 0; i < NUM_SENSORS; i++) {
    if (sensorValues[i] <= BLACK_THRESHOLD) {
      return false;
    }
  }

  return true;
}

void finishRace() {
  lightsOff();

  // Finishing sequence: forward, backward, then drop object.
  driveForward(FINISH_SEQUENCE_SPEED, FINISH_SEQUENCE_SPEED);
  delay(FINISH_FORWARD_MS);
  stopMotors();

  driveBackward(FINISH_SEQUENCE_SPEED, FINISH_SEQUENCE_SPEED);
  delay(FINISH_BACKWARD_MS);
  stopMotors();

  // Release gripper before final forward move.
  servoWrite(SERVO_OPEN_PULSE);

  driveBackward(FINISH_SEQUENCE_SPEED, FINISH_SEQUENCE_SPEED);
  delay(FINISH_BACKWARD_MS);
  stopMotors();


  // Hold final state forever: gripper open and motors stopped.
  while (true) {
    servoWrite(SERVO_OPEN_PULSE);
    stopMotors();
  }
}

void servoWrite(int pulseWidth) {
  unsigned long start = millis();

  while (millis() - start < 20) {
    digitalWrite(SERVO_PIN, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(SERVO_PIN, LOW);
    delayMicroseconds(20000 - pulseWidth);
  }
}

bool isObstacleDetected() {
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);

  long duration = pulseIn(ULTRASONIC_ECHO, HIGH, 25000UL);

  if (duration == 0) {
    return false;
  }

  long distanceCm = duration / 58;

  return (distanceCm > 0 && distanceCm <= OBSTACLE_DISTANCE_CM);
}

void avoidObject() {
  servoWrite(SERVO_CLOSED_PULSE);
  stopMotors();

  // Turn right ~90 degrees
  tankTurnRight(SPEED_TANK_SHARP);
  delay(350);
  stopMotors();

  // Move forward
  driveForward(SPEED_FULL, SPEED_FULL);
  delay(500);
  stopMotors();

  // Turn left ~90 degrees
  tankTurnLeft(SPEED_TANK_SHARP);
  delay(400);
  stopMotors();

  // Move forward more
  driveForward(SPEED_FULL, SPEED_FULL);
  delay(700);
  stopMotors();

  // Turn left ~90 degrees
  tankTurnLeft(SPEED_TANK_SHARP);
  delay(450);
  stopMotors();

  // Move forward until line is found, then stop and exit
  driveForward(SPEED_FULL, SPEED_FULL);
  while (true) {
    servoWrite(SERVO_CLOSED_PULSE);
    bool lineFound = false;

    for (int i = 0; i < NUM_SENSORS; i++) {
      if (analogRead(SENSOR_PINS[i]) > BLACK_THRESHOLD) {
        lineFound = true;
        break;
      }
    }

    if (lineFound) {
      stopMotors();
      return;
    }

    delay(5);
  }
}