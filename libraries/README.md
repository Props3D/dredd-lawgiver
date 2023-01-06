## Arduino Libraries
We used anumber of open source libraries to make the Dredd Lawgiver come to life. These are packaged here for convenience. YOu could also follow the instructions to install the latest versions, but these are the versions that were used at time of building this prop.

## Required Libraries
There's are number of libraries that you will need to install using the Arduino Library Manager:
 1. U8g2 - modified with the necessary changes
 2. FastLED
 3. FastTimer
 3. VoiceRecognitionV3
 4. DFPlayerMini_Fast
 5. ezButton

All of these are included in the libraries directory. Copy each one into your <intall directory>/Arduino/libraries directory, make backups of any you have installed already in case you need to put them back for other projects.


### Installing U8g2 and required modifications
If you want to install and use the latest version of the U8G2 library, then follow these instructions.

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
If you want to install and use the latest version of the VoiceRecognitionV3 library, then follow these instructions.

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
There are two options to train the voice recogition commands.

The easiest option is load the vr_module_cmd_training sketch.
 1. Open the extras/vr_module_cmd_training sketch
 2. Set the correct RX/TX pins numbers to match your wiring.
 3. Choose right Arduino board (Tool -> Board, Arduino Nano recommended), Choose right serial port.
 4. Click Upload button, wait until Arduino is uploaded.
 5. Open Serial Monitor. Set baud rate 115200, set send with Newline or Both NL & CR.
 6. Read and follow the instructions
 7. The script will prompt you to record each command / phrase - one at a time
 8. When ready, press enter to begin recording
    - When Serial Monitor prints "Speak now", you need speak the command
    - When Serial Monitor prints "Speak again", you need repeat the command again.
    - If these two voice commands are matched, Serial Monitor prints "Success", and "record X" is trained
    - or if are not matched, repeat the process until success.
 9. Training will be completed when you record all 7 commands successfully
 10. At this point, you can test the training by speaking the commands
 11. Follow the next section to enable autoloading of the trained recordings
 

The second option is to use the example sketch that is packaged with VoiceRecognitionV3 library.
The voice commands must be trained in the expected slots. The order is defined in the config.h file
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
 2. Choose right Arduino board (Tool -> Board, Arduino Nano recommended), Choose right serial port.
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
There are two options to enable the autoloading trained commands on startup.

The easiest option is load the vr_module_set_autoload sketch.
 1. Open the extras/vr_module_set_autoload sketch
 2. Set the correct RX/TX pins numbers to match your wiring
 3. Upload the sketch to the Arduino
 4. Open the Serial Monitor. Set baud rate 115200
 5. Wait until it says Power Down and unplug


The second option is to use the example sketch that is packaged with VoiceRecognitionV3 library.
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
