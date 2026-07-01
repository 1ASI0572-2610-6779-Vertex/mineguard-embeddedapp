#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

/**
 * @file LcdDisplay.h
 * @brief 16x2 I2C character LCD actuator for the MineGuard embedded application.
 *
 * Text is staged per row through setLine() and pushed to the physical screen
 * with refresh().
 */

#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

class LcdDisplay {
private:
    LiquidCrystal_I2C lcd;
    uint8_t columns;
    uint8_t rows;
    String lineBuffer[2]; ///< Staged text for each row (16x2).

public:
    /**
     * @brief Constructs the LCD actuator.
     * @param address I2C address (e.g., 0x27).
     * @param columns Column count (e.g., 16).
     * @param rows Row count (e.g., 2).
     */
    LcdDisplay(uint8_t address, uint8_t columns, uint8_t rows);

    /** @brief Initializes the LCD hardware and backlight. */
    bool initialize();

    /** @brief Stages a text string into a row buffer (truncated/padded to width). */
    void setLine(uint8_t row, const String& text);

    /** @brief Commits the staged text buffers to the screen. */
    void refresh();
};

#endif // LCD_DISPLAY_H
