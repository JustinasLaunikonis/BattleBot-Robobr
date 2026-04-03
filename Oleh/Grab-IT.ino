// ===== MOTOR CONTROL PINS =====
const int MOTOR_LEFT_FORWARD = 11;
const int MOTOR_LEFT_BACK = 10;
const int MOTOR_RIGHT_BACK = 9;
const int MOTOR_RIGHT_FORWARD = 6;

// ===== ENCODERS =====
// Sensors that count wheel rotation (pulses)
const int ROTATION_SENSOR_LEFT = 3;
const int ROTATION_SENSOR_RIGHT = 2;

// Counters updated by interrupts
volatile long rotationCounterLeft = 0;
volatile long rotationCounterRight = 0;

// ===== WHEEL CONSTANTS =====
const float WHEEL_CIRCUMFERENCE = 205.0; 
const int PULSES_PER_ROTATION = 20;

// ===== SERVO =====
// Servo pin (used for gripper)
int servoPin = 4;

// ===== STATE MACHINE =====
// step controls robot behavior sequence
int step = 0;
unsigned long previousMillis = 0;

// ===== MOTOR SPEED =====
// Adjust to keep robot driving straight
int baseSpeedLeft = 190;
int baseSpeedRight = 240;

// ===== DISTANCE CONTROL =====
int targetPulses = 0; // how many pulses needed to reach distance
bool driving = false; // is robot currently moving

void setup() {

  pinMode(servoPin, OUTPUT);

  // Motor pins setup
  pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
  pinMode(MOTOR_LEFT_BACK, OUTPUT);
  pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_BACK, OUTPUT);

  // Encoder pins with pull-up resistors
  pinMode(ROTATION_SENSOR_LEFT, INPUT_PULLUP);
  pinMode(ROTATION_SENSOR_RIGHT, INPUT_PULLUP);

  // Attach interrupts to count wheel pulses
  attachInterrupt(digitalPinToInterrupt(3), onLeftWheelPulse, RISING);
  attachInterrupt(digitalPinToInterrupt(2), onRightWheelPulse, RISING);
}

void loop() {
  // ===== STEP 0 =====
  // Open gripper
  if (step == 0) {            
    servoWrite(1500);
    if (waitMillis(1000)) step++;
  }
    
  // ===== STEP 1 =====
  // Close gripper
  else if (step == 1) {       
    servoWrite(1000);
    if (waitMillis(1000)) step++;
  }

  // ===== STEP 2 =====
  // Open gripper
  else if (step == 2) {      
    servoWrite(1500);
    if (waitMillis(1000)) step++;
  }

  // ===== STEP 3 =====
  // Drive forward a fixed distance
  else if (step == 3) {       
    if (!driving) startDriveDistance(250);

    if (checkDistance()) {
      stopMotors();           
      previousMillis = millis();
      step++;
    }
  }

  // ===== STEP 4 =====
  // Stop and close servo 
  else if (step == 4) {       
    stopMotors();     
    servoWrite(1000);
    if (waitMillis(1000)) step++;
  }

  // ===== STEP 5 =====
  // Drive forward again
  else if (step == 5) {      
    if (!driving) startDriveDistance(250);

    if (checkDistance()) {
      stopMotors();
      step++;
    }
  }

  // ===== STEP 6 =====
  // Final stop
  else if (step == 6) {       
    stopMotors();
  }

}

// ===== DISTANCE FUNCTIONS =====
// Start driving a specific distance (in mm)
void startDriveDistance(int millimeters) {

  // Reset encoder counters
  rotationCounterLeft = 0;
  rotationCounterRight = 0;

  float rotationsNeeded = millimeters / WHEEL_CIRCUMFERENCE; // Calculate how many rotations are needed
  targetPulses = rotationsNeeded * PULSES_PER_ROTATION; // Convert rotations to encoder pulses

  moveForward();
  driving = true;
}

// Check if robot has reached the target distance
bool checkDistance() {

  long pulses = max(rotationCounterLeft, rotationCounterRight); // Use the higher value (more reliable)

  if (pulses >= targetPulses) {
    stopMotors();
    driving = false;
    return true;
  }

  return false;
}

// ===== SERVO CONTROL =====
// Manually generate PWM signal for servo
void servoWrite(int pulseWidth) {

  unsigned long start = millis();

   // Repeat signal for ~20 ms (servo refresh period)
  while (millis() - start < 20) {
    digitalWrite(servoPin, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(servoPin, LOW);
    delayMicroseconds(20000 - pulseWidth);
  }
}

// ===== MOTOR FUNCTIONS =====

// Move forward with set speeds
void moveForward() {
  analogWrite(MOTOR_LEFT_FORWARD, baseSpeedLeft);
  analogWrite(MOTOR_LEFT_BACK, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, baseSpeedRight);
  analogWrite(MOTOR_RIGHT_BACK, 0);
}

// Stop all motors
void stopMotors() {
  analogWrite(MOTOR_LEFT_FORWARD, 0);
  analogWrite(MOTOR_LEFT_BACK, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, 0);
  analogWrite(MOTOR_RIGHT_BACK, 0);
}

// ===== ENCODERS =====

// Interrupt function: left wheel pulse detected
void onLeftWheelPulse() {
  rotationCounterLeft++;
}

// Interrupt function: right wheel pulse detected
void onRightWheelPulse() {
  rotationCounterRight++;
}

// ===== TIMER =====

// Non-blocking delay using millis()
// Returns true when time has passed
bool waitMillis(unsigned long duration) {
  if (millis() - previousMillis >= duration) {
    previousMillis = millis();
    return true;
  }
  return false;
}
