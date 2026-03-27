#include <Adafruit_NeoPixel.h>

// ===== NEOPIXELS =====
const int PIN_NEO = 12;
const int NUM_PIXELS = 4;

Adafruit_NeoPixel pixels(NUM_PIXELS, PIN_NEO, NEO_RGB + NEO_KHZ800);

// ===== SENSOR PINS =====
const int NUM_SENSORS = 8;
const int SENSOR_PINS[NUM_SENSORS] = { A0, A1, A2, A3, A4, A5, A6, A7 };

// ===== MOTOR CONTROL PINS =====
const int MOTOR_LEFT_FORWARD = 11;
const int MOTOR_LEFT_BACK = 10;
const int MOTOR_RIGHT_BACK = 9;
const int MOTOR_RIGHT_FORWARD = 6;

// ===== ENCODERS =====
const int ROTATION_SENSOR_LEFT = 3;
const int ROTATION_SENSOR_RIGHT = 2;

// ===== ULTRASONIC SENSOR =====
const int ULTRASONIC_TRIG = 7;
const int ULTRASONIC_ECHO = 8;
const int OBSTACLE_DISTANCE_CM = 12;

// ===== GRIPPER SERVO =====
const int SERVO_PIN = 4;
const int SERVO_CLOSED_PULSE = 1000;
const int SERVO_OPEN_PULSE = 1500;

// ===== THRESHOLDS AND SPEEDS =====
const int BLACK_THRESHOLD = 800;       // 1:00         // 0:54   // 0:52
const int SPEED_FULL = 255;            //default 255   // 255    // 255
const int SPEED_SLIGHT_CORRECT = 210;  //default 170   // 210    // 210
const int SPEED_HARD_CORRECT = 120;    //default 60   // 60     // 120
const int SPEED_TANK_SHARP = 230;      //default 230   // 230    // 230
const int SPEED_SEARCH = 255;          //default 180   // 180    // 255
const int SPEED_STRAIGHT_LOST = 200;   //default 120   // 120    // 200
const unsigned long FINISH_BLACK_HOLD_MS = 50;
const unsigned long FINISH_FORWARD_MS = 500;
const unsigned long FINISH_BACKWARD_MS = 750;
const int FINISH_SEQUENCE_SPEED = SPEED_FULL;

// -1 = last turned right
// +1 = last turned left
// 0 = initial state
int lastTurnDir = 0;

int sensorValues[NUM_SENSORS] = { 0 };
float sensorMin[NUM_SENSORS];
float sensorMax[NUM_SENSORS];
unsigned long allBlackStartMs = 0;

// ===== PID VARIABLES =====
float Kp = 8.0;
float Ki = 0.2;
float Kd = 1.2;

// ===== ROBOT STATE =====
int state = 0;
int subState = -1;
unsigned long actionStartTime = 0;

// ===== MOVEMENT FLAG =====
bool moveForward = true;

// ===== BASE SPEED =====
const int baseSpeed = 200;

// ===== WHEEL CONSTANTS =====
const float WHEEL_DIAMETER = 65.0;
const int PULSES_PER_ROTATION = 20;

// ===== MOTOR BIAS =====
int motorBiasLeft = -35;
int motorBiasRight = 15;

int lineTransitions = 0;
bool onLine = false;
unsigned long lineStartTime = 0;
const unsigned long lineHoldTime = 120;

// ===== DYNAMIC THRESHOLD =====
int dynamicBlackThreshold = 600;

long duration;
int distance;

volatile long rotationCounterLeft = 0;
volatile long rotationCounterRight = 0;

// ===== FUNCTIONS PROTOTYPES =====
void stopMotors();
void moveStraightPID(int baseSpeed, int biasLeft, int biasRight);
void driveDistance(float millimeters, int baseSpeed);
void turnLeft90();
void readSensors();
void servoWrite(int pulseWidth);
void onLeftWheelPulse();
void onRightWheelPulse();
void updateCalibration();
void driveForward(int leftSpeed, int rightSpeed);
void tankTurnRight(int speed);
void tankTurnLeft(int speed);
void driveBackward(int leftSpeed, int rightSpeed);
void leftSignalLights();
void rightSignalLights();
void lightsOff();
bool areAllSensorsBlack();
void finishRace();
bool isObstacleDetected();
void avoidObject();

void setup() {
  Serial.begin(9600);

  pinMode(SERVO_PIN, OUTPUT);

  pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
  pinMode(MOTOR_LEFT_BACK, OUTPUT);
  pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_BACK, OUTPUT);
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);

  pinMode(ROTATION_SENSOR_LEFT, INPUT_PULLUP);
  pinMode(ROTATION_SENSOR_RIGHT, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ROTATION_SENSOR_LEFT), onLeftWheelPulse, RISING);
  attachInterrupt(digitalPinToInterrupt(ROTATION_SENSOR_RIGHT), onRightWheelPulse, RISING);

  for (int i = 0; i < NUM_SENSORS; i++) {
    sensorMin[i] = 1023;
    sensorMax[i] = 0;
  }

  pixels.begin();
  pixels.setBrightness(50);
  pixels.show();

  // Start with gripper closed.
  servoWrite(SERVO_CLOSED_PULSE);

  delay(1000);
}

void loop() {
  switch (state) {
    case 0:
      {
        if (subState == -1) {
          digitalWrite(ULTRASONIC_TRIG, LOW);
          delayMicroseconds(2);
          digitalWrite(ULTRASONIC_TRIG, HIGH);
          delayMicroseconds(10);
          digitalWrite(ULTRASONIC_TRIG, LOW);

          long duration = pulseIn(ULTRASONIC_ECHO, HIGH, 25000UL);
          long distanceCm = duration / 58;

          if (distanceCm > 30 && distanceCm > 1) {
            subState = 0;
          }
        }
        if (subState >= 0) {
          if (moveForward) {
            analogWrite(MOTOR_LEFT_FORWARD, baseSpeed + motorBiasLeft);
            analogWrite(MOTOR_LEFT_BACK, 0);
            analogWrite(MOTOR_RIGHT_FORWARD, baseSpeed + motorBiasRight);
            analogWrite(MOTOR_RIGHT_BACK, 0);
          }

          readSensors();
          updateCalibration();

          unsigned long now = millis();

          int blackCount = 0;
          for (int i = 0; i < NUM_SENSORS; i++) {
            if (sensorValues[i] > dynamicBlackThreshold) blackCount++;
          }

          if (blackCount >= 1) {
            if (!onLine) {
              lineStartTime = now;
              onLine = true;
            } else if (now - lineStartTime >= lineHoldTime) {
              lineTransitions++;
              onLine = false;
              /*Serial.print("Line #: ");
                Serial.println(lineTransitions);*/
            }
          } else {
            onLine = false;
          }

          dynamicBlackThreshold = 0;
          for (int i = 0; i < NUM_SENSORS; i++) {
            dynamicBlackThreshold += sensorMax[i];
          }
          dynamicBlackThreshold /= NUM_SENSORS;
          dynamicBlackThreshold -= 50;


          if (lineTransitions >= 6 && subState == 0) subState = 2;
        }
        if (subState == 2) {
          servoWrite(2000);
          int blackCount = 0;
          Serial.print("Square state");
          for (int i = 0; i < NUM_SENSORS; i++) {
            if (sensorValues[i] > dynamicBlackThreshold) blackCount++;
          }
          Serial.print(blackCount);

          if (blackCount >= 6) {
            Serial.print("Square");
            stopMotors();
            moveForward = false;
            actionStartTime = millis();
            subState = 3;
          }
        }

        else if (subState == 3) {
          Serial.print("State 3");
          Serial.print(actionStartTime);
          if (millis() - actionStartTime > 1000) subState = 4;
        }

        else if (subState == 4) {
          Serial.print("State 4");
          actionStartTime = millis();
          subState = 5;
        }

        else if (subState == 5) {
          if (millis() - actionStartTime > 500) subState = 6;
        }

        else if (subState == 6) {
          servoWrite(1000);
          driveDistance(80.0, baseSpeed);
          subState = 7;
        }

        else if (subState == 7) {
          turnLeft90();
          subState = 8;
        }

        else if (subState == 8) {
          int blackCount = 0;
          for (int i = 0; i < NUM_SENSORS; i++) {
            if (sensorValues[i] > dynamicBlackThreshold) blackCount++;
          }

          if (blackCount >= 1 && blackCount <= 3) {
            stopMotors();
            state = 1;
            moveForward = false;
          } else {
            moveForward = true;
          }
        }

        break;
      }


    case 1:
      {
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

        if (sensorValues[3] > dynamicBlackThreshold && sensorValues[4] > dynamicBlackThreshold) {
          // Perfectly centered. Full speed forward
          driveForward(SPEED_FULL, SPEED_FULL);
          lastTurnDir = 0;

        } else if (sensorValues[3] > dynamicBlackThreshold) {
          // Slight right. Both wheels moving, left faster
          driveForward(SPEED_FULL, SPEED_SLIGHT_CORRECT);
          lastTurnDir = -1;

        } else if (sensorValues[4] > dynamicBlackThreshold) {
          // Slight left. Both wheels moving, right faster
          driveForward(SPEED_SLIGHT_CORRECT, SPEED_FULL);
          lastTurnDir = 1;

        } else if (sensorValues[2] > dynamicBlackThreshold) {
          // Moderate right. Tighter differential
          driveForward(SPEED_FULL, SPEED_HARD_CORRECT);
          lastTurnDir = -1;

        } else if (sensorValues[5] > dynamicBlackThreshold) {
          // Moderate left. Tighter differential
          driveForward(SPEED_HARD_CORRECT, SPEED_FULL);
          lastTurnDir = 1;

        } else if (sensorValues[1] > dynamicBlackThreshold) {
          // Sharp right. Tank turn (left fwd, right rev)
          tankTurnRight(SPEED_TANK_SHARP);
          lastTurnDir = -1;

        } else if (sensorValues[6] > dynamicBlackThreshold) {
          // Sharp left. Tank turn (right fwd, left rev)
          tankTurnLeft(SPEED_TANK_SHARP);
          lastTurnDir = 1;

        } else if (sensorValues[0] > dynamicBlackThreshold) {
          // Very sharp right
          tankTurnRight(SPEED_FULL);
          lastTurnDir = -1;

        } else if (sensorValues[7] > dynamicBlackThreshold) {
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
        break;
      }
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
  pixels.setPixelColor(0, pixels.Color(255, 172, 28));  // front-left
  pixels.setPixelColor(3, pixels.Color(255, 172, 28));  // back-left
  pixels.show();
}

void rightSignalLights() {
  pixels.clear();
  pixels.setPixelColor(1, pixels.Color(255, 172, 28));  // front-right
  pixels.setPixelColor(2, pixels.Color(255, 172, 28));  // back-right
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


void onLeftWheelPulse() {
  rotationCounterLeft++;
}

void onRightWheelPulse() {
  rotationCounterRight++;
}

void readSensors() {
  for (int i = 0; i < NUM_SENSORS; i++) {
    sensorValues[i] = analogRead(SENSOR_PINS[i]);
  }
}

void updateCalibration() {
  float alpha = 0.05;

  for (int i = 0; i < NUM_SENSORS; i++) {
    sensorMin[i] = sensorMin[i] * (1 - alpha) + sensorValues[i] * alpha;
    sensorMax[i] = sensorMax[i] * (1 - alpha) + sensorValues[i] * alpha;
  }
}

void driveDistance(float millimeters, int baseSpeed) {
  rotationCounterLeft = 0;
  rotationCounterRight = 0;

  float targetPulses =
    (millimeters / (3.14159 * WHEEL_DIAMETER)) * PULSES_PER_ROTATION;

  moveForward = true;

  while ((abs(rotationCounterLeft) + abs(rotationCounterRight)) / 2 < targetPulses) {
    moveStraightPID(baseSpeed, motorBiasLeft, motorBiasRight);
  }

  stopMotors();
  moveForward = false;
}


void turnLeft90() {
  rotationCounterLeft = 0;
  rotationCounterRight = 0;

  float wheelBase = 120.0; // 10 см між колесами
  float arcLength = (3.14159 * wheelBase) / 4.0;

  float targetPulses =
    (arcLength / (3.14159 * WHEEL_DIAMETER)) * PULSES_PER_ROTATION;

  while ((abs(rotationCounterLeft) + abs(rotationCounterRight)) / 2 < targetPulses) {

    // ТАНКОВИЙ ПОВОРОТ (найстабільніший)
    analogWrite(MOTOR_LEFT_FORWARD, 0);
    analogWrite(MOTOR_LEFT_BACK, 180);

    analogWrite(MOTOR_RIGHT_FORWARD, 180);
    analogWrite(MOTOR_RIGHT_BACK, 0);
  }

  stopMotors();
  delay(200);
}

void moveStraightPID(int baseSpeed, int biasLeft, int biasRight) {
  static long lastLeft = 0;
  static long lastRight = 0;
  static float integral = 0;
  static float previousError = 0;

  long speedLeft = rotationCounterLeft - lastLeft;
  long speedRight = rotationCounterRight - lastRight;

  lastLeft = rotationCounterLeft;
  lastRight = rotationCounterRight;

  float error = speedLeft - speedRight;
  integral += error;
  float derivative = error - previousError;
  previousError = error;

  float correction = Kp * error + Ki * integral + Kd * derivative;
  correction = constrain(correction, -80, 80);

  int left = constrain(baseSpeed - correction + biasLeft, 0, 255);
  int right = constrain(baseSpeed + correction + biasRight, 0, 255);

  analogWrite(MOTOR_LEFT_FORWARD, left);
  analogWrite(MOTOR_LEFT_BACK, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, right);
  analogWrite(MOTOR_RIGHT_BACK, 0);
}
