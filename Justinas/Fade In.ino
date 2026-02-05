/*
  Fade in
  Example 2
*/

// On an Arduino Nano, the PWM (analogWrite) pins are:
// 3, 5, 6, 9, 10, 11
//
// Not PWM:
// 0, 1, 2, 4, 7, 8, 12, 13

const int ledPinRed = 11;
const int ledPinYellow = 10;
const int ledPinGreen = 9;

int brightness = 0;

void setup() {
  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinYellow, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
}

void loop() {
  for (brightness = 0; brightness <= 255; brightness += 5) {
    analogWrite(ledPinRed, brightness);
    analogWrite(ledPinYellow, brightness);
    analogWrite(ledPinGreen, brightness);
    delay(30);
  }
  
  for (brightness = 255; brightness>= 0; brightness -= 5){
    analogWrite(ledPinRed, brightness);
    analogWrite(ledPinYellow, brightness);
    analogWrite(ledPinGreen, brightness);
    delay(30);
  }
}
