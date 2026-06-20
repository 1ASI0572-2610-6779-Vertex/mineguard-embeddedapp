#ifndef MINEGUARD_DEVICE_H
#define MINEGUARD_DEVICE_H

/**
 * @file MineGuardDevice.h
 * @brief Application mediator for the MineGuard mining-safety IoT node.
 *
 * Mirrors the Modest-IoT "Device" application pattern: it owns every sensor and
 * actuator, reacts to their events via on(), drives local alert indicators, and
 * pushes telemetry to an Edge Service through an injected metric callback.
 *
 * Wokwi wiring (mineguard_V2.1):
 *   HC-SR04  TRIG:5  ECHO:18      Pulse/Pot: 32
 *   KY-031:  4       SOS btn: 13
 *   Red LED: 2       Green LED: 15      Buzzer: 23
 *   LCD I2C: SDA:21 SCL:22 (0x27)      GPS UART2: RX:16 TX:17
 */

#include "Device.h"
#include "UltrasonicSensor.h"
#include "PulseSensor.h"
#include "CollisionSensor.h"
#include "EmergencyButton.h"
#include "GPSSensor.h"
#include "Led.h"
#include "Buzzer.h"
#include "LcdDisplay.h"

class MineGuardDevice : public Device {
private:
    // -- Actuators --
    Led         dangerLed;
    Led         safeLed;
    Buzzer      buzzer;
    LcdDisplay  display;

    // -- Sensors --
    UltrasonicSensor proximity;
    PulseSensor      pulse;
    CollisionSensor  collision;
    EmergencyButton  sos;
    GPSSensor        gps;

    // -- Latched alert windows (momentary events held for visibility) --
    unsigned long collisionHoldUntil;
    unsigned long sosHoldUntil;

    // -- Telemetry throttling --
    unsigned long lastMetricSentAt;
    bool          metricPending;

    // -- Edge Service callback: (lat, lng, distanceCm, bpm, proximity, collision, sos) --
    void (*metricCallback)(double, double, float, int, bool, bool, bool);

    void refreshIndicators(bool danger);
    void refreshDisplay(bool proximityAlert, bool hrAlert,
                        bool collisionActive, bool sosActive, int bpm);

public:
    // -- Pin layout (matches the MineGuard Wokwi diagram) --
    static const int RED_LED_PIN    = 2;
    static const int GREEN_LED_PIN  = 15;
    static const int BUZZER_PIN     = 23;
    static const uint8_t LCD_ADDRESS = 0x27;
    static const int TRIG_PIN       = 5;
    static const int ECHO_PIN       = 18;
    static const int PULSE_PIN      = 32;
    static const int COLLISION_PIN  = 4;
    static const int SOS_PIN        = 13;
    static const int GPS_SERIAL     = 2;  // UART2
    static const int GPS_RX_PIN     = 16;
    static const int GPS_TX_PIN     = 17;

    // -- Behaviour timings --
    static const unsigned long COLLISION_HOLD_MS = 3000;
    static const unsigned long SOS_HOLD_MS       = 5000;
    static const unsigned long METRIC_MIN_INTERVAL_MS = 5000;

    MineGuardDevice(
        int redLedPin   = RED_LED_PIN,
        int greenLedPin = GREEN_LED_PIN,
        int buzzerPin   = BUZZER_PIN,
        uint8_t lcdAddr = LCD_ADDRESS,
        int trigPin     = TRIG_PIN,
        int echoPin     = ECHO_PIN,
        int pulsePin    = PULSE_PIN,
        int collisionPin = COLLISION_PIN,
        int sosPin      = SOS_PIN,
        int gpsSerial   = GPS_SERIAL,
        int gpsRx       = GPS_RX_PIN,
        int gpsTx       = GPS_TX_PIN
    );

    // -- Framework contract --
    void on(Event event) override;
    void handle(Command command) override;

    // -- Application lifecycle --
    bool initialize();
    void update();
    void setMetricCallback(void (*callback)(double, double, float, int, bool, bool, bool));

    // -- Accessors --
    Led& getDangerLed();
    Led& getSafeLed();
    Buzzer& getBuzzer();
    LcdDisplay& getDisplay();
    UltrasonicSensor& getProximitySensor();
    PulseSensor& getPulseSensor();
    CollisionSensor& getCollisionSensor();
    EmergencyButton& getEmergencyButton();
    GPSSensor& getGPSSensor();
};

#endif // MINEGUARD_DEVICE_H
