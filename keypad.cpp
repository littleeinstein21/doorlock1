#ifndef KEYPADCONTROLLER_H
#define KEYPADCONTROLLER_H

#include <Arduino.h>
#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
    {'A', 'B', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {4, 5, 6, 7};
byte colPins[COLS] = {8, 9, 10, 11};

class KeypadController {
public:
    KeypadController()
        : keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS) {}

    char getKey() {
        return keypad.getKey();
    }

private:
    Keypad keypad;
};

#endif // KEYPADCONTROLLER_H
