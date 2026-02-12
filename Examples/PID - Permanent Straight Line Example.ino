// ===== MOTOR CONTROL PINS =====
const int MOTOR_LEFT_FORWARD = 11;
const int MOTOR_LEFT_BACK = 10;
const int MOTOR_RIGHT_BACK = 9;
const int MOTOR_RIGHT_FORWARD = 6;

// ===== ROTATION COUNTERS =====
int rotationCounterLeft = 0;
int rotationCounterRight = 0;

void setup() {
  pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
  pinMode(MOTOR_LEFT_BACK, OUTPUT);
  pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_BACK, OUTPUT);

  // configure rotation counters as inputs
  pinMode(3, INPUT_PULLUP);  // Left on pin 3
  pinMode(2, INPUT_PULLUP);  // Right on pin 2

  // RISING = trigger when signal goes from LOW to HIGH
  // runs countLeft() every time the left wheel rotates
  attachInterrupt(digitalPinToInterrupt(3), countLeft, RISING);
  attachInterrupt(digitalPinToInterrupt(2), countRight, RISING);

  delay(2000);
}

void loop() {
  // Continuously balance the motors so the robot drives straight
  balanceMotors(MOTOR_LEFT_FORWARD, MOTOR_LEFT_BACK, MOTOR_RIGHT_FORWARD, MOTOR_RIGHT_BACK, rotationCounterLeft, rotationCounterRight, 150);
}

// ===== AUTOMATIC MOTOR BALANCING FUNCTION =====
// keeps both wheels synchronized
void balanceMotors(int leftFwd, int leftBack, int rightFwd, int rightBack, long leftEncoder, long rightEncoder, int baseSpeed) {

  // PROPORTIONAL_GAIN - How aggressively to correct current errors (higher = more responsive)
  // INTEGRAL_GAIN - How much to "remember" past errors and eliminate drift
  const float PROPORTIONAL_GAIN = 15.0;
  const float INTEGRAL_GAIN = 0.05; //keep at 0.01 - 0.05 (larger numbers can make the robot overshoot correction)
  
  // tracks accumulated error over time
  float errorIntegral = 0;
  
  // check the differences in rotation count
  int difference = leftEncoder - rightEncoder;
  
  // increase error count
  errorIntegral =+ difference;
  
  // calculate how much to adjust motor speeds
  // proportional - responds to current error
  // integral - corrects accumulated movement
  float correction = (PROPORTIONAL_GAIN * difference) + (INTEGRAL_GAIN * errorIntegral);
  
  // if left is ahead, slow it down and speed up right
  int speedLeft = baseSpeed - correction;
  int speedRight = baseSpeed + correction;
  
  // keep speed within valid range
  speedLeft = constrain(speedLeft, 0, 255);
  speedRight = constrain(speedRight, 0, 255);
  
  // Send speed commands to motors
  analogWrite(leftFwd, speedLeft);
  analogWrite(leftBack, 0);
  analogWrite(rightFwd, speedRight);
  analogWrite(rightBack, 0);
  
  delay(10);
}

// ===== HELPER FUNCTIONS =====
// These run automatically when encoders detect wheel rotation

void countLeft() {
  rotationCounterLeft++;
}

void countRight() {
  rotationCounterRight++;
}
