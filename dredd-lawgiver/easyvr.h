#ifndef easyvr_h
#define easyvr_h

#include "Arduino.h"
#include "wiring_private.h"
#include "SoftwareSerial.h"
#include <avr/pgmspace.h>

#define VR_DEFAULT_TIMEOUT (1000)

/***************************************************************************/
#define FRAME_HEAD (0xAA)
#define FRAME_END (0x0A)

/***************************************************************************/
#define FRAME_CMD_CHECK_SYSTEM (0x00)
#define FRAME_CMD_CHECK_BSR (0x01)
#define FRAME_CMD_CHECK_TRAIN (0x02)
#define FRAME_CMD_CHECK_SIG (0x03)

#define FRAME_CMD_RESET_DEFAULT (0x10)  //reset configuration
#define FRAME_CMD_SET_BR (0x11)         //baud rate
#define FRAME_CMD_SET_IOM (0x12)        //IO mode
#define FRAME_CMD_SET_PW (0x13)         //pulse width
#define FRAME_CMD_RESET_IO (0x14)       // reset IO OUTPUT
#define FRAME_CMD_SET_AL (0x15)         // Auto load

#define FRAME_CMD_TRAIN (0x20)
#define FRAME_CMD_SIG_TRAIN (0x21)
#define FRAME_CMD_SET_SIG (0x22)

#define FRAME_CMD_LOAD (0x30)         //Load N records
#define FRAME_CMD_CLEAR (0x31)        //Clear BSR buffer
#define FRAME_CMD_GROUP (0x32)        //
#define FRAME_CMD_GROUP_SET (0x00)    //
#define FRAME_CMD_GROUP_SUGRP (0x01)  //
#define FRAME_CMD_GROUP_LSGRP (0x02)  //
#define FRAME_CMD_GROUP_LUGRP (0x03)  //
#define FRAME_CMD_GROUP_CUGRP (0x04)  //

#define FRAME_CMD_TEST (0xEE)
#define FRAME_CMD_TEST_READ (0x01)
#define FRAME_CMD_TEST_WRITE (0x00)


#define FRAME_CMD_VR (0x0D)  //Voice recognized
#define FRAME_CMD_PROMPT (0x0A)
#define FRAME_CMD_ERROR (0xFF)


class EasyVR : public SoftwareSerial {
public:
  typedef enum {
    PULSE = 0,
    TOGGLE = 1,
    SET = 2,
    CLEAR = 3
  } io_mode_t;

  typedef enum {
    LEVEL0 = 0,
    LEVEL1,
    LEVEL2,
    LEVEL3,
    LEVEL4,
    LEVEL5,
    LEVEL6,
    LEVEL7,
    LEVEL8,
    LEVEL9,
    LEVEL10,
    LEVEL11,
    LEVEL12,
    LEVEL13,
    LEVEL14,
    LEVEL15,
  } pulse_width_level_t;

  typedef enum {
    GROUP0 = 0,
    GROUP1,
    GROUP2,
    GROUP3,
    GROUP4,
    GROUP5,
    GROUP6,
    GROUP7,
    GROUP_ALL = 0xFF,
  } group_t;


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
