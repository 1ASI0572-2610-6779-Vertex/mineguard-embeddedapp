#ifndef COLLISION_SENSOR_H
#define COLLISION_SENSOR_H

/**
 * @file CollisionSensor.h
 * @brief KY-031 shock/collision sensor for the MineGuard embedded application.
 *
 * Concrete Sensor that reads the KY-031 digital output (active LOW) and raises a
 * COLLISION_DETECTED_EVENT when an impact pulse is observed.
 */

#include "Sensor.h"

class CollisionSensor : public Sensor {
private:
    bool collisionDetected; ///< True when the last scan saw an impact.

public:
    static const int COLLISION_DETECTED_EVENT_ID = 13;
    static const Event COLLISION_DETECTED_EVENT;

    /**
     * @brief Constructs the collision sensor.
     * @param pin Digital input GPIO pin connected to the KY-031 DO output.
     * @param eventHandler Optional upstream handler (default: nullptr).
     */
    CollisionSensor(int pin, EventHandler* eventHandler = nullptr);

    /** @brief Samples the pin and fires COLLISION_DETECTED_EVENT on an impact. */
    void scanCollision();

    /** @brief Returns whether an impact was detected on the last scan. */
    bool getCollisionStatus() const;
};

#endif // COLLISION_SENSOR_H
