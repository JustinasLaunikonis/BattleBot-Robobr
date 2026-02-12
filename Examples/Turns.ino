const int MOTOR_LEFT_FORWARD = 11;
const int MOTOR_LEFT_BACK = 10;
const int MOTOR_RIGHT_BACK = 9;
const int MOTOR_RIGHT_FORWARD = 3;

const int BUTTON_1 = 12;
const int BUTTON_2 = 8;
const int BUTTON_3 = 7;

unsigned long turnStartTime = 0;
unsigned long pauseStartTime = 0;

unsigned long turnDuration = 650;
unsigned long pauseDuration = 30;

int turnCount = 0;

bool isTurning = false;
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

  if (digitalRead(BUTTON_1) == LOW && !isTurning && !isPausing) {
    turnCount = 1;
    startTurning();
  }

  if (digitalRead(BUTTON_2) == LOW && !isTurning && !isPausing) {
    turnCount = 2;
    startTurning();
  }

  if (digitalRead(BUTTON_3) == LOW && !isTurning && !isPausing) {
    turnCount = 4;
    startTurning();
  }

  if (isTurning) {

    analogWrite(MOTOR_LEFT_FORWARD, 255);
    analogWrite(MOTOR_LEFT_BACK, 0);

    analogWrite(MOTOR_RIGHT_FORWARD, 0);
    analogWrite(MOTOR_RIGHT_BACK, 0);

    if (millis() - turnStartTime >= turnDuration) {
      stopMotors();
      turnCount--;
      isTurning = false;

      if (turnCount > 0) {
        isPausing = true;
        pauseStartTime = millis();
      }
    }
  }

  if (isPausing) {
    if (millis() - pauseStartTime >= pauseDuration) {
      isPausing = false;
      startTurning();
    }
  }
}

void startTurning() {
  delay(500);
  turnStartTime = millis();
  isTurning = true;
}

void stopMotors() {
  analogWrite(MOTOR_LEFT_FORWARD, 0);
  analogWrite(MOTOR_LEFT_BACK, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, 0);
  analogWrite(MOTOR_RIGHT_BACK, 0);
}
