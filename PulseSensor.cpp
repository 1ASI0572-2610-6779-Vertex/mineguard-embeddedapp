#include "PulseSensor.h"
#include <Arduino.h>

PulseSensor::PulseSensor(int pin, MineGuardEventSink* sink)
    : inputPin(pin), currentBpm(0), signalAvailable(false), eventSink(sink) {
    pinMode(inputPin, INPUT);
}

void PulseSensor::scanPulse() {
    int raw = analogRead(inputPin); // ESP32 ADC: 0..4095
    if (raw < MIN_VALID_RAW_SIGNAL) {
        signalAvailable = false;
        currentBpm = 0;
        return;
    }

    signalAvailable = true;
    // Map the analog reading to the configured BPM range.
    currentBpm = map(raw, MIN_VALID_RAW_SIGNAL, 4095, BPM_MIN, BPM_MAX);

    if (eventSink == nullptr) {
        return;
    }

    if (currentBpm >= HIGH_BPM_THRESHOLD) {
        eventSink->notify(MineGuardEvent::HighHeartRate);
    } else if (currentBpm < FATIGUE_BPM_THRESHOLD) {
        eventSink->notify(MineGuardEvent::Fatigue);
    }
}

int PulseSensor::getBpm() const {
    return currentBpm;
}

bool PulseSensor::hasSignal() const {
    return signalAvailable;
}
