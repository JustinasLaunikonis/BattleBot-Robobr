const int LED_RED = 13;

const int BUTTON_1 = 12;

unsigned long ledTurnOnTime = 0;
bool ledIsOn = false;

void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(BUTTON_1, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(BUTTON_1) == LOW) {
    ledTurnOnTime = millis();
    ledIsOn = true;
  }

  if (ledIsOn) {
    digitalWrite(LED_RED, LOW);

    if (millis() - ledTurnOnTime >= 1000) {
      ledIsOn = false;
      digitalWrite(LED_RED, HIGH);
    }
  }
}
