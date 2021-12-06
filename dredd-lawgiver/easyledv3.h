#ifndef easyledv3_h
#define easyledv3_h

#include <Arduino.h>
#include <FastLED.h>
#include "ezPattern.h"
#include "debug.h"


/**
 * A simple class for managing a collection of WS2812 leds.
 *
 * Use the delcaration to specify the LED configuration at compile time:
 * eg. EasyLedsv3<LED_STRIP_PIN, NUM_LEDS> leds;
 * 
 * In the setup, you can use the begin() function to initialize the brightness.
 * eg. leds.begin(75);
 * 
 * Use various functions control the leds.
 * eg. leds.clear(); leds.fill(CRGB::Red); leds.show();
 * 
 * Led pattern can be activated by passing an ezPattern class. 
 * e.g. Pulsing Stun - blue
 *   ezBlasterShot stun(CRGB::Blue, CRGB::Blue);
 *   leds.activate(stun); // sets the pattern to display
 * 
 * The main functionality is intended to be non-blocking so the leds display is always
 * called in the main loop.
 *   leds.updateDisplay() // should be added to the main loop
 * 
 * REQUIRED LIBRARY: FastLED
 */

template <int LED_COUNT, int LED_PIN_IN>
class EasyLedv3
{
  protected:
    // variable declaration
    CRGB leds[LED_COUNT];
    ezPattern *pattern = 0;

  public:
    //some constants for functions
    const CRGB BLACK = CRGB::Black;
    const CRGB RED = CRGB::Red;
    const CRGB BLUE = CRGB::Blue;
    const CRGB GREEN = CRGB::Green;
    const CRGB ORANGE = CRGB(255, 95,0); //darker orange
    const CRGB YELLOW = CRGB::Yellow;
    const CRGB PURPLE = CRGB::Purple;
    const CRGB WHITE = CRGB::White;

    EasyLedv3() {};
    
    void begin(int brightness) {
      if (LED_COUNT > 0 && LED_PIN_IN > 0) {
        FastLED.addLeds<WS2812, LED_PIN_IN, GRB>(leds, LED_COUNT);
        FastLED.setBrightness(brightness);
        FastLED.setMaxPowerInVoltsAndMilliamps(5, 450); //5v and 450mA
        clear();
      }
    }

    //===============================================================
    // Apply LED color changes
    void clear() {
        FastLED.clear();
        FastLED.show();
    }

    void show() {
      FastLED.show();
    }

    // fill all leds with solid color
    void fill(CRGB color) {
        fill_solid(leds, LED_COUNT, color);
    }

    // Set a LED color (not yet visible)
    void setPixel(int Pixel, byte red, byte green, byte blue) {
      if (Pixel < LED_COUNT) {
        leds[Pixel].r = red;
        leds[Pixel].g = green;
        leds[Pixel].b = blue;
      }
    }

    /**
     * Sets the pattern to activate the leds. Expects an instance of ezPattern.
     * See ezPattern for classes.
     */
    void activate(ezPattern &ptn) {
      debugLog("activating led pattern");
      pattern = &ptn;
      if (pattern)
        pattern->activate(leds, LED_COUNT);
    }

    /**
     * This should be called in the main program loop().
     * The call is a proxy to the ezPattern, if one has been provided.
     */
    void updateDisplay() {
      if(LED_COUNT > 0 && LED_PIN_IN > 0) {
        if (pattern)
          pattern->updateDisplay(leds, LED_COUNT);
      }
    }
};

#endif
