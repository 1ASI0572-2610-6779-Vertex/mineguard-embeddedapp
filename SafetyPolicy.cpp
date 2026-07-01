#include "SafetyPolicy.h"

MineGuardStatus SafetyPolicy::evaluate(const MineGuardSnapshot& snapshot) const {
    MineGuardStatus status;
    status.proximityAlert = snapshot.distanceCm <= PROXIMITY_ALERT_DISTANCE_CM;
    status.highHeartRateAlert = snapshot.pulseAvailable && snapshot.bpm >= HIGH_BPM_THRESHOLD;
    status.fatigueAlert = snapshot.pulseAvailable && snapshot.bpm < FATIGUE_BPM_THRESHOLD;
    status.collisionAlert = snapshot.collisionActive;
    status.sosAlert = snapshot.sosActive;
    status.danger = status.proximityAlert ||
                    status.highHeartRateAlert ||
                    status.fatigueAlert ||
                    status.collisionAlert ||
                    status.sosAlert;
    return status;
}
