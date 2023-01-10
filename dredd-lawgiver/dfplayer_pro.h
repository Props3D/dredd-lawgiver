#ifndef dfplayerpro_h
#define dfplayerpro_h

#include <Arduino.h>

const static char CMD_AT[] PROGMEM = "AT\r\n";
const static char CMD_FUNCTION_MUSIC[] PROGMEM = "AT+FUNCTION=1\r\n";
const static char CMD_PLAYMODE_SINGLE[] PROGMEM = "AT+PLAYMODE=3\r\n";
const static char CMD_AMP_ON[] PROGMEM = "AT+AMP=ON\r\n";
const static char CMD_PLAY_NUM[] PROGMEM = "AT+PLAYNUM=";
const static char CMD_SET_VOLUME[] PROGMEM = "AT+VOL=";
const static char CMD_END[] PROGMEM = "\r\n";
const static char CMD_OK[] PROGMEM = "OK\r\n";
const static char CMD_ERROR[] PROGMEM = "error";

/**
 * Define the basic structure of class DF Player Pro DF1201S, the implementation of basic methods.
 * This module is a conversion board, which can drive DF1201S DFPlayer PRO MP3 through I2C
 */
class DFPlayerPro {
public:
  typedef enum {
    SINGLECYCLE = 1,  // Repeat one song
    ALLCYCLE,         // Repeat all
    SINGLE,           // Play one song only
    RANDOM,           // Random
    FOLDER,           // Repeat all songs in folder
    ERROR,
  } ePlayMode_t;

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
    writeATCommand(getString(CMD_AT));
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
    char* command = getString(CMD_SET_VOLUME);
    uint8_t data[10];

    itoa(vol, data, 10);
    strncat(command, data, 39 - strlen(command));
    strncat_P(command, CMD_END, 39 - strlen(command));
    writeATCommand(command);
    return readAck();
  }

  /**
   * Set working mode 
   *   function eFunction_t:MUSIC,RECORD,UFDISK
   * Returns Boolean type, the result of seted
   *   true The setting succeeded
   *   false Setting failed
   */
  bool musicMode() {
    writeATCommand(getString(CMD_FUNCTION_MUSIC));
    if (readAck()) {
      delay(2000);
      return true;
    }
    return false;
  }

  /**
   * Set playback mode 
   *   mode ePlayMode_t:SINGLECYCLE,ALLCYCLE,SINGLE,RANDOM,FOLDER
   * Returns Boolean type, the result of seted
   *   true The setting succeeded
   *   false Setting failed
   */
  bool singlePlayMode() {
    writeATCommand(getString(CMD_PLAYMODE_SINGLE));
    return readAck();
  }

  /**
   * Enable Amplifier chip
   * Returns Boolean type, the result of operation
   *   true The setting succeeded
   *   false Setting failed
   */
  bool enableAMP() {
    writeATCommand(getString(CMD_AMP_ON));
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
  bool playFileNum(int16_t num) {
    char* command = getString(CMD_PLAY_NUM);

    uint8_t data[10];
    itoa(num, data, 10);
    strncat(command, data, 39 - strlen(command));
    strncat_P(command, CMD_END, 39 - strlen(command));
    writeATCommand(command);
    return true; // do not wait for confirmation
    //return readAck();
  }

private:
  Stream* _s = NULL;
  char _output[40];

  void writeATCommand(char* command) {
    DBGSTR(F("COMMAND: "));
    DBGLN(command);
    uint8_t data[40];
    while (_s->available()) {
      _s->read();
    }
    uint8_t length = strlen(command);
    for (uint8_t i = 0; i < length; i++)
      data[i] = command[i];
    _s->write(data, length);
  }

  bool readAck() {
    char* response = read(4);
    DBGSTR(F("RESONSE: "));
    DBGLN(response);

    if (strcmp_P(response, CMD_OK) == 0) {
      return true;
    }
    return false;
  }

  char* read(uint8_t len) {
    size_t offset = 0, left = len;
    long long curr = millis();
    if (len == 0) {
      while (1) {
        if (_s->available()) {
          _output[offset] = (char)_s->read();
          offset++;
        }
        if ((_output[offset - 1]) == '\n' && (_output[offset - 2] == '\r')) break;
        if (millis() - curr > 1000) {
          return getString(CMD_ERROR);
          break;
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
          return getString(CMD_ERROR);
          break;
        }
      }
      _output[len] = 0;
    }
    return _output;
  }

  char* getString(const char* str) {
    memset(_output, 0, sizeof(_output));
    strncpy_P(_output, (char*)str, 39);
    return _output;
  }
};
#endif
