#ifndef DFROBOT_DF1201S_H
#define DFROBOT_DF1201S_H

#include <Arduino.h>
#include <string.h>

/**
 * Define the basic structure of class DF Player Pro DF1201S, the implementation of basic methods.
 * This module is a conversion board, which can drive DF1201S DFPlayer PRO MP3 through I2C
 */
class DFPlayerPro {
public:
  typedef enum {
    MUSIC = 1, /**<Music Mode */
    UFDISK,    /**<Slave mode */
  } eFunction_t;

  typedef struct {
    String str;
    uint8_t length;
  } sPacket_t;

  typedef enum {
    SINGLECYCLE = 1, /**<Repeat one song */
    ALLCYCLE,        /**<Repeat all */
    SINGLE,          /**<Play one song only */
    RANDOM,          /**<Random*/
    FOLDER,          /**<Repeat all songs in folder */
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
   * @fn setBaudRate
   * @brief Set baud rate(Need to power off and restart, power-down save)
   * @param baud 9600,19200,38400,57600,115200
   * @return Boolean type, the result of seted
   * @retval true The setting succeeded
   * @retval false Setting failed
   */
  bool setBaudRate(uint32_t baud) {
    sPacket_t cmd;
    cmd = pack("BAUDRATE", String(baud));
    writeATCommand(cmd.str, cmd.length);
    return readAck();
  }

  /**
   * @fn setLED
   * @brief Set indicator LED(Power-down save) 
   * @param on true or false
   * @return Boolean type, the result of seted
   * @retval true The setting succeeded
   * @retval false Setting failed
   */
  bool setLED(bool on) {

    sPacket_t cmd;
    String mode;
    if (on == true)
      mode = "ON";
    else
      mode = "OFF";
    cmd = pack("LED", mode);
    writeATCommand(cmd.str, cmd.length);
    return readAck();
  }

  /**
   * @fn setPrompt
   * @brief Set the prompt tone(Power-down save) 
   * @param on true or false
   * @return Boolean type, the result of seted
   * @retval true The setting succeeded
   * @retval false Setting failed
   */
  bool setPrompt(bool on) {
    sPacket_t cmd;
    String mode;
    if (on == true)
      mode = "ON";
    else
      mode = "OFF";
    cmd = pack("PROMPT", mode);
    writeATCommand(cmd.str, cmd.length);
    return readAck();
  }

private:
  Stream *_s = NULL;
  String atCmd;

  sPacket_t pack() {
    return pack(" ", " ");
  }

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
