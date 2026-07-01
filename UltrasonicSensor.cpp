#include "UltrasonicSensor.h"
#include <Arduino.h>

UltrasonicSensor::UltrasonicSensor(int trigPin, int echoPin, MineGuardEventSink* sink)
    : triggerPin(trigPin), echoPin(echoPin), distanceCm(MAX_RANGE_CM), eventSink(sink) {
    pinMode(triggerPin, OUTPUT);
    pinMode(echoPin, INPUT);  // echo
}

void UltrasonicSensor::scanDistance() {
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);

    long duration = pulseIn(echoPin, HIGH, 25000); // 25 ms timeout
    if (duration == 0) {
        distanceCm = MAX_RANGE_CM; // no echo -> treat as far / safe
    } else {
        distanceCm = (duration * 0.0343f) / 2.0f;
        if (distanceCm > MAX_RANGE_CM) distanceCm = MAX_RANGE_CM;
    }

    if (distanceCm <= PROXIMITY_THRESHOLD_CM && eventSink != nullptr) {
        eventSink->notify(MineGuardEvent::ObjectNearby);
    }
}

float UltrasonicSensor::getDistance() const {
    return distanceCm;
}
