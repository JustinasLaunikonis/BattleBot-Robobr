// ===== MOTOR CONTROL PINS =====
const int MOTOR_LEFT_FORWARD = 11;
const int MOTOR_LEFT_BACK = 10;
const int MOTOR_RIGHT_BACK = 9;
const int MOTOR_RIGHT_FORWARD = 6;

// ===== ENCODERS =====
const int ROTATION_SENSOR_LEFT = 3;
const int ROTATION_SENSOR_RIGHT = 2;

volatile long rotationCounterLeft = 0;
volatile long rotationCounterRight = 0;

// ===== SENSOR PINS =====
const int NUM_SENSORS = 8;
const int SENSOR_PINS[NUM_SENSORS] = {A0,A1,A2,A3,A4,A5,A6,A7};

// ===== SERVO =====
int servoPin = 4;

// ===== LINE SENSOR VARIABLES =====
int sensorValues[NUM_SENSORS] = {0};
int sensorMin[NUM_SENSORS];
int sensorMax[NUM_SENSORS];

int lineTransitions = 0;
bool onLine = false;
unsigned long lineStartTime = 0;
const unsigned long lineHoldTime = 120;

int tempThreshold = 500;
int dynamicBlackThreshold = 600;

// ===== PID VARIABLES =====
float Kp = 8.0;
float Ki = 0.5;
float Kd = 2.5;

// ===== MOTOR BIAS =====
int motorBiasLeft = -40;
int motorBiasRight = 10;

// ===== ROBOT STATE =====
int state = 0;
int subState = 0;
unsigned long actionStartTime = 0;

// ===== BASE SPEED =====
const int baseSpeed = 180;

// ===== PID START FLAG =====
bool pidActive = false;
const int minPulsesForPID = 3;

// ===== WHEEL CONSTANTS =====
const float WHEEL_DIAMETER = 65.0; // мм
const int PULSES_PER_ROTATION = 20;

// ===== ROBOT WIDTH FOR TURN =====
const float ROBOT_WIDTH = 1000.0; // мм

// =====================================
// SETUP
// =====================================
void setup() {
  Serial.begin(9600);

  pinMode(servoPin, OUTPUT);

  pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
  pinMode(MOTOR_LEFT_BACK, OUTPUT);
  pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_BACK, OUTPUT);

  stopMotors();

  pinMode(ROTATION_SENSOR_LEFT, INPUT_PULLUP);
  pinMode(ROTATION_SENSOR_RIGHT, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ROTATION_SENSOR_LEFT), onLeftWheelPulse, RISING);
  attachInterrupt(digitalPinToInterrupt(ROTATION_SENSOR_RIGHT), onRightWheelPulse, RISING);

  for(int i=0;i<NUM_SENSORS;i++){
    sensorMin[i] = 1023;
    sensorMax[i] = 0;
  }

  delay(1000);
}

// =====================================
// MAIN LOOP
// =====================================
void loop() {
  switch(state){
    case 0:{
      // ---------- SUBSTATE 0: MOVE STRAIGHT ----------
      if(subState == 0){
        if(!pidActive){
          analogWrite(MOTOR_LEFT_FORWARD, baseSpeed + motorBiasLeft);
          analogWrite(MOTOR_LEFT_BACK, 0);
          analogWrite(MOTOR_RIGHT_FORWARD, baseSpeed + motorBiasRight);
          analogWrite(MOTOR_RIGHT_BACK, 0);

          if(rotationCounterLeft >= minPulsesForPID && rotationCounterRight >= minPulsesForPID){
              pidActive = true;
          }
        } else {
          moveStraightPID(baseSpeed, motorBiasLeft, motorBiasRight);
        }

        readSensors();

        unsigned long now = millis();
        bool allBlack = true;
        for(int i=0;i<NUM_SENSORS;i++){
          if(sensorValues[i] <= tempThreshold){
            allBlack = false;
            break;
          }
        }

        if(allBlack){
          if(!onLine){
            lineStartTime = now;
            onLine = true;
          }
          else if(now - lineStartTime >= lineHoldTime){
            lineTransitions++;
            onLine = false;
            Serial.print("Transition #: ");
            Serial.println(lineTransitions);
          }
        } else {
          onLine = false;
        }

        for(int i=0;i<NUM_SENSORS;i++){
          if(sensorValues[i] < sensorMin[i]) sensorMin[i] = sensorValues[i];
          if(sensorValues[i] > sensorMax[i]) sensorMax[i] = sensorValues[i];
        }

        if(lineTransitions >= 6){
          stopMotors();
          int totalMin = 0;
          int totalMax = 0;
          for(int i=0;i<NUM_SENSORS;i++){
            totalMin += sensorMin[i];
            totalMax += sensorMax[i];
          }
          dynamicBlackThreshold = (totalMin + totalMax) / (2 * NUM_SENSORS);
          subState = 1;
        }
      }

      // ---------- SUBSTATE 1: OPEN GRIPPER ----------
      else if(subState == 1){
        servoWrite(2000);
        if(millis() - actionStartTime > 1000){
          actionStartTime = millis();
          subState = 2;
        }
      }

      // ---------- SUBSTATE 2: DRIVE TO SQUARE ----------
      else if(subState == 2){
        moveStraightPID(baseSpeed, motorBiasLeft, motorBiasRight);
        readSensors();

        int blackCount = 0;
        for(int i=0;i<NUM_SENSORS;i++){
          if(sensorValues[i] > dynamicBlackThreshold) blackCount++;
        }

        if(blackCount >=7){
          stopMotors();
          actionStartTime = millis();
          subState = 3;
        }
      }

      // ---------- SUBSTATE 3: SMALL STOP ----------
      else if(subState == 3){
        if(millis() - actionStartTime > 1000){
          actionStartTime = millis();
          subState = 4;
        }
      }

      // ---------- SUBSTATE 4: CLOSE GRIPPER ----------
      else if(subState == 4){
        servoWrite(1000);
        if(millis() - actionStartTime > 1000){
          actionStartTime = millis();
          subState = 5;
        }
      }

      // ---------- SUBSTATE 5: HOLD OBJECT ----------
      else if(subState == 5){
        servoWrite(1000);
        if(millis() - actionStartTime > 1000){
          subState = 6;
        }
      }

      // ---------- SUBSTATE 6: MOVE 3 CM FORWARD ----------
      else if(subState == 6){
        driveDistance(90.0, baseSpeed); // рух на 3 см прямо перед поворотом
        subState = 7;
      }

      // ---------- SUBSTATE 7: TURN LEFT 90 ----------
      else if(subState == 7){
        turnLeft90();  // твій оригінальний поворот
        subState = 8;
      }

      // ---------- SUBSTATE 8: DRIVE FORWARD AFTER TURN ----------
      else if(subState == 8){
        moveStraightPID(baseSpeed, motorBiasLeft, motorBiasRight);
        readSensors();

        int blackCount = 0;
        for(int i=0;i<NUM_SENSORS;i++){
          if(sensorValues[i] > dynamicBlackThreshold) blackCount++;
        }

        if(blackCount >= 1 && blackCount <= 3){
          stopMotors();
          state = 1;
        }
      }

      break;
    }

    case 1:{
      stopMotors();
      break;
    }

    case 2:{
      stopMotors();
      break;
    }
  }
}

// =====================================
// PID FUNCTION
// =====================================
void moveStraightPID(int baseSpeed, int biasLeft, int biasRight){
  static long lastLeft = 0;
  static long lastRight = 0;
  static float integral = 0;
  static float previousError = 0;

  long currentLeft = rotationCounterLeft;
  long currentRight = rotationCounterRight;

  long speedLeft = currentLeft - lastLeft;
  long speedRight = currentRight - lastRight;

  lastLeft = currentLeft;
  lastRight = currentRight;

  float error = speedLeft - speedRight;
  integral += error;
  float derivative = error - previousError;
  previousError = error;

  float correction = Kp * error + Ki * integral + Kd * derivative;

  int targetLeft = baseSpeed - correction + biasLeft;
  int targetRight = baseSpeed + correction + biasRight;

  targetLeft = constrain(targetLeft, 0, 255);
  targetRight = constrain(targetRight, 0, 255);

  analogWrite(MOTOR_LEFT_FORWARD, targetLeft);
  analogWrite(MOTOR_LEFT_BACK, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, targetRight);
  analogWrite(MOTOR_RIGHT_BACK, 0);
}

// =====================================
// DRIVE DISTANCE WITH PID BALANCE
// =====================================
void driveDistance(float millimeters, int baseSpeed){
  rotationCounterLeft = 0;
  rotationCounterRight = 0;

  float targetPulses = (millimeters / (3.14159 * WHEEL_DIAMETER)) * PULSES_PER_ROTATION;

  while((abs(rotationCounterLeft)+abs(rotationCounterRight))/2 < targetPulses){
    moveStraightPID(baseSpeed, motorBiasLeft, motorBiasRight);
  }

  stopMotors();
}

// =====================================
// TURN LEFT 90
// =====================================
void turnLeft90() {
  rotationCounterLeft = 0;
  rotationCounterRight = 0;

  float wheelCircumference = 3.14159 * WHEEL_DIAMETER;
  float arcLength = (3.14159 * 250) / 4.0; // 90 градусів
  int targetPulses = (arcLength / wheelCircumference) * PULSES_PER_ROTATION;

  while(rotationCounterRight < targetPulses) {
    // зберігаємо баланс між колесами
    long error = rotationCounterRight - rotationCounterLeft;
    int correction = error * 2; // простий P-контроль

    int leftSpeed = constrain(0 - correction + motorBiasLeft, 0, 255);
    int rightSpeed = constrain(200 + correction + motorBiasRight, 0, 255);

    analogWrite(MOTOR_LEFT_FORWARD, leftSpeed);
    analogWrite(MOTOR_LEFT_BACK, 0);
    analogWrite(MOTOR_RIGHT_FORWARD, rightSpeed);
    analogWrite(MOTOR_RIGHT_BACK, 0);
  }

  stopMotors();
  delay(200);
}

// =====================================
// MOTOR FUNCTIONS
// =====================================
void stopMotors(){
  analogWrite(MOTOR_LEFT_FORWARD,0);
  analogWrite(MOTOR_LEFT_BACK,0);
  analogWrite(MOTOR_RIGHT_FORWARD,0);
  analogWrite(MOTOR_RIGHT_BACK,0);
}

// =====================================
// ENCODERS
// =====================================
void onLeftWheelPulse(){ rotationCounterLeft++; }
void onRightWheelPulse(){ rotationCounterRight++; }

// =====================================
// SERVO
// =====================================
void servoWrite(int pulseWidth){
  unsigned long start = millis();
  while(millis() - start < 20){
    digitalWrite(servoPin,HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(servoPin,LOW);
    delayMicroseconds(20000 - pulseWidth);
  }
}

// =====================================
// SENSOR READ
// =====================================
void readSensors(){
  for(int i=0;i<NUM_SENSORS;i++){
    sensorValues[i] = analogRead(SENSOR_PINS[i]);
  }
}
