const int MOTOR_LEFT_FORWARD = 11;
const int MOTOR_LEFT_BACK = 10;
const int MOTOR_RIGHT_BACK = 9;
const int MOTOR_RIGHT_FORWARD = 3;

const int BUTTON_1 = 12;
const int BUTTON_2 = 8;
const int BUTTON_3 = 7;

unsigned long rotationStartTime = 0;
unsigned long pauseStartTime = 0;

unsigned long rotationDuration = 700;
unsigned long pauseDuration = 30;

int rotationCount = 0;

bool isRotating = false;
bool isPausing = false;

void setup() {
  pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
  pinMode(MOTOR_LEFT_BACK, OUTPUT);
  pinMode(MOTOR_RIGHT_BACK, OUTPUT);
  pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);

  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  pinMode(BUTTON_3, INPUT_PULLUP);
}

void loop() {

  if (digitalRead(BUTTON_1) == LOW && !isRotating && !isPausing) {
    rotationCount = 1;
    startRotating(rotationCount);
  }

  if (digitalRead(BUTTON_2) == LOW && !isRotating && !isPausing) {
    rotationCount = 2;
    startRotating(rotationCount);
  }

  if (digitalRead(BUTTON_3) == LOW && !isRotating && !isPausing) {
    rotationCount = 4;
    startRotating(rotationCount);
  }
}

void startRotating(int rotationCount) {
  delay(500);
  rotationStartTime = millis();
  isRotating = true;

  if (isRotating) {
    analogWrite(MOTOR_LEFT_FORWARD, 150);
    analogWrite(MOTOR_LEFT_BACK, 0);

    analogWrite(MOTOR_RIGHT_FORWARD, 0);
    analogWrite(MOTOR_RIGHT_BACK, 240);

    if (millis() - rotationStartTime >= rotationDuration) {
      stopMotors();
      rotationCount--;
      isRotating = false;

      if (rotationCount > 0) {
        isPausing = true;
        pauseStartTime = millis();
      }
    }
  }
  
  if (isPausing) {
    if (millis() - pauseStartTime >= pauseDuration) {
      isPausing = false;
      rotationStartTime = millis();
      isRotating = true;
    }
  }
}

void stopMotors() {
  analogWrite(MOTOR_LEFT_FORWARD, 0);
  analogWrite(MOTOR_LEFT_BACK, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, 0);
  analogWrite(MOTOR_RIGHT_BACK, 0);
}
