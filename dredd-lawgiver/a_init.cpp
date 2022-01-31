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
   All components are controlled or enabled by "config.h". Before running,
   review and change all configurations based on your setup.

   There's no need to change any of the following code or functions.
*/
// Counters for each firing mode
EasyCounter apCounter("ap");
EasyCounter inCounter("in");
EasyCounter heCounter("he");
EasyCounter fmjCounter("fmj");
// Audio setup
EasyAudio audio(AUDIO_RX_PIN, AUDIO_TX_PIN);
// LED setup
EasyLedv3<FIRE_LED_CNT, FIRE_LED_PIN> fireLed;
ezBlasterShot blasterShot(fireLed.RED, fireLed.ORANGE, 4 /*speed*/);  // initialize colors to starting fire mode
// OLED Display
EasyOLED<OLED_SCL_PIN, OLED_SDA_PIN, OLED_CS_PIN, OLED_DC_PIN, OLED_RESET_PIN> oled(DISPLAY_USER_ID);
// VR module
EasyVoice<VOICE_RECORDS_ARR, VOICE_RECORDS_ARR_SZ> voice(VOICE_RX_PIN, VOICE_TX_PIN);

// function declarations
void initLedIndicators(void);
void ammoIndicators(void);
void startUpSequence(void);
void runAudioPlayback(void);
void runOledDisplay(void);
void runLedDisplay(void);
void handleSelectorMode(void);
void handleAmmoDown(void);
void reloadAmmo(void);
EasyCounter& getTriggerCounter(void);
uint8_t getSelectedTrack(uint8_t idx);
void ammoDownISR(void);
void reloadAmmoISR(void);
uint8_t* getCounters(void);

/**
 * Variable for tracking stages through the main loop:
 * - Startup sequence
 * - Main loop
 * - Fail / Stop
 */
uint8_t loopStage = STARTUP_LOOP;

/**
 * Variables for tracking and updating ammo counts on the OLED
 */
static uint8_t counters[4];
volatile uint8_t screenUpdates   = 0;

/**
 * Variables for tracking ISR 
 */
volatile bool activateAmmoDown = false;
volatile bool activateReload = false;
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

  // init the display
  selectedTriggerMode = SELECTOR_FMJ_MODE;
  // Initialize the clip counters for different modes
  apCounter.begin(0, 25, COUNTER_MODE_DOWN);
  inCounter.begin(0, 25, COUNTER_MODE_DOWN);
  heCounter.begin(0, 25, COUNTER_MODE_DOWN);
  fmjCounter.begin(0, 50, COUNTER_MODE_DOWN);
  oled.begin(selectedTriggerMode, getCounters());

  // init the voice recog module
  voice.begin();

  // set up the fire trigger and the debounce threshold
  // attach the interrupts
  debugLog("Attach Interrupts");
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  pinMode(RELOAD_PIN, INPUT_PULLUP);
  noInterrupts();
  attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), ammoDownISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(RELOAD_PIN), reloadAmmoISR, FALLING);
  interrupts();
}

/**
 * Startup loop only needs to update OLED display and play audio tracks.
 */
void startUpLoop () {
  startUpSequence();
  audio.playQueuedTrack();
  oled.updateDisplay(selectedTriggerMode, getCounters());
}

/**
 * Main loop routine:
 * - check ISR inputs
 * - display LEDS if activated
 * - playback any queued audio 
 * - update OLED display on changes
 * - check the VR module for any commands
 */
void mainLoop () {
  // always handle the activated components first, before processing new inputs
  if (activateAmmoDown == true) {
    handleAmmoDown();
  }
  if (activateReload == true) {
    reloadAmmo();
  }
  runLedDisplay();
  runAudioPlayback();
  runOledDisplay();
  // check for new inputs
  handleSelectorMode();
}

/**
 * The main loop.
 */
void loop () {
  // run the start up sequence first
  if (loopStage == STARTUP_LOOP) {
    startUpLoop();
  }
  if (loopStage == MAIN_LOOP) {
    mainLoop();
  }
}

/**
 * Routine for the startup sequence. Heavy OLED updates on startup.
 * - Flash 3d props logo
 * - Comm OK
 * - DNA Check
 * - ID OK or ID FAIL
 * - Flash Judge name
 */
void startUpSequence(void) {
  // Logo
  if (millis() < STARTUP_LOGO_MS) {
    oled.setDisplayMode(oled.DISPLAY_LOGO);
  }
  // Comm OK Sequence
  if (millis() > STARTUP_LOGO_MS && millis() < STARTUP_COMM_OK_MS) {
    digitalWrite(RED_LED_PIN, HIGH);
    oled.setDisplayMode(oled.DISPLAY_COMM_CHK);
  }
  // switch to DNA Check and wait a second
  if (millis() > STARTUP_COMM_OK_MS && millis() < STARTUP_DNA_CHK_MS) {
    oled.setDisplayMode(oled.DISPLAY_DNA_CHK);
  }

  // DNA Check Sequence
  // A trigger press is required to complete the DNA check
  bool buttonState = (digitalRead(TRIGGER_PIN) == LOW);
  if (buttonState && millis() > STARTUP_DNA_CHK_MS && millis() < STARTUP_DNA_PRG_MS) {
    if (oled.currentDisplayMode() != oled.DISPLAY_DNA_PRG) {
      audio.queuePlayback(TRACK_DNA_CHK);
    }
    oled.setDisplayMode(oled.DISPLAY_DNA_PRG);
  }
  if (!buttonState && millis() > STARTUP_DNA_CHK_MS && millis() < STARTUP_DNA_PRG_MS) {
    if (oled.currentDisplayMode() == oled.DISPLAY_DNA_PRG) {
      audio.queuePlayback(TRACK_DNA_FAIL);
    }
    oled.setDisplayMode(oled.DISPLAY_ID_FAIL);
  }

  if (oled.currentDisplayMode() != oled.DISPLAY_ID_FAIL) {
    if (millis() > STARTUP_DNA_PRG_MS && millis() < STARTUP_ID_OK_MS) {
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
      oled.setDisplayMode(oled.DISPLAY_ID_NAME);
    }

    // Firing Mode
    if (millis() > STARTUP_JUDGE_NAME_MS) {
      oled.setDisplayMode(oled.DISPLAY_MAIN);
      digitalWrite(GREEN_LED_PIN, LOW);
    }

    if (millis() > STARTUP_END_MS) {
      interrupts();
      loopStage = MAIN_LOOP;
    }

  } else {
    if (millis() > STARTUP_DNA_PRG_MS) {
      oled.drawIDFail(0);
      loopStage = STOP_LOOP;
    }
  }
}

/**
 * Playback the next queued track
 */
void runAudioPlayback(void) {
  audio.playQueuedTrack();
}

/**
 * Run the led pattern
 */
void runLedDisplay(void) {
  fireLed.updateDisplay();
}

/**
 * Run the oled pattern
 */
void runOledDisplay(void) {
  if (screenUpdates > 0) {
    oled.updateDisplay(selectedTriggerMode, getCounters());
    screenUpdates--;
  }
}

/**
 * Checks the fire trigger momentary switch.
 */
void ammoDownISR(void) {
  if ((millis() - lastTriggerTimeDebounce) > debounceTriggerTimeField) {
    if (digitalRead(TRIGGER_PIN) == LOW) {
      activateAmmoDown = true;
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
        activateReload = true;
    }
    lastReloadTimeDebounce = millis();
  }
}

/**
 * Routine for resetting the ammo counters
 */
void reloadAmmo(void) {
  activateReload = false;
  debugLog("Reloading all counters");
  apCounter.resetCount();
  inCounter.resetCount();
  heCounter.resetCount();
  fmjCounter.resetCount();
  // trigger the low-ammo indicators
  ammoIndicators();
  //queue the track
  audio.queuePlayback(getSelectedTrack(STATE_RELOAD));
  screenUpdates = 1;
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
  activateAmmoDown = false;
  debugLog("send led pulse");
  bool emptyClip = getTriggerCounter().isEmpty();
  // check the track number before ticking, in case it's the last round
  int trackIdx = getTriggerCounter().getState(); // returns the approapriate track, or empty clip
  // move the counter, if possible
  getTriggerCounter().tick();

  // trigger the low-ammo indicators
  ammoIndicators();
  //queue the track
  audio.queuePlayback(getSelectedTrack(trackIdx));
  if (emptyClip == false) {
    fireLed.activate(blasterShot);
  }
  screenUpdates = 1;
}

/**
 * Convenience method for selecting a track to playback based on the selected
 * trigger mode and a state variable
 */
uint8_t getSelectedTrack(uint8_t trackIdx) {
  if (selectedTriggerMode == SELECTOR_AP_MODE)
    return TRACK_AP_ARR[trackIdx];
  if (selectedTriggerMode == SELECTOR_IN_MODE)
    return TRACK_IN_ARR[trackIdx];
  if (selectedTriggerMode == SELECTOR_HE_MODE)
    return TRACK_HE_ARR[trackIdx];
  if (selectedTriggerMode == SELECTOR_HS_MODE)
    return TRACK_HS_ARR[trackIdx];
  if (selectedTriggerMode == SELECTOR_ST_MODE)
    return TRACK_ST_ARR[trackIdx];
  if (selectedTriggerMode == SELECTOR_RAPID_MODE)
    return TRACK_RAPID_ARR[trackIdx];
  return TRACK_FMJ_ARR[trackIdx];
}

/**
 * Routine for getting the selected trigger counter
 */
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
    audio.playTrackNow(TRACK_CHANGE_MODE);
    screenUpdates = 1;
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
  if (ammo < 4) {
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);    
  } else {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
  }
}
