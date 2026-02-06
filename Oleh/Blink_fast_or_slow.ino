int redLed = 11;
int buttonFast = 6;  
int buttonSlow = 7;      
int mode = 0;
       
void setup(){
  pinMode(redLed, OUTPUT);
  pinMode(buttonFast, INPUT);
  pinMode(buttonSlow, INPUT);
}

void loop(){
  if (digitalRead(buttonFast) == LOW) {
    mode = 1;
  }
  else if (digitalRead(buttonSlow) == LOW) {
    mode = 2;
  }

  int waitTime;

  if (mode == 0) {
    waitTime = 1000;
  }
  else if (mode == 1) {
    waitTime = 500;
  }
  else {
    waitTime = 2000;
  }

  digitalWrite(redLed, LOW);
  delay(50);
  digitalWrite(redLed, HIGH);
  delay(waitTime);
}
