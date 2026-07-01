#ifndef LED_H
#define LED_H

/**
 * @file Led.h
 * @brief LED indicator actuator for MineGuard.
 *
 * Controls MineGuard LED indicators through toggle, on, and off commands.
 */

#include <Arduino.h>

enum class LedCommand {
    Toggle,
    TurnOn,
    TurnOff
};

class Led {
private:
    int outputPin;
    bool state; ///< Current state of the LED (true = ON, false = OFF).

public:
    /**
     * @brief Constructs an Led actuator.
     * @param pin The GPIO pin for the LED (configured as OUTPUT).
     * @param initialState Initial state of the LED (default: false/OFF).
     */
    Led(int pin, bool initialState = false);

    /**
     * @brief Handles commands to control the LED state.
     * @param command The command to execute.
     */
    void handle(LedCommand command);

    /**
     * @brief Gets the current state of the LED.
     * @return True if the LED is ON, false if OFF.
     */
    bool getState() const;

    /**
     * @brief Sets the LED state directly.
     * @param newState The new state (true = ON, false = OFF).
     */
    void setState(bool newState);
};

#endif // LED_H
