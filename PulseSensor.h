#ifndef PULSE_SENSOR_H
#define PULSE_SENSOR_H

/**
 * @file PulseSensor.h
 * @brief Heart-rate sensor for the MineGuard embedded application.
 *
 * Reads an analog input (a potentiometer in the Wokwi simulation acting as a
 * heart-rate dial) and maps it to a beats-per-minute value. Raises a
 * HIGH_HEART_RATE_EVENT on overexertion and a FATIGUE_EVENT on an abnormally
 * low rate.
 */

#include "Sensor.h"

class PulseSensor : public Sensor {
private:
    int currentBpm; ///< Last computed heart rate in beats per minute.

public:
    static const int HIGH_HEART_RATE_EVENT_ID = 11;
    static const int FATIGUE_EVENT_ID         = 12;
    static const Event HIGH_HEART_RATE_EVENT;
    static const Event FATIGUE_EVENT;

    static const int BPM_MIN              = 40;   ///< Mapped lower bound.
    static const int BPM_MAX              = 180;  ///< Mapped upper bound.
    static const int HIGH_BPM_THRESHOLD   = 100;  ///< Overexertion threshold.
    static const int FATIGUE_BPM_THRESHOLD = 55;  ///< Fatigue threshold.

    /**
     * @brief Constructs the pulse sensor.
     * @param pin Analog input GPIO pin.
     * @param eventHandler Optional upstream handler (default: nullptr).
     */
    PulseSensor(int pin, EventHandler* eventHandler = nullptr);

    /** @brief Samples the analog input and fires HR events on threshold crossings. */
    void scanPulse();

    /** @brief Returns the last computed heart rate in BPM. */
    int getBpm() const;
};

#endif // PULSE_SENSOR_H
