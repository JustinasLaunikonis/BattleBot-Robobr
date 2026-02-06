int ledRed =11;
int ledGreen=9;
int ledYellow=10;
int buttonPin=7;
int buttonState=0;

void setup() {
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(ledRed,LOW);
  digitalWrite(ledGreen,HIGH);
  digitalWrite(ledYellow,HIGH);
  
}

void loop() {
  buttonState = digitalRead(buttonPin);
  if(buttonState == LOW){
    delay(1000);  
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledGreen, LOW);
    delay(3000);
    digitalWrite(ledGreen, HIGH);
    digitalWrite(ledYellow, LOW);
    delay(1000);
    digitalWrite(ledYellow, HIGH);
    digitalWrite(ledRed, LOW); 
  }
}
