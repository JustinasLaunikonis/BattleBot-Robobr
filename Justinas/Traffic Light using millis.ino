/*
  Traffic Light using millis()

  Sequence:
  - Red    ON for 3 seconds
  - Green  ON for 4 seconds
  - Yellow ON for 1 second
  - Repeat forever
*/

const int LedPinRed = 11;
const int LedPinYellow = 10;
const int LedPinGreen = 9;

const int ButtonPin = 7;

//stores the time (in ms) when the last light change happened
unsigned long previousMillis = 0;

//create named states for the traffic light
enum LightState {
  RED_IDLE,
  WAIT_GREEN,
  GREEN,
  YELLOW
};

LightState currentState = RED_IDLE;

void setup() {
  pinMode(LedPinRed, OUTPUT);
  pinMode(LedPinYellow, OUTPUT);
  pinMode(LedPinGreen, OUTPUT);

  pinMode(ButtonPin, INPUT);

  //red light on at start
  setLights(LOW, HIGH, HIGH); 

  previousMillis = millis();
}

void loop() {
  
  //get the current time since the Arduino started (in milliseconds)
  unsigned long currentMillis = millis();
  int buttonState = digitalRead(ButtonPin);

  switch (currentState) {
    
    case RED_IDLE:
      setLights(LOW, HIGH, HIGH);  // Red Idle
      
      if (buttonState == LOW) {
        previousMillis = currentMillis;
        currentState = WAIT_GREEN;
      }
      break;

    case WAIT_GREEN:
      if (currentMillis - previousMillis >= 1000) {
        previousMillis = currentMillis;
        currentState = GREEN;
        setLights(HIGH, HIGH, LOW); // Green On
      }
      break;

    case GREEN:
      if (currentMillis - previousMillis >= 4000) {
        previousMillis = currentMillis;
        currentState = YELLOW;
        setLights(HIGH, LOW, HIGH);  // Yellow On
      }
      break;

    case YELLOW:
      if (currentMillis - previousMillis >= 1000) {
        previousMillis = currentMillis;
        currentState = RED_IDLE;
        setLights(LOW, HIGH, HIGH);  // Back to Red
      }
      break;
  }
}

// Turns LED pins ON or OFF with one function
void setLights(bool red, bool yellow, bool green) {
  digitalWrite(LedPinRed, red);
  digitalWrite(LedPinYellow, yellow);
  digitalWrite(LedPinGreen, green);
}
