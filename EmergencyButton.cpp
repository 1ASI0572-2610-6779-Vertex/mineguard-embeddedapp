#include "EmergencyButton.h"
#include <Arduino.h>

const Event EmergencyButton::SOS_PRESSED_EVENT = Event(EmergencyButton::SOS_PRESSED_EVENT_ID);

EmergencyButton::EmergencyButton(int pin, EventHandler* eventHandler)
    : Sensor(pin, eventHandler), pressed(false) {
    pinMode(pin, INPUT_PULLUP);
}

void EmergencyButton::scanButton() {
    int state = digitalRead(pin);
    if (state == LOW) {
        pressed = true;
        on(SOS_PRESSED_EVENT);
    } else {
        pressed = false;
    }
}

bool EmergencyButton::isPressed() const {
    return pressed;
}
