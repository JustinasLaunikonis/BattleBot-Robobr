// ===== TIMER VARIABLES =====
unsigned long startMillis;  //last time LED changed
unsigned long currentMillis; //current time
unsigned long period = 1000; //how fast LED blinks (in ms)

// ===== PIN DEFINITIONS =====
int redLed = 11; // LED pin
int buttonFast = 6;  // button to make blinking faster
int buttonSlow = 7;  // button to make blinking slower    
       
void setup(){
  // Set LED as output
  pinMode(redLed, OUTPUT);

  // Set buttons as input
  pinMode(buttonFast, INPUT);
  pinMode(buttonSlow, INPUT);

  // Save starting time
  startMillis = millis();  
}

void loop(){
  // Get current time
  currentMillis=millis();

  // ===== LED BLINK LOGIC =====
  // If enough time passed → toggle LED
  if(currentMillis-startMillis >= period){
    digitalWrite(redLed,!digitalRead(redLed));  // switch ON/OFF
    startMillis=currentMillis; // reset timer
  }
  // ===== BUTTON CONTROL =====
  // If fast button pressed → faster blinking
  if(digitalRead(buttonFast)==LOW){
    period=500; // 0.5 seconds
  }
  // If slow button pressed → slower blinking
  else if(digitalRead(buttonSlow)==LOW){
    period=2000; // 2 seconds
  }

}
