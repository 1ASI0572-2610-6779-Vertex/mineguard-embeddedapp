#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "CommandHandler.h"

class Actuator : public CommandHandler {
protected:
    int pin; ///< GPIO pin assigned to the actuator.
    CommandHandler* handler; ///< Optional handler to receive propagated commands.

public:
    /**
     * @brief Constructs an Actuator with a pin and optional command handler.
     * @param pin The GPIO pin for the actuator.
     * @param commandHandler Pointer to a CommandHandler to receive commands (default: nullptr).
     */
    Actuator(int pin, CommandHandler* commandHandler = nullptr);

    /**
     * @brief Handles a command by propagating it to the assigned handler.
     * @param command The command to handle.
     */
    void handle(Command command) override;

    /**
     * @brief Sets or updates the command handler for this actuator.
     * @param commandHandler Pointer to the new CommandHandler.
     */
    void setHandler(CommandHandler* commandHandler);
};


#endif //ACTUATOR_H