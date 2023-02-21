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
class EasyButton {
private:
  ezButton _button;
  bool _longPressOnRelease = true;
  unsigned long _pressedTime  = 0;
  unsigned long _releasedTime = 0;
  bool _isPressing     = false;
  bool _isLongDetected = false;

public:
  static const int BUTTON_NOT_PRESSED   = 0;
  static const int BUTTON_PRESSED       = 1;
  static const int BUTTON_SHORT_PRESS   = 2;
  static const int BUTTON_HOLD_PRESS    = 3;
  static const int BUTTON_LONG_PRESS    = 4;
  static const long LONG_PRESS_TIME     = 2000L;  // 2 seconds

#if ENABLE_EASY_BUTTON == 1
  EasyButton(uint8_t pin, bool signalOnRelease = true)
    : _button(pin) {
    _longPressOnRelease = signalOnRelease;
    _button.setDebounceTime(50);  // set debounce time to 50 milliseconds
  }
#else
  // do not initialize the button on the pin
  EasyButton(uint8_t pin, bool signalOnRelease = true)
    : _button(-1) {
    _longPressOnRelease = signalOnRelease;
    _button.setDebounceTime(50);  // set debounce time to 50 milliseconds
  }
#endif
  void begin(int debounce) {
    _button.setDebounceTime(debounce);
  }

  int checkState() {
#if ENABLE_EASY_BUTTON == 1
    _button.loop();  // MUST call the loop() function first
    // track previous state to capture initial press
    bool wasPressed = _isPressing;
    if (_button.isPressed()) {
      //Serial.println(F("button pressed"));
      _pressedTime = millis();
      _isPressing = true;
      _isLongDetected = false;
    }

    if (_button.isReleased() && _isPressing == true) {
      //Serial.println(F("button released"));
      _releasedTime = millis();
      long pressDuration = _releasedTime - _pressedTime;
      //Serial.println(pressDuration);

      // check if we have a short press
      if (pressDuration <= LONG_PRESS_TIME) {
        _isPressing = false;
        _isLongDetected = false;
        //Serial.println(F("A short press is released"));
        return BUTTON_SHORT_PRESS;
      }
      // when configured, return long press on release
      if (_longPressOnRelease && (pressDuration >= LONG_PRESS_TIME)) {
        _isPressing = false;
        _isLongDetected = true;
        //Serial.println(F("A long press is released"));
        return BUTTON_LONG_PRESS;
      }
      if (!_longPressOnRelease && _isLongDetected) {
        _isPressing = false;
        //Serial.println(F("A long press released"));
      }
    }

    // if configured, return long press signal when duration has been reached
    if ((_longPressOnRelease == false) && (_isPressing == true) && (_isLongDetected == false)) {
      long pressDuration = millis() - _pressedTime;

      if (pressDuration > LONG_PRESS_TIME) {
        //Serial.println(F("A long press is detected"));
        _isLongDetected = true;
        return BUTTON_LONG_PRESS;
      }
    }

    // initial press or button held down
    if (_isPressing) {
      if (!wasPressed)
        return BUTTON_PRESSED;
      return BUTTON_HOLD_PRESS;
    }
#endif
    // nothing happening
    return BUTTON_NOT_PRESSED;
  }

  bool pressedLongerThan(int duration) {
    // if the button is still pressed use millis()
    if (_isPressing) {
      return (millis() - _pressedTime) > duration;
    }
    // if the button was released use _releaseTime
    return (_releasedTime - _pressedTime) > duration;
  }
};

#endif
