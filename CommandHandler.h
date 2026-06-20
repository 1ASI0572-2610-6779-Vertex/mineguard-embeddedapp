#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

struct Command {
    int id; ///< Unique identifier for the command type.

    explicit Command(int commandId) : id(commandId) {}
    bool operator==(const Command& other) const { return id == other.id; }
};

class CommandHandler {
public:
    virtual void handle(Command command) = 0; ///< Pure virtual method to process a command.
    virtual ~CommandHandler() = default; ///< Virtual destructor for safe inheritance.
};

#endif //COMMAND_HANDLER_H