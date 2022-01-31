## dredd-lawgiver V0.1 - Prerelease


Repository for the Dredd Lawgiver from Props3D. 

Licensed under the Creative Commons - Attribution - Non-Commercial - Share Alike license. https://creativecommons.org/licenses/by-nc-sa/4.0/

***DO NOT SELL OR DISTRIBUTE MODELS OR CODE WITHOUT PERMISSION***

Note that prerelease models have been tested and work, but do not come with instructions or guarantee that everything fits together properly. If you want have feedback please post on our discord(https://discord.gg/NSfZcCfJU6) on the #project-starwars-se14r channel

## Props 3D
* Join the Props3D Discord: https://discord.gg/NSfZcCfJU6
* Follow progress on instagram: https://www.instagram.com/props3dpro/ or https://www.instagram.com/craiggulliver/
* Subscribe on Youtube: https://www.youtube.com/c/Props3D
* Download released designs: https://www.thingiverse.com/props3dpro/designs

## Electonic Components

Electronic | Components
------------ | -------------
![Arduino Nano v3](https://user-images.githubusercontent.com/20442880/137374161-c34e9e7f-19bf-4b33-ba91-6ae4edc709ff.jpg) | ![DF Player Mini with Micro SD Card](https://user-images.githubusercontent.com/20442880/137374190-bcfa4d14-6f8d-4973-b493-c87bd1d0676d.jpg)
Arduino Nano v3 | DF Player Mini + 64mb Micro SD Card
![PAM8302A Amp](https://user-images.githubusercontent.com/20442880/137374587-abdad903-b008-4d2e-830e-c8b3c6a2c837.jpg) | ![7 bit LED - Neopixel](https://user-images.githubusercontent.com/20442880/137374620-dca6e2eb-40d9-4092-86c2-f67ab250e8d5.jpg)
Speaker Amp - PAM8302A | 7 bit LED - Neopixel (circular, NOT the ring)
![40mm 2W speaker](https://user-images.githubusercontent.com/20442880/137374750-e579754a-7173-41bb-a8a1-b5eaa5d234b1.jpg) | ![7mm Momentary Switch](https://user-images.githubusercontent.com/20442880/137374760-36ab96b2-bcd5-487a-a510-a3efed1216dc.jpg)
40mm 2W speaker | 7mm momentary switch
![Mini 360 DC-DC Buck Converter](https://user-images.githubusercontent.com/20442880/137374870-9687ea11-6a7c-48f6-8c0b-2ff4e34c62d3.jpg) | ![Lipo 7 4v](https://user-images.githubusercontent.com/20442880/137374882-cb61cde1-8c05-4817-9e06-7526e851bfad.jpg)
Mini 360 DC-DC Buck Converter | Lipo 7.4v
![1K Resister](https://user-images.githubusercontent.com/20442880/137374919-023f0bea-65f6-46a1-9d45-69f5a79e6916.jpg) | ![DRV2605L with 8mm ERM](https://user-images.githubusercontent.com/20442880/137374950-a2673b35-5a02-46b6-b322-de556f20e97e.jpg)
2 X 1K Resister | DRV2605L with 8mm ERM (Optional)

## Parts

## Required Libraries
There's are number of libraries that you will need to install using the Arduino Library Manager:
 1. U8g2lib
 2. FastLED
 3. VoiceRecognitionV3
 4. DFPlayerMini_Fast

## Installing U8g2lib and required modifications
The U8g2 library has a pretty large program foot print, so we need to make a number of changes to
minimize the size of the program. See https://github.com/olikraus/u8g2/wiki/u8g2optimization for details.
 1. Open <intall directory>/Arduino/libraries/u8g2/clib/u8g2.h
 2. Comment out following lines to reduce program size
    - #define U8G2_WITH_INTERSECTION
    - #define U8G2_WITH_CLIP_WINDOW_SUPPORT
    - #define U8G2_WITH_FONT_ROTATION

## Voice Recognition module
The voice recognition module is from Elechouse. This library has to be installed
manually to the Arduino library
 1. Open https://github.com/elechouse/VoiceRecognitionV3
 2. Download the zip or clone the repo
 3. Extract contents into the /Arduino Sketch/libraries directory

Once installed, follow the instructions to train the VR module. It can have up to 7
active voice commands.Train each command using the example sketches that come with the
library. Follow the instructions on the github page, but here's a few tips:
 1. Make sure to change the RX and TX pin based on how you connected it to the Arduino
 2. The commands should be trained based on the programs expected order - defined in config.h
 3. Once all of the commands are trained, update the module to autoload the commands on startup
    - This step is crucual to make the VR module work with this sketch.

To configure the autoload feature:
 1. Open the vr_sample_bridge sketch
 2. Set the correct RX/TX pins numbers to match your wiring
 3. Upload the sketch to the Arduino
 4. Open the Serial Monitor. Set baud rate 115200, set send with Newline or Both NL & CR.
 5. Enter command: 00
    - you should see output like: AA 08 00 00 00 00 00 00 00 0A
 6. Enter command: 15 7F 00 01 02 03 04 05 06
    - This tells the boards to autoload 7 records and which trained slots to load
 7. Enter command: 00
    - you should see output like: AA 08 00 00 00 00 00 7F 00 0A

If you can't get the autoload feature configured, you could uncomment code in the EasyVoice.begin() function
to manually load each record on startup. Recommend avoiding this if possible, it may slow the startup sequence
or have other unintended problems. That part is untested.

## Setup and Configuration
The code can be used by updating the values in config.h based on your components,
wiring, and audio tracks.

```c++
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

// Customizable ID badge for DNA Check sequence 
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
 * Timing Defintions for start up sequence timings
 */
#define STARTUP_LOGO_MS           2000  // 2 sec
#define STARTUP_COMM_OK_MS        3200  // 1.2 sec
#define STARTUP_DNA_CHK_MS        4200  // 1 sec
#define STARTUP_DNA_PRG_MS        5700  // 1.5 sec
#define STARTUP_ID_OK_MS          7500  // 1.8 sec
#define STARTUP_JUDGE_NAME_MS     9300  // 1.8 sec
#define STARTUP_END_MS            10000

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

static const uint8_t VOICE_RECORDS_ARR_SZ   = 7;
static const uint8_t VOICE_RECORDS_ARR[]    = {SELECTOR_AP_MODE, SELECTOR_IN_MODE, SELECTOR_HS_MODE, SELECTOR_HE_MODE, SELECTOR_ST_MODE, SELECTOR_FMJ_MODE, SELECTOR_RAPID_MODE};

```

## Audio File Setup
The DF Mini Player will play back both mp3 and wav files. However, the program play
back tracks based on index. The index is determined by the order the files are loaded
onto the card. It also expects all files to be loaded into a sub directory on the card
called "/mp3".

When using a MAC to load files, it will create hidden files that will cause the playback
to seem like it's not working. You'll need to use the terminal window to rm all of these
files and directories from the card.

## Example Wiring Diagram:
![SampleWiring](https://user-images.githubusercontent.com/20442880/137413974-aae3f170-1d8a-4850-9287-8ae751f7e130.png)

## Test Board Reference Design
![SE14r](https://user-images.githubusercontent.com/20442880/137375634-33618817-6704-49bb-8df2-a8f4f221b191.JPG)

