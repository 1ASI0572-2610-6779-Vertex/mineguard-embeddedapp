#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

/**
 * @file UltrasonicSensor.h
 * @brief HC-SR04 proximity sensor for the MineGuard embedded application.
 *
 * Concrete Sensor that measures distance via trigger/echo timing and raises an
 * OBJECT_NEARBY_EVENT when an obstacle is detected within the danger threshold.
 */

#include "Sensor.h"

class UltrasonicSensor : public Sensor {
private:
    int echoPin;        ///< GPIO pin reading the echo pulse.
    float distanceCm;   ///< Last measured distance in centimeters.

public:
    static const int OBJECT_NEARBY_EVENT_ID = 10;
    static const Event OBJECT_NEARBY_EVENT;

    static const int PROXIMITY_THRESHOLD_CM = 40;  ///< Danger distance in cm.
    static const int MAX_RANGE_CM           = 400; ///< Sensor max range clamp.

    /**
     * @brief Constructs the ultrasonic sensor.
     * @param trigPin Trigger output GPIO pin.
     * @param echoPin Echo input GPIO pin.
     * @param eventHandler Optional upstream handler (default: nullptr).
     */
    UltrasonicSensor(int trigPin, int echoPin, EventHandler* eventHandler = nullptr);

    /** @brief Performs a ranging cycle and fires OBJECT_NEARBY_EVENT if too close. */
    void scanDistance();

    /** @brief Returns the last measured distance in centimeters. */
    float getDistance() const;
};

#endif // ULTRASONIC_SENSOR_H
