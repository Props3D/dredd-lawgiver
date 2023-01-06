/**
  ******************************************************************************
  * @file    vr_module_config.ino
  * @author  props3d_craig
  * @brief   This file provides a simple way to preconfig your new VR module
  ******************************************************************************
  */
#include "easyvr.h"
// Pin configuration for voice recognition module
#define VOICE_RX_PIN  2
#define VOICE_TX_PIN  3
#define VOICE_BAUD    9600

/**
 * These are used to enable the autoload feature. This requires the 7 commands to be trained.
 */
static const uint8_t RECORD_CNT   = 7;
static const uint8_t RECORDS[]    = {0, 1, 2, 3, 4, 5, 6};

EasyVR _vr(VOICE_RX_PIN, VOICE_TX_PIN);  // 2:RX 3:TX, you can choose your favourite pins.

void setup(void) {
  _vr.begin(VOICE_BAUD);

  /** initialize */
  Serial.begin(115200);
  Serial.println(F("Props3D Preconfiguration for Elechouse Voice Recognition V3 Module."));

  //_vr.setBaud(VOICE_BAUD);
  if (_vr.setAutoLoad(RECORDS, RECORD_CNT) >= 0) {
    Serial.println(F("Autoload records successful"));
  }
}

void loop() {
  Serial.println("Power Down");
  while (1) delay(2000);
}