#ifndef dfplayerpro_h
#define dfplayerpro_h

#include <Arduino.h>

static const char CMD_OK[] PROGMEM =              {"OK\r\n"};
static const char CMD_ERROR[] PROGMEM =           {"error"};

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
    writeATCommand(F("AT\r\n"));
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
    char data[10];
    memset(data, '\0', sizeof(char)*10);
    itoa(vol, data, 10);

    drain();
    writeBuffer(F("AT+VOL="));
    writeBuffer(data);
    writeBuffer(F("\r\n"));
    delay(30);

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
    writeATCommand(F("AT+FUNCTION=1\r\n"));
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
    writeATCommand(F("AT+PLAYMODE=3\r\n"));
    return readAck();
  }

  /**
   * Enable Amplifier chip
   * Returns Boolean type, the result of operation
   *   true The setting succeeded
   *   false Setting failed
   */
  bool enableAMP() {
    writeATCommand(F("AT+AMP=ON\r\n"));
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
    char data[10];
    memset(data, '\0', sizeof(char)*10);
    itoa(num, data, 10);

    drain();
    writeBuffer(F("AT+PLAYNUM="));
    writeBuffer(data);
    writeBuffer(F("\r\n"));
    delay(30);
    if (waitReply)
      return readAck();      
    return true;
  }

private:
  Stream* _s = NULL;

  void drain() {
    //DBGLN(F("Drain buffer"));
    while (_s->available()) {
      _s->read();
    }
  }

  void writeBuffer(const __FlashStringHelper* buffer) {
    DBGLN(buffer);
    _s->print(buffer);
  }

  void writeATCommand(const __FlashStringHelper* command) {
    DBGSTR(F("COMMAND: "));
    DBGLN(command);
    drain();
    _s->print(command);
    delay(30);
  }

  void writeBuffer(const char* buffer) {
    DBGLOG(buffer);
    _s->write(buffer);
  }


  bool readAck() {
    char buf[30];
    char* response = read(buf, 4);
    DBGSTR(F("RESPONSE: "));
    DBGLOG(response);

     if (strcmp_P(response, CMD_OK) == 0) {
      return true;
    }
    return false;
  }

  char* read(char* buffer, uint8_t len) {
    size_t offset = 0, left = len;
    memset(buffer, '\0', sizeof(char)*30);
    long curr = millis();
    if (len == 0) {
      while (1) {
        if (_s->available()) {
          buffer[offset] = (char)_s->read();
          offset++;
        }
        if ((buffer[offset - 1]) == '\n' && (buffer[offset - 2] == '\r')) break;
        if (millis() - curr > 1000) {
          return getString_P(buffer, CMD_ERROR, 6);
        }
      }
    } else {
      while (left) {
        if (_s->available()) {
          buffer[offset] = (char)_s->read();
          left--;
          offset++;
        }
        if (buffer[offset - 1] == '\n' && buffer[offset - 2] == '\r') break;
        if (millis() - curr > 1000) {
          return getString_P(buffer, CMD_ERROR, 6);
        }
      }
      buffer[len] = 0;
    }
    return buffer;
  }

  char* getString_P(char* txtbuf, const char* str, uint8_t len) {
    memset(txtbuf, '\0', sizeof(char)*30);
    strncpy_P(txtbuf, str, len);
    return txtbuf;
  }

};
#endif
