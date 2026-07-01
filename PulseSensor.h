#ifndef PULSE_SENSOR_H
#define PULSE_SENSOR_H

/**
 * @file PulseSensor.h
 * @brief Heart-rate sensor for the MineGuard embedded application.
 *
 * Reads an analog heart-rate input and maps it to beats per minute. Notifies
 * the application when the BPM indicates overexertion or fatigue.
 */

#include "MineGuardEvent.h"

class PulseSensor {
private:
    int inputPin;    ///< Analog input GPIO pin.
    int currentBpm; ///< Last computed heart rate in beats per minute.
    bool signalAvailable;
    MineGuardEventSink* eventSink;

public:
    static const int MIN_VALID_RAW_SIGNAL = 30;   ///< Ignores disconnected or idle ADC readings.
    static const int BPM_MIN              = 40;   ///< Mapped lower bound.
    static const int BPM_MAX              = 180;  ///< Mapped upper bound.
    static const int HIGH_BPM_THRESHOLD   = 100;  ///< Overexertion threshold.
    static const int FATIGUE_BPM_THRESHOLD = 55;  ///< Fatigue threshold.

    /**
     * @brief Constructs the pulse sensor.
     * @param pin Analog input GPIO pin.
     * @param sink Optional receiver for heart-rate events.
     */
    PulseSensor(int pin, MineGuardEventSink* sink = nullptr);

    /** @brief Samples the analog input and fires HR events on threshold crossings. */
    void scanPulse();

    /** @brief Returns the last computed heart rate in BPM. */
    int getBpm() const;

    /** @brief Returns whether the last analog sample looked like a valid pulse signal. */
    bool hasSignal() const;
};

#endif // PULSE_SENSOR_H
