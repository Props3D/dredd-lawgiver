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
![30mm 8ohm 2W](https://user-images.githubusercontent.com/20442880/137374750-e579754a-7173-41bb-a8a1-b5eaa5d234b1.jpg) | ![7mm Momentary Switch](https://user-images.githubusercontent.com/20442880/137374760-36ab96b2-bcd5-487a-a510-a3efed1216dc.jpg)
2 X 30mm 8ohm 2W speaker | 7mm momentary switch
![Hdffe15eda27b49ba97ad490bd8b2de15D](https://user-images.githubusercontent.com/20442880/182526580-1f583d36-dbea-4a45-8db2-4190c60dd473.jpg) | ![LG_Battery](https://user-images.githubusercontent.com/20442880/182526300-c6b9f371-2000-4e31-8856-fb4e421f89de.jpg)
Mini 560 DC-DC Buck Converter | Lipo 7.4v 800mAh
![Blue OLED 2.08" 256x64](https://user-images.githubusercontent.com/88125656/151882739-797e3aff-45a8-403a-9b00-b62ded0c7fb2.png) | ![Voice Recognition Module V3](https://user-images.githubusercontent.com/88125656/151881080-59063635-61f9-4151-8e54-19e14b0e2d9f.jpeg)
Blue OLED Display 2.08" 256x64 7Pin SPI | Voice Recognition Module V3 from Elechouse
![1K Resister](https://user-images.githubusercontent.com/20442880/137374919-023f0bea-65f6-46a1-9d45-69f5a79e6916.jpg) | ![3mm LED Diode](https://user-images.githubusercontent.com/88125656/151883361-de0aba59-4e1e-45c2-bd5d-a0d0c10af270.png)
1 X 1K Resister, 2 X 25ohm Resister | 3mm LED Diode (Red/Green) x 2 
## Assembly Parts

Hex Flat Head
* M3X12mm x 2 - Sight2
* M3X18mm X 2 Electronics Caddy
Hex Cap Head
* M3X4mm X 6 - Fire LED / Electronics Caddy Voice Reg
* M3X16mm X3 - Trigger Group, Reload
* M3X20mm	X1 -	Reload
Hex CM Flat Head
* M3X18mm	1	Battery Cap (can be flat head too)
Hex Button Head		
* M3X8mm	X2	- Trigger Group to Frame
* M4X12mm	X2	- Barrel
Any head bolt		
* M1.4X4	X4 -	Electronics Caddy Arduino

Other Parts
* 1/8in rod X 9mm X 1 - Trigger axle
* 1/8in rod x 54mm x 2 - Cocking Mechanism
* 5/16 X 1 1/8 Extension Spring - Cocking Mechanism
* 20mm Compression Spring - Magazine latch 


## Arduino Pinout
* D2 Voice Recognition RX
* D3 Voice Recognition TX
* D4 DF Player Mini TX
* D5 DF Player Mini RX
* D6 3mm LED GREEN
* D7 3mm LED RED
* D8 Trigger
* D9 Reload
* D10 OLED RST
* D11 OLED DC
* D12 OLED CS
* A4 OLED SDA
* A5 OLED SCL
* D13 FIRE LED

## Required Libraries
There's are number of libraries that you will need to install using the Arduino Library Manager:
 1. U8g2
 2. FastLED
 3. VoiceRecognitionV3
 4. DFPlayerMini_Fast

### Installing U8g2 and required modifications
The U8g2 library has a pretty large program foot print, so we need to make a number of changes to
minimize the size of the program. See https://github.com/olikraus/u8g2/wiki/u8g2optimization for details.
 1. Open <intall directory>/Arduino/libraries/u8g2/src/clib/u8g2.h
 2. Comment out following lines to reduce program size
    - #define U8G2_WITH_INTERSECTION
    - #define U8G2_WITH_CLIP_WINDOW_SUPPORT
    - #define U8G2_WITH_FONT_ROTATION

If you are using the same SH1122 OLED compoment, you'll want to try an minimize 
the noise of the OLED by reducing it's refresh rate and various voltages. Spec 
sheet include the supported commands: https://www.displayfuture.com/Display/datasheet/controller/SH1122.pdf
 1. Open <intall directory>/Arduino/libraries/u8g2/src/clib/u8x8_d_sh1122.c
 2. Find and replace the setting for clock divide ratio and fosc (0xD5)
    - U8X8_CA(0xd5, 0x50) should be U8X8_CA(0xd5, 0x31)
 3. Find and replace the setting for pre-charge (0xDC)
    - U8X8_CA(0xdc, 0x35) should be U8X8_CA(0xdc, 0x1a)

### Voice Recognition module
The voice recognition module is from Elechouse. Their library has to be installed
manually to the Arduino library, and can be found on Github.
 1. Open https://github.com/elechouse/VoiceRecognitionV3
 2. Download the zip or clone the repo
 3. Extract contents into the /Arduino Sketch/libraries directory

Once installed, follow the instructions to train the VR module. You can train up to 7
active voice commands. Train each command using the example sketche that comes with the
library. You can find detailed instructions in the manual or the
[VoiceRecognitionV3 github](https://github.com/elechouse/VoiceRecognitionV3) page, but here's
a few tips to make you successful:
 1. Make sure to change the RX and TX pin based on how you connected it to the Arduino
 2. The commands should be trained based on the programs expected order - defined in config.h
 3. Once all of the commands are trained, update the module to autoload the commands on startup
    - This step is crucual to make the VR module work with this sketch.

#### Training commands
The voice commands must be trained in the expected slots. This is defined in the config.h file
and should not be changed. For reference, see below:
```c++
/**
 *  Voice Recognition Commands.
 *  IMPORTANT - The VR module must be trained this sequence.
 */
#define VR_CMD_AMMO_MODE_AP         0 // speak "Armor Piercing" or just "Armor"
#define VR_CMD_AMMO_MODE_IN         1 // speak "Incendiary"
#define VR_CMD_AMMO_MODE_HS         2 // speak "Hotshot"
#define VR_CMD_AMMO_MODE_HE         3 // speak "High Ex"
#define VR_CMD_AMMO_MODE_ST         4 // speak "Stun"
#define VR_CMD_AMMO_MODE_FMJ        5 // speak "Full Metal"
#define VR_CMD_AMMO_MODE_RAPID      6 // speak "Rapid"
```

You can find instructions, with pictures, on the [VoiceRecognitionV3 Github](https://github.com/elechouse/VoiceRecognitionV3) page.
 1. Open vr_sample_train (File -> Examples -> VoiceRecognitionV3 -> vr_sample_train)
 2. Choose right Arduino board (Tool -> Board, UNO recommended), Choose right serial port.
 3. Click Upload button, wait until Arduino is uploaded.
 4. Open Serial Monitor. Set baud rate 115200, set send with Newline or Both NL & CR.
 5. Check the Voice Recognition Module settings. Input settings
    - Enter command: settings
    - Hit Enter to send.
 6. Train Voice Recognition Module.
    - To train record 0, enter command: train 0
    - When Serial Monitor prints "Speak now", you need speak your command
    - When Serial Monitor prints "Speak again", you need repeat your command again.
    - If these two voice commands are matched, Serial Monitor prints "Success", and "record 0" is trained
    - or if are not matched, repeat speaking until success.
 7. Repeat the process for each command i.e. train 1, train 2, train 3, etc
    - check out the config.h file for the order and phrases to follow
 8. Once all the commands are recorded, load and test the voice commands
    - To load all 7 voice commands, enter command: load 0 1 2 3 4 5 6
    - Speak the command to see if the Voice Recognition Module can recognize your words.
    - When recognized it will display the VR index
 9. If some of the commands don't work well, you can re-train until you're satisfied
 10. Training complete
 11. Follow the next section to enable autoloading of the trained recordings
 
#### Configuring Autoload feature
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
The component configurations can be easily customized by updating the values in config.h based on your components,
wiring, and audio tracks.

```c++
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

```


## Audio File Setup
The DF Mini Player will play back both mp3 and wav files. However, the program play
back tracks based on index. The index is determined by the order the files are loaded
onto the card. It also expects all files to be loaded into a sub directory on the card
called "/mp3".

When using a MAC to load files, it will create hidden files that will cause the playback
to seem like it's not working. You'll need to use the terminal window to rm all of these
files and directories from the card.

The audio file indices are listed in the config.h file. Do not remove any of the entries.
You can reuse sound effects for each slot with two options:
1. Copy the audio file in each slot where you want to use it
2. Change the index values below to match the files on the SD Card

I followed option 1 for the included audio files in this repo, it's just easier and there's
plenty of room on the micro SD card.
 
```c++
/**
 * Audio tracks by file index - upload these to the SD card in the correct order.
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
```

## Example Wiring Diagram:
![SampleWiring](https://user-images.githubusercontent.com/88125656/151897932-dd6698a9-7f91-4609-b71c-de937fd8352a.png)

## Test Board Reference Design
![Lawgiver]()
