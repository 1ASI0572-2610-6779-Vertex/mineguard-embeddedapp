#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

/**
 * @file LcdDisplay.h
 * @brief 16x2 I2C character LCD actuator for the MineGuard embedded application.
 *
 * Concrete Actuator wrapping LiquidCrystal_I2C. Text is staged per-row through
 * setLine() and pushed to the physical screen with the REFRESH_COMMAND.
 */

#include "Actuator.h"
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

class LcdDisplay : public Actuator {
private:
    LiquidCrystal_I2C lcd;
    uint8_t columns;
    uint8_t rows;
    String lineBuffer[2]; ///< Staged text for each row (16x2).

public:
    static const int REFRESH_COMMAND_ID = 30;
    static const Command REFRESH_COMMAND;

    /**
     * @brief Constructs the LCD actuator.
     * @param address I2C address (e.g., 0x27).
     * @param columns Column count (e.g., 16).
     * @param rows Row count (e.g., 2).
     * @param commandHandler Optional upstream handler (default: nullptr).
     */
    LcdDisplay(uint8_t address, uint8_t columns, uint8_t rows, CommandHandler* commandHandler = nullptr);

    /** @brief Initializes the LCD hardware and backlight. */
    bool initialize();

    /** @brief Stages a text string into a row buffer (truncated/padded to width). */
    void setLine(uint8_t row, const String& text);

    /** @brief Handles REFRESH_COMMAND by committing buffers to the screen. */
    void handle(Command command) override;
};

#endif // LCD_DISPLAY_H
