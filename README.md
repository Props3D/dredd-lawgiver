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
 1. U8g2Lib
 2. DFPlayerMini_Fast
 3. FastLED
 4. FireTimer
 5. ezButton

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

