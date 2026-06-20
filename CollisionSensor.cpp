#include "CollisionSensor.h"
#include <Arduino.h>

const Event CollisionSensor::COLLISION_DETECTED_EVENT = Event(CollisionSensor::COLLISION_DETECTED_EVENT_ID);

CollisionSensor::CollisionSensor(int pin, EventHandler* eventHandler)
    : Sensor(pin, eventHandler), collisionDetected(false) {
    pinMode(pin, INPUT_PULLUP); // KY-031 DO is active-low
}

void CollisionSensor::scanCollision() {
    int state = digitalRead(pin);
    if (state == LOW) {
        collisionDetected = true;
        on(COLLISION_DETECTED_EVENT);
    } else {
        collisionDetected = false;
    }
}

bool CollisionSensor::getCollisionStatus() const {
    return collisionDetected;
}
