#ifndef ezpattern_h
#define ezpattern_h

#include <FastLED.h>

typedef void (*callback_function)(void); // type for conciseness

/**
 *  A set of classes for producing LED patterns.
 *
 *  Each pattern has it's own constructor. Some exmaples:
 *     ezBlasterShot hotshot(CRGB:Red, CRGB::Orange); // red fade to orange
 *     ezBlasterPulse bluepulse(CRGB:Blue, 2);        // 2 pixel blue animation
 *     ezBlasterPulse bluepulse(CRGB:Blue, 2);        // 2 pixel blue animation
 *
 *  A pattern is passed to an EasyLedv3 to control the LED set.
 *  e.g. leds.activate(hotshot);
 *
 *  REQUIRED LIBRARY: FastLED
 */

/**
 *  Interface for building different LED patterns and behaviours
 */
class ezPattern {
  protected:
    callback_function _callbackPtr = 0;     // pointer to callback function
    volatile uint8_t _activated    = 0;     // signal when the pattern should be active

    uint8_t _frameRate             = 16;    // larger number is a slower fade
    unsigned long _flashTimer      = 0;     // time when the white flash started

    uint8_t _flashDuration              = 50;    // larger number will hold a white flash longer
    static const uint8_t _delta         = 1;     // Sets forward or backwards direction amount.
    static const uint8_t _fadeRate      = 220;   // How fast to fade out tail. [0-255]

    // fucntion declartions
    void show() {
      FastLED.show();
    }
    void clear(CRGB *leds, uint8_t count) {
      fill_solid(leds, count, CRGB::Black);
    }
    void completed(struct CRGB *leds, uint8_t count) {
      clear(leds, count);
      if (_callbackPtr) _callbackPtr();
    }
    void whiteflash(CRGB *leds, int count) {
      _flashTimer = millis(); // capture the time for timing the flash
      fill_solid(leds, count, CRGB::White);
      show();
    }
    void fadeToBlack(CRGB *leds, int count) {
      fadeToBlackBy(leds, count, _fadeRate);
    }
  public:
    virtual void activate(CRGB *leds, uint8_t count) = 0;
    virtual void updateDisplay(CRGB *leds, uint8_t count) = 0;
    virtual ~ezPattern() = default;
};

/**
 *  Single blaster shot
 *  1. Flash White
 *  2. Starting color with fade to secondary color
 *  3. fade to black to show cooling
 */
class ezBlasterShot : public ezPattern
{
  protected:
    CRGB _currentColor;  // color
    CRGB _startColor;    // starting color
    CRGB _targetColor;   // target color
    const CRGB _coolOffColor = CRGB::Black;

    // processing variables
    uint8_t _blendRate     = 32;  // larger number will be a faster color blend
    uint8_t _blendSteps    = 1;   // larger number will be a slower color blend

    uint8_t calcBlendRate(CRGB& cur, const CRGB& target) {
      uint8_t red = delta(cur.red, target.red) / _blendSteps;
      uint8_t green = delta(cur.green, target.green) / _blendSteps;
      uint8_t blue = delta(cur.blue, target.blue) / _blendSteps;
      return max(red, max(green, blue));
    }

    inline uint8_t delta(uint8_t a, uint8_t b) {
      return (a < b) ? b - a : a - b;
    }

    // helper functions
    bool checkShotCooled(CRGB *leds, uint8_t count) {
      if ((_activated == 1) && (_currentColor == _coolOffColor)) {
        _activated = 0;
        this->completed(leds, count);
        this->show();
        return true;
      }
      return false;
    }

    bool checkShotBlended(CRGB *leds, uint8_t count) {
      if ((_activated == 2) && (_currentColor == _targetColor)) {
        // on match, we set to target to black to fade out
        _activated = 1;    // phase 2
        _blendRate = calcBlendRate(_startColor, _coolOffColor);    // reset blend rate
        return true;
      }
      return false;
    }

    bool coolingShot(CRGB *leds, uint8_t count) {
      if (_activated == 1) {
        fadeTowardColor(_currentColor, _coolOffColor, _blendRate);
        fill_solid(leds, count, _currentColor);
        this->show();
        return true;
      }
      return false;
    }

    bool blendingShot(CRGB *leds, uint8_t count) {
      if (_activated == 2) {
        fadeTowardColor(_currentColor, _targetColor, _blendRate);
        fill_solid(leds, count, _currentColor);
        this->show();
        return true;
      }
      return false;
    }

    bool checkWhiteFlash(CRGB *leds, uint8_t count) {
      if (_activated == 4) {
        this->whiteflash(leds, count);
        _currentColor = CRGB(_startColor.r, _startColor.g, _startColor.b);
        _activated = 3;   // hold the flash
        return true;
      }
      if (_activated == 3) {
        long duration = millis() - _flashTimer;
        if (duration > _flashDuration) {
          _activated = 2;   // start the fade
          _blendRate = calcBlendRate(_startColor, _targetColor);    // reset blend rate
        }
        return true;
      }
      return false;
    }

    /**
     *  Blend one CRGB color toward another CRGB color by a given amount.
     *  Blending is linear, and done in the RGB color space.
     *
     *  NOTE: This function modifies 'cur' color in place, and returns the same object.
     */
    CRGB fadeTowardColor( CRGB& cur, const CRGB& target, uint8_t amount) {
      nblendU8TowardU8( cur.red,   target.red,   amount);
      nblendU8TowardU8( cur.green, target.green, amount);
      nblendU8TowardU8( cur.blue,  target.blue,  amount);
      return cur;
    }

    /**
     *   Helper function that blends one uint8_t toward another by a given amount
     */
    void nblendU8TowardU8( uint8_t& cur, const uint8_t target, uint8_t amount) {
      if ( cur == target) return;

      if ( cur < target ) {
        uint8_t delta = target - cur;
        if (amount < delta) {
          //delta = scale8_video(delta, amount);
          delta = amount;
          cur += delta;
        } else
          cur = target;
      } else {
        uint8_t delta = cur - target;
        if (amount < delta) {
          //delta = scale8_video(delta, amount);
          delta = amount;
          cur -= delta;
        } else
          cur = target;
      }
    }

  public:
    ezBlasterShot(CRGB initialColor, CRGB endColor, uint8_t speed = 6, callback_function callback = 0) {
      initialize(initialColor, endColor);
      _frameRate = 30;
      _callbackPtr = callback;
      _blendSteps = max (speed, 1);
    }
    ~ezBlasterShot() {
      _callbackPtr = 0;
    }

    void initialize(CRGB initialColor, CRGB endColor) {
      _startColor = CRGB(initialColor.r, initialColor.g, initialColor.b);
      _targetColor = CRGB(endColor.r, endColor.g, endColor.b);
    }

    void activate(CRGB *leds, uint8_t count) {
      //Serial.println(F("BlasterShot - activated"));
      _activated = 3;    // start with white flash and color fade
      //reset the current color to the start
      _currentColor = CRGB(_startColor.r, _startColor.g, _startColor.b);
      this->whiteflash(leds, count);
    }

    void updateDisplay(CRGB *leds, uint8_t count) {
      EVERY_N_MILLISECONDS(_frameRate) {
        // stop fading and clear
        if (checkShotCooled(leds, count)) {
          //Serial.println(F("BlasterShot - ending blaster shot"));
        }
        // fade to black
        if (checkShotBlended(leds, count)) {
          //Serial.println(F("BlasterShot - fade to black on blaster shot"));
        }
        if (coolingShot(leds, count)) {
          //Serial.println(F("BlasterShot - Cooling off blaster shot"));
        }
        if (blendingShot(leds, count)) {
          //Serial.println(F("BlasterShot - blending colors on blaster shot"));
        }
        if (checkWhiteFlash(leds, count)) {
          //Serial.println(F("BlasterShot - checking white flash"));
        }
      }
    }
};

/**
 *  Repeating blaster shot
 *  1. Flash Color
 *  2. Black out and repeat 6 times
 *  3. Fade to black after number of repitions
 */
class ezBlasterRepeatingShot : public ezBlasterShot
{
  protected:
    const uint8_t _maxRepetitions;   // Number of times to fire

    // processing variables
    uint8_t _repetitions  = _maxRepetitions;

    bool blendingShot(CRGB *leds, uint8_t count) {
      return false;
    }

    bool checkWhiteFlash(CRGB *leds, uint8_t count) {
      if (_activated == 4) {
        this->whiteflash(leds, count);
        _currentColor = CRGB(_startColor.r, _startColor.g, _startColor.b);
        _activated = 3;   // hold the flash
        return true;
      }
      if (_activated == 3) {
        long duration = millis() - _flashTimer;
        if (duration > _flashDuration) {
          if (_repetitions > 0) {
            _activated = 4;   // clear and flash again
            _repetitions--;
            this->clear(leds, count);
            this->show();
            if (_repetitions == 0) {
              _activated = 1;   // fade to black
            }
          }
        }
        return true;
      }
      return false;
    }

  public:
    ezBlasterRepeatingShot(uint8_t reps = 8, uint8_t speed = 6, callback_function callback = 0) : ezBlasterRepeatingShot(CRGB::White, reps, speed, callback) {}
    ezBlasterRepeatingShot(CRGB initialColor, uint8_t reps = 8, uint8_t speed = 6, callback_function callback = 0) : _maxRepetitions(reps), ezBlasterShot(initialColor, CRGB::Black, speed, callback) {
      _repetitions = _maxRepetitions;
      _flashDuration = 25;
      _frameRate = 5;
    }
    ~ezBlasterRepeatingShot() {
      _callbackPtr = 0;
    }

    void activate(CRGB *leds, uint8_t count) {
      //Serial.println(F("StrobeShot - activated"));
      _repetitions = _maxRepetitions;
      //reset the current color to the start
      _currentColor = CRGB(_startColor.r, _startColor.g, _startColor.b);
      //this->whiteflash(leds, count);
      _activated = 4;    // start with white flash and hold
    }

    void updateDisplay(CRGB *leds, uint8_t count) {
      EVERY_N_MILLISECONDS(_frameRate) {
        // stop fading and clear
        if (checkShotCooled(leds, count)) return;

        // fade to black
        if (coolingShot(leds, count)) return;

        // strobe the flash
        checkWhiteFlash(leds, count);
      }
    }
};

#endif
