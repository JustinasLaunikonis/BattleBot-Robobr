// ===== MOTOR CONTROL PINS =====
const int MOTOR_LEFT_FORWARD = 11;
const int MOTOR_LEFT_BACK = 10;
const int MOTOR_RIGHT_BACK = 9;
const int MOTOR_RIGHT_FORWARD = 6;

const int ROTATION_SENSOR_LEFT = 3;
const int ROTATION_SENSOR_RIGHT = 2;

// ===== WHEEL CONSTANTS =====
const float WHEEL_DIAMETER = 65.0;
const int PULSES_PER_ROTATION = 20;

const float ROBOT_WIDTH = 1000.0;

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
  // runs countLeft() every time the left wheel rotates
  attachInterrupt(digitalPinToInterrupt(3), countLeft, RISING);
  attachInterrupt(digitalPinToInterrupt(2), countRight, RISING);

  delay(2000);
}

void loop() {
  int baseSpeed = 200;
  turn90(true, baseSpeed); // left 90
  delay(1000);

  turn90(false, baseSpeed); // right 90

  while (true);
}


void turn90(bool turnLeft, int baseSpeed) {

  rotationCounterLeft = 0;
  rotationCounterRight = 0;

  // full circle = 2piR
  float arcLength = (PI * ROBOT_WIDTH) / 16.0;  // 90 degrees
  float wheelCircumference = PI * WHEEL_DIAMETER;

  int targetPulses = (arcLength / wheelCircumference) * PULSES_PER_ROTATION;

  while (true) {

    if (turnLeft) {
      if (rotationCounterRight >= targetPulses) break;

      // left wheel stopped
      analogWrite(MOTOR_LEFT_FORWARD, 0);
      analogWrite(MOTOR_LEFT_BACK, 0);

      // right wheel forward
      analogWrite(MOTOR_RIGHT_FORWARD, baseSpeed);
      analogWrite(MOTOR_RIGHT_BACK, 0);
    }
    else {
      if (rotationCounterLeft >= targetPulses) break;

      // right wheel stopped
      analogWrite(MOTOR_RIGHT_FORWARD, 0);
      analogWrite(MOTOR_RIGHT_BACK, 0);

      // left wheel forward
      analogWrite(MOTOR_LEFT_FORWARD, baseSpeed);
      analogWrite(MOTOR_LEFT_BACK, 0);
    }
  }

  stopMotors();
  delay(500);
}

// ===== HELPER FUNCTIONS =====
// These run automatically when encoders detect wheel rotation

void countLeft() {
  rotationCounterLeft++;
}

void countRight() {
  rotationCounterRight++;
}

void stopMotors() {
  analogWrite(MOTOR_LEFT_FORWARD, 0);
  analogWrite(MOTOR_LEFT_BACK, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, 0);
  analogWrite(MOTOR_RIGHT_BACK, 0);
}
