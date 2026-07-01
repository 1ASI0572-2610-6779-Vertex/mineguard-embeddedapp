#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

/**
 * @file UltrasonicSensor.h
 * @brief HC-SR04 proximity sensor for the MineGuard embedded application.
 *
 * Measures distance via trigger/echo timing and notifies ObjectNearby when an
 * obstacle is detected within the danger threshold.
 */

#include "MineGuardEvent.h"

class UltrasonicSensor {
private:
    int triggerPin;     ///< GPIO pin sending the trigger pulse.
    int echoPin;        ///< GPIO pin reading the echo pulse.
    float distanceCm;   ///< Last measured distance in centimeters.
    MineGuardEventSink* eventSink;

public:
    static const int PROXIMITY_THRESHOLD_CM = 20;  ///< Danger distance in cm.
    static const int MAX_RANGE_CM           = 400; ///< Sensor max range clamp.

    /**
     * @brief Constructs the ultrasonic sensor.
     * @param trigPin Trigger output GPIO pin.
     * @param echoPin Echo input GPIO pin.
     * @param sink Optional receiver for proximity events.
     */
    UltrasonicSensor(int trigPin, int echoPin, MineGuardEventSink* sink = nullptr);

    /** @brief Performs a ranging cycle and notifies when an obstacle is too close. */
    void scanDistance();

    /** @brief Returns the last measured distance in centimeters. */
    float getDistance() const;
};

#endif // ULTRASONIC_SENSOR_H
