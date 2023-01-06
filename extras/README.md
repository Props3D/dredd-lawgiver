## Arduino Scripts 
There's a number of Arduino sketches to help you get up and running once you have your electronics ready to go and soldered up.

Directories:
 1. vr_module_cmd_training - Load this sketch to help train the VR module on the seven commands.
 2. vr_module_set_autoload - Load this sketch after the VR commands are trained to enable the autoload of those commands on startup. This is always required after running a training session.
 3. vr_module_set_baud - Load this sketch only if you want to modify the baud rate from the factory setting. This should not be needed as our code works from the factory setting. This sketch is for the DIYer that is experimenting.
 
### Training commands

The easiest option is load the vr_module_cmd_training sketch.
 1. Open the vr_module_cmd_training sketch
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
 
### Configuring Autoload feature

The easiest option is load the vr_module_set_autoload sketch.
 1. Open the vr_module_set_autoload sketch
 2. Set the correct RX/TX pins numbers to match your wiring
 3. Upload the sketch to the Arduino
 4. Open the Serial Monitor. Set baud rate 115200
 5. Wait until it says Power Down and unplug

### Configuring DF Player Pro module
This is experimental. We are testing new component for the audio module. The DF Player Pro (DF1201S) is a full featured mp3 player that includes a DAC, onboard 128M of memory, and onboard amp (PAM8302). This would replace three components used in the current build.

Out of the box, this module has a voice prompt that needs to be disabled. Run this script to disable the voice prompt.
 1. Open the dfplayerpro_config sketch
 2. Set the correct RX/TX pins numbers to match your wiring
 3. Upload the sketch to the Arduino
 4. Open the Serial Monitor. Set baud rate 115200
 5. Wait until it says Power Down and unplug
