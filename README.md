# MineGuard — Embedded Application

Mining-safety IoT node for ESP32, built on the Modest-IoT Nano-framework
(C++ Edition) following the same architecture as the BykerZ example: a
self-contained, event-driven device that scans sensors, drives local alerts,
and streams telemetry to an Edge Service over HTTP.

## Architecture
- **Framework core** (unmodified, by Angel Velasquez): `EventHandler`,
  `CommandHandler`, `Sensor`, `Actuator`, `Device`, `Led`.
- **Sensors** (extend `Sensor`, expose `scanXxx()` + getters + events):
  `UltrasonicSensor`, `PulseSensor`, `CollisionSensor`, `EmergencyButton`, `GPSSensor`.
- **Actuators** (extend `Actuator`, command-driven): `Led`, `Buzzer`, `LcdDisplay`.
- **Application mediator**: `MineGuardDevice` (`on`, `handle`, `update`, `initialize`).
- **Runtime canvas**: `mineguard-sketch.ino` (WiFi, Edge Service auth, sensing loop).

## Pin layout (mineguard_V2.1 Wokwi)
| Peripheral        | Pin(s)            |
|-------------------|-------------------|
| HC-SR04           | TRIG 5 / ECHO 18  |
| Pulse (pot)       | 32                |
| KY-031 collision  | 4                 |
| SOS button        | 13                |
| Red LED / Green   | 2 / 15            |
| Buzzer            | 23                |
| LCD I2C (0x27)    | SDA 21 / SCL 22   |
| GPS (UART2)       | RX 16 / TX 17     |

## Telemetry payload (POST /api/v1/metrics)
`minerId, latitude, longitude, distanceCm, heartRateBpm, proximityAlert,
collisionDetected, sosActive`

## Setup
1. Wokwi libraries: ArduinoJson, TinyGPSPlus, LiquidCrystal_I2C (see libraries.txt).
2. Set `ssid`/`password` (Wokwi-GUEST by default) and the Edge Service URLs in the sketch.
3. Flash and open the serial monitor at 115200 baud.
