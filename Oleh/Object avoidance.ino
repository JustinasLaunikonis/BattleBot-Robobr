// ===== MOTOR CONTROL PINS =====
const int MOTOR_LEFT_FORWARD = 11;
const int MOTOR_LEFT_BACK = 10;
const int MOTOR_RIGHT_BACK = 9;
const int MOTOR_RIGHT_FORWARD = 6;

// ===== ULTRASONIC SENSOR PINS ====
const int TRIG_PIN = 7;
const int ECHO_PIN = 8;

// ==== Mesuare variables ====
long duration;
int distance;

void setup() {
  pinMode(TRIG_PIN,OUTPUT); 
  pinMode(ECHO_PIN,INPUT); 
  pinMode(MOTOR_LEFT_BACK,OUTPUT) ; 
  pinMode(MOTOR_RIGHT_BACK,OUTPUT) ;
  pinMode(MOTOR_RIGHT_FORWARD,OUTPUT) ; 
  pinMode(MOTOR_LEFT_FORWARD,OUTPUT) ;
  delay(500);
}

//move forward for some time
void moveForward(int time) {
  analogWrite(MOTOR_LEFT_FORWARD, 200);
  analogWrite(MOTOR_RIGHT_FORWARD, 235);
  delay(time);
  stopMottor();
}

//turn 90 degree right staying at the same place
void turnRight90() {
  digitalWrite(MOTOR_LEFT_FORWARD, HIGH);
  digitalWrite(MOTOR_RIGHT_BACK, HIGH);
  delay(420); // initial guess
  digitalWrite(MOTOR_LEFT_FORWARD, LOW);
  digitalWrite(MOTOR_RIGHT_BACK, LOW);
  stopMottor();
}

//turn 90 degree left staying at the same place
void turnLeft90() {
  digitalWrite(MOTOR_RIGHT_FORWARD, HIGH);
  digitalWrite(MOTOR_LEFT_BACK, HIGH);
  delay(420); // initial guess
  digitalWrite(MOTOR_RIGHT_FORWARD, LOW);
  digitalWrite(MOTOR_LEFT_BACK, LOW);
  stopMottor();
}

//full stop
void stopMottor(){
  digitalWrite(MOTOR_RIGHT_FORWARD, LOW);
  digitalWrite(MOTOR_LEFT_BACK, LOW);
  digitalWrite(MOTOR_LEFT_FORWARD, LOW);
  digitalWrite(MOTOR_RIGHT_BACK, LOW);
}

void loop() {  
  digitalWrite(TRIG_PIN,LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN,HIGH);
  delayMicroseconds(10);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;
  //Obstacle detection 
  if(distance<15 and distance!=0){ 
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