#ifndef dfplayermini_h
#define dfplayermini_h

#include <Arduino.h>

/**
 *  Namespace for constants
 */
namespace dfplayer {
/** Packet Values */
const uint8_t STACK_SIZE = 10;  // total number of bytes in a stack/packet (same for cmds and queries)
const uint8_t SB = 0x7E;        // start byte
const uint8_t VER = 0xFF;       // version
const uint8_t LEN = 0x6;        // number of bytes after "LEN" (except for checksum data and EB)
const uint8_t FEEDBACK = 1;     // feedback requested
const uint8_t NO_FEEDBACK = 0;  // no feedback requested
const uint8_t EB = 0xEF;        // end byte

/** Control Command Values */
const uint8_t VOLUME = 0x06;
const uint8_t USE_MP3_FOLDER = 0x12;
}


/**
 *  Class for interacting with DFPlayerMini MP3 player based on the DFPlayerMini_Fast.
 *
 *  This is the documentation for the YX5200-24SS MP3 player driver code for the
 *  Arduino platform.  It is designed specifically to work with the
 *  <a href="https://wiki.dfrobot.com/DFPlayer_Mini_SKU_DFR0299">DFPlayerMini breakout</a>
 *
 *  These MP3 players use UART to communicate, 2 pins (TX + RX) are required
 *  to interface with the breakout. An external power supply and output amp
 *  improves the MP3 player's functionality. Also, see <a href="https://wiki.dfrobot.com/DFPlayer_Mini_SKU_DFR0299">
 *  the wiki</a> for more detailed wiring instructions.
 */
class DFPlayerMini {
public:
  /**
   *  Configure the class.
   *    stream
   *      A reference to the Serial instance (hardware or software) used to communicate with the MP3 player.
   *    debug
   *      Boolean used to specify if debug prints should be automatically printed to the serial monitor.
   *    threshold
   *      Number of ms allowed for the MP3 player to respond (timeout) to a query.
   *  Returns True.
   */
  bool begin(Stream& stream, bool debug = false) {
    _serial = &stream;
    _debug = debug;

    sendStack.start_byte = dfplayer::SB;
    sendStack.version = dfplayer::VER;
    sendStack.length = dfplayer::LEN;
    sendStack.end_byte = dfplayer::EB;

    recStack.start_byte = dfplayer::SB;
    recStack.version = dfplayer::VER;
    recStack.length = dfplayer::LEN;
    recStack.end_byte = dfplayer::EB;

    return true;
  }

  /**
   *  Set the volume to a specific value out of 30.
   *  volume
   *    The volume level (0 - 30).
   */
  void volume(uint8_t volume) {
    if (volume <= 30) {
      sendStack.commandValue = dfplayer::VOLUME;
      sendStack.feedbackValue = dfplayer::NO_FEEDBACK;
      sendStack.paramMSB = 0;
      sendStack.paramLSB = volume;

      findChecksum(sendStack);
      sendData();
    }
  }

  /**
   *   Play a specific track in the folder named "MP3".
   *   trackNum
   *     The track number to play.
   */
  void playFromMP3Folder(uint16_t trackNum) {
    sendStack.commandValue = dfplayer::USE_MP3_FOLDER;
    sendStack.feedbackValue = dfplayer::NO_FEEDBACK;
    sendStack.paramMSB = (trackNum >> 8) & 0xFF;
    sendStack.paramLSB = trackNum & 0xFF;

    findChecksum(sendStack);
    sendData();
  }


private:
  /**
   * Struct to store entire serial datapacket used for MP3 config/control 
   */
  struct stack {
    uint8_t start_byte;
    uint8_t version;
    uint8_t length;
    uint8_t commandValue;
    uint8_t feedbackValue;
    uint8_t paramMSB;
    uint8_t paramLSB;
    uint8_t checksumMSB;
    uint8_t checksumLSB;
    uint8_t end_byte;
  } sendStack, recStack;


  Stream* _serial;
  bool _debug;


  /**
   *  Determine and insert the checksum of a given config/command packet into that same packet struct.
   *    _stack
   *      Reference to a struct containing the config/command packet
   *      to calculate the checksum over.
   */
  void findChecksum(stack& _stack) {
    int16_t checksum = 0 - (_stack.version + _stack.length + _stack.commandValue + _stack.feedbackValue + _stack.paramMSB + _stack.paramLSB);

    _stack.checksumMSB = checksum >> 8;
    _stack.checksumLSB = checksum & 0x00FF;
  }



  /**
   *  Send a config/command packet to the MP3 player.
   */
  void sendData() {
    _serial->write(sendStack.start_byte);
    _serial->write(sendStack.version);
    _serial->write(sendStack.length);
    _serial->write(sendStack.commandValue);
    _serial->write(sendStack.feedbackValue);
    _serial->write(sendStack.paramMSB);
    _serial->write(sendStack.paramLSB);
    _serial->write(sendStack.checksumMSB);
    _serial->write(sendStack.checksumLSB);
    _serial->write(sendStack.end_byte);

    if (_debug) {
      Serial.print("Sent ");
      printStack(sendStack);
      Serial.println();
    }
  }


  /**
   *  Print the entire contents of the specified config/command packet for debugging purposes.
   *    _stack
   *        Struct containing the config/command packet to print.
   */
  void printStack(stack _stack) {
    Serial.println(F("Stack:"));
    Serial.print(_stack.start_byte, HEX);
    Serial.print(' ');
    Serial.print(_stack.version, HEX);
    Serial.print(' ');
    Serial.print(_stack.length, HEX);
    Serial.print(' ');
    Serial.print(_stack.commandValue, HEX);
    Serial.print(' ');
    Serial.print(_stack.feedbackValue, HEX);
    Serial.print(' ');
    Serial.print(_stack.paramMSB, HEX);
    Serial.print(' ');
    Serial.print(_stack.paramLSB, HEX);
    Serial.print(' ');
    Serial.print(_stack.checksumMSB, HEX);
    Serial.print(' ');
    Serial.print(_stack.checksumLSB, HEX);
    Serial.print(' ');
    Serial.println(_stack.end_byte, HEX);
  }
};
#endif
