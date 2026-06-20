#ifndef EMERGENCY_BUTTON_H
#define EMERGENCY_BUTTON_H

/**
 * @file EmergencyButton.h
 * @brief Manual SOS push button for the MineGuard embedded application.
 *
 * Concrete Sensor that reads a momentary push button (active LOW with internal
 * pull-up) and raises an SOS_PRESSED_EVENT when the operator requests help.
 */

#include "Sensor.h"

class EmergencyButton : public Sensor {
private:
    bool pressed; ///< True when the button is held on the last scan.

public:
    static const int SOS_PRESSED_EVENT_ID = 14;
    static const Event SOS_PRESSED_EVENT;

    /**
     * @brief Constructs the emergency button.
     * @param pin Digital input GPIO pin connected to the button.
     * @param eventHandler Optional upstream handler (default: nullptr).
     */
    EmergencyButton(int pin, EventHandler* eventHandler = nullptr);

    /** @brief Samples the button and fires SOS_PRESSED_EVENT while held. */
    void scanButton();

    /** @brief Returns whether the button was held on the last scan. */
    bool isPressed() const;
};

#endif // EMERGENCY_BUTTON_H
