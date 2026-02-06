int LedRed =11;
int LedGreen=9;
int LedYellow=10;
int buttonPin=7;
int buttonState=0;

void setup() {
  pinMode(LedRed, OUTPUT);
  pinMode(LedGreen, OUTPUT);
  pinMode(LedYellow, OUTPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(LedRed,LOW);
  digitalWrite(LedGreen,HIGH);
  digitalWrite(LedYellow,HIGH);
  
}

void loop() {
  buttonState = digitalRead(buttonPin);
  if(buttonState == LOW){
    delay(1000);  
    digitalWrite(LedRed, HIGH);
    digitalWrite(LedGreen, LOW);
    delay(3000);
    digitalWrite(LedGreen, HIGH);
    digitalWrite(LedYellow, LOW);
    delay(1000);
    digitalWrite(LedYellow, HIGH);
    digitalWrite(LedRed, LOW); 
  }
}
