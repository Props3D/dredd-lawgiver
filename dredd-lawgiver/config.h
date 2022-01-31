#ifndef blasterconfig_h
#define blasterconfig_h

/**
 * This file can be used to define the configuration of the circuit.
 * 
 * For each component that is enabled you should update the related 
 * configuration section.
 */

/** 
 * Enable debug logging:
 *    enabled == 1
 *    disabled == 0
 */
#define ENABLE_DEBUG                 0

// Comment out if you want to disable any component
#define ENABLE_EASY_AUDIO            1 //Enable all audio 
#define ENABLE_EASY_LED              1 //Enable LEDs
#define ENABLE_EASY_OLED             1 //Enable OLED Display
#define ENABLE_EASY_VOICE            1 //Enable Voice Recognition Display

// Configurable User ID
#define DISPLAY_USER_ID              "Dredd"

// Pin configuration for MP3 Player
#define AUDIO_RX_PIN        4
#define AUDIO_TX_PIN        5

// Pin configuration for MP3 Player
#define VOICE_RX_PIN        6
#define VOICE_TX_PIN        7

// Pin configuration for all momentary triggers
#define TRIGGER_PIN         2
#define RELOAD_PIN          3

// Pin configuration for front barrel WS2812B LED
// set these to 0 if you want to disable the component
#define FIRE_LED_PIN        12
#define FIRE_LED_CNT        7

// Pin Configuration for 3mm LEDs
#define RED_LED_PIN         14
#define GREEN_LED_PIN       15

// Pin configuration for oled display
#define OLED_SCL_PIN        13
#define OLED_SDA_PIN        11
#define OLED_RESET_PIN      9
#define OLED_DC_PIN         10
#define OLED_CS_PIN         8

// track by file index - upload these to the SD card in the correct order
#define TRACK_DNA_CHK         1
#define TRACK_CHANGE_MODE     2
#define TRACK_AP_FIRE         3
#define TRACK_IN_FIRE         4
#define TRACK_HE_FIRE         5
#define TRACK_HS_FIRE         6
#define TRACK_ST_FIRE         7
#define TRACK_FMJ_FIRE        8
#define TRACK_RAPID_FIRE      9
#define TRACK_CLIP_EMPTY      10
#define TRACK_CLIP_RELOAD     11
#define TRACK_DNA_FAIL        12
#define TRACK_START_UP        99
#define TRACK_ID_OK           99
#define TRACK_THEME           99  // TODO: add feature to playback theme

/**
 * Defintions for tracking startup sequence
 */
#define STARTUP_LOOP              2  // Startup sequence
#define MAIN_LOOP                 1  // main loop
#define STOP_LOOP                 0  // stop loop

/**
 * Timing Defintions for start up sequence timings
 */
#define STARTUP_LOGO_MS           2000  // 2 sec
#define STARTUP_COMM_OK_MS        3200  // 1.2 sec
#define STARTUP_DNA_CHK_MS        4200  // 1 sec
#define STARTUP_DNA_PRG_MS        5700  // 1.5 sec
#define STARTUP_ID_OK_MS          7500  // 1.8 sec
#define STARTUP_JUDGE_NAME_MS     9300  // 1.8 sec
#define STARTUP_END_MS            10000

// Common constant definitions - DO NOT CHANGE
static const uint8_t TRACK_AP_ARR[]       = {TRACK_AP_FIRE, TRACK_CLIP_EMPTY, TRACK_CLIP_RELOAD};
static const uint8_t TRACK_IN_ARR[]       = {TRACK_IN_FIRE, TRACK_CLIP_EMPTY, TRACK_CLIP_RELOAD};
static const uint8_t TRACK_HS_ARR[]       = {TRACK_HS_FIRE, TRACK_CLIP_EMPTY, TRACK_CLIP_RELOAD};
static const uint8_t TRACK_HE_ARR[]       = {TRACK_HE_FIRE, TRACK_CLIP_EMPTY, TRACK_CLIP_RELOAD};
static const uint8_t TRACK_ST_ARR[]       = {TRACK_ST_FIRE, TRACK_CLIP_EMPTY, TRACK_CLIP_RELOAD};
static const uint8_t TRACK_FMJ_ARR[]      = {TRACK_FMJ_FIRE, TRACK_CLIP_EMPTY, TRACK_CLIP_RELOAD};
static const uint8_t TRACK_RAPID_ARR[]    = {TRACK_RAPID_FIRE, TRACK_CLIP_EMPTY, TRACK_CLIP_RELOAD};

/**
 *  Voice Recognition Commands
 *  Each command must be trained to follow this sequence.
 */
#define SELECTOR_AP_MODE         0 // speak "Armor Piercing"
#define SELECTOR_IN_MODE         1 // speak "Incendiary"
#define SELECTOR_HS_MODE         2 // speak "Hotshot"
#define SELECTOR_HE_MODE         3 // speak "High Ex"
#define SELECTOR_ST_MODE         4 // speak "Stun"
#define SELECTOR_FMJ_MODE        5 // speak "Full Metal"
#define SELECTOR_RAPID_MODE      6 // speak "Rapid"


static const uint8_t VOICE_RECORDS_ARR_SZ   = 7;
static const uint8_t VOICE_RECORDS_ARR[]    = {SELECTOR_AP_MODE, SELECTOR_IN_MODE, SELECTOR_HS_MODE, SELECTOR_HE_MODE, SELECTOR_ST_MODE, SELECTOR_FMJ_MODE, SELECTOR_RAPID_MODE};
#endif
