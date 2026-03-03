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

const float TURN_ARC_PER_DEGREE = 2.18;  // arc length per degree of turn

// ===== ROTATION COUNTERS =====
volatile long rotationCounterLeft = 0;
volatile long rotationCounterRight = 0;

void setup() {
  pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
  pinMode(MOTOR_LEFT_BACK, OUTPUT);
  pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_BACK, OUTPUT);

  // configure rotation counters as inputs
  pinMode(ROTATION_SENSOR_LEFT, INPUT_PULLUP);  // Left on pin 3
  pinMode(ROTATION_SENSOR_RIGHT, INPUT_PULLUP);  // Right on pin 2

  // RISING = trigger when signal goes from LOW to HIGH
  // runs onLeftWheelPulse() every time the left wheel rotates
  attachInterrupt(digitalPinToInterrupt(3), onLeftWheelPulse, RISING);
  attachInterrupt(digitalPinToInterrupt(2), onRightWheelPulse, RISING);

  delay(2000);
}

void loop() {
  int baseSpeed = 200;
  turn(-90, baseSpeed); // left 90
  delay(1000);

  turn(180, baseSpeed); // right 180

  while (true);
}


void turn(int degrees, int baseSpeed) {

  rotationCounterLeft = 0;
  rotationCounterRight = 0;

  // calculate arc length for the turn angle
  float arcLength = TURN_ARC_PER_DEGREE * abs(degrees);
  int targetPulses = (arcLength / WHEEL_CIRCUMFERENCE) * PULSES_PER_ROTATION;

  while (true) {

    if (degrees < 0) {  // negative = turn left
      if (rotationCounterRight >= targetPulses) break;
      setLeftMotor(0);
      setRightMotor(baseSpeed);
    }
    else {  // positive = turn right
      if (rotationCounterLeft >= targetPulses) break;
      setRightMotor(0);
      setLeftMotor(baseSpeed);
    }
  }

  stopMotors();
  delay(500);
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
