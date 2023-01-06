#ifndef easyvoice_h
#define easyvoice_h

#if ENABLE_EASY_VOICE == 1
#include "easyvr.h"
#endif 

#define VOICE_BAUD_RATE 9600

/**
 * This is the factory baud rate the module is shipped with.
 * This can be changed using the vr_module_set_baudrate sketch.
 */
#define VOICE_BAUD_RATE 9600

/**
 *  The voice recognition module is from Elechouse.
 *
 *  For training the vr commands he easiest option is load the vr_module_cmd_training sketch.
 *  Once the commands are trained, enable the autoload feature using the vr_module_set_autoload sketch.
 *    1. Open the vr_module_set_autoload sketch
 *    2. Set the correct RX/TX pins numbers to match your wiring
 *    3. Upload the sketch to the Arduino
 *    4. Open the Serial Monitor. Set baud rate 115200
 *    5. Wait until it says Power Down and unplug
 *
 *  USAGE
 *  =====
 *  Use the delcaration to specify the Voice module configuration at compile time:
 *  eg. EasyVoice<VOICE_RECORDS_ARR, VOICE_RECORDS_ARR_SZ> voice;
 *
 *  In the setup, you can use the begin() function to initialize the brightness.
 *  eg. voice.begin(VOICE_RX_PIN, VOICE_TX_PIN);
 *
 *  The main functionality is intended to be called in the main loop.
 *    int command = voice.readCommand(); // should be added to the main loop
 *
 *  REQUIRED LIBRARY: VoiceRecognitionV3
 *  See https://github.com/elechouse/VoiceRecognitionV3
 */
template <const uint8_t *RECORDS, const uint8_t RECORD_CNT>
class EasyVoice
{
  private:
#if ENABLE_EASY_VOICE == 1
    EasyVR _myVR;   // 6:RX 7:TX, you can choose your favourite pins.
#endif

    uint8_t *_records = RECORDS;
    uint8_t _recordCnt = RECORD_CNT;
    uint8_t _buf[64];

  public:
#if ENABLE_EASY_VOICE == 1
    EasyVoice(uint8_t rxPin, uint8_t txPin) : _myVR(rxPin, txPin) {
    };
#else    
    EasyVoice(uint8_t rxPin, uint8_t txPin) {}
#endif

    /**
     *    Use this to initialize the module. Must be called in setup
     *    and before readComamand() is used.
     */
    void begin(void) {
#if ENABLE_EASY_VOICE == 1
      // DBGLN(F("setup voice"));
      _myVR.begin(VOICE_BAUD_RATE);
#endif
    }

    /**
     *  Check the senors buffer for recognized commands.
     *  Returns the index of the recognized command.
     *  Otherwise returns -1
     */
    int readCommand()
    {
#if ENABLE_EASY_VOICE == 1
      int ret = _myVR.recognize(_buf, 50);
      if (ret > 0) {
        return _buf[1];
      }
#endif
      return -1;
    }

};

#endif
