#ifndef dfplayerpro_h
#define dfplayerpro_h

#include <Arduino.h>
#include <string.h>

/**
 * Define the basic structure of class DF Player Pro DF1201S, the implementation of basic methods.
 * This module is a conversion board, which can drive DF1201S DFPlayer PRO MP3 through I2C
 */
class DFPlayerPro {
public:
  typedef enum {
    MUSIC = 1,  // Music Mode
    UFDISK,     // Slave mode
  } eFunction_t;

  typedef struct {
    String str;
    uint8_t length;
  } sPacket_t;

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
  bool begin(Stream &s) {
    sPacket_t cmd;
    _s = &s;
    cmd = pack();

    writeATCommand(cmd.str, cmd.length);
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
    sPacket_t cmd;
    cmd = pack("VOL", String(vol));
    writeATCommand(cmd.str, cmd.length);
    return readAck();
  }

  /**
   * Set working mode 
   *   function eFunction_t:MUSIC,RECORD,UFDISK
   * Returns Boolean type, the result of seted
   *   true The setting succeeded
   *   false Setting failed
   */
  bool switchFunction(eFunction_t function) {
    sPacket_t cmd;
    cmd = pack("FUNCTION", String(function));
    curFunction = function;
    writeATCommand(cmd.str, cmd.length);
    if (readAck()) {
      delay(1500);
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
  bool setPlayMode(ePlayMode_t mode) {
    if (curFunction != MUSIC) return false;
    sPacket_t cmd;
    cmd = pack("PLAYMODE", String(mode));
    writeATCommand(cmd.str, cmd.length);
    return readAck();
  }

  /**
   * Enable Amplifier chip
   * Returns Boolean type, the result of operation
   *   true The setting succeeded
   *   false Setting failed
   */
  bool enableAMP() {
    if (curFunction != MUSIC) return false;
    sPacket_t cmd;
    cmd = pack("AMP", "ON");
    writeATCommand(cmd.str, cmd.length);
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
    if (curFunction != MUSIC) return false;
    sPacket_t cmd;
    cmd = pack("PLAYNUM", String(num));
    writeATCommand(cmd.str, cmd.length);
    return readAck();
  }

private:
  Stream *_s = NULL;
  String atCmd;
  eFunction_t curFunction;

  sPacket_t pack() {
    return pack(" ", " ");
  }

  /**
   * Create packet 
   */
  sPacket_t pack(String cmd, String para) {
    sPacket_t pack;
    atCmd = "";
    atCmd += "AT";
    if (cmd != " ") {
      atCmd += "+";
      atCmd += cmd;
    }

    if (para != " ") {
      atCmd += "=";
      atCmd += para;
    }
    atCmd += "\r\n";
    pack.str = atCmd;
    pack.length = atCmd.length();
    return pack;
  }

  void writeATCommand(String command, uint8_t length) {
    uint8_t data[40];
    while (_s->available()) {
      _s->read();
    }
    for (uint8_t i = 0; i < length; i++)
      data[i] = command[i];
    _s->write(data, length);
  }

  bool readAck() {
    if (read(4) == "OK\r\n") {
      return true;
    }
    return false;
  }

  String read(uint8_t len) {
    String str = "";
    size_t offset = 0, left = len;
    long long curr = millis();
    if (len == 0) {
      while (1) {
        if (_s->available()) {
          str += (char)_s->read();
          offset++;
        }
        if ((str[offset - 1]) == '\n' && (str[offset - 2] == '\r')) break;
        if (millis() - curr > 1000) {
          return F("error");
          break;
        }
      }
    } else {
      while (left) {
        if (_s->available()) {
          str += (char)_s->read();
          left--;
          offset++;
        }
        if (str[offset - 1] == '\n' && str[offset - 2] == '\r') break;
        if (millis() - curr > 1000) {
          return F("error");
          break;
        }
      }
      str[len] = 0;
    }
    return str;
  }
};
#endif
