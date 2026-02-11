int directionPinLB = 9;
int directionPinRF = 6; 
int directionPinLF = 10;
int directionPinRB = 8; 

const int encoderPin = 2;   
volatile long encoderCount = 0;

const int pulsesPerMeter = 21;  

void setup() {

  pinMode(directionPinLB, OUTPUT); 
  pinMode(directionPinRB, OUTPUT);
  pinMode(directionPinRF, OUTPUT); 
  pinMode(directionPinLF, OUTPUT);

  pinMode(encoderPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoderPin), countPulse, RISING);

  delay(1000);
}


void countPulse() {
  encoderCount++;
}


void moveForwardDistance(int targetPulses) {

  encoderCount = 0;

  analogWrite(directionPinLF, 200);
  analogWrite(directionPinRF, 230);

  while (encoderCount < targetPulses) {
    // waiting
  }

  stopMottor();
}


void moveBackwardDistance(int targetPulses) {

  encoderCount = 0;

  analogWrite(directionPinLB, 200);
  analogWrite(directionPinRB, 230);

  while (encoderCount < targetPulses) {
    // waiting
  }

  stopMottor();
}


void turnLeft90() {
  digitalWrite(directionPinRF, HIGH);
  digitalWrite(directionPinLB, HIGH);
  delay(420);   // adjust if needed
  stopMottor();
}


void turnRight90() {
  digitalWrite(directionPinLF, HIGH);
  digitalWrite(directionPinRB, HIGH);
  delay(420);   // adjust if needed
  stopMottor();
}

void stopMottor(){
  digitalWrite(directionPinRF, LOW);
  digitalWrite(directionPinLB, LOW);
  digitalWrite(directionPinLF, LOW);
  digitalWrite(directionPinRB, LOW);
}

void loop() {

  moveForwardDistance(pulsesPerMeter);   // 1 meter forward
  delay(500);

  moveBackwardDistance(pulsesPerMeter);  // 1 meter backward
  delay(500);

  turnLeft90();
  delay(500);

  turnRight90();
  delay(1000);
}
