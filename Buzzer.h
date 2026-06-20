#ifndef BUZZER_H
#define BUZZER_H

/**
 * @file Buzzer.h
 * @brief Passive buzzer actuator for the MineGuard embedded application.
 *
 * Concrete Actuator (mirrors the Led pattern) that drives a passive piezo buzzer
 * with TURN_ON (continuous alarm tone) and TURN_OFF commands.
 */

#include "Actuator.h"

class Buzzer : public Actuator {
private:
    bool active;            ///< Current buzzer state.
    unsigned int frequency; ///< Alarm tone frequency in Hz.

public:
    static const int TURN_ON_COMMAND_ID  = 20;
    static const int TURN_OFF_COMMAND_ID = 21;
    static const Command TURN_ON_COMMAND;
    static const Command TURN_OFF_COMMAND;

    /**
     * @brief Constructs the buzzer actuator.
     * @param pin Output GPIO pin connected to the buzzer.
     * @param frequency Alarm tone frequency in Hz (default: 1000).
     * @param commandHandler Optional upstream handler (default: nullptr).
     */
    Buzzer(int pin, unsigned int frequency = 1000, CommandHandler* commandHandler = nullptr);

    /** @brief Handles TURN_ON / TURN_OFF commands. */
    void handle(Command command) override;

    /** @brief Returns whether the buzzer is currently sounding. */
    bool isActive() const;
};

#endif // BUZZER_H
