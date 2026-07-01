# MineGuard Embedded Application

MineGuard is an event-driven ESP32 embedded application for miner safety
monitoring. It reads distance, pulse, collision, SOS, and GPS inputs, then
updates the LCD, LEDs, buzzer, and optional telemetry callback only when the
device state changes.

## Hardware Pins

| Peripheral | ESP32 Pin |
| --- | --- |
| GPS UART2 | RX2 = GPIO16, TX2 = GPIO17 |
| SOS button | GPIO19 |
| Pulse sensor | GPIO32 |
| Buzzer | GPIO23 |
| LCD I2C | SDA = GPIO21, SCL = GPIO22 |
| Distance sensor | TRIG = GPIO5, ECHO = GPIO18 |
| Red LED | GPIO15 |
| Green LED | GPIO2 |
| Collision sensor | GPIO4 |

## Safety Rules

- Distance alarm activates only when the measured distance is `20 cm` or less.
- High heart-rate alarm activates at `100 BPM` or more.
- Fatigue alarm activates below `55 BPM`.
- Collision and SOS are interrupt-captured, then held briefly on the LCD so the event is visible.
- The buzzer uses a lower `450 Hz` tone to reduce perceived loudness.

## Architecture

- `SafetyPolicy` contains MineGuard domain rules.
- `MineGuardDevice` coordinates application state, event handling, display updates, and telemetry.
- Sensor classes isolate input hardware.
- Actuator classes isolate LED, LCD, and buzzer output behavior.
- The Arduino `loop()` is intentionally idle; runtime work is handled by interrupts and a timer tick.

## Arduino Dependencies

Install these dependencies from the Arduino Library Manager:

- TinyGPSPlus
- LiquidCrystal_I2C

## Edge Service Connection

1. Run the edge service on the computer connected to the same WiFi network.
   The service must listen on `0.0.0.0`, not only on `127.0.0.1`, so the ESP32 can reach it:

```sh
flask --app app run --host 0.0.0.0 --port 5001
```

2. Find that computer's LAN IP address.

3. Update these constants in `mineguard-embeddedapp.ino`:

```cpp
static const char* WIFI_SSID = "YOUR_WIFI_SSID";
static const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
static const char* EDGE_BASE_URL = "http://YOUR_COMPUTER_IP:5001";
```

Use the edge seeded credentials unless you changed them:

```cpp
static const char* EDGE_DEVICE_ID = "smart-band-001";
static const char* EDGE_API_KEY = "test-api-key-123";
```

The embedded app posts telemetry to:

```http
POST /api/v1/telemetry
X-API-Key: test-api-key-123
Content-Type: application/json
```

Example payload:

```json
{
  "device_id": "smart-band-001",
  "bpm": 72,
  "distance_cm": 120.0,
  "collision": false,
  "lat": -12.046374,
  "lng": -77.042793
}
```
