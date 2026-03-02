# BattleBot

An Arduino-based autonomous robot designed for competitive battles, featuring motor control, rotation sensing, ultrasonic distance detection, LED indicators, and button input controls.

## Features

- **Dual Motor Control**: Independent left and right motor control with forward and reverse capabilities
- **PID Line Following**: Precise straight-line movement using PID control algorithm
- **Rotation Sensing**: Dual rotation sensors for accurate movement tracking and positioning
- **Ultrasonic Distance Detection**: Real-time obstacle detection and distance measurement
- **LED Status Indicators**: Three-color LED system for robot state visualization
- **Button Input Controls**: Three programmable buttons for mode selection and control

## Team

- Justinas Launikonis
- Oleh Haliuk

[GitHub Repository](https://github.com/JustinasLaunikonis/BattleBot-Robobr)

## Pin Configuration

### Motor Pins (Left to Right)
**Wiring: Blue wires required**

| Pin | Function | Description |
|---|---|---|
| 11 | `MOTOR_LEFT_FORWARD` | Left motor forward direction |
| 10 | `MOTOR_LEFT_BACK` | Left motor reverse direction |
| 9 | `MOTOR_RIGHT_BACK` | Right motor reverse direction |
| 6 | `MOTOR_RIGHT_FORWARD` | Right motor forward direction |

*See [PID Function Example](https://github.com/JustinasLaunikonis/BattleBot-Robobr/blob/main/Examples/PID%20-%20Permanent%20Straight%20Line%20Example.ino) for implementation*

### Rotation Sensor Pins
**Wiring: Black wires required**

| Pin | Function | Label | Description |
|---|---|---|---|
| 3 | `ROTATION_SENSOR_LEFT` | R2 | Left wheel rotation tracking |
| 2 | `ROTATION_SENSOR_RIGHT` | R1 | Right wheel rotation tracking |

*See [Rotation Sensor Test](https://github.com/JustinasLaunikonis/BattleBot-Robobr/blob/main/Examples/Rotation%20Sensor%20Test.ino) for testing*

### Ultrasonic Sensor Pins

| Pin | Function | Description |
|---|---|---|
| 7 | `TRIG_PIN` | Ultrasonic trigger signal |
| 8 | `ECHO_PIN` | Ultrasonic echo receive |

### LED Pins (Left to Right)
**Wiring: Wire colors must match LED colors**

| Pin | Function | Color |
|---|---|---|
| 13 | `LED_RED` | Red status indicator |
| 12 | `LED_YELLOW` | Yellow status indicator |
| 8 | `LED_GREEN` | Green status indicator |

*See [Light Example](https://github.com/JustinasLaunikonis/BattleBot-Robobr/blob/main/Examples/Lights%20Example.ino) for usage*

### Button Pins (B1 to B3)
**Wiring: White wires required**

| Pin | Function | Label | Description |
|---|---|---|---|
| 12 | `BUTTON_1` | B1 | Primary control button |
| 8 | `BUTTON_2` | B2 | Secondary control button |
| 7 | `BUTTON_3` | B3 | Tertiary control button |

*Examples: [Hold Down Button](https://github.com/JustinasLaunikonis/BattleBot-Robobr/blob/main/Examples/Hold%20Down%20Button%20Example.ino) | [Press Button](https://github.com/JustinasLaunikonis/BattleBot-Robobr/blob/main/Examples/Press%20Button%20Example.ino)*

## Quick Start

1. Connect all components according to the pin configuration above
2. Ensure wire colors match the requirements (Blue for motors, Black for sensors, White for buttons, LED-matching for indicators)
3. Upload your Arduino sketch with the specified pin constants
4. Test individual components using the provided example sketches
5. Integrate PID control for autonomous movement

## Examples

The repository includes several example sketches to help you get started:

- **PID Control**: Autonomous straight-line movement with error correction
- **Rotation Sensors**: Wheel rotation tracking and distance calculation
- **LED Indicators**: Status display and debugging visualization
- **Button Controls**: Mode selection and manual control input

Visit the [Examples folder](https://github.com/JustinasLaunikonis/BattleBot-Robobr/tree/main/Examples) for complete implementations.

## Authors

Group [TD-INF-IT1C]
- Justinas Launikonis
- Oleh Haliuk