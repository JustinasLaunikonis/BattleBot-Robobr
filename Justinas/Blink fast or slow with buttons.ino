/*
  Blink (fast or slow) using millis()

  - Default: blink once per second
  - Button 1: blink twice per second
  - Button 2: blink once every 2 seconds
*/

const int ledRedPin = 11;

const int buttonFastPin = 7; //button 1
const int buttonSlowPin = 6; //button 2

unsigned long previousMillis = 0;
unsigned long blinkInterval = 1000;

bool ledRedState = LOW;

bool lastFastButtonState = HIGH;
bool lastSlowButtonState = HIGH;


void setup() {
  pinMode(ledRedPin, OUTPUT);

  pinMode(buttonFastPin, INPUT_PULLUP);
  pinMode(buttonSlowPin, INPUT_PULLUP);
}


void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= blinkInterval) {
    previousMillis = currentMillis;
    ledRedState = !ledRedState; //toggle LED
    digitalWrite(ledRedPin, ledRedState);
  }

  bool fastButtonState = digitalRead(buttonFastPin);
  bool slowButtonState = digitalRead(buttonSlowPin);

  if (fastButtonState == LOW && lastFastButtonState == HIGH) {
    blinkInterval = 500; // 2 blinks per second
  }

  if (slowButtonState == LOW && lastSlowButtonState == HIGH) {
    blinkInterval = 2000; // 1 blink per 2 seconds
  }

  lastFastButtonState = fastButtonState;
  lastSlowButtonState = slowButtonState;
}
