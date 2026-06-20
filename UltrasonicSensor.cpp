#include "UltrasonicSensor.h"
#include <Arduino.h>

const Event UltrasonicSensor::OBJECT_NEARBY_EVENT = Event(UltrasonicSensor::OBJECT_NEARBY_EVENT_ID);

UltrasonicSensor::UltrasonicSensor(int trigPin, int echoPin, EventHandler* eventHandler)
    : Sensor(trigPin, eventHandler), echoPin(echoPin), distanceCm(MAX_RANGE_CM) {
    pinMode(pin, OUTPUT);     // trigger
    pinMode(echoPin, INPUT);  // echo
}

void UltrasonicSensor::scanDistance() {
    digitalWrite(pin, LOW);
    delayMicroseconds(2);
    digitalWrite(pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(pin, LOW);

    long duration = pulseIn(echoPin, HIGH, 25000); // 25 ms timeout
    if (duration == 0) {
        distanceCm = MAX_RANGE_CM; // no echo -> treat as far / safe
    } else {
        distanceCm = (duration * 0.0343f) / 2.0f;
        if (distanceCm > MAX_RANGE_CM) distanceCm = MAX_RANGE_CM;
    }

    if (distanceCm <= PROXIMITY_THRESHOLD_CM) {
        on(OBJECT_NEARBY_EVENT);
    }
}

float UltrasonicSensor::getDistance() const {
    return distanceCm;
}
