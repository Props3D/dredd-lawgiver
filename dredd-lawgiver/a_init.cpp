/*
   name:  Lawgiver 2012
   Author:  FriskyDingo
   Date:  2021-11-21
   Description: Code for Lawgiver setup
*/
#include <Arduino.h>
#include "config.h"
#include "debug.h"
#include "easybutton.h"
#include "easycounter.h"
#include "easyaudio.h"
#include "easyledv3.h"
#include "easyoled.h"
#include "easyvoice.h"

/**
   All components are controlled or enabled by "config.h". Before running,
   review and change all configurations based on your setup.

   There's no need to change any of the following code or functions.
*/
// Counters for each firing mode
EasyCounter apCounter("ap");
EasyCounter inCounter("in");
EasyCounter heCounter("he");
EasyCounter fmjCounter("fmj");

EasyAudio audio(AUDIO_RX_PIN, AUDIO_TX_PIN);
//EasyButton trigger(TRIGGER_PIN);

EasyLedv3<FIRE_LED_CNT, FIRE_LED_PIN> fireLed;
ezBlasterShot blasterShot(fireLed.RED, fireLed.ORANGE, 4);  // initialize colors to starting fire mode


EasyOLED<OLED_CS_PIN, OLED_DC_PIN, OLED_RESET_PIN> oled(DISPLAY_USER_ID);
EasyVoice<VOICE_RECORDS_ARR, VOICE_RECORDS_ARR_SZ> voice(VOICE_RX_PIN, VOICE_TX_PIN);

// function declarations
void initLedIndicators(void);
void startUpSequence(void);
void runAudioPlayback(void);
void runOledDisplay(void);
void runLedDisplay(void);
void handleFireTrigger(void);
void handleSelectorMode(void);
void sendBlasterPulse(void);
EasyCounter& getTriggerCounter(void);
uint8_t getSelectedTrack(uint8_t idx);
void ammoDown(void);

/**
 *  Variables for tracking selected trigger mode.
 *  Variables must be marked as volatile because they are updated in the ISR.
 */
volatile uint8_t selectedTriggerMode   = SELECTOR_FMJ_MODE;   // sets the fire mode to blaster to start
volatile long lastTriggerTimeDebounce = 0;                    // handling trigger bounce
const static long debounceTriggerTimeField = 250;             // time measured between bounces

void setup() {
  Serial.begin (115200);
  debugLog("Starting setup");

  //initializes the audio player and sets the volume
  audio.begin(25);

  // initialize all the leds
  initLedIndicators();
  // initialize the trigger led and set brightness
  fireLed.begin(75);

  // init the display
  selectedTriggerMode = SELECTOR_FMJ_MODE;
  // Initialize the clip counters for different modes
  apCounter.begin(0, 25, COUNTER_MODE_DOWN);
  inCounter.begin(0, 25, COUNTER_MODE_DOWN);
  heCounter.begin(0, 25, COUNTER_MODE_DOWN);
  fmjCounter.begin(0, 50, COUNTER_MODE_DOWN);
  uint8_t counters[4] = {apCounter.getCount(), inCounter.getCount(), heCounter.getCount(), fmjCounter.getCount()};
  oled.begin(selectedTriggerMode, counters);

  // init the voice module
  voice.begin();

  // attach the interrupts
#ifndef ENABLE_EASY_BUTTON
  debugLog("Attach Interrupts");
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  noInterrupts();
  attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), ammoDown, FALLING);
  interrupts();
#endif
// set up the fire trigger and the debounce threshold
#ifdef ENABLE_EASY_BUTTON
  trigger.begin(25);
#endif

}

void initLedIndicators(void) {
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
}

void ammoIndicators(void) {
  int ammo = getTriggerCounter().getCount();
  if (ammo < 4) {
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);    
  } else {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
  }
}

/**
 * The main loop.
 */
void loop () {
  // run the start up sequence first
  if (millis() < STARTUP_END_MS) {
    startUpSequence();
    runOledDisplay();
    runAudioPlayback();
  } else {
    // always handle the activated components first, before processing new inputs
    runOledDisplay();
    runAudioPlayback();
    runLedDisplay();
    // check for new inputs
    handleSelectorMode();
  }
}

/**
 * 
 */
void startUpSequence(void) {
  // Logo
  if (millis() < STARTUP_LOGO_MS) {
    debugLog("OLED display - LOGO");
    oled.setDisplayMode(oled.DISPLAY_LOGO);
  }
  // Comm OK Sequence
  if (millis() > STARTUP_LOGO_MS && millis() < STARTUP_COMM_OK_MS) {
    digitalWrite(RED_LED_PIN, HIGH);
    debugLog("OLED display - Comm OK");
    oled.setDisplayMode(oled.DISPLAY_COMM_CHK);
  }
  // switch to DNA Check and wait a second
  if (millis() > STARTUP_COMM_OK_MS && millis() < STARTUP_DNA_CHK_MS) {
    debugLog("OLED display - DNA Check");
    oled.setDisplayMode(oled.DISPLAY_DNA_CHK);
  }

  // DNA Check Sequence
  bool buttonState = true; //trigger.checkState() == trigger.BUTTON_HOLD_PRESS;
  if (buttonState && millis() > STARTUP_DNA_CHK_MS && millis() < STARTUP_DNA_PRG_MS) {
    if (oled.currentDisplayMode() != oled.DISPLAY_DNA_PRG) {
      audio.queuePlayback(TRACK_DNA_CHK);
    }
    debugLog("OLED display - DNA Progress");
    oled.setDisplayMode(oled.DISPLAY_DNA_PRG);
  }
  if (!buttonState && millis() > STARTUP_DNA_CHK_MS && millis() < STARTUP_DNA_PRG_MS) {
    if (oled.currentDisplayMode() == oled.DISPLAY_DNA_PRG) {
      audio.queuePlayback(TRACK_DNA_FAIL);
    }
    debugLog("OLED display - DNA Check FAILED");
    oled.setDisplayMode(oled.DISPLAY_ID_FAIL);
  }

  if (oled.currentDisplayMode() != oled.DISPLAY_ID_FAIL) {
    if (millis() > STARTUP_DNA_PRG_MS && millis() < STARTUP_ID_OK_MS) {
      debugLog("OLED display - ID OK");
      oled.setDisplayMode(oled.DISPLAY_ID_OK);
      // blink the green
      bool _blink = (millis() % (500 + 500) < 500);
      if (_blink) {
          digitalWrite(RED_LED_PIN, LOW);
          digitalWrite(GREEN_LED_PIN, HIGH);
      } else {
          digitalWrite(RED_LED_PIN, LOW);
          digitalWrite(GREEN_LED_PIN, LOW);
      }
    }
    // Display Judge Name
    if (millis() > STARTUP_ID_OK_MS && millis() < STARTUP_JUDGE_NAME_MS) {
      debugLog("OLED display - ID NAME");
      oled.setDisplayMode(oled.DISPLAY_ID_NAME);
    }

    // Firing Mode
    if (millis() > STARTUP_JUDGE_NAME_MS) {
      debugLog("OLED display - Main");
      oled.setDisplayMode(oled.DISPLAY_MAIN);
      digitalWrite(GREEN_LED_PIN, LOW);
    }
  }
}
/**
   Playback the next queued track
*/
void runAudioPlayback(void) {
  audio.playQueuedTrack();
}
/**
   Run the led pattern
*/
void runLedDisplay(void) {
  fireLed.updateDisplay();
}

/**
   Run the oled pattern
*/
void runOledDisplay(void) {
  oled.updateDisplay(selectedTriggerMode, getTriggerCounter().getCount());
}

/**
 * Checks the fire trigger momentary switch.
 */
void ammoDown(void) {
    if ((millis() - lastTriggerTimeDebounce) > debounceTriggerTimeField) {
    if (digitalRead(TRIGGER_PIN) == LOW) {
      sendBlasterPulse();
    }
    lastTriggerTimeDebounce = millis();
  }
}
/**
   Checks the fire trigger momentary switch.
   Short press should send an alternating blaster pulse
   Long press should change modes between A/B and C/D
*/
/*
void handleFireTrigger(void) {
  // check trigger button
  int buttonStateFire = trigger.checkState();
  // check if a trigger is pressed.
  if (buttonStateFire == trigger.BUTTON_SHORT_PRESS) {
    sendBlasterPulse(getTriggerCounter());
  }
  if (buttonStateFire == trigger.BUTTON_LONG_PRESS) {
    // ignore
  }
}
*/

/**
   Sends a blaster pulse.
     1. Toggles a clip counter
     2. Checks for an empty clip
     3. If clip is not empty
        a. play audio track
        b. update led strip
        c. update led counter to the current clip counter
     4. Otherwise play empty clip track
*/
void sendBlasterPulse(void) {
  debugLog("send alternating blaster pulse");
  bool emptyClip = getTriggerCounter().isEmpty();
  // check the track number before ticking, in case it's the last round
  int trackIdx = getTriggerCounter().getState(); // returns the approapriate track, or empty clip
  // move the counter, if possible
  getTriggerCounter().tick();
  
  // We redo this when the clip is full to get the reload track
  //if (emptyClip && trackIdx != getTriggerCounter().getState())
  //  trackIdx = getTriggerCounter().getState();   

  // trigger the low-ammo indicators
  ammoIndicators();
  //queue the track
  audio.queuePlayback(getSelectedTrack(trackIdx));
  if (emptyClip == false) {
    fireLed.activate(blasterShot);
  }
}

uint8_t getSelectedTrack(uint8_t idx) {
  if (selectedTriggerMode == SELECTOR_AP_MODE)
    return TRACK_AP_ARR[idx];
  if (selectedTriggerMode == SELECTOR_IN_MODE)
    return TRACK_IN_ARR[idx];
  if (selectedTriggerMode == SELECTOR_HE_MODE)
    return TRACK_HE_ARR[idx];
  if (selectedTriggerMode == SELECTOR_HS_MODE)
    return TRACK_HS_ARR[idx];
  if (selectedTriggerMode == SELECTOR_ST_MODE)
    return TRACK_ST_ARR[idx];
  if (selectedTriggerMode == SELECTOR_RAPID_MODE)
    return TRACK_RAPID_ARR[idx];
  return TRACK_FMJ_ARR[idx];
}

EasyCounter& getTriggerCounter(void) {
  if (selectedTriggerMode == SELECTOR_AP_MODE)
    return apCounter;
  if (selectedTriggerMode == SELECTOR_IN_MODE)
    return inCounter;
  if (selectedTriggerMode == SELECTOR_HS_MODE)
    return inCounter;
  if (selectedTriggerMode == SELECTOR_HE_MODE)
    return heCounter;
  return fmjCounter;
}

/**
   Checks the selector mode momentary switch.
   1. if a long preess plackback the theme track
   2. If a short press
     a. playback changee mode track
     b. toggle fire mode
*/
void handleSelectorMode(void) {
  int cmd = voice.readCommand();
  
  if (cmd > -1) {
    selectedTriggerMode = cmd;
    // Check for Switching modes
    if (selectedTriggerMode == SELECTOR_AP_MODE) {
      blasterShot.initialize(fireLed.RED, fireLed.ORANGE);  // shot - flash with color fade
      debugLog("Armor Piercing Mode selected");
    }
    if (selectedTriggerMode == SELECTOR_IN_MODE) {
      blasterShot.initialize(fireLed.ORANGE, fireLed.WHITE);  // shot - flash with color fade
      debugLog("Incendiary Mode selected");
    }
    if (selectedTriggerMode == SELECTOR_HE_MODE) {
      blasterShot.initialize(fireLed.ORANGE, fireLed.WHITE);  // shot - flash with color fade
      debugLog("High Ex Mode selected");
    }
    if (selectedTriggerMode == SELECTOR_HS_MODE) {
      blasterShot.initialize(fireLed.RED, fireLed.ORANGE);  // shot - flash with color fade
      debugLog("Hotshot Mode selected");
    }
    if (selectedTriggerMode == SELECTOR_ST_MODE) {
      blasterShot.initialize(fireLed.YELLOW, fireLed.WHITE);  // shot - flash with color fade
      debugLog("Stun Mode selected");
    }
    if (selectedTriggerMode == SELECTOR_FMJ_MODE) {
      blasterShot.initialize(fireLed.RED, fireLed.ORANGE);  // shot - flash with color fade
      debugLog("FMJ Mode selected");
    }
    if (selectedTriggerMode == SELECTOR_RAPID_MODE) {
      blasterShot.initialize(fireLed.RED, fireLed.ORANGE);  // shot - flash with color fade
      debugLog("FMJ Mode selected");
    }
    audio.queuePlayback(TRACK_CHANGE_MODE);
  }

}
