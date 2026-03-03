// ===== MOTOR CONTROL PINS =====
const int MOTOR_LEFT_FORWARD = 11;
const int MOTOR_LEFT_BACK = 10;
const int MOTOR_RIGHT_BACK = 9;
const int MOTOR_RIGHT_FORWARD = 6;

const int ROTATION_SENSOR_LEFT = 3;
const int ROTATION_SENSOR_RIGHT = 2;

// ===== WHEEL CONSTANTS =====
const float WHEEL_CIRCUMFERENCE = 205.0; // WHEEL_DIAMETER * PI
const int PULSES_PER_ROTATION = 20;

// ===== ROTATION COUNTERS =====
volatile long rotationCounterLeft = 0;
volatile long rotationCounterRight = 0;

// tracks accumulated error over time
float errorIntegral = 0;

void setup() {
  pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
  pinMode(MOTOR_LEFT_BACK, OUTPUT);
  pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_BACK, OUTPUT);

  // configure rotation counters as inputs
  pinMode(ROTATION_SENSOR_LEFT, INPUT_PULLUP);  // Left on pin 3
  pinMode(ROTATION_SENSOR_RIGHT, INPUT_PULLUP);  // Right on pin 2

  // RISING = trigger when signal goes from LOW to HIGH
  // runs countLeft() every time the left wheel rotates
  attachInterrupt(digitalPinToInterrupt(3), onLeftWheelPulse, RISING);
  attachInterrupt(digitalPinToInterrupt(2), onRightWheelPulse, RISING);

  delay(2000);
}

void loop() {
  int baseSpeed = 150;

  driveDistance(1000, baseSpeed); // forward 1m
  delay(1000);

  driveDistance(-1000, baseSpeed); // backward 1m
  delay(1000);

  while (true);
}

void driveDistance(float millimeters, int baseSpeed) {

  rotationCounterLeft = 0;
  rotationCounterRight = 0;
  errorIntegral = 0;

  float rotationsNeeded = millimeters / WHEEL_CIRCUMFERENCE;

  int targetPulses = abs(rotationsNeeded * PULSES_PER_ROTATION);

  bool forward = (millimeters >= 0);

  while ((abs(rotationCounterLeft) + abs(rotationCounterRight)) / 2 < targetPulses) {
    balanceMotors(baseSpeed, forward);
    delay(10);
  }

  stopMotors();
}

// ===== AUTOMATIC MOTOR BALANCING FUNCTION =====
void balanceMotors(int baseSpeed, bool forward) {
  
  // PROPORTIONAL_GAIN - How aggressively to correct current errors (higher = more responsive)
  // INTEGRAL_GAIN - How much to "remember" past errors and eliminate drift
  // DERIVATIVE_GAIN - How much to resist sudden changes (prevents overshooting)
  const float PROPORTIONAL_GAIN = 15;
  const float INTEGRAL_GAIN = 0.05;
  const float DERIVATIVE_GAIN = 0.3;
  
  const float MAX_INTEGRAL = 150.0;

  static int lastDifference = 0;

  // check the differences in rotation count
  int difference = rotationCounterLeft - rotationCounterRight;

  // update integral with anti-windup
  errorIntegral += difference;
  errorIntegral = constrain(errorIntegral, -MAX_INTEGRAL, MAX_INTEGRAL);
  
  // calculate derivative (rate of change of error)
  int derivative = difference - lastDifference;
  lastDifference = difference;

  float correction = (PROPORTIONAL_GAIN * difference) + (INTEGRAL_GAIN * errorIntegral) + (DERIVATIVE_GAIN * derivative);

  // if left is ahead, slow it down and speed up right
  int powerLeft = baseSpeed - correction;
  int powerRight = baseSpeed + correction;

  // keep power within valid range
  powerLeft = constrain(powerLeft, 0, 255);
  powerRight = constrain(powerRight, 0, 255);

  // send power commands to motors
  if (forward) {
    setLeftMotor(powerLeft);
    setRightMotor(powerRight);
  } else {
    setLeftMotor(-powerLeft);
    setRightMotor(-powerRight);
  }
  
  delay(10); 
}

// ===== HELPER FUNCTIONS =====
// These run automatically when encoders detect wheel rotation

void onLeftWheelPulse() {
  rotationCounterLeft++;
}

void onRightWheelPulse() {
  rotationCounterRight++;
}

void setLeftMotor(int speed) {
  if (speed >= 0) {
    analogWrite(MOTOR_LEFT_FORWARD, speed);
    analogWrite(MOTOR_LEFT_BACK, 0);
  } else {
    analogWrite(MOTOR_LEFT_FORWARD, 0);
    analogWrite(MOTOR_LEFT_BACK, -speed);
  }
}

void setRightMotor(int speed) {
  if (speed >= 0) {
    analogWrite(MOTOR_RIGHT_FORWARD, speed);
    analogWrite(MOTOR_RIGHT_BACK, 0);
  } else {
    analogWrite(MOTOR_RIGHT_FORWARD, 0);
    analogWrite(MOTOR_RIGHT_BACK, -speed);
  }
}

void stopMotors() {
  setLeftMotor(0);
  setRightMotor(0);
}