const int SENSOR_PINS[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
int sensorValues[8];

// ===== MOTOR CONTROL PINS =====
const int MOTOR_LEFT_FORWARD = 11;
const int MOTOR_LEFT_BACK = 10;
const int MOTOR_RIGHT_BACK = 9;
const int MOTOR_RIGHT_FORWARD = 6;

const int WHITE_THRESHOLD = 730;
const int BLACK_THRESHOLD = 800;

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
  pinMode(MOTOR_LEFT_BACK, OUTPUT);
  pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_BACK, OUTPUT);
}

void loop() {
  for (int i = 0; i < 8; i++) {
    sensorValues[i] = analogRead(SENSOR_PINS[i]);
  }  
  if(sensorValues[3]>BLACK_THRESHOLD && sensorValues[4]>BLACK_THRESHOLD){
    moveForward(220);
  }
  else if(sensorValues[3]>BLACK_THRESHOLD){
    turnRight(210);
  }
  else if(sensorValues[4]>BLACK_THRESHOLD){
    turnLeft(210);
  }
  else if(sensorValues[2]>BLACK_THRESHOLD){
    turnRight(220);
  }
  else if(sensorValues[5]>BLACK_THRESHOLD){
    turnLeft(220);
  }
  else if(sensorValues[1]>BLACK_THRESHOLD){
    turnRight(230);
  }
  else if(sensorValues[6]>BLACK_THRESHOLD){
    turnLeft(230);
  }
  else if(sensorValues[0]>BLACK_THRESHOLD){
    turnRight(250);
  }
  else if(sensorValues[7]>BLACK_THRESHOLD){
    turnLeft(250);
  }
  else{
    stopMotors();
  }
}

void moveForward(int speed) {
  analogWrite(MOTOR_LEFT_FORWARD, speed);
  analogWrite(MOTOR_LEFT_BACK, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, speed);
  analogWrite(MOTOR_RIGHT_BACK, 0);
}

void turnLeft(int speed) {
  analogWrite(MOTOR_LEFT_FORWARD, 0);
  analogWrite(MOTOR_LEFT_BACK, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, speed);
  analogWrite(MOTOR_RIGHT_BACK, 0);
}

void turnRight(int speed) {
  analogWrite(MOTOR_LEFT_FORWARD, speed);
  analogWrite(MOTOR_LEFT_BACK, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, 0);
  analogWrite(MOTOR_RIGHT_BACK, 0);
}

void stopMotors() {
  analogWrite(MOTOR_LEFT_FORWARD, 0);
  analogWrite(MOTOR_LEFT_BACK, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, 0);
  analogWrite(MOTOR_RIGHT_BACK, 0);
}