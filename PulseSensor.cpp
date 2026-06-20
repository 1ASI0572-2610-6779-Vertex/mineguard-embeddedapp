#include "PulseSensor.h"
#include <Arduino.h>

const Event PulseSensor::HIGH_HEART_RATE_EVENT = Event(PulseSensor::HIGH_HEART_RATE_EVENT_ID);
const Event PulseSensor::FATIGUE_EVENT         = Event(PulseSensor::FATIGUE_EVENT_ID);

PulseSensor::PulseSensor(int pin, EventHandler* eventHandler)
    : Sensor(pin, eventHandler), currentBpm(0) {
    pinMode(pin, INPUT);
}

void PulseSensor::scanPulse() {
    int raw = analogRead(pin); // ESP32 ADC: 0..4095
    // Map the analog dial to a physiological BPM range for the simulation
    currentBpm = map(raw, 0, 4095, BPM_MIN, BPM_MAX);

    if (currentBpm >= HIGH_BPM_THRESHOLD) {
        on(HIGH_HEART_RATE_EVENT);
    } else if (currentBpm < FATIGUE_BPM_THRESHOLD) {
        on(FATIGUE_EVENT);
    }
}

int PulseSensor::getBpm() const {
    return currentBpm;
}
