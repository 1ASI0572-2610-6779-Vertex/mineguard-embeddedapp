#ifndef SAFETY_POLICY_H
#define SAFETY_POLICY_H

/**
 * @file SafetyPolicy.h
 * @brief Domain rules for the MineGuard embedded safety state.
 *
 * Keeps alert decisions independent from GPIO, LCD, GPS, and networking code.
 * The application layer provides the latest sensor snapshot and receives a
 * normalized status with the active alert flags.
 */

struct MineGuardSnapshot {
    float distanceCm;
    int bpm;
    bool pulseAvailable;
    bool collisionActive;
    bool sosActive;
};

struct MineGuardStatus {
    bool proximityAlert;
    bool highHeartRateAlert;
    bool fatigueAlert;
    bool collisionAlert;
    bool sosAlert;
    bool danger;
};

class SafetyPolicy {
public:
    static const int PROXIMITY_ALERT_DISTANCE_CM = 20;
    static const int HIGH_BPM_THRESHOLD = 100;
    static const int FATIGUE_BPM_THRESHOLD = 55;

    /**
     * @brief Evaluates current sensor values against MineGuard safety rules.
     * @param snapshot Latest measured embedded-device state.
     * @return Computed alert state for indicators, telemetry, and display.
     */
    MineGuardStatus evaluate(const MineGuardSnapshot& snapshot) const;
};

#endif // SAFETY_POLICY_H
