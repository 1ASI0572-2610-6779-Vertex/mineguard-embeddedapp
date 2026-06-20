#include "Buzzer.h"
#include <Arduino.h>

const Command Buzzer::TURN_ON_COMMAND  = Command(Buzzer::TURN_ON_COMMAND_ID);
const Command Buzzer::TURN_OFF_COMMAND = Command(Buzzer::TURN_OFF_COMMAND_ID);

Buzzer::Buzzer(int pin, unsigned int frequency, CommandHandler* commandHandler)
    : Actuator(pin, commandHandler), active(false), frequency(frequency) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void Buzzer::handle(Command command) {
    if (command == TURN_ON_COMMAND) {
        tone(pin, frequency); // continuous alarm until silenced
        active = true;
    } else if (command == TURN_OFF_COMMAND) {
        noTone(pin);
        active = false;
    }
    Actuator::handle(command); // propagate to handler if set
}

bool Buzzer::isActive() const {
    return active;
}
