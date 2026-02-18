const int leftSensor = A3;
const int rightSensor = A4;

int leftMotorForward  = 11;
int leftMotorBackward = 10;
int rightMotorForward = 6;
int rightMotorBackward= 9;

const int WHITE_THRESHOLD = 730;
const int BLACK_THRESHOLD = 800;

void setup() {
  Serial.begin(9600);

  pinMode(leftMotorForward, OUTPUT);
  pinMode(leftMotorBackward, OUTPUT);
  pinMode(rightMotorForward, OUTPUT);
  pinMode(rightMotorBackward, OUTPUT);
}

void loop() {

  int leftValue  = analogRead(leftSensor);
  int rightValue = analogRead(rightSensor);

  Serial.print(leftValue);
  Serial.print("  ");
  Serial.println(rightValue);

  bool leftBlack  = leftValue  > BLACK_THRESHOLD;
  bool rightBlack = rightValue > BLACK_THRESHOLD;

  if (leftBlack && rightBlack) {
    moveForward();
  }
  else if (!leftBlack && rightBlack) {
    turnLeft();
  }
  else if (leftBlack && !rightBlack) {
    turnRight();
  }
  else {
    stopMotors();
  }

  delay(10);
}

void moveForward() {
  analogWrite(leftMotorForward, 200);
  analogWrite(leftMotorBackward, 0);
  analogWrite(rightMotorForward, 200);
  analogWrite(rightMotorBackward, 0);
}

void turnLeft() {
  analogWrite(leftMotorForward, 0);
  analogWrite(leftMotorBackward, 0);
  analogWrite(rightMotorForward, 180);
  analogWrite(rightMotorBackward, 0);
}

void turnRight() {
  analogWrite(leftMotorForward, 180);
  analogWrite(leftMotorBackward, 0);
  analogWrite(rightMotorForward, 0);
  analogWrite(rightMotorBackward, 0);
}

void stopMotors() {
  analogWrite(leftMotorForward, 0);
  analogWrite(leftMotorBackward, 0);
  analogWrite(rightMotorForward, 0);
  analogWrite(rightMotorBackward, 0);
}