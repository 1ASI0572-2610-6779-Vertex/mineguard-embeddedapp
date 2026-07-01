#ifndef COLLISION_SENSOR_H
#define COLLISION_SENSOR_H

/**
 * @file CollisionSensor.h
 * @brief KY-031 shock/collision sensor — interrupt-driven (event-based).
 *
 * Instead of polling, the sensor attaches a hardware interrupt on the KY-031
 * digital output (active LOW). Each impact sets a flag inside the ISR; the
 * captured event is later notified when poll()
 * runs in task context (the ISR itself must stay tiny).
 */

#include "MineGuardEvent.h"
#include <Arduino.h>

class CollisionSensor {
private:
    int inputPin;
    volatile bool triggeredFlag;     ///< Set by the ISR on each impact edge.
    bool collisionDetected;          ///< Last consumed state.
    MineGuardEventSink* eventSink;

    static void IRAM_ATTR isrTrampoline(void* arg);

public:
    /**
     * @brief Constructs the collision sensor and attaches its interrupt.
     * @param pin Digital input GPIO connected to the KY-031 DO output.
     * @param sink Optional receiver for collision events.
     */
    CollisionSensor(int pin, MineGuardEventSink* sink = nullptr);

    /** @brief Consumes any interrupt-captured impact and emits the event. */
    void poll();

    /** @brief Returns whether an impact was captured on the last poll. */
    bool getCollisionStatus() const;
};

#endif // COLLISION_SENSOR_H
