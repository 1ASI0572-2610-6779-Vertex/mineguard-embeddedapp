#ifndef MINEGUARD_EVENT_H
#define MINEGUARD_EVENT_H

/**
 * @file MineGuardEvent.h
 * @brief Event contracts used by MineGuard input adapters.
 */

enum class MineGuardEvent {
    ObjectNearby,
    HighHeartRate,
    Fatigue,
    CollisionDetected,
    SosPressed
};

class MineGuardEventSink {
public:
    virtual void notify(MineGuardEvent event) = 0;
    virtual ~MineGuardEventSink() = default;
};

#endif // MINEGUARD_EVENT_H
