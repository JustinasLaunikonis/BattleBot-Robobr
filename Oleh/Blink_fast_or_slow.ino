unsigned long startMillis;  
unsigned long currentMillis;
unsigned long period = 1000;
int redLed = 11;
int buttonFast = 6;  
int buttonSlow = 7;      
       
void setup(){
  pinMode(redLed, OUTPUT);
  pinMode(buttonFast, INPUT);
  pinMode(buttonSlow, INPUT);
  startMillis = millis();  //initial start time
}

void loop(){
  currentMillis=millis();
  if(currentMillis-startMillis >= period){
    digitalWrite(redLed,!digitalRead(redLed));
    startMillis=currentMillis;
  }
  if(digitalRead(buttonFast)==LOW){
    period=500;
  }
  else if(digitalRead(buttonSlow)==LOW){
    period=2000;
  }

}
