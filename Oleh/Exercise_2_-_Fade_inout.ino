/*
  Exercise 2 - Fade in
*/
int LedRed =12;
int brightness=0;
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LedRed, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  for (brightness=0;brightness<= 255; brightness+=5){
    analogWrite(LedRed,brightness);
    delay(30);
  }
  for (brightness=255;brightness>= 0; brightness-=5){
    analogWrite(LedRed,brightness);
    delay(30);
  }
}
