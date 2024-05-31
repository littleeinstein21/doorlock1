#include <LiquidCrystal_I2C.h>

class LCDI2C {
public:
    LCDI2C(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows)
        : lcd(lcd_addr, lcd_cols, lcd_rows) {}

    void init() {
        lcd.init();
    }

    void print(const char* message) {
        lcd.print(message);
    }

    void setCursor(uint8_t col, uint8_t row) {
        lcd.setCursor(col, row);
    }

    void clear() {
        lcd.clear();
    }

    void backlight() {
        lcd.backlight();
    }

private:
    LiquidCrystal_I2C lcd;
};
