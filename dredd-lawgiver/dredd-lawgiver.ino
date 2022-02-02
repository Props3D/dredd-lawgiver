/**
 * This is the main file for the Lawgiver blaster.
 * 
 * Hardware setup
 * ==============
 *  - arduino nano
 *  - DF mini player and amp
 *  - Neopixel 7 LED 
 *  - voice recognition
 *  - OLED 2.08" 
 *  - 7mm momentary switch
 *  - micro lever switch
 *  - sliding on/aff switch
 *  - 7.4v lipo battery and buck converter
 *  - 20mm speaker
 * 
 * All components settings can be found in "config.h". Before running, 
 * review and change all configurations based on your setup.
 * 
 * Required Libraries
 * ==================
 * There's are number of libraries that you will need to install using the 
 * Library Manager:
 * 1. U8g2lib
 * 2. FastLED
 * 3. VoiceRecognitionV3
 * 4. DFPlayerMini_Fast
 * 
 * Installing U8g2lib and required modifications
 * =============================================
 * The U8g2 library has a pretty large program foot print, so we need to make 
 * a number of changes to minimize the size of the program. 
 * See https://github.com/olikraus/u8g2/wiki/u8g2optimization for details.
 * 1. Open <intall directory>/Arduino/libraries/u8g2/clib/u8g2.h
 * 2. Comment out following lines to reduce program size
 *    a. #define U8G2_WITH_INTERSECTION
 *    b. #define U8G2_WITH_CLIP_WINDOW_SUPPORT
 *    c. #define U8G2_WITH_FONT_ROTATION
 * 
 * Voice Recognition module
 * ========================
 * The voice recognition module is from Elechouse. This library has to be installed 
 * manually to the Arduino library
 * 1. Open https://github.com/elechouse/VoiceRecognitionV3
 * 2. Download the zip or clone the repo
 * 3. Extract contents into the /Arduino Sketch/libraries directory
 * 
 * Once installed, follow the instructions to train the VR module. It can have up to 7
 * active voice commands.Train each command using the example sketches that come with the
 * library. Follow the instructions on the github page, but here's a few tips:
 * 1. Make sure to change the RX and TX pin based on how you connected it to the Arduino
 * 2. The commands should be trained based on the programs expected order - defined in config.h
 * 3. Once all of the commands are trained, update the module to autoload the commands on startup
 *    a. This step is crucual to make the VR module work with this sketch.
 * 
 * To configure the autoload feature:
 * 1. Open the vr_sample_bridge sketch
 * 2. Set the correct RX/TX pins numbers to match your wiring
 * 3. Upload the sketch to the Arduino
 * 4. Open the Serial Monitor. Set baud rate 115200, set send with Newline or Both NL & CR.
 * 5. Enter command: 00
 *    a. you should see output like: AA 08 00 00 00 00 00 00 00 0A
 * 6. Enter command: 15 7F 00 01 02 03 04 05 06
 *    a. This tells the boards to autoload 7 records and which trained slots to load
 * 7. Enter command: 00
 *    a. you should see output like: AA 08 00 00 00 00 00 7F 00 0A
 * 
 * If you can't get the autoload feature configured, you could uncomment code in the EasyVoice.begin() function
 * to manually load each record on startup. Recommend avoiding this if possible, it may slow the startup sequence
 * or have other unintended problems. That part is untested.
 * 
 * 
 */
 
