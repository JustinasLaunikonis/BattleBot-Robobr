const int TrigPin = 3;
const int EchoPin = 4;
int directionPinLB = 9;
int directionPinRF = 6; 
int directionPinLF = 10;
int directionPinRB = 8; 
long duration;
int distance;
bool running=true;
void setup() {
  // put your setup code here, to run once:
  pinMode(TrigPin,OUTPUT); //Trig pin
  pinMode(EchoPin,INPUT); //Echo pin
  pinMode(directionPinLB,OUTPUT) ; //Logic pins are also set as output
  pinMode(directionPinRB,OUTPUT) ;
  pinMode(directionPinRF,OUTPUT) ; //Logic pins are also set as output
  pinMode(directionPinLF,OUTPUT) ;
}

void moveForward(int time) {
  analogWrite(directionPinLF, 200);
  analogWrite(directionPinRF, 235);
  delay(time);
  stopMottor();
}


void turnRight90() {

  digitalWrite(directionPinLF, HIGH);
  digitalWrite(directionPinRB, HIGH);
  delay(400); // initial guess
  digitalWrite(directionPinLF, LOW);
  digitalWrite(directionPinRB, LOW);
  stopMottor();
}

void turnLeft90() {

  digitalWrite(directionPinRF, HIGH);
  digitalWrite(directionPinLB, HIGH);
  delay(400); // initial guess
  digitalWrite(directionPinRF, LOW);
  digitalWrite(directionPinLB, LOW);
  running=false;
  stopMottor();
}

void stopMottor(){
   digitalWrite(directionPinRF, LOW);
  digitalWrite(directionPinLB, LOW);
  digitalWrite(directionPinLF, LOW);
  digitalWrite(directionPinRB, LOW);
}
void loop() {  
  digitalWrite(TrigPin,LOW);
  delayMicroseconds(2);
  digitalWrite(TrigPin,HIGH);
  delayMicroseconds(10);
  duration = pulseIn(EchoPin, HIGH);
  distance = duration * 0.034 / 2;
  if(distance<20){
     turnRight90();    // turn to avoid
     moveForward(1500); // move past object (adjust timing)
     turnLeft90();
     moveForward(2000);
     turnLeft90();
     moveForward(1500);
     turnRight90(); 
  }
  else{
     moveForward(50);
  }



}
