// ===== MOTOR CONTROL PINS =====
const int MOTOR_LEFT_FORWARD = 11;
const int MOTOR_LEFT_BACK = 10;
const int MOTOR_RIGHT_BACK = 9;
const int MOTOR_RIGHT_FORWARD = 6;

// ===== ULTRASONIC SENSOR PINS ====
const int TRIG_PIN = 7;
const int ECHO_PIN = 8;

// ==== Mesuare variables ====
long duration; // duration = time for sound to travel
int distance; // distance = calculated distance in cm

void setup() {
  // Set sensor pins
  pinMode(TRIG_PIN,OUTPUT); 
  pinMode(ECHO_PIN,INPUT); 

   // Set motor pins as outputs
  pinMode(MOTOR_LEFT_BACK,OUTPUT) ; 
  pinMode(MOTOR_RIGHT_BACK,OUTPUT) ;
  pinMode(MOTOR_RIGHT_FORWARD,OUTPUT) ; 
  pinMode(MOTOR_LEFT_FORWARD,OUTPUT) ;

  // Small delay to stabilize system
  delay(500);
}

void loop() {  
  // ===== ULTRASONIC MEASUREMENT =====
  digitalWrite(TRIG_PIN,LOW);  // ensure clean signal
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN,HIGH); // send 10µs pulse
  delayMicroseconds(10);
  duration = pulseIn(ECHO_PIN, HIGH);    // Read echo time
  distance = duration * 0.034 / 2;       // Convert time to distance (cm)
  
  // ===== OBSTACLE DETECTION =====
  // If object is closer than 15 cm (and valid reading)
  if(distance<15 and distance!=0){  
    // Avoid obstacle (simple path around it)
     turnLeft90();   
     moveForward(500); 
     turnRight90();
     moveForward(500);
     turnRight90();
     moveForward(500);
     turnLeft90(); 
  }
  else{
      // No obstacle → keep moving forward
     moveForward(50);
  }
  
}

// ===== MOVE FORWARD =====
// Moves robot forward for a given time (ms)
void moveForward(int time) {
  analogWrite(MOTOR_LEFT_FORWARD, 200);
  analogWrite(MOTOR_RIGHT_FORWARD, 235);
  delay(time);
  stopMottor();
}

// ===== TURN RIGHT 90° =====
// Rotates robot in place to the right
void turnRight90() {
  digitalWrite(MOTOR_LEFT_FORWARD, HIGH);
  digitalWrite(MOTOR_RIGHT_BACK, HIGH);
  delay(420); // initial guess
  digitalWrite(MOTOR_LEFT_FORWARD, LOW);
  digitalWrite(MOTOR_RIGHT_BACK, LOW);
  stopMottor();
}

// ===== TURN LEFT 90° =====
// Rotates robot in place to the left
void turnLeft90() {
  digitalWrite(MOTOR_RIGHT_FORWARD, HIGH);
  digitalWrite(MOTOR_LEFT_BACK, HIGH);
  delay(420); // initial guess
  digitalWrite(MOTOR_RIGHT_FORWARD, LOW);
  digitalWrite(MOTOR_LEFT_BACK, LOW);
  stopMottor();
}

// ===== STOP MOTORS =====
// Stops all motor movement
void stopMottor(){
  digitalWrite(MOTOR_RIGHT_FORWARD, LOW);
  digitalWrite(MOTOR_LEFT_BACK, LOW);
  digitalWrite(MOTOR_LEFT_FORWARD, LOW);
  digitalWrite(MOTOR_RIGHT_BACK, LOW);
}
