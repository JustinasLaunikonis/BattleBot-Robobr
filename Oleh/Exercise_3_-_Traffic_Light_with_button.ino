int LedRed =12;
int LedGreen=11;
int LedYellow=10;
int buttonPin=9;
int buttonState=0;
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LedRed, OUTPUT);
  pinMode(LedGreen, OUTPUT);
  pinMode(LedYellow, OUTPUT);
  pinMode(buttonPin, INPUT);
}

// the loop function runs over and over again forever
void loop() {
  buttonState = digitalRead(buttonPin);
  if(buttonState == LOW){
     digitalWrite(LedRed, LOW);   // turn the LED on (HIGH is the voltage level)
     delay(3000);                       // wait for a second
     digitalWrite(LedRed, HIGH);    // turn the LED off by making the voltage LOW
     delay(30);
     digitalWrite(LedGreen, LOW);   // turn the LED on (HIGH is the voltage level)
     delay(4000);                       // wait for a second
     digitalWrite(LedGreen, HIGH);    // turn the LED off by making the voltage LOW
     delay(30);
     digitalWrite(LedYellow, LOW);   // turn the LED on (HIGH is the voltage level)
     delay(1000);                       // wait for a second
     digitalWrite(LedYellow, HIGH);    // turn the LED off by making the voltage LOW
     delay(30);
  }
 
 
}
