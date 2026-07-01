#include "Buzzer.h"
#include <Arduino.h>

Buzzer::Buzzer(int pin, unsigned int frequency)
    : outputPin(pin), active(false), frequency(frequency) {
    pinMode(outputPin, OUTPUT);
    digitalWrite(outputPin, LOW);
}

void Buzzer::handle(BuzzerCommand command) {
    if (command == BuzzerCommand::TurnOn) {
        tone(outputPin, frequency);
        active = true;
    } else if (command == BuzzerCommand::TurnOff) {
        noTone(outputPin);
        active = false;
    }
}

bool Buzzer::isActive() const {
    return active;
}
