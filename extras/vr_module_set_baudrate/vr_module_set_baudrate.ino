/**
  ******************************************************************************
  * @file    vr_module_set_baudrate.ino
  * @author  props3d_craig
  * @brief   This file provides a simple way to preconfig your new VR module
  ******************************************************************************
  */
#include "easyvr.h"
// Pin configuration for voice recognition module
#define VOICE_RX_PIN  2
#define VOICE_TX_PIN  3
#define VOICE_BAUD    9600 // Factory setting is 9600

EasyVR _vr(VOICE_RX_PIN, VOICE_TX_PIN);  // 2:RX 3:TX is the usual default wiring here

void setup(void) {
  _vr.begin(VOICE_BAUD);

  /** initialize */
  Serial.begin(115200);
  Serial.println(F("Props3D Preconfiguration for Elechouse Voice Recognition V3 Module."));

  // set a new baud rate
  if (_vr.setBaud(38400) == 0) {
    Serial.println(F("Baud rate changed successfully"));
  } else {
    Serial.println(F("Baud rate update failed"));
  }
}

void loop() {
  Serial.println("Power Down");
  while (1) delay(2000);
}