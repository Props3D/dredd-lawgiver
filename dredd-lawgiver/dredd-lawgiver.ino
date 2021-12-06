/**
 * This is the main file for the Lawgiver blaster.
 * 
 * Hardware setup:
 *  - 40mm speaker
 *  - arduino nano
 *  - DF mini player and amp
 *  - 7mm momentary switch
 *  - sliding on/aff switch
 *  - 7.4v lipo battery and buck converter
 *  - Neopixel LED 
 *  - voice recognition
 *  - OLED 2.08" 
 * 
 * All components settings can be found in "config.h". Before running, 
 * review and change all configurations based on your setup.
 * 
 * There's are number of libraries that you will need to install using the 
 * Library Manager:
 * 1. U8g2lib
 * 2. FastLED
 * 3. DFPlayerMini_Fast
 * 4. FireTimer
 * 5. ezButton
 * 
 * The main loop:
 * 1. Refresh or Update the OLED Display
 * 2. Check audio queue for playback
 * 3. Check if the haptic motors have been activated
 *   a. play the script
 * 4. Check if the leds have been activated
 *   a. update the leds following the pattern that has been set
 * 5. Check the fire trigger momentary switch.
 * 6. Send a blaster pulse, if fire trigger is a short press
 *   a. Toggles a clip counter
 *   b. Checks for an empty clip 
 *   c. If clip is not empty
 *      i. play audio tracks for selected fire mode
 *     ii. activate front led with blaster shot
 *         1. Flash white
 *         2. Burn Red to orange, then fade as it cools
 *   d. Otherwise play empty clip track
 * 6. Check the recognition module for voice commannds
 *     i. playback change mode track
 *    ii. toggle fire mode baed on the recognized command
 * 
 */
 
