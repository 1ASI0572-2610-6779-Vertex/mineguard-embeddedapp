#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

struct Event {
    int id; ///< Unique identifier for the event type.

    explicit Event(int eventId) : id(eventId) {}
    bool operator==(const Event& other) const { return id == other.id; }
};

class EventHandler {
public:
    virtual void on(Event event) = 0; ///< Pure virtual method to handle an event.
    virtual ~EventHandler() = default; ///< Virtual destructor for safe inheritance.
};

#endif //EVENT_HANDLER_H