int trigPin = 3;
int echoPin = 4;
int directionPinLB = 9;
int directionPinRF = 6; 
int directionPinLF = 10;
int directionPinRB = 8; 
long duration;
int distance;
void setup() {
  pinMode(trigPin,OUTPUT); //Trig pin
  pinMode(echoPin,INPUT); //Echo pin
  pinMode(directionPinLB,OUTPUT) ; 
  pinMode(directionPinRB,OUTPUT) ;
  pinMode(directionPinRF,OUTPUT) ; 
  pinMode(directionPinLF,OUTPUT) ;
  delay(500);
}

//move forward for some time
void moveForward(int time) {
  analogWrite(directionPinLF, 200);
  analogWrite(directionPinRF, 230);
  delay(time);
  stopMottor();
}

//turn 90 degree right staying at the same place
void turnRight90() {
  digitalWrite(directionPinLF, HIGH);
  digitalWrite(directionPinRB, HIGH);
  delay(420); // initial guess
  digitalWrite(directionPinLF, LOW);
  digitalWrite(directionPinRB, LOW);
  stopMottor();
}

//turn 90 degree left staying at the same place
void turnLeft90() {
  digitalWrite(directionPinRF, HIGH);
  digitalWrite(directionPinLB, HIGH);
  delay(420); // initial guess
  digitalWrite(directionPinRF, LOW);
  digitalWrite(directionPinLB, LOW);
  stopMottor();
}

//full stop
void stopMottor(){
   digitalWrite(directionPinRF, LOW);
  digitalWrite(directionPinLB, LOW);
  digitalWrite(directionPinLF, LOW);
  digitalWrite(directionPinRB, LOW);
}

void loop() {  
  digitalWrite(trigPin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  if(distance<10 and distance!=0){
     turnLeft90();   
     moveForward(500); 
     turnRight90();
     moveForward(500);
     turnRight90();
     moveForward(500);
     turnLeft90(); 
  }
  else{
     moveForward(50);
  }
  

}
