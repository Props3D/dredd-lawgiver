/**
 *  name:  Lawgiver 2012
 *  Author:  Props3D_Craig
 *  Date:  2021-11-21
 *  Description: Code for Lawgiver setup
 */
#include <Arduino.h>
#include "config.h"
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
 * There's no need to change any of the following code or functions.
 */
// Audio setup
EasyAudio audio(AUDIO_RX_PIN, AUDIO_TX_PIN);

// LED setup
EasyLedv3<FIRE_LED_CNT, FIRE_LED_PIN> fireLed;
ezBlasterShot blasterShot(fireLed.RED, fireLed.ORANGE, 4 /*speed*/);  // initialize colors to starting fire mode
ezBlasterRepeatingShot repeatingShot(8 /*reps*/, 4 /*speed*/);
// OLED Display
EasyOLED<OLED_SCL_PIN, OLED_SDA_PIN, OLED_CS_PIN, OLED_DC_PIN, OLED_RESET_PIN> oled;
// VR module
EasyVoice<VOICE_CMD_ARR, VOICE_CMD_ARR_SZ> voice(VOICE_RX_PIN, VOICE_TX_PIN);
// Counters for each firing mode
EasyCounter apCounter;
EasyCounter inCounter;
EasyCounter heCounter;
EasyCounter fmjCounter;

EasyButton trigger(TRIGGER_PIN, true);
EasyButton reload(RELOAD_PIN, true);

/**
 * function declarations
 */
void startUpSequence(void);
void mainLoop(void);

// main loop functions
void activateLowAmmoIndicators(void);
void resetAmmoIndicators(void);
void handleAmmoDown(void);
void reloadAmmo(void);
bool lowAmmoReached(void);
bool checkTriggerSwitch(void);
bool checkReloadSwitch(void);
void checkVoiceCommands(void);
void setNextAmmoMode(void);
void changeAmmoMode(int mode);


// utility functions
EasyCounter& getTriggerCounter(void);
void playSelectedTrack(uint8_t trackIdx);
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
uint8_t loopStage = LOOP_STATE_START;

/**
 *Variables for tracking the start up sequence updates
 */
unsigned long lastDisplayUpdate = 0;
uint8_t progressBarUpdates      = 0;

/**
 *Variables for toggling an LED
 */
int blinkState                = LOW;
uint8_t ledBlinks             = 0;
unsigned long lastBlinkUpdate = 0;

/**
 * Variables for tracking and updating ammo counts on the OLED
 */
uint8_t counters[4] = { 25, 25, 25, 50 };
volatile uint8_t screenUpdates           = 0;
volatile uint8_t activateLowAmmo         = 0;
volatile unsigned long lowAmmoChangeTime = 0;

/**
 *  Variables for tracking trigger state
 */
volatile uint8_t selectedAmmoMode = VR_CMD_AMMO_MODE_FMJ;  // sets the ammo mode to start
volatile uint8_t activateAmmoDown    = 0;                     // sets main loop to fire a round
volatile uint8_t activateReload      = 0;                     // sets main loop to reload ammo
volatile bool    activateThemeTrack  = 0;                     // play theme track

void setup() {
#if ENABLE_DEBUG == 1
  Serial.begin(115200);
#endif
  DBGLN(F("Starting setup"));

  // initialize all the leds
  initLedIndicators();

  // Initialize the ammo counters for different modes
  apCounter.begin(0, 25, EasyCounter::COUNTER_MODE_DOWN);
  inCounter.begin(0, 25, EasyCounter::COUNTER_MODE_DOWN);
  heCounter.begin(0, 25, EasyCounter::COUNTER_MODE_DOWN);
  fmjCounter.begin(0, 50, EasyCounter::COUNTER_MODE_DOWN);

  // select the initial ammo mode
  selectedAmmoMode = VR_CMD_AMMO_MODE_FMJ;

  //initializes the audio player and sets the volume
  int bootAttempts = 0;
  while (bootAttempts < 3 && !audio.begin(30)) {
    bootAttempts++;
    delay(3000);
  }
  if (bootAttempts == 3) loopStage = LOOP_STATE_ERROR;

  // initialize the trigger led and set brightness
  fireLed.begin(75);

  // init the voice recognition module
  voice.begin();

  // init the display
  oled.begin(selectedAmmoMode, getCounters());

  // set up all the triggers as pullup inputs
  // set up the fire trigger and the debounce threshold
  trigger.begin(25);
  reload.begin(25);
}

/**
 *  The main loop.
 */
void loop() {
  // Shortcut the loop if it's failed
  if (LOOP_STATE_ERROR == loopStage) {
    DBGLN(F("Boot error - check connections and battery level"));
    // try to inform the user
    oled.updateDisplayMode(oled.DISPLAY_BOOT_ERROR, 0);
    loopStage = LOOP_STATE_FAIL;
  }
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
 *  The Main loop Routine
 *  1. Check the AmmoDown activation
 *    a. Toggles a clip counter
 *    b. Checks for an empty clip
 *    c. If clip is not empty
 *       i. queue audio track for selected fire mode
 *      ii. activate front led with blaster shot
 *          1. Flash white
 *          2. Burn Red to orange, then fade as it cools
 *    d. Otherwise queue empty clip track
 *  2. Check the ReloadAmmo activation
 *    a. Resets all counters
 *    b. Queues reload audio track
 *    c. Resets the led indicators
 *    d. Activates OLED update
 *  3. Check if the front leds have been activated
 *    a. update the leds following the pattern that has been set
 *  4. Check audio queue for playback
 *  5. Check low ammo mode has been activated
 *    a. Update the led indicators
 *    b. playback low ammo track
 *  6. Check the recognition module for voice commands
 *    a. playback change mode track
 *    b. toggle fire mode based on the recognized command
 *  7. Refresh or Update the OLED Display
 */
void mainLoop(void) {
  // always check the triggers first
  bool audioPlayed = !checkTriggerSwitch() ? checkReloadSwitch() : true;
  // Update the triggers LEDS in case they were activated. This should always be run in the main loop.
  //if (audioPlayed)   DBGLN(F("main - led update"));
  bool ledsUpdated = fireLed.updateDisplay();

  // check low ammo or voice commands if no audio was played
  if (!activateThemeTrack && !audio.isBusy() && !ledsUpdated) {
    // check the low-ammo indicator
    if (activateLowAmmo) {
      // small delay so not to collide with ammo playback
      if (millis() > (lowAmmoChangeTime + TIMING_LOW_AMMO_WAIT_MS)) {
        activateLowAmmoIndicators();
      }
    } else if (screenUpdates) {
      DBGLN(F("main - screen update"));
      oled.updateDisplay(selectedAmmoMode, getCounters());
      screenUpdates--;
    } else {
      // check for new voice commands, only if no audio sounds were triggered
      //DBGLN(F("main - check VR"));
      checkVoiceCommands();
    }
  }
}

/**
 * Checks the fire trigger momentary switch.
 * - Immediate press should trigger ammo fire
 * - Press and hold for 6 secs will play theme track
 * - Long press and release should change ammo modes
 */
bool checkTriggerSwitch(void) {
  // check trigger button
  int buttonStateFire = trigger.checkState();
  // check if a trigger is pressed.
  if (buttonStateFire == EasyButton::BUTTON_PRESSED) {
    handleAmmoDown();
    activateThemeTrack = 0;
    return true;
  }

  if (buttonStateFire == EasyButton::BUTTON_HOLD_PRESS) {
    if (!activateThemeTrack && trigger.pressedLongerThan(6000)) {
      audio.playTrack(AUDIO_TRACK_THEME);
      activateThemeTrack = 1;
      return true;
    }
  }

  if (buttonStateFire == EasyButton::BUTTON_LONG_PRESS) {
    if (!trigger.pressedLongerThan(6000)) {
      activateThemeTrack = 0;
      setNextAmmoMode();
      return true;
    }
  }
  return false;
}

/**
 *  Checks the reload trigger momentary switch.
 *  Short press should activate the reload sequence.
 */
bool checkReloadSwitch(void) {
  // check trigger button
  int buttonStateFire = reload.checkState();
  // check if a trigger is pressed.
  if (buttonStateFire == EasyButton::BUTTON_SHORT_PRESS || buttonStateFire == EasyButton::BUTTON_LONG_PRESS) {
    reloadAmmo();
    return true;
  }
  return false;
}

/**
 *  Routine for the startup sequence. Heavy OLED updates on startup.
 *  1. Display Props3D logo - (2000ms)
 *  2. Display Comms OK - 90 pixels (900ms)
 *  3. Display DNA Check - wait for trigger press (1000ms)
 *  4. Display DNA Check - 90 pixels (900ms)
 *  5. Display ID FAIL - blink 4 times (1500ms) then solid
 *  6. Display ID OK - blink 3 times with LED - 1000ms
 *  7. Display Judge ID - wait (1800ms)
 *  8. Display Ammo mode
 */
void startUpSequence(void) {
  int _sequenceMode = oled.getDisplayMode();

  if (_sequenceMode == 0) {
    DBGLN(F("Startup - Logo"));
    oled.updateDisplayMode(oled.DISPLAY_LOGO, 0);
    lastDisplayUpdate = millis();
  }
  if (_sequenceMode == oled.DISPLAY_LOGO) {
    if (millis() > (TIMING_STARTUP_LOGO_MS + lastDisplayUpdate)) {
      DBGLN(F("Startup - Comm Ok"));
      oled.updateDisplayMode(oled.DISPLAY_COMM_CHK, 0);
      // Red led on
      toggleLED(RED_LED_PIN);
    }
  }
  if (_sequenceMode == oled.DISPLAY_COMM_CHK) {
    if (millis() > (TIMING_PROGRESS_INTERVAL_MS + lastDisplayUpdate)) {
      oled.updateDisplayMode(oled.DISPLAY_COMM_CHK, progressBarUpdates);
      progressBarUpdates++;
      lastDisplayUpdate = millis();
    }
    if (progressBarUpdates > 9) {
      DBGLN(F("Startup - DNA Chk"));
      oled.updateDisplayMode(oled.DISPLAY_DNA_CHK, progressBarUpdates);
      lastDisplayUpdate = millis();
    }
  }
  if (_sequenceMode == oled.DISPLAY_DNA_CHK) {
    // A trigger press is required to complete the DNA check
    bool buttonPressed = (digitalRead(TRIGGER_PIN) == LOW);
    if (buttonPressed || millis() > (TIMING_STARTUP_DNA_CHK_MS + lastDisplayUpdate)) {
      if (buttonPressed) {
        DBGLN(F("Startup - DNA Progress - start audio"));
        oled.updateDisplayMode(oled.DISPLAY_DNA_PRG, progressBarUpdates);
#ifndef ENABLE_EASY_AUDIO_PRO
        // This is a hack around specifically for df mini players
        audio.playTrack(AUDIO_TRACK_SILENCE);
        delay(1000);
#endif
        audio.playTrack(AUDIO_TRACK_DNA_CHK);
      } else {
        DBGLN(F("Startup - ID FAIL"));
        oled.updateDisplayMode(oled.DISPLAY_ID_FAIL, progressBarUpdates);
#ifndef ENABLE_EASY_AUDIO_PRO
        // This is a hack around specifically for df mini players
        audio.playTrack(AUDIO_TRACK_SILENCE);
        delay(1000);
#endif
        audio.playTrack(AUDIO_TRACK_ID_FAIL);
      }
      lastDisplayUpdate = millis();
    }
  }
  if (_sequenceMode == oled.DISPLAY_DNA_PRG) {
    if (millis() > (TIMING_PROGRESS_INTERVAL_MS + lastDisplayUpdate)) {
      // A trigger press is required to complete the DNA check
      bool buttonPressed = (digitalRead(TRIGGER_PIN) == LOW);
      if (buttonPressed) {
        DBGLN(F("Startup - DNA Progress - tick"));
        oled.updateDisplayMode(oled.DISPLAY_DNA_PRG, progressBarUpdates);
        progressBarUpdates++;
        lastDisplayUpdate = millis();
      } else {
        DBGLN(F("Startup - ID FAIL 2"));
        oled.updateDisplayMode(oled.DISPLAY_ID_FAIL, progressBarUpdates);
        audio.playTrack(AUDIO_TRACK_ID_FAIL);
      }
    }
    if (progressBarUpdates > 18) {
      DBGLN(F("Startup - ID OK"));
      oled.updateDisplayMode(oled.DISPLAY_ID_OK, progressBarUpdates);
      // RED LED off
      toggleLED(RED_LED_PIN);
      lastDisplayUpdate = millis();
    }
  }
  if (_sequenceMode == oled.DISPLAY_ID_OK) {
    if (ledBlinks < 3 && blinkNow()) {
      // blink the ID OK when the Green LED is on
      oled.updateDisplayMode(oled.DISPLAY_ID_OK, progressBarUpdates, blinkState == HIGH);
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
      DBGLN(F("Startup - ID Name"));
      oled.updateDisplayMode(oled.DISPLAY_ID_NAME, progressBarUpdates);
      lastDisplayUpdate = millis();
    }
  }
  // Display ID Name for a fixed duration
  if (_sequenceMode == oled.DISPLAY_ID_NAME) {
    if (millis() > (TIMING_STARTUP_ID_NAME_MS + lastDisplayUpdate)) {
      DBGLN(F("Startup - Main loop"));
      audio.playTrack(AUDIO_TRACK_AMMO_LOAD);
      oled.updateDisplayMode(oled.DISPLAY_MAIN, progressBarUpdates);
      // let's turn off the ammo indicators
      resetAmmoIndicators();
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
      oled.updateDisplayMode(oled.DISPLAY_ID_FAIL, progressBarUpdates, blinkState == HIGH);
      // toggle the LED after OLED update
      setBlinkState(RED_LED_PIN);
      // count only when LED is on
      if (blinkState == HIGH) {
        DBGLN(F("Startup - ID FAIL BLINK"));
        ledBlinks++;
      }
    }
    // blink the red LED four times before moving on
    if (ledBlinks > 3 && millis() > (TIMING_STARTUP_ID_FAIL_MS + lastDisplayUpdate)) {
      // switch to main loop
      loopStage = LOOP_STATE_FAIL;
    }
  }
  if (_sequenceMode != oled.getDisplayMode())
    DBGLN(F("Startup - changing modes"));
}

/**
 *  Routine for resetting the ammo counters
 *  1. all counters are reset
 *  2. leds are turned off
 *  3. queue audio track
 *  4. activate oled refresh
 */
void reloadAmmo(void) {
  activateReload = 0;
  activateLowAmmo = 0;

  //DBGLN(F("Reloading all counters"));
  apCounter.resetCount();
  inCounter.resetCount();
  heCounter.resetCount();
  fmjCounter.resetCount();
  // Reset the low-ammo indicator
  resetAmmoIndicators();
  //play the track
  audio.playTrack(AUDIO_TRACK_AMMO_RELOAD);
  // Refresh the display at least once
  ++screenUpdates;
}

/**
 * Sends a blaster pulse.
 *   1. Toggles a clip counter
 *    2. Checks for an empty clip
 *       a. play empty clip track
 *    3. If clip is not empty
 *       a. queue audio track
 *       b. activate led strip
 *       c. activate oled refresh
 *       d. Check for low ammo
 */
void handleAmmoDown(void) {
  activateAmmoDown = 0;
  // move the counter
  bool emptyClip = !getTriggerCounter().tick();
  if (emptyClip) {
    //DBGLN(F("Empty clip"));
    playSelectedTrack(AMMO_MODE_IDX_EMTY);
    return;
  }
  //DBGLN(F("Ammo fire sequence"));
  //play the track
  playSelectedTrack(AMMO_MODE_IDX_FIRE);
  // activate the led pulse
  //DBGLN(F("handleAmmo - activate leds"));
  if (selectedAmmoMode == VR_CMD_AMMO_MODE_RAPID)
    fireLed.activate(repeatingShot);  // rapid shot - mulitple flashes with fade
  else
    fireLed.activate(blasterShot);

  // check for low ammo, and set the timer
  if (lowAmmoReached()) {
    activateLowAmmo++;
    lowAmmoChangeTime = millis();
  }

  // Refresh the display at least once
  ++screenUpdates;
}

/**
 * Convenience method for playing a track and specifying the wait time (delay)
 */
void playSelectedTrack(uint8_t trackIdx) {
  if (selectedAmmoMode == VR_CMD_AMMO_MODE_RAPID && trackIdx == AMMO_MODE_IDX_FIRE)
    audio.playTrack(getSelectedTrack(trackIdx), 200L);  // give the rapid fire a little extra time
  else
    audio.playTrack(getSelectedTrack(trackIdx));
}
/**
 *  Convenience method for selecting a track to playback based on the selected
 *  trigger mode and a state variable
 */
uint8_t getSelectedTrack(uint8_t trackIdx) {
  return AUDIO_TRACK_AMMO_MODE_ARR[selectedAmmoMode][trackIdx];
}

/**
 *  Routine for getting the selected trigger counter
 */
EasyCounter& getTriggerCounter(void) {
  if (selectedAmmoMode == VR_CMD_AMMO_MODE_AP)
    return apCounter;
  if (selectedAmmoMode == VR_CMD_AMMO_MODE_IN || selectedAmmoMode == VR_CMD_AMMO_MODE_HS)
    return inCounter;
  if (selectedAmmoMode == VR_CMD_AMMO_MODE_HE)
    return heCounter;
  return fmjCounter;
}

/**
 *  Change to the next ammo mode in the cycle
 *  1. change the selected ammo mode
 *  2. initialize the LED sequence
 *  3. queue playback
 *  4. set screen refresh
 */
void setNextAmmoMode(void) {
  // Increment the trigger mode index or reset to 0
  int mode = selectedAmmoMode + 1;
  if (mode == 7) mode = 0;
  changeAmmoMode(mode);
}

/**
 *  Checks the voice recognition module for new voice commands
 *  1. change the selected ammo mode
 *  2. initialize the LED sequence
 *  3. queue playback
 *. 4. set screen refresh
 */
void checkVoiceCommands(void) {
  int cmd = voice.readCommand();

  if (cmd > -1) {
    changeAmmoMode(cmd);
  }
}

/**
 *  1. Set the selected ammo mode
 *  2. Initialize the LED sequence
 *  3. Queue playback
 *  4. Set screen refresh
 */
void changeAmmoMode(int mode) {
  if (mode > -1 && mode < 7) {
    selectedAmmoMode = mode;
    // Check for Switching modes
    if (selectedAmmoMode == VR_CMD_AMMO_MODE_AP) {
      blasterShot.initialize(fireLed.RED, fireLed.ORANGE);  // shot - flash with color fade
      //DBGLN(F("Armor Piercing Mode selected"));
    }
    if (selectedAmmoMode == VR_CMD_AMMO_MODE_IN) {
      blasterShot.initialize(fireLed.ORANGE, fireLed.WHITE);  // shot - flash with color fade
      //DBGLN(F("Incendiary Mode selected"));
    }
    if (selectedAmmoMode == VR_CMD_AMMO_MODE_HE) {
      blasterShot.initialize(fireLed.ORANGE, fireLed.WHITE);  // shot - flash with color fade
      //DBGLN(F("High Ex Mode selected"));
    }
    if (selectedAmmoMode == VR_CMD_AMMO_MODE_HS) {
      blasterShot.initialize(fireLed.RED, fireLed.ORANGE);  // shot - flash with color fade
      //DBGLN(F("Hotshot Mode selected"));
    }
    if (selectedAmmoMode == VR_CMD_AMMO_MODE_ST) {
      blasterShot.initialize(fireLed.YELLOW, fireLed.WHITE);  // shot - flash with color fade
      //DBGLN(F("Stun Mode selected"));
    }
    if (selectedAmmoMode == VR_CMD_AMMO_MODE_FMJ) {
      blasterShot.initialize(fireLed.RED, fireLed.ORANGE);  // shot - flash with color fade
      //DBGLN(F("FMJ Mode selected"));
    }
    if (selectedAmmoMode == VR_CMD_AMMO_MODE_RAPID) {
      repeatingShot.initialize(fireLed.WHITE, fireLed.BLACK);  // shot - flash with color fade
      //DBGLN(F("Rapid Mode selected"));
    }
    // check for low ammo, and set the timer
    if (lowAmmoReached()) {
      ++activateLowAmmo;
      lowAmmoChangeTime = millis();
    } else {
      // Reset the low-ammo indicator
      resetAmmoIndicators();
    }
    playSelectedTrack(AMMO_MODE_IDX_CHGE);
    // Refresh the display at least once
    ++screenUpdates;
  }
}

/**
 *  Convenient method for collecting ammo counters for display updates.
 */
uint8_t* getCounters(void) {
  counters[0] = apCounter.getCount();
  counters[1] = inCounter.getCount();
  counters[2] = heCounter.getCount();
  counters[3] = fmjCounter.getCount();
  return counters;
}

/**
 *  Set Red/Green leds on startup
 */
void initLedIndicators(void) {
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
}

/**
 *  Test whether we've reached low ammo condition
 */
bool lowAmmoReached(void) {
  return getTriggerCounter().getCount() < 5;
}

/**
 *  Set Red/Green leds when low on ammo
 */
void activateLowAmmoIndicators(void) {
  activateLowAmmo = 0;
  // let's make sure the screen redraws to count the low ammo before switching the LEDs
  oled.checkAmmoLevels();
  oled.updateDisplay(selectedAmmoMode, getCounters());
  // set the LED
  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(GREEN_LED_PIN, LOW);
  // playback for low ammo immediately
  audio.playTrack(AUDIO_TRACK_AMMO_LOW);
}

/**
 *  Set Red/Green leds when low on ammo
 */
void resetAmmoIndicators(void) {
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
}
/**
 *  Blink controller.
 *  Returns true when it's time to toggle state every 500ms
 */
bool blinkNow(void) {
  if (millis() > (lastBlinkUpdate + TIMING_FAST_BLINK_WAIT_MS)) {
    lastBlinkUpdate = millis();
    blinkState = !blinkState;
    return true;
  }
  return false;
}

/**
 *  Sets the state of the LED pin
 */
void setBlinkState(uint8_t pin) {
  digitalWrite(pin, blinkState);
}

/**
 *  Toggles LED state immediately
 */
void toggleLED(uint8_t pin) {
  blinkState = !blinkState;
  digitalWrite(pin, blinkState);
}
