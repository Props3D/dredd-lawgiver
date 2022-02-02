#ifndef blasterconfig_h
#define blasterconfig_h

/**
 * This file can be used to define the configuration of the circuit.
 * 
 * For each component that is enabled you should update the related 
 * configuration section.
 */

/** 
 * Enable debug logging by uncommenting this line
 */
//#define ENABLE_DEBUG          1

// Comment out if you want to disable any component
#define ENABLE_EASY_AUDIO     1 //Enable audio 
#define ENABLE_EASY_LED       1 //Enable LEDs
#define ENABLE_EASY_OLED      1 //Enable OLED Display
#define ENABLE_EASY_VOICE     1 //Enable Voice Recognition Display

// Customizable ID badge for DNA Check sequence 
#define DISPLAY_USER_ID       "Dredd"

// Pin configuration for all momentary triggers
#define TRIGGER_PIN           2
#define RELOAD_PIN            3

// Pin configuration for MP3 Player
#define AUDIO_RX_PIN          4
#define AUDIO_TX_PIN          5

// Pin configuration for MP3 Player
#define VOICE_RX_PIN          6
#define VOICE_TX_PIN          7

// Pin configuration for oled display
#define OLED_SCL_PIN          13
#define OLED_SDA_PIN          11
#define OLED_RESET_PIN        9
#define OLED_DC_PIN           10
#define OLED_CS_PIN           8

// Pin configuration for front barrel WS2812B LED
// set these to 0 if you want to disable the component
#define FIRE_LED_PIN          12
#define FIRE_LED_CNT          7

// Pin Configuration for 3mm LEDs
#define RED_LED_PIN           14
#define GREEN_LED_PIN         15

/**
 * Audio tracks by file index - upload these to the SD card in the correct order.
 * See the instructions on the Github page about loading the SD card.
 * 
 * You can reuse sound effects for each slot with two options:
 * 1. Copy the audio file in each slot where you want to use it
 * 2. Change the index values below to match the files on the SD Card
 */
#define AUDIO_TRACK_DNA_CHK         1
#define AUDIO_TRACK_ID_OK           2
#define AUDIO_TRACK_ID_FAIL         3
#define AUDIO_TRACK_AMMO_LOAD       4
#define AUDIO_TRACK_AP_FIRE         5
#define AUDIO_TRACK_IN_FIRE         6
#define AUDIO_TRACK_HS_FIRE         7
#define AUDIO_TRACK_HE_FIRE         8
#define AUDIO_TRACK_ST_FIRE         9
#define AUDIO_TRACK_FMJ_FIRE        10
#define AUDIO_TRACK_RAPID_FIRE      11
#define AUDIO_TRACK_AP_CHANGE       12
#define AUDIO_TRACK_IN_CHANGE       13
#define AUDIO_TRACK_HS_CHANGE       14
#define AUDIO_TRACK_HE_CHANGE       15
#define AUDIO_TRACK_ST_CHANGE       16
#define AUDIO_TRACK_FMJ_CHANGE      17
#define AUDIO_TRACK_RAPID_CHANGE    18
#define AUDIO_TRACK_AMMO_LOW        19
#define AUDIO_TRACK_AMMO_EMPTY      20
#define AUDIO_TRACK_AMMO_RELOAD     21
#define AUDIO_TRACK_THEME           22  // TODO: add feature to playback theme


/**
 *  Common constant definitions - DO NOT CHANGE
 */

/**
 * Timing Defintions for start up sequence timings
 */
#define TIMING_STARTUP_LOGO_MS           2000
#define TIMING_STARTUP_COMM_OK_MS        1000
#define TIMING_STARTUP_DNA_CHK_MS        1200
#define TIMING_STARTUP_DNA_PRG_MS        1000
#define TIMING_STARTUP_ID_OK_MS          2400
#define TIMING_STARTUP_ID_FAIL_MS        1400
#define TIMING_STARTUP_ID_NAME_MS        1800
#define TIMING_PROGRESS_INTERVAL_MS      100
#define TIMING_LOW_AMMO_WAIT_MS          1000
#define TIMING_FAST_BLINK_WAIT_MS        350

/**
 *  Voice Recognition Commands
 *  Each command must be trained to follow this sequence.
 *  
 *  IMPORTANT - if you change this sequence, you must update the VOICE_RECORDS_ARR
 */
#define VR_CMD_AMMO_MODE_AP         0 // speak "Armor Piercing" or just "Armor"
#define VR_CMD_AMMO_MODE_IN         1 // speak "Incendiary"
#define VR_CMD_AMMO_MODE_HS         2 // speak "Hotshot"
#define VR_CMD_AMMO_MODE_HE         3 // speak "High Ex"
#define VR_CMD_AMMO_MODE_ST         4 // speak "Stun"
#define VR_CMD_AMMO_MODE_FMJ        5 // speak "Full Metal"
#define VR_CMD_AMMO_MODE_RAPID      6 // speak "Rapid"

/**
 * These are only useful if the autoload feature is not enabled.
 * See code comments in the EasyVoice.begins() function.
 */
static const uint8_t VOICE_CMD_ARR_SZ   = 7;
static const uint8_t VOICE_CMD_ARR[]    = {VR_CMD_AMMO_MODE_AP, VR_CMD_AMMO_MODE_IN, VR_CMD_AMMO_MODE_HS, VR_CMD_AMMO_MODE_HE, VR_CMD_AMMO_MODE_ST, VR_CMD_AMMO_MODE_FMJ, VR_CMD_AMMO_MODE_RAPID};

/**
 * Defintions for tracking startup sequence. DO NOT CHANGE
 */
#define LOOP_STATE_START              2  // Startup sequence
#define LOOP_STATE_MAIN               1  // main loop
#define LOOP_STATE_FAIL               0  // stop loop

/**
 * Audio track index for lookup array. DO NOT CHANGE
 */
#define AMMO_MODE_IDX_FIRE        0  // fire
#define AMMO_MODE_IDX_EMTY        1  // empty clip
#define AMMO_MODE_IDX_CHGE        2  // change mode

/**
 * Lookup array for audio tracks based on ammo mode. DO NOT CHANGE
 */
static const uint8_t AUDIO_TRACK_AMMO_MODE_ARR[7][3]  =  {
  {AUDIO_TRACK_AP_FIRE,    AUDIO_TRACK_AMMO_EMPTY, AUDIO_TRACK_AP_CHANGE},
  {AUDIO_TRACK_IN_FIRE,    AUDIO_TRACK_AMMO_EMPTY, AUDIO_TRACK_IN_CHANGE},
  {AUDIO_TRACK_HS_FIRE,    AUDIO_TRACK_AMMO_EMPTY, AUDIO_TRACK_HS_CHANGE},
  {AUDIO_TRACK_HE_FIRE,    AUDIO_TRACK_AMMO_EMPTY, AUDIO_TRACK_HE_CHANGE},
  {AUDIO_TRACK_ST_FIRE,    AUDIO_TRACK_AMMO_EMPTY, AUDIO_TRACK_ST_CHANGE},
  {AUDIO_TRACK_FMJ_FIRE,   AUDIO_TRACK_AMMO_EMPTY, AUDIO_TRACK_FMJ_CHANGE},
  {AUDIO_TRACK_RAPID_FIRE, AUDIO_TRACK_AMMO_EMPTY, AUDIO_TRACK_RAPID_CHANGE}
};

#endif
