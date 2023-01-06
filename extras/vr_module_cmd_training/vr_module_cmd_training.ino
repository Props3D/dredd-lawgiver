/**
  ******************************************************************************
  * @file    vr_module_config.ino
  * @author  props3d_craig
  * @brief   This file provides a simple way to preconfig your new VR module
  ******************************************************************************
  */
#include "easyvr.h"

// Pin configuration for voice recognition module
#define VOICE_RX_PIN 2
#define VOICE_TX_PIN 3
#define VOICE_BAUD 9600

/**
 * These are used to enable the autoload feature. This requires the 7 commands to be trained.
 */
static const uint8_t CMD_NUM = 7;
static const uint8_t CMD_RECORDS[] = { 0, 1, 2, 3, 4, 5, 6 };

const char cmdList[CMD_NUM][15] = {  // command list table
  { "Armor Piercing" },
  { "Incendiary" },
  { "Hotshot" },
  { "High Ex" },
  { "Stun" },
  { "Full Metal" },
  { "Rapid" }
};

EasyVR _vr(VOICE_RX_PIN, VOICE_TX_PIN);  // 2:RX 3:TX, you can choose your favourite pins.
uint8_t cmd_cnt;

int  cmdTrain(int cmdIdx);
bool setAutoload(void);
void trainCommands(void);
void testCommands(void);
/***************************************************************************/
/** declare print functions */
void printSeperator();
void printStart(void);
void printPrompt(int cmdIdx);
void printVR(uint8_t *buf);
int  printTrain(uint8_t *buf, uint8_t len);
int  waitPrompt(void);

void setup(void) {
  _vr.begin(VOICE_BAUD);

  /** initialize */
  Serial.begin(115200);
  printStart();
  cmd_cnt = 0;
}

void loop() {
  if (cmd_cnt < CMD_NUM)
    trainCommands();
  else
    testCommands();
}

/**
 * Prompt and train one command at a time, in sequence.
 */
void trainCommands() {
  if (cmd_cnt == CMD_NUM) return;
  printPrompt(cmd_cnt);
  if (cmdTrain(cmd_cnt)) {
    cmd_cnt++;
    if (cmd_cnt == CMD_NUM) {
      setAutoload();
      Serial.println(F("Training completed. Try speaking the commands to test the recognition."));
    }
  }
}

uint8_t buf[255];
/**
 * Try to recognize spoken commands.
 */
void testCommands(void) {
  if (cmd_cnt < CMD_NUM) return;
  int ret;
  ret = _vr.recognize(buf, 50);
  if (ret > 0) {
    /** voice recognized, print result */
    printVR(buf);
  }
}

/**
 * Print the prompt for a specific command.
 *   cmdIdx -> inddex of command
 */
void printPrompt(int cmdIdx) {
  printSeperator();
  Serial.print(F("Training for command: \""));
  Serial.print(cmdList[cmdIdx]);
  Serial.println(F("\""));
  Serial.println(F("Press enter to start: "));
  waitPrompt();
}

/**
 * Wait until user press enter.
 */
int waitPrompt(void) {
  int ret;
  unsigned long start_millis;
  start_millis = millis();
  while (1) {
    ret = Serial.read();
    if (ret > 0) {
      start_millis = millis();
      if (ret == '\n') {
        return 1;
      }
    }
  }
  return 0;
}

/**
 * Handle "train" command
 *   cmdIdx     --> command index
 */
int cmdTrain(int cmdIdx) {
  uint8_t buf[255];
  printSeperator();
  int ret = _vr.train(cmdIdx, buf);
  if (ret >= 0) {
    return printTrain(buf, ret);
  } else if (ret == -1) {
    Serial.println(F("Training failed."));
  } else if (ret == -2) {
    Serial.println(F("Training Timeout."));
  }
  return 0;
}

/**
 * Enable autoload for all seven records.
 */
bool setAutoload() {
  if (_vr.setAutoLoad(CMD_RECORDS, CMD_NUM) >= 0) {
    Serial.println(F("Autoload records successful"));
    delay(1500);
    return 1;
  }
  return 0;
}

/**
 * Print seperator. Print 80 '-'.
 */
void printSeperator() {
  for (int i = 0; i < 80; i++) {
    Serial.write('-');
  }
  Serial.println();
}

/**
 * Print help text and instructions.
 */
void printStart(void) {
  Serial.println(F("Props3D command training for Elechouse Voice Recognition V3 Module."));
  Serial.println(F("Follow the instructions as we train the seven commands for your VR module."));
  printSeperator();
  Serial.println(F("All seven commands will be trained in the expected sequence."));
  Serial.println(F("For each command, you will be prompted twice to say the same word or phrase."));
  Serial.println(F("If the training is not successful, you will have to repeat the process."));
  Serial.println(F("When the training is successful, you will be prompted to train the next command"));
  printSeperator();
  Serial.println(F("Let's begin ..."));
}

/**
 * Print "train" command return value.
 *   buf  -->  "train" command return value
 *     buf[0]    -->  number of records which are trained successfully.
 *     buf[2i+1]  -->  record number
 *     buf[2i+2]  -->  record train status.
 *       00 --> Trained 
 *       FE --> Train Time Out
 *       FF --> Value out of range"
 *   len  -->  length of buf
 */
int printTrain(uint8_t *buf, uint8_t len) {
  if (len == 0) {
    Serial.println(F("Training Finish."));
    return -1;
  }
  Serial.print(F("Commands attempted: "));
  Serial.println(buf[0], DEC);
  for (int i = 0; i < len - 1; i += 2) {
    Serial.print(F("Command "));
    Serial.print(buf[i + 1], DEC);
    Serial.print(F("\t"));
    switch (buf[i + 2]) {
      case 0:
        Serial.println(F("Trained"));
        return 1;
      case 0xFE:
        Serial.println(F("Training Time Out"));
        break;
      case 0xFF:
        Serial.println(F("Value out of range"));
        break;
      default:
        Serial.print(F("Unknown status "));
        Serial.println(buf[i + 2], HEX);
        break;
    }
  }
  return 0;
}

/**
 * Print signature, if the character is invisible print hexible value instead.
 * buf  -->  VR module return value when voice is recognized.
 *   buf[0]  -->  Group mode(FF: None Group, 0x8n: User, 0x0n:System
 *   buf[1]  -->  number of record which is recognized. 
 *   buf[2]  -->  Recognizer index(position) value of the recognized record.
 *   buf[3]  -->  Signature length
 *   buf[4]~buf[n] --> Signature
 */
void printVR(uint8_t *buf) {
  Serial.println(F("VR Index\tGroup\tRecordNum\tSignature"));

  Serial.print(buf[2], DEC);
  Serial.print(F("\t\t"));

  if (buf[0] == 0xFF) {
    Serial.print(F("NONE"));
  } else if (buf[0] & 0x80) {
    Serial.print(F("UG "));
    Serial.print(buf[0] & (~0x80), DEC);
  } else {
    Serial.print(F("SG "));
    Serial.print(buf[0], DEC);
  }
  Serial.print(F("\t"));

  Serial.print(buf[1], DEC);
  Serial.print(F("\t\t"));
  Serial.print(F("NONE"));
  Serial.println(F("\r\n"));
}
