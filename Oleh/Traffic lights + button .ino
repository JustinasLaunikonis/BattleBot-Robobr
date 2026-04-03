// ===== LED PINS =====
int ledRed = 11; // red traffic light
int ledGreen = 9; // green traffic light
int ledYellow = 10; // yellow traffic light

// ===== BUTTON =====
int buttonPin = 7; // button pin
int buttonState = 0; // stores button state

void setup() {
  // Set LEDs as outputs
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledYellow, OUTPUT);

  // Set button as input
  pinMode(buttonPin, INPUT);

  // Initial state:
  // Red ON, others OFF
  digitalWrite(ledRed,LOW);
  digitalWrite(ledGreen,HIGH);
  digitalWrite(ledYellow,HIGH);
  
}

void loop() {
  
  // Read button state
  buttonState = digitalRead(buttonPin);

   // If button is pressed
  if(buttonState == LOW){
    delay(1000);  // wait before changing

     // ===== GREEN ON =====
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledGreen, LOW);
    
    delay(3000); // stay green for 3 seconds

     // ===== YELLOW ON =====
    digitalWrite(ledGreen, HIGH);
    digitalWrite(ledYellow, LOW);
    
    delay(1000);

    // ===== BACK TO RED =====
    digitalWrite(ledYellow, HIGH);
    digitalWrite(ledRed, LOW); 
  }
}
