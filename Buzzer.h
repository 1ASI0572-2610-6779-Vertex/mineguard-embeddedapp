#ifndef BUZZER_H
#define BUZZER_H

/**
 * @file Buzzer.h
 * @brief Passive buzzer actuator for the MineGuard embedded application.
 *
 * Drives a passive piezo buzzer with a restrained alarm tone.
 */

#include <Arduino.h>

enum class BuzzerCommand {
    TurnOn,
    TurnOff
};

class Buzzer {
private:
    int outputPin;
    bool active;            ///< Current buzzer state.
    unsigned int frequency; ///< Alarm tone frequency in Hz.

public:
    /**
     * @brief Constructs the buzzer actuator.
     * @param pin Output GPIO pin connected to the buzzer.
     * @param frequency Alarm tone frequency in Hz (default: 1000).
     */
    Buzzer(int pin, unsigned int frequency = 1000);

    /** @brief Handles TURN_ON / TURN_OFF commands. */
    void handle(BuzzerCommand command);

    /** @brief Returns whether the buzzer is currently sounding. */
    bool isActive() const;
};

#endif // BUZZER_H
