/*
   name:  Lawgiver 2012
   Author:  FriskyDingo
   Date:  2021-11-21
   Description: Code for Lawgiver setup
*/
#include <Arduino.h>
#include "config.h"
#include "debug.h"
#include "easycounter.h"
#include "easyaudio.h"
#include "easyledv3.h"
#include "easyoled.h"
#include "easyvoice.h"

/**
 * All components are controlled or enabled by "config.h". Before running,
 * review and change all configurations based on your setup.
 *
 *  There's no need to change any of the following code or functions.
 */
// Audio setup
EasyAudio audio(AUDIO_RX_PIN, AUDIO_TX_PIN);
// LED setup
EasyLedv3<FIRE_LED_CNT, FIRE_LED_PIN> fireLed;
ezBlasterShot blasterShot(fireLed.RED, fireLed.ORANGE, 4 /*speed*/);  // initialize colors to starting fire mode
// OLED Display
EasyOLED<OLED_SCL_PIN, OLED_SDA_PIN, OLED_CS_PIN, OLED_DC_PIN, OLED_RESET_PIN> oled(DISPLAY_USER_ID);
// VR module
EasyVoice<VOICE_RECORDS_ARR, VOICE_RECORDS_ARR_SZ> voice(VOICE_RX_PIN, VOICE_TX_PIN);
// Counters for each firing mode
EasyCounter apCounter("ap");
EasyCounter inCounter("in");
EasyCounter heCounter("he");
EasyCounter fmjCounter("fmj");

/**
 * function declarations 
 */
// program loop
void startUpSequence(void);
void mainLoop(void);

// main loop functions
void ammoIndicators(void);
void handleVoiceCommands(void);
void handleAmmoDown(void);
void reloadAmmo(void);

// ISR Functions for triggers
void ammoDownISR(void);
void reloadAmmoISR(void);

// utility functions
EasyCounter& getTriggerCounter(void);
uint8_t getSelectedTrack(uint8_t idx);
uint8_t* getCounters(void);
void initLedIndicators(void);
bool blinkNow();
void setBlinkState(uint8_t pin);
void toggleLED(uint8_t pin);

/**
 * Variable for tracking stages through the main loop:
 * - Startup sequence
 * - Main loop
 * - Fail / Stop
 */
uint8_t loopStage           = LOOP_STATE_START;

/**
 * Variables for tracking the start up sequence updates
 */
long lastDisplayUpdate      = 0;
uint8_t progressBarUpdates  = 0;

/**
 * Variables for toggling an LED
 */
uint8_t blinkState          = LOW;
uint8_t ledBlinks           = 0;
long lastBlinkUpdate        = 0;

/**
 * Variables for tracking and updating ammo counts on the OLED
 */
static uint8_t counters[4] = {25,25,25,50};
volatile uint8_t screenUpdates   = 0;

/**
 * Variables for tracking ISR 
 */
volatile uint8_t activateAmmoDown = 0;
volatile uint8_t activateReload   = 0;
volatile long lastTriggerTimeDebounce = 0;                    // handling trigger bounce
volatile long lastReloadTimeDebounce = 0;                     // handling reload bounce
const static long debounceTriggerTimeField = 250;             // time measured between trigger bounces
const static long debounceReloadTimeField = 1000;             // time measured between reload bounces

/**
 *  Variables for tracking selected trigger mode.
 *  Variables must be marked as volatile because they are updated in the ISR.
 */
volatile uint8_t selectedTriggerMode   = SELECTOR_FMJ_MODE;   // sets the fire mode to blaster to start

void setup() {
  Serial.begin (115200);
  debugLog("Starting setup");

  // initialize the trigger led and set brightness
  fireLed.begin(75);

  //initializes the audio player and sets the volume
  audio.begin(25);

  // initialize all the leds
  initLedIndicators();

  // Initialize the clip counters for different modes
  apCounter.begin(0, 25, COUNTER_MODE_DOWN);
  inCounter.begin(0, 25, COUNTER_MODE_DOWN);
  heCounter.begin(0, 25, COUNTER_MODE_DOWN);
  fmjCounter.begin(0, 50, COUNTER_MODE_DOWN);
  // select the initial ammo mode
  selectedTriggerMode = SELECTOR_FMJ_MODE;
  // init the display
  oled.begin(selectedTriggerMode, getCounters());

  // init the voice recognition module
  voice.begin();

  // set up the fire trigger and the debounce threshold
  // attach the interrupts
  debugLog("Attach Interrupts");
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  pinMode(RELOAD_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), ammoDownISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(RELOAD_PIN), reloadAmmoISR, FALLING);
  interrupts();
}

/**
 * The main loop.
 */
void loop () {
  // Shortcut the loop if it's failed
  if (LOOP_STATE_FAIL == loopStage) return;

  // run the start up sequence first
  if (LOOP_STATE_START == loopStage) {
    startUpSequence();
  } else {
    // run the main loop
    mainLoop();
  }
}

/**
 * The Main loop Routine
 * 1. Check the AmmoDownISR activation
 *   a. Toggles a clip counter
 *   b. Checks for an empty clip 
 *   c. If clip is not empty
 *      i. queue audio track for selected fire mode
 *     ii. activate front led with blaster shot
 *         1. Flash white
 *         2. Burn Red to orange, then fade as it cools
 *   d. Otherwise queue empty clip track
 * 2. Check the ReloadAmmoISR activation
 *   a. Resets all counters
 *   b. Queues reload audio track
 *   c. Resets the led indicators
 *   d. Activates OLED update
 * 3. Check if the leds have been activated
 *   a. update the leds following the pattern that has been set
 * 4. Check audio queue for playback
 * 5. Refresh or Update the OLED Display
 * 6. Check the recognition module for voice commands
 *     i. playback change mode track
 *    ii. toggle fire mode based on the recognized command
 */
void mainLoop (void) {
  // always handle the activated components first, before processing new inputs
  // process trigger activation
  if (activateAmmoDown) {
    handleAmmoDown();
  }
  // process reload activation
  if (activateReload) {
    reloadAmmo();
  }
  // display activated LEDS
  fireLed.updateDisplay();
  // playback queued tracks
  audio.playQueuedTrack();
  // update OLED
  if (screenUpdates) {
    oled.updateDisplay(selectedTriggerMode, getCounters());
    screenUpdates--;
  }
  // check for new voice commands
  handleVoiceCommands();
}

/**
 * Routine for the startup sequence. Heavy OLED updates on startup.
 * 1. Display Props3D logo - (2000ms)
 * 2. Display Comms OK - 90 pixels (900ms)
 * 3. Display DNA Check - wait for trigger press (1000ms)
 * 4. Display DNA Check - 90 pixels (900ms)
 * 5. Display ID FAIL - blink 3 times (1500ms) then solid
 * 6. Display ID OK - blink 3 times with LED - 1800ms
 * 7. Display Judge ID - wait (1800ms)
 * 8. Display Ammo mode
 */
void startUpSequence(void) {
  uint8_t _sequenceMode = oled.getDisplayMode();
  if (_sequenceMode == 0) {
      debugLog("Startup - Logo");
      oled.startupDisplay(oled.DISPLAY_LOGO, 0);
  }
  if (_sequenceMode == oled.DISPLAY_LOGO) {
      if (millis() > STARTUP_LOGO_MS + lastDisplayUpdate) {
        debugLog("Startup - Comm Ok");
        oled.startupDisplay(oled.DISPLAY_COMM_CHK, 0);
        // Red led on
        toggleLED(RED_LED_PIN);
      }
  }
  if (_sequenceMode == oled.DISPLAY_COMM_CHK) {
      if (millis() > PROGRESS_INTERVAL_MS + lastDisplayUpdate) {
        oled.startupDisplay(oled.DISPLAY_COMM_CHK, progressBarUpdates);
        progressBarUpdates++;
        lastDisplayUpdate = millis();
      }
      if (progressBarUpdates > 9) {
        debugLog("Startup - DNA Chk");
        oled.startupDisplay(oled.DISPLAY_DNA_CHK, progressBarUpdates);        
        lastDisplayUpdate = millis();
      }
  }
  if (_sequenceMode == oled.DISPLAY_DNA_CHK) {
      // A trigger press is required to complete the DNA check
      bool buttonPressed = (digitalRead(TRIGGER_PIN) == LOW);
      if (buttonPressed || millis() > (STARTUP_DNA_CHK_MS + lastDisplayUpdate)) {
        if (buttonPressed) {
          audio.playTrack(TRACK_DNA_CHK);
          oled.startupDisplay(oled.DISPLAY_DNA_PRG, progressBarUpdates);
        } else {
          debugLog("Startup - ID FAIL");
          audio.playTrack(TRACK_ID_FAIL);
          oled.startupDisplay(oled.DISPLAY_ID_FAIL, progressBarUpdates);
        }
        lastDisplayUpdate = millis();
      }
  }
  if (_sequenceMode == oled.DISPLAY_DNA_PRG) {
      if (millis() > PROGRESS_INTERVAL_MS + lastDisplayUpdate) {
        // A trigger press is required to complete the DNA check
        bool buttonPressed = (digitalRead(TRIGGER_PIN) == LOW);
        if (buttonPressed) {
          oled.startupDisplay(oled.DISPLAY_DNA_PRG, progressBarUpdates);
          progressBarUpdates++;
          lastDisplayUpdate = millis();
        } else {
          debugLog("Startup - ID FAIL");
          audio.playTrack(TRACK_ID_FAIL);
          oled.startupDisplay(oled.DISPLAY_ID_FAIL, progressBarUpdates);
        }
      }
      if (progressBarUpdates > 18) {
        debugLog("Startup - ID OK");
        oled.startupDisplay(oled.DISPLAY_ID_OK, progressBarUpdates);
        // RED LED off
        toggleLED(RED_LED_PIN);
        lastDisplayUpdate = millis();
      }
  }
  if (_sequenceMode == oled.DISPLAY_ID_OK) {
      if (blinkNow()) {
          // blink the ID OK when the Green LED is on
          oled.startupDisplay(oled.DISPLAY_ID_OK, progressBarUpdates, blinkState == HIGH);
          // toggle the LED after OLED update
          setBlinkState(GREEN_LED_PIN);
          // playback only when LED is on 
          if (blinkState == HIGH) {
            audio.playTrack(TRACK_ID_OK);
            ledBlinks++;
          }
      }
      // blink the green led three times before moving on
      if (ledBlinks > 3 && millis() > (STARTUP_ID_OK_MS + lastDisplayUpdate)) {
        debugLog("Startup - ID Name");
        oled.startupDisplay(oled.DISPLAY_ID_NAME, progressBarUpdates);
        lastDisplayUpdate = millis();
      }
  }
  // Display ID Name for a fixed duration
  if (_sequenceMode == oled.DISPLAY_ID_NAME) {
      if (millis() > (STARTUP_ID_NAME_MS + lastDisplayUpdate)) {
        debugLog("Startup - Main loop");
        audio.playTrack(TRACK_AMMO_LOAD);
        oled.startupDisplay(oled.DISPLAY_MAIN, progressBarUpdates);
        // let's turn off the ammo indicators
        ammoIndicators();
        // make sure the ISR doesn't trigger ammo shot during the DNA Check
        activateAmmoDown = false;
        // switch to main loop
        loopStage = LOOP_STATE_MAIN;
        lastDisplayUpdate = millis();
      }
  }
  if (_sequenceMode == oled.DISPLAY_ID_FAIL) {
      if (blinkNow()) {
          // blink the ID FAIL when the RED LED is on
          oled.startupDisplay(oled.DISPLAY_ID_FAIL, progressBarUpdates, blinkState == HIGH);
          // toggle the LED after OLED update
          setBlinkState(RED_LED_PIN);
          // count only when LED is on 
          if (blinkState == HIGH) {
            ledBlinks++;
          }
      }
      // blink the red LED four times before moving on
      if (ledBlinks > 4 && millis() > STARTUP_ID_FAIL_MS + lastDisplayUpdate) {
        // switch to main loop
        loopStage = LOOP_STATE_FAIL;
      }
  }
}

/**
 * Checks the fire trigger momentary switch.
 */
void ammoDownISR(void) {
  if ((millis() - lastTriggerTimeDebounce) > debounceTriggerTimeField) {
    if (digitalRead(TRIGGER_PIN) == LOW) {
      activateAmmoDown++;
    }
    lastTriggerTimeDebounce = millis();
  }
}

/**
 * Checks the reload micro-switch.
 */
void reloadAmmoISR(void) {
  if ((millis() - lastReloadTimeDebounce) > debounceReloadTimeField) {
    if (digitalRead(RELOAD_PIN) == LOW) {
        activateReload++;
    }
    lastReloadTimeDebounce = millis();
  }
}

/**
 * Routine for resetting the ammo counters
 */
void reloadAmmo(void) {
  activateReload--;
  debugLog("Reloading all counters");
  apCounter.resetCount();
  inCounter.resetCount();
  heCounter.resetCount();
  fmjCounter.resetCount();
  // Reset the low-ammo indicator
  ammoIndicators();
  //queue the track
  audio.queuePlayback(TRACK_AMMO_RELOAD);
  // Refresh the display at least once
  screenUpdates++;
}

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
void handleAmmoDown(void) {
  activateAmmoDown--;
  bool emptyClip = getTriggerCounter().isEmpty();
  if (emptyClip) {
     debugLog("Empty clip");
     audio.queuePlayback(getSelectedTrack(AMMO_MODE_EMTY_IDX));
     return;
  }
  debugLog("Ammo fire sequence");
  // move the counter
  getTriggerCounter().tick();
  //queue the track
  audio.queuePlayback(getSelectedTrack(AMMO_MODE_FIRE_IDX));
  // activate the led pulse
  fireLed.activate(blasterShot);
  // trigger the low-ammo indicators
  ammoIndicators();
  // Refresh the display at least once
  screenUpdates++;
}

/**
 * Convenience method for selecting a track to playback based on the selected
 * trigger mode and a state variable
 */
uint8_t getSelectedTrack(uint8_t trackIdx) {
  return TRACK_AMMO_MODE_ARR[selectedTriggerMode][trackIdx];
}

/**
 * Routine for getting the selected trigger counter
 */
EasyCounter& getTriggerCounter(void) {
  if (selectedTriggerMode == SELECTOR_AP_MODE)
    return apCounter;
  if (selectedTriggerMode == SELECTOR_IN_MODE ||
      selectedTriggerMode == SELECTOR_HS_MODE)
    return inCounter;
  if (selectedTriggerMode == SELECTOR_HE_MODE)
    return heCounter;
  return fmjCounter;
}

/**
 * Checks the voice recognition module for new voice commands
 * 1. change the selected ammo mode
 * 2. initialize the LED sequence
 * 3. queue playback
 * 4. set screen refresh
 */
void handleVoiceCommands(void) {
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
    audio.queuePlayback(getSelectedTrack(AMMO_MODE_CHGE_IDX));
    // Refresh the display at least once
    screenUpdates++;
  }
}


/**
 * Convenient method for collecting ammo counters for display updates.
 */
uint8_t* getCounters(void) {
  counters[0] = apCounter.getCount();
  counters[1] = inCounter.getCount();
  counters[2] = heCounter.getCount();
  counters[3] = fmjCounter.getCount();
  return counters;
}

/**
 * Set Red/Green leds on startup
 */
void initLedIndicators(void) {
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
}

/**
 * Set Red/Green leds when low on ammo
 */
void ammoIndicators(void) {
  int ammo = getTriggerCounter().getCount();
  if (ammo == 4) {
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
    // queue playback for low ammo
    audio.queuePlayback(TRACK_AMMO_LOW);
    // let's make sure the screen redraws to count the low ammo
    screenUpdates++;
  } else {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
  }
}

/**
 * Blink controller.
 * Returns true when it's time to toggle state every 500ms
 */
bool blinkNow() {
  if (millis() > lastBlinkUpdate + 500) {
    lastBlinkUpdate = millis();
    blinkState = !blinkState;
    return true;
  }
  return false;
}

/**
 * Sets the state of the LED pin
 */
void setBlinkState(uint8_t pin) {
  digitalWrite(pin, blinkState);
}

/**
 * Toggles LED state immediately
 */
void toggleLED(uint8_t pin) {
  blinkState = !blinkState;
  digitalWrite(pin, blinkState);
}
