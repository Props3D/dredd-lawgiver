#ifndef easyvoice_h
#define easyvoice_h

//#include <Arduino.h>
#include "VoiceRecognitionV3.h"


/**
  Connection
  Arduino    VoiceRecognitionModule
   3   ------->     TX
   4   ------->     RX
*/
template <const uint8_t *RECORDS, const uint8_t RECORD_CNT>
class EasyVoice
{
  private:
    VR _myVR;   // 3:RX 4:TX, you can choose your favourite pins.
    uint8_t *_records = RECORDS;
    uint8_t _recordCnt = RECORD_CNT;
    uint8_t _buf[64];

  public:
    EasyVoice(uint8_t rxPin, uint8_t txPin) : _myVR(rxPin, txPin) {
    };

    void begin(void) {
      debugLog("setup voice");
      _myVR.begin(9600);

#ifndef ENABLE_EASY_VOICE_AUTOLOAD
      if (_myVR.clear() == 0) {
        debugLog("Recognizer cleared.");
      }
      if (_myVR.setAutoLoad(_records, _recordCnt) >= 0) {
        debugLog("Record Autoload");
      }
#endif
    }

    int readCommand()
    {
      int ret = _myVR.recognize(_buf, 100);
      if (ret > 0) {
        return _buf[1];
      }
      return -1;
    }

};

#endif
