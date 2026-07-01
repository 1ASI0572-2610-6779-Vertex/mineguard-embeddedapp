#include "Led.h"
#include <Arduino.h>

Led::Led(int pin, bool initialState)
    : outputPin(pin), state(initialState) {
    pinMode(outputPin, OUTPUT);
    digitalWrite(outputPin, state);
}

void Led::handle(LedCommand command) {
    if (command == LedCommand::Toggle) {
        state = !state;
        digitalWrite(outputPin, state);
    } else if (command == LedCommand::TurnOn) {
        state = true;
        digitalWrite(outputPin, state);
    } else if (command == LedCommand::TurnOff) {
        state = false;
        digitalWrite(outputPin, state);
    }
}

bool Led::getState() const {
    return state;
}

void Led::setState(bool newState) {
    state = newState;
    digitalWrite(outputPin, state);
}
