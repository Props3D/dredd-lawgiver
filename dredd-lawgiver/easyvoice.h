#ifndef easyvoice_h
#define easyvoice_h

#include "VoiceRecognitionV3.h"
#include "debug.h"


/**
  Connection
  Arduino    VoiceRecognitionModule
   6   ------->     TX
   7   ------->     RX
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

    void begin(void) {
#ifdef ENABLE_EASY_VOICE
      debugLog("setup voice");
      _myVR.begin(9600);
/**
 * Only uncomment this code if you can't enable the autoload feature on the VR module.
 * Startup sequence is untested with this code.
 */
//      if (_myVR.clear() == 0) {
//        debugLog("Recognizer cleared.");
//      }
//      if (_myVR.load(_records, _recordCnt) >= 0) {
//        debugLog("Record Autoload");
//      }
#endif
    }
    

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
