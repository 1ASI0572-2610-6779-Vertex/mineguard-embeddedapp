#include "LcdDisplay.h"

LcdDisplay::LcdDisplay(uint8_t address, uint8_t columns, uint8_t rows)
    : lcd(address, columns, rows), columns(columns), rows(rows) {
    lineBuffer[0] = "";
    lineBuffer[1] = "";
}

bool LcdDisplay::initialize() {
    lcd.init();
    lcd.backlight();
    lcd.clear();
    return true;
}

void LcdDisplay::setLine(uint8_t row, const String& text) {
    if (row >= rows) return;
    String padded = text;
    while (padded.length() < columns) padded += ' ';      // pad
    if (padded.length() > columns) padded = padded.substring(0, columns); // truncate
    lineBuffer[row] = padded;
}

void LcdDisplay::refresh() {
    for (uint8_t r = 0; r < rows; ++r) {
        lcd.setCursor(0, r);
        lcd.print(lineBuffer[r]);
    }
}
