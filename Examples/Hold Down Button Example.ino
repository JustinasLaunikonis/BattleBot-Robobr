const int LED_RED = 13;

const int BUTTON_1 = 12;
const int BUTTON_2 = 8;
const int BUTTON_3 = 7;

void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(BUTTON_1, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(BUTTON_1) == LOW) {
    digitalWrite(LED_RED, LOW);
  } else {
    digitalWrite(LED_RED, HIGH);
  }
}
