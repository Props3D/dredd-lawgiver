#ifndef easyvoice_h
#define easyvoice_h

#include "VoiceRecognitionV3.h"
#include "debug.h"


/**
 * The voice recognition module is from Elechouse. 
 * http://www.elechouse.com/elechouse/index.php?main_page=product_info&cPath=&products_id=2254
 * This library has to be installed manually to the Arduino library:
 *   1. Open https://github.com/elechouse/VoiceRecognitionV3
 *   2. Download the zip or clone the repo
 *   3. Extract contents into the /Arduino Sketch/libraries directory
 *
 * Once installed, follow the instructions to train the VR module. It can have up to 7
 * active voice commands.Train each command using the example sketches that come with the
 * library. Follow the instructions on the github page, but here's a few tips:
 *   1. Make sure to change the RX and TX pin based on how you connected it to the Arduino
 *   2. The commands should be trained based on the programs expected order - defined in config.h
 *   3. Once all of the commands are trained, update the module to autoload the commands on startup
 *     a. This step is crucual to make the VR module work with this sketch.
 *
 * To configure the autoload feature:
 *   1. Open the vr_sample_bridge sketch
 *   2. Set the correct RX/TX pins numbers to match your wiring
 *   3. Upload the sketch to the Arduino
 *   4. Open the Serial Monitor. Set baud rate 115200, set send with Newline or Both NL & CR.
 *   5. Enter command: 00
 *      a. you should see output like: AA 08 00 00 00 00 00 00 00 0A
 *   6. Enter command: 15 7F 00 01 02 03 04 05 06
 *      a. This tells the boards to autoload 7 records and which trained slots to load
 *   7. Enter command: 00
 *      a. you should see output like: AA 08 00 00 00 00 00 7F 00 0A
 *
 * If you can't get the autoload feature configured, you could uncomment code in the EasyVoice.begin() function
 * to manually load each record on startup. Recommend avoiding this if possible, it may slow the startup sequence
 * or have other unintended problems. That part is untested.
 *
 * USAGE
 * =====
 * Use the delcaration to specify the Voice module configuration at compile time:
 * eg. EasyVoice<VOICE_RECORDS_ARR, VOICE_RECORDS_ARR_SZ> voice;
 * 
 * In the setup, you can use the begin() function to initialize the brightness.
 * eg. voice.begin(VOICE_RX_PIN, VOICE_TX_PIN);
 * 
 * The main functionality is intended to be called in the main loop.
 *   int command = voice.readCommand(); // should be added to the main loop
 * 
 * REQUIRED LIBRARY: VoiceRecognitionV3
 * See https://github.com/elechouse/VoiceRecognitionV3
 */
template <const uint8_t *RECORDS, const uint8_t RECORD_CNT>
class EasyVoice
{
  private:
    VR _myVR;   // 6:RX 7:TX, you can choose your favourite pins.
    uint8_t *_records = RECORDS;
    uint8_t _recordCnt = RECORD_CNT;
    uint8_t _buf[64];

  public:
    EasyVoice(uint8_t rxPin, uint8_t txPin) : _myVR(rxPin, txPin) {
    };

    /** 
     *  Use this to initialize the module. Must be called in setup
     *  and before readComamand() is used.
     */
    void begin(void) {
#ifdef ENABLE_EASY_VOICE
      debugLog("setup voice");
      _myVR.begin(9600);

       // *** IMPORTANT ***
       // Only uncomment this code if you can't enable the autoload feature on the VR module.
       // This should be a last resort, the startup sequence is untested with this code.
       // i.e. this might affect the timing of the startup sequence
//      if (_myVR.clear() == 0) {
//        debugLog("Recognizer cleared.");
//      }
//      if (_myVR.load(_records, _recordCnt) >= 0) {
//        debugLog("Record Autoload");
//      }
#endif
    }
    
    /**
     * Check the senors buffer for recognized commands.
     * Returns the index of the recognized command.
     * Otherwise returns -1
     */
    int readCommand()
    {
#ifdef ENABLE_EASY_VOICE
        int ret = _myVR.recognize(_buf, 50);
        if (ret > 0) {
          return _buf[1];
        }
#endif
      return -1;
    }

};

#endif
