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
#define ENABLE_DEBUG                 1

// Comment out if you want to disable any component
#define ENABLE_EASY_AUDIO            1 //Enable all audio 
#define ENABLE_EASY_BUTTON           1 //Enable all buttons
#define ENABLE_EASY_OLED             1 //Enable OLED Display
#define ENABLE_EASY_VOICE            1 //Enable Voice Recognition Display
#define ENABLE_EASY_VOICE_AUTOLOAD   1 //Enable OLED Display

// Configurable User ID
#define DISPLAY_USER_ID              "Dredd"

// Pin configuration for MP3 Player
#define AUDIO_RX_PIN        6
#define AUDIO_TX_PIN        5

// Pin configuration for MP3 Player
#define VOICE_RX_PIN        3
#define VOICE_TX_PIN        4

// Pin configuration for all momentary triggers
#define TRIGGER_PIN         2

// Pin configuration for front barrel WS2812B LED
#define FIRE_LED_PIN        7
#define FIRE_LED_CNT        7

// Pin Configuration for 3mm LEDs
#define RED_LED_PIN         14
#define GREEN_LED_PIN       15

// Pin configuration for oled display
#define OLED_CL_PIN         13
#define OLED_DC_PIN         11
#define OLED_CS_PIN         10
#define OLED_DC_PIN         9
#define OLED_RESET_PIN      8

// track by file index - upload these to the SD card in the correct order
#define TRACK_START_UP        1
#define TRACK_CHANGE_MODE     2
#define TRACK_FIRE            3
#define TRACK_HOTSHOT         4
#define TRACK_HIGHEX          5
#define TRACK_STUN            6
#define TRACK_CLIP_RELOAD     7
#define TRACK_CLIP_EMPTY      8
#define TRACK_DNA_CHK         9
#define TRACK_DNA_FAIL        10
#define TRACK_THEME           11

// Common constant definitions - DO NOT CHANGE
static const uint8_t TRACK_FIRE_ARR[]    = {TRACK_FIRE, TRACK_CLIP_EMPTY, TRACK_CLIP_RELOAD};
static const uint8_t TRACK_INCD_ARR[]    = {TRACK_HOTSHOT, TRACK_CLIP_EMPTY, TRACK_CLIP_RELOAD};
static const uint8_t TRACK_HIEX_ARR[]    = {TRACK_HIGHEX, TRACK_CLIP_EMPTY, TRACK_CLIP_RELOAD};
static const uint8_t TRACK_STUN_ARR[]    = {TRACK_STUN, TRACK_CLIP_EMPTY, TRACK_CLIP_RELOAD};

// Voice Recognition Commands
#define SELECTOR_ARMOR_MODE         0
#define SELECTOR_HOTSHOT_MODE       1
#define SELECTOR_HIGHEX_MODE        2
#define SELECTOR_STUN_MODE          3

static const uint8_t VOICE_RECORDS_ARR_SZ   = 4;
static const uint8_t VOICE_RECORDS_ARR[]    = {SELECTOR_ARMOR_MODE, SELECTOR_HOTSHOT_MODE, SELECTOR_HIGHEX_MODE, SELECTOR_STUN_MODE};
#endif
