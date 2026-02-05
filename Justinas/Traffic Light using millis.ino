/*
  Traffic Light using millis()

  Sequence:
  - Red    ON for 3 seconds
  - Green  ON for 4 seconds
  - Yellow ON for 1 second
  - Repeat forever
*/

const int ledPinRed = 11;
const int ledPinYellow = 10;
const int ledPinGreen = 9;

//stores the time (in ms) when the last light change happened
unsigned long previousMillis = 0;

//create named states for the traffic light
enum LightState {
  RED,        // Red LED is active
  YELLOW,      // Yellow LED is active
  GREEN      // Green LED is active
};

//keeps track of the current active light
LightState currentState = RED;

void setup() {
  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinYellow, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);

  //red light on at start
  previousMillis = millis();
  setLights(LOW, HIGH, HIGH); 
}

void loop() {
  
  //get the current time since the Arduino started (in milliseconds)
  unsigned long currentMillis = millis();

  switch (currentState) {
    
    case RED:
      if (currentMillis - previousMillis >= 3000) {
        previousMillis = currentMillis;
        currentState = GREEN;
        setLights(HIGH, HIGH, LOW);  // GREEN ON
      }

    case GREEN:
      if (currentMillis - previousMillis >= 4000) {
        previousMillis = currentMillis;
        currentState = YELLOW;
        setLights(HIGH, LOW, HIGH);  // YELLOW ON
      }
      break;

    case YELLOW:
      if (currentMillis - previousMillis >= 1000) {
        previousMillis = currentMillis;
        currentState = RED;
        setLights(LOW, HIGH, HIGH);  // RED ON
      }
      break;
  }
}

// Turns LED pins ON or OFF with one function
void setLights(bool red, bool yellow, bool green) {
  digitalWrite(ledPinRed, red);
  digitalWrite(ledPinYellow, yellow);
  digitalWrite(ledPinGreen, green);
}
