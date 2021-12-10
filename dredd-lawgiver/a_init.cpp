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
EasyCounter apCounter("fire");
EasyCounter inCounter("incend");
EasyCounter heCounter("highex");
EasyCounter fmjCounter("stun");

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
void sendBlasterPulse(EasyCounter &counter);
EasyCounter& getTriggerCounter(void);
uint8_t getSelectedTrack(uint8_t idx);
void ammoDown(void);

// trigger mode status
volatile uint8_t selectedTriggerMode   = SELECTOR_FMJ_MODE;   // sets the fire mode to blaster to start
long lastTriggerTimeDebounce = 0;
const static long debounceTriggerTimeField = 50;

void setup() {
  Serial.begin (115200);
  debugLog("Starting setup");

  // set up the fire trigger and the debounce threshold
  //trigger.begin(25);

  //initializes the audio player and sets the volume
  audio.begin(25);

  // initialize all the leds
  initLedIndicators();
  // initialize the trigger led and set brightness
  fireLed.begin(75);

  // init the display
  selectedTriggerMode = SELECTOR_FMJ_MODE;
  // Initialize the clip counters for different modes
  apCounter.begin(0, 25, COUNTER_MODE_DOWN, false);
  inCounter.begin(0, 25, COUNTER_MODE_DOWN, false);
  heCounter.begin(0, 25, COUNTER_MODE_DOWN, false);
  fmjCounter.begin(0, 50, COUNTER_MODE_DOWN, false);
  uint8_t counters[4] = {apCounter.getCount(), inCounter.getCount(), heCounter.getCount(), fmjCounter.getCount()};
  oled.begin(selectedTriggerMode, counters);

  // init the voice module
  voice.begin();

  // attach the interrupts
#ifndef ENABLE_EASY_BUTTON
  debugLog("Attach Interrupts");
  noInterrupts();
  attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), ammoDown, RISING);
  interrupts();
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
    digitalWrite(GREEN_LED_PIN, HIGH);
  }
}

/**
 * The main loop.
 */
void loop () {
  // run the start up sequence first
  if (millis() < 9600) {
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
void startUpSequence() {
  // Logo - 2sec
  if (millis() < 2000) {
    debugLog("OLED display - LOGO");
    oled.setDisplayMode(oled.DISPLAY_LOGO);
  }
  // Comm OK Sequence - 1800 msec
  if (millis() > 2000 && millis() < 3200) {
    digitalWrite(RED_LED_PIN, HIGH);
    debugLog("OLED display - Comm OK");
    oled.setDisplayMode(oled.DISPLAY_COMM_CHK);
  }
  if (millis() > 3200 && millis() < 4200) {
    if (oled.currentDisplayMode() != oled.DISPLAY_DNA_CHK) {
      audio.queuePlayback(TRACK_DNA_CHK);
    }
    debugLog("OLED display - DNA Check");
    oled.setDisplayMode(oled.DISPLAY_DNA_CHK);
  }

  // DNA Check Sequence - 1200 msec
  bool buttonState = true; //trigger.checkState() == trigger.BUTTON_HOLD_PRESS;
  if (buttonState && millis() > 4200 && millis() < 5400) {
    debugLog("OLED display - DNA Progress");
    oled.setDisplayMode(oled.DISPLAY_DNA_PRG);
  }
  if (!buttonState && millis() > 4200 && millis() < 5400) {
    if (oled.currentDisplayMode() == oled.DISPLAY_DNA_PRG) {
      audio.queuePlayback(TRACK_DNA_FAIL);
    }
    debugLog("OLED display - DNA Check FAILED");
    oled.setDisplayMode(oled.DISPLAY_ID_FAIL);
  }

  if (oled.currentDisplayMode() != oled.DISPLAY_ID_FAIL) {
    // ID OK 1500ms
    if (millis() > 5400 && millis() < 7400) {
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
    // ID NAME 1500ms
    if (millis() > 7400 && millis() < 9400) {
      debugLog("OLED display - ID NAME");
      oled.setDisplayMode(oled.DISPLAY_ID_NAME);
    }

    // Firing Mode
    if (millis() > 9400) {
      debugLog("OLED display - Main");
      oled.setDisplayMode(oled.DISPLAY_MAIN);
      digitalWrite(GREEN_LED_PIN, LOW);
    }
  }
}
/**
   Playback the next queued track
*/
void runAudioPlayback() {
  audio.playQueuedTrack();
}
/**
   Run the led pattern
*/
void runLedDisplay() {
  fireLed.updateDisplay();
}

/**
   Run the oled pattern
*/
void runOledDisplay() {
  oled.updateDisplay(selectedTriggerMode, getTriggerCounter().getCount());
}

void ammoDown(void) {
    if ((millis() - lastTriggerTimeDebounce) > debounceTriggerTimeField) {
    if (digitalRead(TRIGGER_PIN) == HIGH) {
      sendBlasterPulse(getTriggerCounter());
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
void sendBlasterPulse(EasyCounter &counter) {
  debugLog("send alternating blaster pulse");
  bool emptyClip = counter.isEmpty();
  // check the track number before ticking, in case it's the last round
  int trackIdx = counter.getState(); // returns the approapriate track, or empty clip
  counter.tick();
  if (trackIdx != counter.getState())
    trackIdx = counter.getState();   // We redo this when the clip is full to get the reload track

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

EasyCounter& getTriggerCounter() {
  if (selectedTriggerMode == SELECTOR_AP_MODE)
    return apCounter;
  if (selectedTriggerMode == SELECTOR_IN_MODE)
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
void handleSelectorMode() {
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
