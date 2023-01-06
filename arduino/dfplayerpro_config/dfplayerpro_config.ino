#include <SoftwareSerial.h>
#include "dfplayer_pro.h"

SoftwareSerial mySerial(4, 5);
DFPlayerPro _player;

void setup(void) {
  Serial.begin(115200);
  mySerial.begin(9600);  // factory default is 115200
  while (!_player.begin(mySerial)) {
    Serial.println("Init failed, please check the wire connection or baud rate!");
    delay(1000);
  }

  //Set baud rate to 9600(Need to power off and restart, power-down save)
  Serial.println("Config: 9600 Baud");
  _player.setBaudRate(115200);
  //Turn on indicator LED (Power-down save)
  Serial.println("Config: Disable LED");
  _player.setLED(false);
  //Turn on the prompt tone (Power-down save)
  Serial.println("Config: Disable Prompt");
  _player.setPrompt(false);
}

void loop() {
  Serial.println("Power Down");
  while (1) delay(2000);
}
