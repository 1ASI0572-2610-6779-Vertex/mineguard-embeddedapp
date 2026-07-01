#ifndef EMERGENCY_BUTTON_H
#define EMERGENCY_BUTTON_H

/**
 * @file EmergencyButton.h
 * @brief Manual SOS push button — interrupt-driven (event-based).
 *
 * Attaches a hardware interrupt on the button pin (active LOW, internal pull-up).
 * A press latches a flag inside the ISR; poll() (run in task context) notifies
 * the application. No polling of the pin in a busy loop.
 */

#include "MineGuardEvent.h"
#include <Arduino.h>

class EmergencyButton {
private:
    int inputPin;
    volatile bool triggeredFlag;
    bool pressed;
    MineGuardEventSink* eventSink;

    static void IRAM_ATTR isrTrampoline(void* arg);

public:
    /**
     * @brief Constructs the SOS button and attaches its interrupt.
     * @param pin Digital input GPIO connected to the button.
     * @param sink Optional receiver for SOS events.
     */
    EmergencyButton(int pin, MineGuardEventSink* sink = nullptr);

    /** @brief Consumes any interrupt-captured press and emits the event. */
    void poll();

    /** @brief Returns whether the button was pressed on the last poll. */
    bool isPressed() const;
};

#endif // EMERGENCY_BUTTON_H
