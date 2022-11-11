#ifndef easybutton_h
#define easybutton_h

#include <Arduino.h>
#include <ezButton.h>

/**
 * Use EasyButton to track state on a specific pin.
 * It uses the onboard resistor (INPUT_PULL).
 * Supports tracking following conditions:
 *   BUTTON_NOT_PRESSED - no state change
 *   BUTTON_PRESSED - initial state change
 *   BUTTON_SHORT_PRESS - Short Press on release
 *   BUTTON_LONG_PRESS - Long Press on release
 *   BUTTON_HOLD_PRESS - Pressed and Hold
 * eg. EasyButton selectorOnPin8(8);
 *
 * Call the begin() function in the setup to initilize the pin mode, and set a debouce value.
 * eg. selectorOnPin8.begin(25);
 * 
 * Check the state, and use the defines to determine current state:
 * eg.int selectedMode = selectorOnPin8.checkState()
 * 
 * REQUIRED LIBRARIES - ezButton
 */
class EasyButton
{
  private:
    ezButton button;
    bool longPressOnRelease = true;
    unsigned long pressedTime  = 0;
    unsigned long releasedTime = 0;
    bool isPressing = false;
    bool isLongDetected = false;

  public:

    static const PROGMEM int BUTTON_NOT_PRESSED = 0;
    static const PROGMEM int BUTTON_PRESSED     = 1;
    static const PROGMEM int BUTTON_SHORT_PRESS = 2;
    static const PROGMEM int BUTTON_HOLD_PRESS  = 3;
    static const PROGMEM int BUTTON_LONG_PRESS  = 4;

    static const PROGMEM uint16_t  LONG_PRESS_TIME PROGMEM  = 2000; // 2 seconds

#if ENABLE_EASY_BUTTON == 1
    EasyButton(uint8_t pin, bool signalOnRelease = true) : button(pin) {
      longPressOnRelease = signalOnRelease;
      button.setDebounceTime(50); // set debounce time to 50 milliseconds
    }
#else
    // do not initialize the button on the pin
    EasyButton(uint8_t pin, bool signalOnRelease = true) : button(-1) {
      longPressOnRelease = signalOnRelease;
      button.setDebounceTime(50); // set debounce time to 50 milliseconds
    }
#endif
    void begin(int debounce) {
      button.setDebounceTime(debounce);
    }

    int checkState() {
#if ENABLE_EASY_BUTTON == 1
      button.loop(); // MUST call the loop() function first
      // track previous state to capture initial press
      bool wasPressed = isPressing;
      if(button.isPressed()){
        //Serial.println(F("button pressed"));
        pressedTime = millis();
        isPressing = true;
        isLongDetected = false;
      }
    
      if(button.isReleased() && isPressing == true) {
        //Serial.println(F("button released"));
        releasedTime = millis();
        long pressDuration = releasedTime - pressedTime;
        //Serial.println(pressDuration);

        // check if we have a short press
        if( pressDuration <= LONG_PRESS_TIME ) {
          isPressing = false;
          isLongDetected = false;
          //Serial.println(F("A short press is released"));
          return BUTTON_SHORT_PRESS;
        }
        // when configured, return long press on release
        if(longPressOnRelease && (pressDuration >= LONG_PRESS_TIME)) {
          isPressing = false;
          isLongDetected = true;
          //Serial.println(F("A long press is released"));
          return BUTTON_LONG_PRESS;
        }
        if(!longPressOnRelease && isLongDetected) {
          isPressing = false;
          //Serial.println(F("A long press released"));
        }
      }

      // if configured, return long press signal when duration has been reached
      if((longPressOnRelease == false) && (isPressing == true) && (isLongDetected == false)) {
        long pressDuration = millis() - pressedTime;
    
        if( pressDuration > LONG_PRESS_TIME ) {
          //Serial.println(F("A long press is detected"));
          isLongDetected = true;
          return BUTTON_LONG_PRESS;
        }
      }

      // initial press or button held down
      if (isPressing) {
        if (!wasPressed)
          return BUTTON_PRESSED;
        return BUTTON_HOLD_PRESS;
      }
#endif
      // nothing happening
      return BUTTON_NOT_PRESSED;
    }
};

#endif
