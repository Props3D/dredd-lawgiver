#ifndef easyvr_h
#define easyvr_h

#include "Arduino.h"
#include "wiring_private.h"
#include "SoftwareSerial.h"
#include <avr/pgmspace.h>

/**
 * Defines for packet frame
 */
#define FRAME_HEAD (0xAA)
#define FRAME_END (0x0A)
#define FRAME_CMD_VR (0x0D)  //Voice recognized
#define FRAME_CMD_ERROR (0xFF)

#define VR_DEFAULT_TIMEOUT (1000)

class EasyVR : public SoftwareSerial {
public:
  /**
  * VR class constructor.
  *   receivePin --> software serial RX
  *   transmitPin --> software serial TX
   */
  EasyVR(uint8_t receivePin, uint8_t transmitPin)
    : SoftwareSerial(receivePin, transmitPin) {
  }

  /**
   * Voice recognition routine. Detects when trained command is recognized.
   *  buf --> return data
   *    buf[0]  -->  Group mode(FF: None Group, 0x8n: User, 0x0n:System
   *    buf[1]  -->  number of record which is recognized. 
   *    buf[2]  -->  Recognizer index(position) value of the recognized record.
   *    buf[3]  -->  Signature length
   *    buf[4]~buf[n] --> Signature
   *    timeout --> wait time for receiving packet.
   *  Returns length of valid data in buf
   *    0 means no data received.
   */
  int recognize(uint8_t *buf, int timeout) {
    int ret, i;
    ret = receive_pkt(vr_buf, timeout);
    if (vr_buf[2] != FRAME_CMD_VR) {
      return -1;
    }
    if (ret > 0) {
      for (i = 0; i < (vr_buf[1] - 3); i++) {
        buf[i] = vr_buf[4 + i];
      }
      return i;
    }

    return 0;
  }


private:
  /** temp data buffer */
  uint8_t vr_buf[32];


  /**
   * receive a valid data packet in Voice Recognition module protocol format.
   *   buf --> return value buffer.
   *   timeout --> time of reveiving
   *  Returns integer
   *    '>0' --> success, packet lenght(length of all data in buf)
   *    '<0' --> failed
   */
  int receive_pkt(uint8_t *buf) {
    return receive_pkt(buf, VR_DEFAULT_TIMEOUT);
  }

  int receive_pkt(uint8_t *buf, uint16_t timeout) {
    int ret;
    ret = receive(buf, 2, timeout);
    if (ret != 2) {
      return -1;
    }
    if (buf[0] != FRAME_HEAD) {
      return -2;
    }
    if (buf[1] < 2) {
      return -3;
    }
    ret = receive(buf + 2, buf[1], timeout);
    if (buf[buf[1] + 1] != FRAME_END) {
      return -4;
    }

    return buf[1] + 2;
  }

  /**
   * Receive data from VR module
   *   buf --> return value buffer.
   *   len --> length expect to receive.
   *   timeout --> time to wait for data
   *  Returns number of received bytes
   *    0 means no data received.
   */
  int receive(uint8_t *buf, int len, uint16_t timeout) {
    int read_bytes = 0;
    int ret;
    unsigned long start_millis;

    while (read_bytes < len) {
      start_millis = millis();
      do {
        ret = read();
        if (ret >= 0) {
          break;
        }
      } while ((millis() - start_millis) < timeout);

      if (ret < 0) {
        return read_bytes;
      }
      buf[read_bytes] = (char)ret;
      read_bytes++;
    }

    return read_bytes;
  }
};

#endif
