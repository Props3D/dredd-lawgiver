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
EasyVoice<VOICE_CMD_ARR, VOICE_CMD_ARR_SZ> voice(VOICE_RX_PIN, VOICE_TX_PIN);
// Counters for each firing mode
EasyCounter apCounter("ap");
EasyCounter inCounter("in");
EasyCounter heCounter("he");
EasyCounter fmjCounter("fmj");

EasyButton trigger(TRIGGER_PIN);
EasyButton reload(RELOAD_PIN);

/**
 * function declarations 
 */
// program loop
void startUpSequence(void);
void mainLoop(void);

// main loop functions
void updateAmmoIndicators(void);
void handleVoiceCommands(void);
void handleAmmoDown(void);
void reloadAmmo(void);
bool lowAmmoReached(void);
void handleTrigger(void);
void handleReload(void);


// utility functions
EasyCounter& getTriggerCounter(void);
uint8_t getSelectedTrack(uint8_t idx);
uint8_t* getCounters(void);
void initLedIndicators(void);
bool blinkNow(void);
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
volatile long lowAmmoChangeTime  = 0;
volatile uint8_t activateLowAmmo = 0;


/**
 *  Variables for tracking trigger state
 *  Variables must be marked as volatile because they are updated in the ISR.
 */
volatile uint8_t selectedTriggerMode  = VR_CMD_AMMO_MODE_FMJ; // sets the ammo mode to start
volatile uint8_t activateAmmoDown = 0;                        // sets main loop to fire a round
volatile uint8_t activateReload   = 0;                        // sets main loop to reload ammo

void setup() {
  Serial.begin (115200);
  debugLog("Starting setup");

  // initialize the trigger led and set brightness
  fireLed.begin(75);

  //initializes the audio player and sets the volume
  audio.begin(28);

  // initialize all the leds
  initLedIndicators();

  // Initialize the ammo counters for different modes
  apCounter.begin(0, 25, COUNTER_MODE_DOWN);
  inCounter.begin(0, 25, COUNTER_MODE_DOWN);
  heCounter.begin(0, 25, COUNTER_MODE_DOWN);
  fmjCounter.begin(0, 50, COUNTER_MODE_DOWN);

  // init the voice recognition module
  voice.begin();
  
  // select the initial ammo mode
  selectedTriggerMode = VR_CMD_AMMO_MODE_FMJ;
  
  // init the display
  oled.begin(selectedTriggerMode, getCounters());

  // set up all the triggers as pullup inputs
  // set up the fire trigger and the debounce threshold
  trigger.begin(25);
  reload.begin(25);
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

  // trigger the low-ammo indicator
  if (activateLowAmmo && !activateAmmoDown) {
      // small delay so not to collide with ammo playback
      if (millis() > lowAmmoChangeTime + TIMING_LOW_AMMO_WAIT_MS) {
        updateAmmoIndicators();
      }
  }
  // process trigger activation
  if (activateAmmoDown) {
    handleAmmoDown();
  }
  // process reload activation
  if (activateReload) {
    reloadAmmo();
  }
  // display LEDS if activated
  fireLed.updateDisplay();
  // playback tracks if queued
  audio.playQueuedTrack();
  // update OLED if there are changes
  if (screenUpdates) {
    oled.updateDisplay(selectedTriggerMode, getCounters());
    screenUpdates--;
  }
  // check for new voice commands
  handleVoiceCommands();
  handleTrigger();
  handleReload();
}


/**
 * Checks the fire trigger momentary switch.
 * Short press should activate the blast sequence.
 */
void handleTrigger(void) {
  // check trigger button
  int buttonStateFire = trigger.checkState();
  // check if a trigger is pressed.
  if (buttonStateFire == BUTTON_SHORT_PRESS) {
      //handleAmmoDown();
      activateAmmoDown++;
  }
}

/**
 * Checks the fire trigger momentary switch.
 * Short press should activate the reload sequence.
 */
void handleReload(void) {
  // check trigger button
  int buttonStateFire = reload.checkState();
  // check if a trigger is pressed.
  if (buttonStateFire == BUTTON_SHORT_PRESS || buttonStateFire == BUTTON_LONG_PRESS) {
      //reloadAmmo();
      activateReload++;
  }
}

/**
 * Routine for the startup sequence. Heavy OLED updates on startup.
 * 1. Display Props3D logo - (2000ms)
 * 2. Display Comms OK - 90 pixels (900ms)
 * 3. Display DNA Check - wait for trigger press (1000ms)
 * 4. Display DNA Check - 90 pixels (900ms)
 * 5. Display ID FAIL - blink 4 times (1500ms) then solid
 * 6. Display ID OK - blink 3 times with LED - 1000ms
 * 7. Display Judge ID - wait (1800ms)
 * 8. Display Ammo mode
 */
void startUpSequence(void) {
  uint8_t _sequenceMode = oled.getDisplayMode();
  if (_sequenceMode == 0) {
      debugLog("Startup - Logo");
      oled.updateDisplay(oled.DISPLAY_LOGO, 0);
  }
  if (_sequenceMode == oled.DISPLAY_LOGO) {
      if (millis() > TIMING_STARTUP_LOGO_MS + lastDisplayUpdate) {
        debugLog("Startup - Comm Ok");
        oled.updateDisplay(oled.DISPLAY_COMM_CHK, 0);
        // Red led on
        toggleLED(RED_LED_PIN);
      }
  }
  if (_sequenceMode == oled.DISPLAY_COMM_CHK) {
      if (millis() > TIMING_PROGRESS_INTERVAL_MS + lastDisplayUpdate) {
        oled.updateDisplay(oled.DISPLAY_COMM_CHK, progressBarUpdates);
        progressBarUpdates++;
        lastDisplayUpdate = millis();
      }
      if (progressBarUpdates > 9) {
        debugLog("Startup - DNA Chk");
        oled.updateDisplay(oled.DISPLAY_DNA_CHK, progressBarUpdates);        
        lastDisplayUpdate = millis();
      }
  }
  if (_sequenceMode == oled.DISPLAY_DNA_CHK) {
      // A trigger press is required to complete the DNA check
      bool buttonPressed = (digitalRead(TRIGGER_PIN) == LOW);
      if (buttonPressed || millis() > (TIMING_STARTUP_DNA_CHK_MS + lastDisplayUpdate)) {
        if (buttonPressed) {
          audio.playTrack(AUDIO_TRACK_DNA_CHK);
          oled.updateDisplay(oled.DISPLAY_DNA_PRG, progressBarUpdates);
        } else {
          debugLog("Startup - ID FAIL");
          audio.playTrack(AUDIO_TRACK_ID_FAIL);
          oled.updateDisplay(oled.DISPLAY_ID_FAIL, progressBarUpdates);
        }
        lastDisplayUpdate = millis();
      }
  }
  if (_sequenceMode == oled.DISPLAY_DNA_PRG) {
      if (millis() > TIMING_PROGRESS_INTERVAL_MS + lastDisplayUpdate) {
        // A trigger press is required to complete the DNA check
        bool buttonPressed = (digitalRead(TRIGGER_PIN) == LOW);
        if (buttonPressed) {
          oled.updateDisplay(oled.DISPLAY_DNA_PRG, progressBarUpdates);
          progressBarUpdates++;
          lastDisplayUpdate = millis();
        } else {
          debugLog("Startup - ID FAIL");
          audio.playTrack(AUDIO_TRACK_ID_FAIL);
          oled.updateDisplay(oled.DISPLAY_ID_FAIL, progressBarUpdates);
        }
      }
      if (progressBarUpdates > 18) {
        debugLog("Startup - ID OK");
        oled.updateDisplay(oled.DISPLAY_ID_OK, progressBarUpdates);
        // RED LED off
        toggleLED(RED_LED_PIN);
        lastDisplayUpdate = millis();
      }
  }
  if (_sequenceMode == oled.DISPLAY_ID_OK) {
      if (ledBlinks < 3 && blinkNow()) {
          // blink the ID OK when the Green LED is on
          oled.updateDisplay(oled.DISPLAY_ID_OK, progressBarUpdates, blinkState == HIGH);
          // toggle the LED after OLED update
          setBlinkState(GREEN_LED_PIN);
          // playback only when LED is on 
          if (blinkState == HIGH) {
            audio.playTrack(AUDIO_TRACK_ID_OK);
            ledBlinks++;
          }
      }
      // blink the green led three times before moving on
      if (ledBlinks > 2 && millis() > (TIMING_STARTUP_ID_OK_MS + lastDisplayUpdate)) {
        debugLog("Startup - ID Name");
        oled.updateDisplay(oled.DISPLAY_ID_NAME, progressBarUpdates);
        lastDisplayUpdate = millis();
      }
  }
  // Display ID Name for a fixed duration
  if (_sequenceMode == oled.DISPLAY_ID_NAME) {
      if (millis() > (TIMING_STARTUP_ID_NAME_MS + lastDisplayUpdate)) {
        debugLog("Startup - Main loop");
        audio.playTrack(AUDIO_TRACK_AMMO_LOAD);
        oled.updateDisplay(oled.DISPLAY_MAIN, progressBarUpdates);
        // let's turn off the ammo indicators
        updateAmmoIndicators();
        // make sure the ISR routne didn't trigger an ammo shot during the DNA Check
        activateAmmoDown = false;
        // switch to main loop
        loopStage = LOOP_STATE_MAIN;
        lastDisplayUpdate = millis();
      }
  }
  if (_sequenceMode == oled.DISPLAY_ID_FAIL) {
      if (ledBlinks < 4 && blinkNow()) {
          // blink the ID FAIL when the RED LED is on
          oled.updateDisplay(oled.DISPLAY_ID_FAIL, progressBarUpdates, blinkState == HIGH);
          // toggle the LED after OLED update
          setBlinkState(RED_LED_PIN);
          // count only when LED is on 
          if (blinkState == HIGH) {
            ledBlinks++;
          }
      }
      // blink the red LED four times before moving on
      if (ledBlinks > 3 && millis() > TIMING_STARTUP_ID_FAIL_MS + lastDisplayUpdate) {
        // switch to main loop
        loopStage = LOOP_STATE_FAIL;
      }
  }
}

/**
 * Routine for resetting the ammo counters
 * 1. all counters are reset
 * 2. leds are turned off
 * 3. queue audio track
 * 4. activate oled refresh
 */
void reloadAmmo(void) {
  activateReload = 0;
  activateLowAmmo = 0;

  debugLog("Reloading all counters");
  apCounter.resetCount();
  inCounter.resetCount();
  heCounter.resetCount();
  fmjCounter.resetCount();
  // Reset the low-ammo indicator
  updateAmmoIndicators();
  //queue the track
  audio.queuePlayback(AUDIO_TRACK_AMMO_RELOAD);
  // Refresh the display at least once
  screenUpdates++;
}

/**
   Sends a blaster pulse.
     1. Toggles a clip counter
     2. Checks for an empty clip
        a. play empty clip track
     3. If clip is not empty
        a. queue audio track
        b. activate led strip
        c. activate oled refresh
        d. Check for low ammo
*/
void handleAmmoDown(void) {
  activateAmmoDown = 0;
  // move the counter
  bool emptyClip = !getTriggerCounter().tick();
  if (emptyClip) {
     debugLog("Empty clip");
     audio.queuePlayback(getSelectedTrack(AMMO_MODE_IDX_EMTY));
     return;
  }
  debugLog("Ammo fire sequence");
  //queue the track
  audio.queuePlayback(getSelectedTrack(AMMO_MODE_IDX_FIRE));
  // activate the led pulse
  fireLed.activate(blasterShot);
  // check for low ammo, and set the timer
  if (lowAmmoReached()) {
    activateLowAmmo++;
    lowAmmoChangeTime = millis();
  }
  // Refresh the display at least once
  screenUpdates++;
}

/**
 * Convenience method for selecting a track to playback based on the selected
 * trigger mode and a state variable
 */
uint8_t getSelectedTrack(uint8_t trackIdx) {
  return AUDIO_TRACK_AMMO_MODE_ARR[selectedTriggerMode][trackIdx];
}

/**
 * Routine for getting the selected trigger counter
 */
EasyCounter& getTriggerCounter(void) {
  if (selectedTriggerMode == VR_CMD_AMMO_MODE_AP)
    return apCounter;
  if (selectedTriggerMode == VR_CMD_AMMO_MODE_IN ||
      selectedTriggerMode == VR_CMD_AMMO_MODE_HS)
    return inCounter;
  if (selectedTriggerMode == VR_CMD_AMMO_MODE_HE)
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
    if (selectedTriggerMode == VR_CMD_AMMO_MODE_AP) {
      blasterShot.initialize(fireLed.RED, fireLed.ORANGE);  // shot - flash with color fade
      debugLog("Armor Piercing Mode selected");
    }
    if (selectedTriggerMode == VR_CMD_AMMO_MODE_IN) {
      blasterShot.initialize(fireLed.ORANGE, fireLed.WHITE);  // shot - flash with color fade
      debugLog("Incendiary Mode selected");
    }
    if (selectedTriggerMode == VR_CMD_AMMO_MODE_HE) {
      blasterShot.initialize(fireLed.ORANGE, fireLed.WHITE);  // shot - flash with color fade
      debugLog("High Ex Mode selected");
    }
    if (selectedTriggerMode == VR_CMD_AMMO_MODE_HS) {
      blasterShot.initialize(fireLed.RED, fireLed.ORANGE);  // shot - flash with color fade
      debugLog("Hotshot Mode selected");
    }
    if (selectedTriggerMode == VR_CMD_AMMO_MODE_ST) {
      blasterShot.initialize(fireLed.YELLOW, fireLed.WHITE);  // shot - flash with color fade
      debugLog("Stun Mode selected");
    }
    if (selectedTriggerMode == VR_CMD_AMMO_MODE_FMJ) {
      blasterShot.initialize(fireLed.RED, fireLed.ORANGE);  // shot - flash with color fade
      debugLog("FMJ Mode selected");
    }
    if (selectedTriggerMode == VR_CMD_AMMO_MODE_RAPID) {
      blasterShot.initialize(fireLed.RED, fireLed.ORANGE);  // shot - flash with color fade
      debugLog("FMJ Mode selected");
    }
    // check for low ammo, and set the timer
    if (lowAmmoReached()) {
      activateLowAmmo++;
      lowAmmoChangeTime = millis();
    } else {
      // Reset the low-ammo indicator
      updateAmmoIndicators();
    }
    audio.queuePlayback(getSelectedTrack(AMMO_MODE_IDX_CHGE));
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
 * Test whether we've reached low ammo condition
 */
bool lowAmmoReached(void) {
  return getTriggerCounter().getCount() < 5;
}
/**
 * Set Red/Green leds when low on ammo
 */
void updateAmmoIndicators(void) {
  if (activateLowAmmo) {
    activateLowAmmo = 0;
    // let's make sure the screen redraws to count the low ammo before switching the LEDs
    oled.checkAmmoLevels();
    oled.updateDisplay(selectedTriggerMode, getCounters());
    // set the LED
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
    // playback for low ammo immediately
    audio.playTrack(AUDIO_TRACK_AMMO_LOW);
  } else {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
  }
}

/**
 * Blink controller.
 * Returns true when it's time to toggle state every 500ms
 */
bool blinkNow(void) {
  if (millis() > lastBlinkUpdate + TIMING_FAST_BLINK_WAIT_MS) {
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
