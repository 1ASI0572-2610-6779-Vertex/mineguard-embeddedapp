#include "EmergencyButton.h"
#include <Arduino.h>

void IRAM_ATTR EmergencyButton::isrTrampoline(void* arg) {
    EmergencyButton* self = static_cast<EmergencyButton*>(arg);
    self->triggeredFlag = true;
}

EmergencyButton::EmergencyButton(int pin, MineGuardEventSink* sink)
    : inputPin(pin), triggeredFlag(false), pressed(false), eventSink(sink) {
    pinMode(inputPin, INPUT_PULLUP);
    attachInterruptArg(inputPin, isrTrampoline, this, FALLING);
}

void EmergencyButton::poll() {
    noInterrupts();
    bool captured = triggeredFlag;
    triggeredFlag = false;
    interrupts();

    if (captured) {
        pressed = true;
        if (eventSink != nullptr) {
            eventSink->notify(MineGuardEvent::SosPressed);
        }
    } else {
        pressed = false;
    }
}

bool EmergencyButton::isPressed() const {
    return pressed;
}
