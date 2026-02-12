# Group [TD-INF-IT1C] Battlebot

## Team Members
- Justinas Launikonis<br>
- Oleh Haliuk

[GitHub Repository](https://github.com/JustinasLaunikonis/BattleBot-Robobr)


## Permanent Motor Pins (Left to Right)
<b>Wires must be Blue</b><br>

const int MOTOR_LEFT_FORWARD = 11;<br>
const int MOTOR_LEFT_BACK = 10;<br>
const int MOTOR_RIGHT_BACK = 9;<br>
const int MOTOR_RIGHT_FORWARD = 6;

## Permanent Rotor Sensor Pins
<b>Wires must be Black</b><br>

const int ROTATION_SENSOR_LEFT = 3; (R2)<br>
const int ROTATION_SENSOR_RIGHT = 2; (R1)


## Permanent LED Pins (Left to Right)
<b>Wires must be the same as LED color</b><br>

const int LED_RED = 13;<br>
const int LED_YELLOW = 12;<br>
const int LED_GREEN = 8;

[Light Example](https://github.com/JustinasLaunikonis/BattleBot-Robobr/blob/main/Examples/Lights%20Example.ino)


## Permanent Button Pins (B1 to B3)
<b>Wires must be White</b><br>

const int BUTTON_1 = 12;<br>
const int BUTTON_2 = 8;<br>
const int BUTTON_3 = 7;  

[Hold Down Button Example](https://github.com/JustinasLaunikonis/BattleBot-Robobr/blob/main/Examples/Hold%20Down%20Button%20Example.ino)<br>
[Press Button Example](https://github.com/JustinasLaunikonis/BattleBot-Robobr/blob/main/Examples/Press%20Button%20Example.ino)