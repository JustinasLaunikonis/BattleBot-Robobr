// ===== MOTOR CONTROL PINS =====
const int MOTOR_LEFT_FORWARD = 11;
const int MOTOR_LEFT_BACK = 10;
const int MOTOR_RIGHT_BACK = 9;
const int MOTOR_RIGHT_FORWARD = 6;

// ===== ENCODERS =====
const int ROTATION_SENSOR_LEFT = 3;
const int ROTATION_SENSOR_RIGHT = 2;

volatile long rotationCounterLeft = 0;
volatile long rotationCounterRight = 0;

// ===== WHEEL CONSTANTS =====
const float WHEEL_CIRCUMFERENCE = 205.0; 
const int PULSES_PER_ROTATION = 20;

// ===== SERVO =====
int servoPin = 4;

int step = 0;
unsigned long previousMillis = 0;

int baseSpeedLeft = 190;
int baseSpeedRight = 240;

int targetPulses = 0;
bool driving = false;

void setup() {

  pinMode(servoPin, OUTPUT);

  pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
  pinMode(MOTOR_LEFT_BACK, OUTPUT);
  pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_BACK, OUTPUT);

  pinMode(ROTATION_SENSOR_LEFT, INPUT_PULLUP);
  pinMode(ROTATION_SENSOR_RIGHT, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(3), onLeftWheelPulse, RISING);
  attachInterrupt(digitalPinToInterrupt(2), onRightWheelPulse, RISING);
}

void loop() {
  if (step == 0) {            
    servoWrite(1500);
    if (waitMillis(1000)) step++;
  }
  else if (step == 1) {       
    servoWrite(1000);
    if (waitMillis(1000)) step++;
  }
  else if (step == 2) {      
    servoWrite(1500);
    if (waitMillis(1000)) step++;
  }
  else if (step == 3) {       
    if (!driving) startDriveDistance(250);

    if (checkDistance()) {
      stopMotors();           
      previousMillis = millis();
      step++;
    }
  }
  else if (step == 4) {       
    stopMotors();     
    servoWrite(1000);
    if (waitMillis(1000)) step++;
  }
  else if (step == 5) {      
    if (!driving) startDriveDistance(250);

    if (checkDistance()) {
      stopMotors();
      step++;
    }
  }
  else if (step == 6) {       
    stopMotors();
  }

}

// ===== DISTANCE FUNCTIONS =====

void startDriveDistance(int millimeters) {

  rotationCounterLeft = 0;
  rotationCounterRight = 0;

  float rotationsNeeded = millimeters / WHEEL_CIRCUMFERENCE;
  targetPulses = rotationsNeeded * PULSES_PER_ROTATION;

  moveForward();
  driving = true;
}

bool checkDistance() {

  long pulses = max(rotationCounterLeft, rotationCounterRight);

  if (pulses >= targetPulses) {
    stopMotors();
    driving = false;
    return true;
  }

  return false;
}

// ===== SERVO CONTROL =====

void servoWrite(int pulseWidth) {

  unsigned long start = millis();

  while (millis() - start < 20) {
    digitalWrite(servoPin, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(servoPin, LOW);
    delayMicroseconds(20000 - pulseWidth);
  }
}

// ===== MOTOR FUNCTIONS =====

void moveForward() {
  analogWrite(MOTOR_LEFT_FORWARD, baseSpeedLeft);
  analogWrite(MOTOR_LEFT_BACK, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, baseSpeedRight);
  analogWrite(MOTOR_RIGHT_BACK, 0);
}

void stopMotors() {
  analogWrite(MOTOR_LEFT_FORWARD, 0);
  analogWrite(MOTOR_LEFT_BACK, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, 0);
  analogWrite(MOTOR_RIGHT_BACK, 0);
}

// ===== ENCODERS =====

void onLeftWheelPulse() {
  rotationCounterLeft++;
}

void onRightWheelPulse() {
  rotationCounterRight++;
}

// ===== TIMER =====

bool waitMillis(unsigned long duration) {
  if (millis() - previousMillis >= duration) {
    previousMillis = millis();
    return true;
  }
  return false;
}
