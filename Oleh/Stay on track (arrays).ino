// ===== SENSOR PINS =====
// Array of 8 line sensors 
const int SENSOR_PINS[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
int sensorValues[8]; // Stores sensor readings (0–1023)

// ===== MOTOR CONTROL PINS =====
const int MOTOR_LEFT_FORWARD = 11;
const int MOTOR_LEFT_BACK = 10;
const int MOTOR_RIGHT_BACK = 9;
const int MOTOR_RIGHT_FORWARD = 6;

// ===== THRESHOLDS =====
// Values to detect white vs black surface
const int WHITE_THRESHOLD = 730;
const int BLACK_THRESHOLD = 800;

void setup() {
  Serial.begin(9600);

  // Set motor pins as outputs
  pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
  pinMode(MOTOR_LEFT_BACK, OUTPUT);
  pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_BACK, OUTPUT);
}

void loop() {

  // ===== READ ALL SENSORS =====
  for (int i = 0; i < 8; i++) {
    sensorValues[i] = analogRead(SENSOR_PINS[i]);
  }  

  // ===== LINE FOLLOWING LOGIC =====
  // Center sensors (3 and 4) detect line → go straight
  if(sensorValues[3]>BLACK_THRESHOLD && sensorValues[4]>BLACK_THRESHOLD){
    moveForward(220);
  }

  // Slight right correction
  else if(sensorValues[3]>BLACK_THRESHOLD){
    turnRight(210);
  }

  // Slight left correction
  else if(sensorValues[4]>BLACK_THRESHOLD){
    turnLeft(210);
  }

  // Medium right correction
  else if(sensorValues[2]>BLACK_THRESHOLD){
    turnRight(220);
  }

  // Medium left correction
  else if(sensorValues[5]>BLACK_THRESHOLD){
    turnLeft(220);
  }

  // Strong right correction
  else if(sensorValues[1]>BLACK_THRESHOLD){
    turnRight(230);
  }

  // Strong left correction
  else if(sensorValues[6]>BLACK_THRESHOLD){
    turnLeft(230);
  }

  // Very strong right correction (edge sensor)
  else if(sensorValues[0]>BLACK_THRESHOLD){
    turnRight(250);
  }

  // Very strong left correction (edge sensor)
  else if(sensorValues[7]>BLACK_THRESHOLD){
    turnLeft(250);
  }

  // No line detected → stop
  else{
    stopMotors();
  }
}

// ===== MOTOR FUNCTIONS =====

// Move forward (both motors same speed)
void moveForward(int speed) {
  analogWrite(MOTOR_LEFT_FORWARD, speed);
  analogWrite(MOTOR_LEFT_BACK, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, speed);
  analogWrite(MOTOR_RIGHT_BACK, 0);
}

// Turn left (right motor only)
void turnLeft(int speed) {
  analogWrite(MOTOR_LEFT_FORWARD, 0);
  analogWrite(MOTOR_LEFT_BACK, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, speed);
  analogWrite(MOTOR_RIGHT_BACK, 0);
}

// Turn right (left motor only)
void turnRight(int speed) {
  analogWrite(MOTOR_LEFT_FORWARD, speed);
  analogWrite(MOTOR_LEFT_BACK, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, 0);
  analogWrite(MOTOR_RIGHT_BACK, 0);
}

// Stop all motors
void stopMotors() {
  analogWrite(MOTOR_LEFT_FORWARD, 0);
  analogWrite(MOTOR_LEFT_BACK, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, 0);
  analogWrite(MOTOR_RIGHT_BACK, 0);
}
