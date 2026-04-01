#include <Adafruit_NeoPixel.h>

// ===== HARDWARE PINS =====
const int PIN_NEO = 12;

const int MOTOR_LEFT_FORWARD = 11;
const int MOTOR_LEFT_BACK = 10;
const int MOTOR_RIGHT_BACK = 9;
const int MOTOR_RIGHT_FORWARD = 6;

const int ROTATION_SENSOR_LEFT = 3;
const int ROTATION_SENSOR_RIGHT = 2;

const int ULTRASONIC_TRIG = 7;
const int ULTRASONIC_ECHO = 8;

const int SERVO_PIN = 4;

// ===== SENSOR / PIXEL LAYOUT =====
const int NUM_PIXELS = 4;
const int NUM_SENSORS = 8;
const int SENSOR_PINS[NUM_SENSORS] = { A0, A1, A2, A3, A4, A5, A6, A7 };

Adafruit_NeoPixel pixels(NUM_PIXELS, PIN_NEO, NEO_RGB + NEO_KHZ800);

// ===== MOTION / PHYSICS CONSTANTS =====
const int PULSES_PER_ROTATION = 20;
const int baseSpeed = 200;

// ===== PID TUNING =====
float Kp = 8.0;
float Ki = 0.2;
float Kd = 1.2;

// ===== TRACK / DETECTION THRESHOLDS =====
const int BLACK_THRESHOLD = 800;
int dynamicBlackThreshold = 600;
const int OBSTACLE_DISTANCE_CM = 12;

// ===== SPEED TUNING =====
const int SPEED_FULL = 255;
const int SPEED_SLIGHT_CORRECT = 210;
const int SPEED_HARD_CORRECT = 120;
const int SPEED_TANK_SHARP = 230;
const int SPEED_SEARCH = 255;
const int SPEED_STRAIGHT_LOST = 200;

// ===== TIMING CONSTANTS =====
const unsigned long FINISH_BLACK_HOLD_MS = 60;
const unsigned long FINISH_FORWARD_MS = 500;
const unsigned long FINISH_BACKWARD_MS = 750;
const unsigned long SERVO_HOLD_INTERVAL_MS = 80;
const unsigned long OBSTACLE_CHECK_INTERVAL_MS = 35;
const unsigned long TURN_SETTLE_DELAY_MS = 80;
const unsigned long lineHoldTime = 120;

// ===== SERVO PULSE CONSTANTS =====
const int SERVO_CLOSED_PULSE = 1000;
const int SERVO_OPEN_PULSE = 1500;

// ===== LIGHT MODES =====
const int LIGHT_MODE_OFF = 0;
const int LIGHT_MODE_LEFT_SIGNAL = 1;
const int LIGHT_MODE_RIGHT_SIGNAL = 2;
const int LIGHT_MODE_FORWARD = 3;
const int LIGHT_MODE_STOPPED = 4;

// ===== ROBOT STATES =====
const int STATE_START = 0;
const int STATE_FOLLOW_LINE = 1;
const int STATE_FINISH = 2;

// ===== START PROCEDURE PHASES =====
const int START_PHASE_WAIT_FLAG = -1;
const int START_PHASE_DRIVE_TO_LINE = 0;
const int START_PHASE_CONFIRM_PICKUP_ZONE = 2;
const int START_PHASE_WAIT_BEFORE_CLOSE = 3;
const int START_PHASE_SET_CLOSE_TIMER = 4;
const int START_PHASE_WAIT_AFTER_CLOSE = 5;
const int START_PHASE_CLOSE_AND_DRIVE = 6;
const int START_PHASE_TURN_LEFT = 7;
const int START_PHASE_FIND_LINE = 8;

// ===== RUNTIME STATE: MAIN FLOW =====
int state = STATE_START;
int startPhase = START_PHASE_WAIT_FLAG;
unsigned long actionStartTime = 0;
bool moveForward = true;

// ===== RUNTIME STATE: LINE TRACKING =====
// -1 = last turned right
// +1 = last turned left
// 0 = initial state
int lastTurnDir = 0;
int lineTransitions = 0;
bool onLine = false;
unsigned long lineStartTime = 0;
unsigned long allBlackStartMs = 0;

// ===== RUNTIME STATE: SENSOR DATA =====
int sensorValues[NUM_SENSORS] = { 0 };
float sensorMin[NUM_SENSORS];
float sensorMax[NUM_SENSORS];

// ===== RUNTIME STATE: MOTOR / ENCODER =====
int motorBiasLeft = -35;
int motorBiasRight = 15;
volatile long rotationCounterLeft = 0;
volatile long rotationCounterRight = 0;

// ===== RUNTIME STATE: LIGHT / SERVO / ULTRASONIC =====
int currentLightMode = -1;
int currentServoPulse = -1;
unsigned long lastServoHoldMs = 0;
unsigned long lastObstacleCheckMs = 0;
bool obstacleDetectedCached = false;

// ===== FUNCTIONS PROTOTYPES =====
void stopMotors();
void moveStraightPID(int baseSpeed, int biasLeft, int biasRight);
void driveDistance(float millimeters, int baseSpeed);
void turn(int degrees);
void readSensors();
void servoWrite(int pulseWidth);
void onLeftWheelPulse();
void onRightWheelPulse();
void updateCalibration();
void driveForward(int leftSpeed, int rightSpeed);
void tankTurnRight(int speed);
void tankTurnLeft(int speed);
void driveBackward(int leftSpeed, int rightSpeed);
void setLightsByMode(int mode);
void setGripperTarget(int pulseWidth);
void holdGripper();
bool areAllSensorsBlack();
bool isObstacleDetected();
void avoidObject();
void runStartProcedure();
void runFollowLineProcedure();
void runFinishProcedure();

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

  setGripperTarget(SERVO_OPEN_PULSE);

  delay(1000);
}

void loop() {
  switch (state) {
    case STATE_START:
      {
        runStartProcedure();
        break;
      }

    case STATE_FOLLOW_LINE:
      {
        runFollowLineProcedure();
        break;
      }

    case STATE_FINISH:
      {
        runFinishProcedure();
        break;
      }
  }
}

void runStartProcedure() {
  if (startPhase == START_PHASE_WAIT_FLAG) {
    digitalWrite(ULTRASONIC_TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(ULTRASONIC_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(ULTRASONIC_TRIG, LOW);

    long duration = pulseIn(ULTRASONIC_ECHO, HIGH, 25000UL);
    long distanceCm = duration / 58;

    if (distanceCm > 20 && distanceCm > 1) {
      startPhase = START_PHASE_DRIVE_TO_LINE;
    }
  }

  if (startPhase >= START_PHASE_DRIVE_TO_LINE) {
    if (moveForward) {
      analogWrite(MOTOR_LEFT_FORWARD, baseSpeed + motorBiasLeft);
      analogWrite(MOTOR_LEFT_BACK, 0);
      analogWrite(MOTOR_RIGHT_FORWARD, baseSpeed + motorBiasRight);
      analogWrite(MOTOR_RIGHT_BACK, 0);
      setLightsByMode(LIGHT_MODE_FORWARD);
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

    if (lineTransitions >= 6 && startPhase == START_PHASE_DRIVE_TO_LINE) {
      startPhase = START_PHASE_CONFIRM_PICKUP_ZONE;
    }
  }

  if (startPhase == START_PHASE_CONFIRM_PICKUP_ZONE) {
    setGripperTarget(2000);
    holdGripper();

    int blackCount = 0;

    for (int i = 0; i < NUM_SENSORS; i++) {
      if (sensorValues[i] > dynamicBlackThreshold) blackCount++;
    }

    if (blackCount >= 6) {
      stopMotors();

      moveForward = false;

      actionStartTime = millis();
      startPhase = START_PHASE_SET_CLOSE_TIMER;
    }
  }

  else if (startPhase == START_PHASE_SET_CLOSE_TIMER) {
    actionStartTime = millis();
    startPhase = START_PHASE_WAIT_AFTER_CLOSE;
  }

  else if (startPhase == START_PHASE_WAIT_AFTER_CLOSE) {
    if (millis() - actionStartTime > 500) {
      startPhase = START_PHASE_CLOSE_AND_DRIVE;
    }
  }

  else if (startPhase == START_PHASE_CLOSE_AND_DRIVE) {
    driveDistance(140.0, baseSpeed);
    setGripperTarget(SERVO_CLOSED_PULSE);
    delay(100);
    startPhase = START_PHASE_TURN_LEFT;
  }

  else if (startPhase == START_PHASE_TURN_LEFT) {
    turn(-90);
    startPhase = START_PHASE_FIND_LINE;
  }

  else if (startPhase == START_PHASE_FIND_LINE) {
    int blackCount = 0;

    for (int i = 0; i < NUM_SENSORS; i++) {
      if (sensorValues[i] > dynamicBlackThreshold) {
        blackCount++;
      }
    }

    if (blackCount >= 1 && blackCount <= 3) {
      stopMotors();

      state = STATE_FOLLOW_LINE;
      moveForward = false;
    } else {
      moveForward = true;
    }
  }
}

void runFollowLineProcedure() {
  // Keep gripper closed without blocking every control cycle.
  setGripperTarget(SERVO_CLOSED_PULSE);
  holdGripper();

  for (int i = 0; i < NUM_SENSORS; i++) {
    sensorValues[i] = analogRead(SENSOR_PINS[i]);
  }

  // Finish condition: black square detected by all sensors.
  if (areAllSensorsBlack()) {
    if (allBlackStartMs == 0) {
      allBlackStartMs = millis();
    } else if (millis() - allBlackStartMs >= FINISH_BLACK_HOLD_MS) {
      state = STATE_FINISH;
      return;
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
    setLightsByMode(LIGHT_MODE_RIGHT_SIGNAL);
  } else if (lastTurnDir > 0) {
    setLightsByMode(LIGHT_MODE_LEFT_SIGNAL);
  } else {
    setLightsByMode(LIGHT_MODE_FORWARD);
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
  setLightsByMode(LIGHT_MODE_STOPPED);
}

void setLightsByMode(int mode) {
  if (mode == currentLightMode) {
    return;
  }

  currentLightMode = mode;
  pixels.clear();

  switch (mode) {
    case LIGHT_MODE_LEFT_SIGNAL:
      pixels.setPixelColor(3, pixels.Color(255, 172, 28));  // front-left
      pixels.setPixelColor(0, pixels.Color(255, 172, 28));  // back-left
      break;

    case LIGHT_MODE_RIGHT_SIGNAL:
      pixels.setPixelColor(1, pixels.Color(255, 172, 28));  // front-right
      pixels.setPixelColor(2, pixels.Color(255, 172, 28));  // back-right
      break;

    case LIGHT_MODE_FORWARD:
      pixels.setPixelColor(2, pixels.Color(255, 255, 255)); // front-right
      pixels.setPixelColor(3, pixels.Color(255, 255, 255)); // front-left
      break;

    case LIGHT_MODE_STOPPED:
      pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // back-left
      pixels.setPixelColor(1, pixels.Color(255, 0, 0)); // back-right
      break;

    case LIGHT_MODE_OFF:
    default:
      break;
  }

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

void runFinishProcedure() {
  setLightsByMode(LIGHT_MODE_OFF);

  // Finishing sequence: forward, backward, then drop object.
  driveForward(SPEED_FULL, SPEED_FULL);
  delay(FINISH_FORWARD_MS);
  stopMotors();

  driveBackward(SPEED_FULL, SPEED_FULL);
  delay(FINISH_BACKWARD_MS);
  stopMotors();

  // Release gripper before final forward move.
  setGripperTarget(SERVO_OPEN_PULSE);

  driveBackward(SPEED_FULL, SPEED_FULL);
  delay(FINISH_BACKWARD_MS);
  stopMotors();


  // Hold final state forever: gripper open and motors stopped.
  while (true) {
    holdGripper();
    stopMotors();
  }
}

void holdGripper() {
  if (currentServoPulse < 0) {
    return;
  }

  unsigned long now = millis();

  if (now - lastServoHoldMs >= SERVO_HOLD_INTERVAL_MS) {
    servoWrite(currentServoPulse);
    lastServoHoldMs = now;
  }
}

void setGripperTarget(int pulseWidth) {
  if (currentServoPulse == pulseWidth) {
    return;
  }

  currentServoPulse = pulseWidth;
  servoWrite(currentServoPulse);
  lastServoHoldMs = millis();
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
  unsigned long now = millis();

  if (now - lastObstacleCheckMs < OBSTACLE_CHECK_INTERVAL_MS) {
    return obstacleDetectedCached;
  }

  lastObstacleCheckMs = now;

  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);

  long duration = pulseIn(ULTRASONIC_ECHO, HIGH, 25000UL);

  if (duration == 0) {
    obstacleDetectedCached = false;
    return false;
  }

  long distanceCm = duration / 58;

  obstacleDetectedCached = (distanceCm > 0 && distanceCm <= OBSTACLE_DISTANCE_CM);
  
  return obstacleDetectedCached;
}

void avoidObject() {
  setGripperTarget(SERVO_CLOSED_PULSE);
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
    holdGripper();
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
    (millimeters / (204.2)) * PULSES_PER_ROTATION;

  moveForward = true;

  while ((abs(rotationCounterLeft) + abs(rotationCounterRight)) / 2 < targetPulses) {
    moveStraightPID(baseSpeed, motorBiasLeft, motorBiasRight);
  }

  stopMotors();
  moveForward = false;
}


void turn(int degrees) {
  if (degrees == 0) {
    stopMotors();
    return;
  }

  rotationCounterLeft = 0;
  rotationCounterRight = 0;

  float wheelBase = 120.0;
  float turnFraction = abs(degrees) / 360.0;
  float arcLength = (3.14159 * wheelBase) * turnFraction;

  float targetPulses =
    (arcLength / (204.2)) * PULSES_PER_ROTATION;

  while ((abs(rotationCounterLeft) + abs(rotationCounterRight)) / 2 < targetPulses) {
    if (degrees > 0) {
      // Right turn.
      analogWrite(MOTOR_LEFT_FORWARD, 180);
      analogWrite(MOTOR_LEFT_BACK, 0);
      analogWrite(MOTOR_RIGHT_FORWARD, 0);
      analogWrite(MOTOR_RIGHT_BACK, 180);
    } else {
      // Left turn.
      analogWrite(MOTOR_LEFT_FORWARD, 0);
      analogWrite(MOTOR_LEFT_BACK, 180);
      analogWrite(MOTOR_RIGHT_FORWARD, 180);
      analogWrite(MOTOR_RIGHT_BACK, 0);
    }
  }
  
  if (TURN_SETTLE_DELAY_MS > 0) {
    delay(TURN_SETTLE_DELAY_MS);
  }
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