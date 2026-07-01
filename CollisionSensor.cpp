#include "CollisionSensor.h"
#include <Arduino.h>

void IRAM_ATTR CollisionSensor::isrTrampoline(void* arg) {
    // Keep the ISR minimal: just latch the event flag.
    CollisionSensor* self = static_cast<CollisionSensor*>(arg);
    self->triggeredFlag = true;
}

CollisionSensor::CollisionSensor(int pin, MineGuardEventSink* sink)
    : inputPin(pin), triggeredFlag(false), collisionDetected(false), eventSink(sink) {
    pinMode(inputPin, INPUT_PULLUP); // KY-031 DO is active-low
    attachInterruptArg(inputPin, isrTrampoline, this, FALLING);
}

void CollisionSensor::poll() {
    // Consume the interrupt-captured flag atomically.
    noInterrupts();
    bool captured = triggeredFlag;
    triggeredFlag = false;
    interrupts();

    if (captured) {
        collisionDetected = true;
        if (eventSink != nullptr) {
            eventSink->notify(MineGuardEvent::CollisionDetected);
        }
    } else {
        collisionDetected = false;
    }
}

bool CollisionSensor::getCollisionStatus() const {
    return collisionDetected;
}
