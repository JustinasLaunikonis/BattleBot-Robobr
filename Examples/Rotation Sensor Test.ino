const int ROTATION_SENSOR_LEFT = 3;
const int ROTATION_SENSOR_RIGHT = 2;

volatile long encoderRight = 0;
volatile long encoderLeft = 0;

void setup() {
  pinMode(ROTATION_SENSOR_LEFT, INPUT);
  pinMode(ROTATION_SENSOR_RIGHT, INPUT);

  attachInterrupt(digitalPinToInterrupt(3), countLeft, RISING);
  attachInterrupt(digitalPinToInterrupt(2), countRight, RISING);

  Serial.begin(9600);
}

void loop() {
  Serial.print("Right: ");
  Serial.print(encoderRight);
  Serial.print("   Left: ");
  Serial.println(encoderLeft);
  delay(500);
}

void countRight() {
  encoderRight++;
}

void countLeft() {
  encoderLeft++;
}
