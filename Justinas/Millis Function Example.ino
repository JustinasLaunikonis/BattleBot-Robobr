/*
  Blink
  Makes the Green LED flash repeatedly (every 500ms) with a function millis()
*/

const int ledPinRed = 13;
const int ledPinYellow = 12;
const int ledPinGreen = 11;

unsigned long timerOne = 0;

bool state;

void setup() {
  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinYellow, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
}

void loop() {
  digitalWrite(ledPinRed, HIGH);     //turn off red LED
  digitalWrite(ledPinYellow, HIGH);  //turn off yellow LED
  
  int interval = 500;                //blink every 500ms
  blink(interval);                   //start the function
}

void blink(int interval){
  if (millis() >= timerOne) {        //is time reached?
    timerOne = millis() + interval;  //set new time

    state = !state;
    digitalWrite(ledPinGreen, state);
  }
}
