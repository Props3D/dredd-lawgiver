#ifndef blasterconfig_h
#define blasterconfig_h

/**
 * This file can be used to define the configuration of the circuit.
 * 
 * For each component that is enabled you should update the related 
 * configuration section.
 */

// To disable any component set value to 0
#define ENABLE_DEBUG            0 //Enable Debugging
#define ENABLE_EASY_AUDIO       1 //Enable audio
#define ENABLE_EASY_LED         1 //Enable LEDs
#define ENABLE_EASY_OLED        1 //Enable OLED Display
#define ENABLE_EASY_VOICE       1 //Enable Voice Recognition
#define ENABLE_EASY_BUTTON      1 //Enable triggers


// Customizable ID badge for DNA Check sequence 
const char DISPLAY_USER_ID[] PROGMEM =      {"Dredd"};


// Pin configuration for voice recognition module
#define VOICE_TX_PIN          3
#define VOICE_RX_PIN          2

// Pin configuration for MP3 Player
#define AUDIO_TX_PIN          5
#define AUDIO_RX_PIN          4

// Pin Configuration for 3mm LEDs
#define GREEN_LED_PIN         6
#define RED_LED_PIN           7

// Pin configuration for all momentary triggers
#define TRIGGER_PIN           8
#define RELOAD_PIN            9

// Pin configuration for oled display
#define OLED_SCL_PIN          A5
#define OLED_SDA_PIN          A4
#define OLED_RESET_PIN        10
#define OLED_DC_PIN           11
#define OLED_CS_PIN           12


// Pin configuration for front barrel WS2812B LED
// set these to 0 if you want to disable the component
#define FIRE_LED_PIN          13
#define FIRE_LED_CNT          7


/**
 *  Common constant definitions - DO NOT CHANGE
 */
#define FASTLED_USE_PROGMEM 1

/**
 * Audio tracks by file index - upload these to the SD card in the correct order.
 * See the instructions on the Github page about loading the SD card.
 * 
 * You can reuse sound effects for each slot with two options:
 * 1. Copy the audio file in each slot where you want to use it
 * 2. Change the index values below to match the files on the SD Card
 */
static const int  AUDIO_TRACK_DNA_CHK      =   1;
static const int  AUDIO_TRACK_ID_OK        =   2;
static const int  AUDIO_TRACK_ID_FAIL      =   3;
static const int  AUDIO_TRACK_AMMO_LOAD    =   4;
static const int  AUDIO_TRACK_AP_FIRE      =   5;
static const int  AUDIO_TRACK_IN_FIRE      =   6;
static const int  AUDIO_TRACK_HS_FIRE      =   7;
static const int  AUDIO_TRACK_HE_FIRE      =   8;
static const int  AUDIO_TRACK_ST_FIRE      =   9;
static const int  AUDIO_TRACK_FMJ_FIRE     =   10;
static const int  AUDIO_TRACK_RAPID_FIRE   =   11;
static const int  AUDIO_TRACK_AP_CHANGE    =   12;
static const int  AUDIO_TRACK_IN_CHANGE    =   13;
static const int  AUDIO_TRACK_HS_CHANGE    =   14;
static const int  AUDIO_TRACK_HE_CHANGE    =   15;
static const int  AUDIO_TRACK_ST_CHANGE    =   16;
static const int  AUDIO_TRACK_FMJ_CHANGE   =   17;
static const int  AUDIO_TRACK_RAPID_CHANGE =   18;
static const int  AUDIO_TRACK_AMMO_LOW     =   19;
static const int  AUDIO_TRACK_AMMO_EMPTY   =   20;
static const int  AUDIO_TRACK_AMMO_RELOAD  =   21;
static const int  AUDIO_TRACK_SPACER       =   22;  
static const int  AUDIO_TRACK_THEME        =   23;  // TODO: add feature to playback theme


/**
 *  Voice Recognition Commands.
 *  IMPORTANT - The VR module must be trained this sequence.
 */
static const uint8_t VR_CMD_AMMO_MODE_AP     =    0; // speak "Armor Piercing" or just "Armor"
static const uint8_t VR_CMD_AMMO_MODE_IN     =    1; // speak "Incendiary"
static const uint8_t VR_CMD_AMMO_MODE_HS     =    2; // speak "Hotshot"
static const uint8_t VR_CMD_AMMO_MODE_HE     =    3; // speak "High Ex"
static const uint8_t VR_CMD_AMMO_MODE_ST     =    4; // speak "Stun"
static const uint8_t VR_CMD_AMMO_MODE_FMJ    =    5; // speak "Full Metal"
static const uint8_t VR_CMD_AMMO_MODE_RAPID  =    6; // speak "Rapid"

/**
 * Timing Defintions for start up sequence timings
 */
static const long  TIMING_STARTUP_LOGO_MS       =    2000L;
static const long  TIMING_STARTUP_COMM_OK_MS    =    1000L;
static const long  TIMING_STARTUP_DNA_CHK_MS    =    1200L;
static const long  TIMING_STARTUP_DNA_PRG_MS    =    1000L;
static const long  TIMING_STARTUP_ID_OK_MS      =    2400L;
static const long  TIMING_STARTUP_ID_FAIL_MS    =    1400L;
static const long  TIMING_STARTUP_ID_NAME_MS    =    1800L;
static const long  TIMING_PROGRESS_INTERVAL_MS  =    100L;
static const long  TIMING_LOW_AMMO_WAIT_MS      =    1000L;
static const long  TIMING_FAST_BLINK_WAIT_MS    =    350L;


/**
 * These are only useful if the autoload feature is not enabled.
 * See code comments in the EasyVoice.begins() function.
 */
static const uint8_t VOICE_CMD_ARR_SZ   = 7;
static const uint8_t VOICE_CMD_ARR[]    = {VR_CMD_AMMO_MODE_AP, VR_CMD_AMMO_MODE_IN, VR_CMD_AMMO_MODE_HS, VR_CMD_AMMO_MODE_HE, VR_CMD_AMMO_MODE_ST, VR_CMD_AMMO_MODE_FMJ, VR_CMD_AMMO_MODE_RAPID};

/**
 * Defintions for tracking startup sequence. DO NOT CHANGE
 */
static const uint8_t  LOOP_STATE_ERROR    =          3;  // Boot Error
static const uint8_t  LOOP_STATE_START    =          2;  // Startup sequence
static const uint8_t  LOOP_STATE_MAIN     =          1;  // main loop
static const uint8_t  LOOP_STATE_FAIL     =          0;  // stop loop

/**
 * Audio track index for lookup array. DO NOT CHANGE
 */
static const uint8_t AMMO_MODE_IDX_FIRE  =      0;  // fire
static const uint8_t AMMO_MODE_IDX_EMTY  =      1;  // empty clip
static const uint8_t AMMO_MODE_IDX_CHGE  =      2;  // change mode

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

/**
 * DEBUG Macros
 */
extern inline void DBGLOG(const char* message) {
#if ENABLE_DEBUG == 1
   Serial.println(message);
#endif
}
extern inline void DBGLN(const __FlashStringHelper* message) {
#if ENABLE_DEBUG == 1
   Serial.println(message);
#endif
}
extern inline void DBGSTR(const __FlashStringHelper* message) {
#if ENABLE_DEBUG == 1
   Serial.print(message);
#endif
}

#endif
