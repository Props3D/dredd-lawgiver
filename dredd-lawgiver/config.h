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
#define ENABLE_DEBUG          1

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
#define TRACK_DNA_CHK         1
#define TRACK_ID_OK           2
#define TRACK_ID_FAIL         3
#define TRACK_AMMO_LOAD       4
#define TRACK_AP_FIRE         5
#define TRACK_IN_FIRE         6
#define TRACK_HE_FIRE         7
#define TRACK_HS_FIRE         8
#define TRACK_ST_FIRE         9
#define TRACK_FMJ_FIRE        10
#define TRACK_RAPID_FIRE      11
#define TRACK_AP_CHANGE       12
#define TRACK_IN_CHANGE       13
#define TRACK_HE_CHANGE       14
#define TRACK_HS_CHANGE       15
#define TRACK_ST_CHANGE       16
#define TRACK_FMJ_CHANGE      17
#define TRACK_RAPID_CHANGE    18
#define TRACK_AMMO_LOW        19
#define TRACK_AMMO_EMPTY      20
#define TRACK_AMMO_RELOAD     21
#define TRACK_START_UP        22
#define TRACK_THEME           23  // TODO: add feature to playback theme


/**
 *  Common constant definitions - DO NOT CHANGE
 */

/**
 * Timing Defintions for start up sequence timings
 */
#define STARTUP_LOGO_MS           2000
#define STARTUP_COMM_OK_MS        1000
#define STARTUP_DNA_CHK_MS        1200
#define STARTUP_DNA_PRG_MS        1000
#define STARTUP_ID_OK_MS          2500
#define STARTUP_ID_FAIL_MS        2000
#define STARTUP_ID_NAME_MS        1800
#define STARTUP_END_MS            1000
#define PROGRESS_INTERVAL_MS      100

/**
 *  Voice Recognition Commands
 *  Each command must be trained to follow this sequence.
 *  
 *  IMPORTANT - if you change this sequence, you must update the VOICE_RECORDS_ARR
 */
#define SELECTOR_AP_MODE         0 // speak "Armor Piercing" or just "Armor"
#define SELECTOR_IN_MODE         1 // speak "Incendiary"
#define SELECTOR_HS_MODE         2 // speak "Hotshot"
#define SELECTOR_HE_MODE         3 // speak "High Ex"
#define SELECTOR_ST_MODE         4 // speak "Stun"
#define SELECTOR_FMJ_MODE        5 // speak "Full Metal"
#define SELECTOR_RAPID_MODE      6 // speak "Rapid"

/**
 * These are only useful if the autoload feature is not enabled.
 * See code comments in the EasyVoice.begins() function.
 */
static const uint8_t VOICE_RECORDS_ARR_SZ   = 7;
static const uint8_t VOICE_RECORDS_ARR[]    = {SELECTOR_AP_MODE, SELECTOR_IN_MODE, SELECTOR_HS_MODE, SELECTOR_HE_MODE, SELECTOR_ST_MODE, SELECTOR_FMJ_MODE, SELECTOR_RAPID_MODE};

/**
 * Defintions for tracking startup sequence. DO NOT CHANGE
 */
#define LOOP_STATE_START              2  // Startup sequence
#define LOOP_STATE_MAIN               1  // main loop
#define LOOP_STATE_FAIL               0  // stop loop

/**
 * Track index for ammo mode. DO NOT CHANGE
 */
#define AMMO_MODE_FIRE_IDX        0  // fire
#define AMMO_MODE_EMTY_IDX        1  // empty clip
#define AMMO_MODE_CHGE_IDX        2  // change mode

/**
 * Lookup array for audio tracks based on ammo mode. DO NOT CHANGE
 */
static const uint8_t TRACK_AMMO_MODE_ARR[7][3]  =  {
  {TRACK_AP_FIRE,    TRACK_AMMO_EMPTY, TRACK_AP_CHANGE},
  {TRACK_IN_FIRE,    TRACK_AMMO_EMPTY, TRACK_IN_CHANGE},
  {TRACK_HS_FIRE,    TRACK_AMMO_EMPTY, TRACK_HS_CHANGE},
  {TRACK_HE_FIRE,    TRACK_AMMO_EMPTY, TRACK_HE_CHANGE},
  {TRACK_ST_FIRE,    TRACK_AMMO_EMPTY, TRACK_ST_CHANGE},
  {TRACK_FMJ_FIRE,   TRACK_AMMO_EMPTY, TRACK_FMJ_CHANGE},
  {TRACK_RAPID_FIRE, TRACK_AMMO_EMPTY, TRACK_RAPID_CHANGE}
};

#endif
