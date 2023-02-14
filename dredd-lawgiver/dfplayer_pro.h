#ifndef dfplayerpro_h
#define dfplayerpro_h

#include <Arduino.h>

namespace pro_dfplayer {
static const char CMD_AT[] PROGMEM = "AT\r\n";
static const char CMD_FUNCTION_MUSIC[] PROGMEM = "AT+FUNCTION=1\r\n";
static const char CMD_PLAYMODE_SINGLE[] PROGMEM = "AT+PLAYMODE=3\r\n";
static const char CMD_AMP_ON[] PROGMEM = "AT+AMP=ON\r\n";
static const char CMD_PLAY_NUM[] PROGMEM = "AT+PLAYNUM=";
static const char CMD_SET_VOLUME[] PROGMEM = "AT+VOL=";
static const char CMD_END[] PROGMEM = "\r\n";
static const char CMD_OK[] PROGMEM = "OK\r\n";
static const char CMD_ERROR[] PROGMEM = "error";
}

/**
 * Define the basic structure of class DF Player Pro DF1201S, the implementation of basic methods.
 * This module is a conversion board, which can drive DF1201S DFPlayer PRO MP3 through I2C
 */
class DFPlayerPro {
public:
  /**
   * Class constructor
   */
  DFPlayerPro() {}

  /**
   * init function
   *   s serial
   * Returns Boolean type, Indicates the initialization result
   *   true The setting succeeded
   *   false Setting failed
   */
  bool begin(Stream& s) {
    _s = &s;
    writeATCommand(getString(pro_dfplayer::CMD_AT));
    return readAck();
  }


  /**
   * Set volume 
   *   vol 0-30
   * Returns Boolean type, the result of seted
   *   true The setting succeeded
   *   false Setting failed
  */
  bool setVolume(uint8_t vol) {
    char* command = getString(pro_dfplayer::CMD_SET_VOLUME);
    uint8_t data[10];

    itoa(vol, data, 10);
    strncat(command, data, 39 - strlen(command));
    strncat_P(command, pro_dfplayer::CMD_END, 39 - strlen(command));
    writeATCommand(command);
    return readAck();
  }

  /**
   * Set working mode 
   *   function MUSIC=1,RECORD=2,UFDISK=3
   * Returns Boolean type, the result of seted
   *   true The setting succeeded
   *   false Setting failed
   */
  bool musicMode() {
    writeATCommand(getString(pro_dfplayer::CMD_FUNCTION_MUSIC));
    if (readAck()) {
      delay(2000);
      return true;
    }
    return false;
  }

  /**
   * Set playback mode 
   *   mode SINGLECYCLE=1,ALLCYCLE=2,SINGLE=3,RANDOM=4,FOLDER=5
   * Returns Boolean type, the result of seted
   *   true The setting succeeded
   *   false Setting failed
   */
  bool singlePlayMode() {
    writeATCommand(getString(pro_dfplayer::CMD_PLAYMODE_SINGLE));
    return readAck();
  }

  /**
   * Enable Amplifier chip
   * Returns Boolean type, the result of operation
   *   true The setting succeeded
   *   false Setting failed
   */
  bool enableAMP() {
    writeATCommand(getString(pro_dfplayer::CMD_AMP_ON));
    return readAck();
  }

  /**
   * Play the file of specific number, the numbers are arranged according to the
   * sequence the files are copied onto the U-disk.
   *   num file number, can be obtained by getCurFileNumber()
   * Returns Boolean type, the result of operation
   *   true The setting succeeded
   *   false Setting failed
   */
  bool playFileNum(int16_t num, bool waitReply=false) {
    char* command = getString(pro_dfplayer::CMD_PLAY_NUM);

    char data[10];
    memset(data, 0, sizeof(char)*10);
    itoa(num, data, 10);
    // strncat(command, data, 39 - strlen(command));
    // strncat_P(command, pro_dfplayer::CMD_END, 39 - strlen(command));
    strcat(command, data);
    strcat_P(command, pro_dfplayer::CMD_END);
    writeATCommand(command);
    if (waitReply)
      return readAck();      
    return true;
  }

private:
  Stream* _s = NULL;
  char _output[40];

  void writeATCommand(char* command) {
    DBGSTR(F("COMMAND: "));
    DBGLLOG(command);
    uint8_t data[40];
    while (_s->available()) {
      _s->read();
    }
    uint8_t length = strlen(command);
    for (uint8_t i = 0; i < length; i++)
      data[i] = command[i];
    _s->write(data, length);
    delay(30);
  }

  bool readAck() {
    char* response = read(4);
    DBGSTR(F("RESPONSE: "));
    DBGLLOG(response);

    if (strcmp_P(response, pro_dfplayer::CMD_OK) == 0) {
      return true;
    }
    return false;
  }

  char* read(uint8_t len) {
    size_t offset = 0, left = len;
    memset(_output, 0, sizeof(char)*40);
    long curr = millis();
    if (len == 0) {
      while (1) {
        if (_s->available()) {
          _output[offset] = (char)_s->read();
          offset++;
        }
        if ((_output[offset - 1]) == '\n' && (_output[offset - 2] == '\r')) break;
        if (millis() - curr > 1000) {
          return getString(pro_dfplayer::CMD_ERROR);
        }
      }
    } else {
      while (left) {
        if (_s->available()) {
          _output[offset] = (char)_s->read();
          left--;
          offset++;
        }
        if (_output[offset - 1] == '\n' && _output[offset - 2] == '\r') break;
        if (millis() - curr > 1000) {
          return getString(pro_dfplayer::CMD_ERROR);
        }
      }
      _output[len] = 0;
    }
    return _output;
  }

  char* getString(const char* str) {
    memset(_output, 0, sizeof(char)*40);
    strcpy_P(_output, (char*)str);
    return _output;
  }
};
#endif
