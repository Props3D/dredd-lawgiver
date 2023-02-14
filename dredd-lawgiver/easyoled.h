#ifndef easyoled_h
#define easyoled_h

#if ENABLE_EASY_OLED == 1
#include <U8g2lib.h>
#endif

const static char STR_ARMOR_PIERCING[] PROGMEM = "ARMOR PIERCING";
const static char STR_INCENDIARY[] PROGMEM = "INCENDIARY";
const static char STR_HOT_SHOT[] PROGMEM = "HOT SHOT";
const static char STR_HIGH_EX[] PROGMEM = "HIGH EX";
const static char STR_STUN[] PROGMEM = "STUN";
const static char STR_RAPID[] PROGMEM = "RAPID";
const static char STR_SEMI[] PROGMEM = "SEMI";
const static char STR_EMPTY[] PROGMEM = "EMPTY";
const static char STR_AMMUNITION_LOW[] PROGMEM = "AMMUNITION LOW";
const static char STR_DISTANCE[] PROGMEM = "D:0.0";
const static char STR_CLEAR[] PROGMEM = "";

const static char STR_LOGO[] PROGMEM = "Props3D Pro";
const static char STR_DNA_CHECK[] PROGMEM = "DNA CHECK";
const static char STR_COMM_OK[] PROGMEM = "COMM OK";
const static char STR_ID_OK[] PROGMEM = "I.D. OK";
const static char STR_ID_FAIL[] PROGMEM = "I.D. FAIL";
const static char STR_BOOT_ERROR[] PROGMEM = "Boot Error";
const static char STR_CHK_BAT[] PROGMEM = "Check battery levels";

/**
 * A simple class for managing an LED display. It's mainly based on
 * OLED 2.08 display
 *
 * The main configuration is specified on declaration:
 * eg. EasyOLED<13, 11, 8, 9, 10> oledDisplay;
 *
 * In the setup, you can use the begin() function to initialize the display count and brightness.
 * eg. display.begin();
 *
 * The display count can be updated using the updateDisplay() function
 * eg. display.updateDisplay(ammoSelection, ammoCounter);
 *
 * REQUIRED LIBRARY: U8g2lib
 */
template<int CL_PIN, int DA_PIN, int CS_PIN, int DC_PIN, int RESET_PIN>
class EasyOLED {
public:
  static const int DISPLAY_LOGO = 1;
  static const int DISPLAY_COMM_CHK = 2;
  static const int DISPLAY_DNA_CHK = 3;
  static const int DISPLAY_DNA_PRG = 4;
  static const int DISPLAY_ID_OK = 5;
  static const int DISPLAY_ID_NAME = 6;
  static const int DISPLAY_MAIN = 7;
  static const int DISPLAY_ID_FAIL = 8;
  static const int DISPLAY_BOOT_ERROR = 9;
 
  EasyOLED()
#if ENABLE_EASY_OLED == 1
      : u8g2(U8G2_R2, /* clock=*/CL_PIN, /* data=*/DA_PIN, /* cs=*/CS_PIN, /* dc=*/DC_PIN, /* reset=*/RESET_PIN)
      //u8g2(U8G2_R2, /* cs=*/CS_PIN, /* dc=*/DC_PIN, /* reset=*/RESET_PIN)
#endif
      {}

  /**
     *  In the setup, initialize the display count and brightness.
     */
  void begin(int ammoSelection, uint8_t ammoCounts[]) {
#if ENABLE_EASY_OLED == 1
    //Serial.println(F("Initializing OLED display"));
    u8g2.begin();
    u8g2.setBusClock(8000000);
    _ammoSelection = ammoSelection;
    memcpy(_ammoCounts, ammoCounts, sizeof(_ammoCounts));
#endif
  }

  /**
     * Return the current display mode
     */
  int getDisplayMode() {
    return _displayMode;
  }

  /**
     * This needs to be called separately so the detection can be delayed after the shot.
     */
  void checkAmmoLevels() {
#if ENABLE_EASY_OLED == 1
    int ammoCount = _ammoCounts[_ammoIdx[_ammoSelection]];
    _ammoLow = (ammoCount < 5 && ammoCount > 0);
#endif
  }

  /**
     * This function is used only during the start up sequence.
     * This does not need to be called continuously from the main loop.
     * It's better to only call this when updates are necesary.
     */
  void updateDisplayMode(int displayMode, uint8_t progress, bool blink = false) {
    _displayMode = displayMode;
    _progressBar = progress * _progressBarIncrement;
    _blink = blink;
    drawDisplay(_displayMode, _progressBar);
  }

  /**
     * Update the OLED display.
     * This does not need to be called continuously from the main loop.
     * It's better to only call this when updates are necesary.
     */
  void updateDisplay(int ammoMode, uint8_t counters[], bool blink = false) {
    _blink = blink;
    if (_ammoSelection != ammoMode) {
      _ammoSelection = ammoMode;
      // check switching to ammo that is already low
      checkAmmoLevels();
    }
    memcpy(_ammoCounts, counters, 4*sizeof(int));
    drawDisplay(_displayMode, _progressBar);
  }

private:
  // number eof pixels to move the progress bar on startup
  const uint8_t _progressBarIncrement = 10;
  // index of ammo selections and ammo counters based on the config.h
  const uint8_t _ammoIdx[8] = { 0, 1, 1, 2, 3, 3, 3, 3 };

  // See the instructions for optimizing the U8g2 lib.
#if ENABLE_EASY_OLED == 1
  U8G2_SH1122_256X64_2_4W_SW_SPI u8g2;
  //U8G2_SH1122_256X64_2_4W_HW_SPI u8g2;
  //U8G2_SH1122_256X64_F_4W_HW_SPI u8g2;
#endif
  int _displayMode = 0;  // tracking display modes during start up
  int _progressBar = 0;  // tracking progress during startup sequence

  int _ammoSelection = 0;  // ammo selecetor
  uint8_t _ammoCounts[4];      // ammo counts
  bool _blink = false;         // blink controller
  bool _ammoLow = false;       // ammo low state
  char _buf[10];               // print buffer for ammo counts
  char _txtbuf[80];            // print buffer for text

  void drawDisplay(int displayMode, int progress) {
#if ENABLE_EASY_OLED == 1
    u8g2.firstPage();
    do {
      switch (displayMode) {
        case DISPLAY_MAIN:
          drawFiringMode();
          break;
        case DISPLAY_LOGO:
          drawLogo();
          break;
        case DISPLAY_COMM_CHK:
          // COMM OK
          drawCommOk(progress);
          break;
        case DISPLAY_DNA_CHK:
        case DISPLAY_DNA_PRG:
          // DNA Check
          drawDNACheck(progress);
          break;
        case DISPLAY_ID_OK:
          // ID OK
          drawIDOk(progress);
          break;
        case DISPLAY_ID_NAME:
          // ID NAME
          drawIDName(progress);
          break;
        case DISPLAY_ID_FAIL:
          // ID FAIL
          drawIDFail(progress);
          break;
        default:
          // BOOT Error
          drawBootError();
          break;
      }
    } while (u8g2.nextPage());
#endif
  }

  void drawFiringMode() {
#if ENABLE_EASY_OLED == 1
    // check if ammo was low but got reset befeore drawing components
    if (_ammoLow) checkAmmoLevels();
    drawGrid();
    drawAmmoMode();
    drawAmmoName();
    drawAmmoField();
#endif
  }

  void drawLogo() {
#if ENABLE_EASY_OLED == 1
    u8g2.setFont(u8g2_font_helvB18_tr);
    u8g2.setCursor(40, 42);
    printText(STR_LOGO);
#endif
  }

  void drawBootError() {
#if ENABLE_EASY_OLED == 1
    u8g2.setFont(u8g2_font_helvB14_tr);
    u8g2.setCursor(42, 30);
    printText(STR_BOOT_ERROR);
    u8g2.setCursor(20, 45);
    printText(STR_CHK_BAT);
#endif
  }

  void drawProgress(int progress) {
#if ENABLE_EASY_OLED == 1
    if (progress > 0) {
      u8g2.drawBox(0, 0, progress, 7);
      u8g2.drawDisc(progress, 3, 3);
    }
#endif
  }

  void drawCommOk(int progress) {
#if ENABLE_EASY_OLED == 1
    drawProgress(progress);
    u8g2.setFont(u8g2_font_helvB14_tr);
    u8g2.setCursor(0, 42);
    printText(STR_COMM_OK);
    drawAmmoMode();
    drawGrid();
#endif
  }

  void drawDNACheck(int progress) {
#if ENABLE_EASY_OLED == 1
    drawProgress(progress);
    u8g2.setFont(u8g2_font_helvB14_tr);
    u8g2.setCursor(0, 42);
    printText(STR_DNA_CHECK);
    drawAmmoMode();
    drawGrid();
#endif
  }

  void drawIDOk(int progress) {
#if ENABLE_EASY_OLED == 1
    drawProgress(progress);
    u8g2.setFont(u8g2_font_helvB14_tr);
    u8g2.setCursor(0, 42);
    if (_blink) {
      printText(STR_ID_OK);
    } else {
      printText(STR_CLEAR);
    }
    drawAmmoMode();
    drawGrid();
#endif
  }

  void drawIDFail(int progress) {
#if ENABLE_EASY_OLED == 1
    drawProgress(progress);
    u8g2.setFont(u8g2_font_helvB14_tr);
    u8g2.setCursor(0, 42);
    if (_blink) {
      printText(STR_ID_FAIL);
    } else {
      printText(STR_CLEAR);
    }
    drawAmmoMode();
    drawGrid();
#endif
  }

  void drawIDName(int progress) {
#if ENABLE_EASY_OLED == 1
    drawProgress(progress);
    u8g2.setFont(u8g2_font_helvB14_tr);
    u8g2.setCursor(0, 42);
    printText(DISPLAY_USER_ID);
    drawAmmoMode();
    drawGrid();
#endif
  }

  void drawGrid() {
#if ENABLE_EASY_OLED == 1
    //Battery-State
    u8g2.drawLine(200, 10, 204, 0);
    u8g2.drawLine(201, 10, 205, 0);
    u8g2.drawLine(202, 10, 206, 0);
    //
    u8g2.drawLine(205, 10, 209, 0);
    u8g2.drawLine(206, 10, 210, 0);
    u8g2.drawLine(207, 10, 211, 0);
    //
    u8g2.drawLine(210, 10, 214, 0);
    u8g2.drawLine(211, 10, 215, 0);
    u8g2.drawLine(212, 10, 216, 0);
    //
    u8g2.drawLine(215, 10, 219, 0);
    u8g2.drawLine(216, 10, 220, 0);
    u8g2.drawLine(217, 10, 221, 0);
    //
    u8g2.drawLine(220, 10, 224, 0);
    u8g2.drawLine(221, 10, 225, 0);
    u8g2.drawLine(222, 10, 226, 0);
    //

    //Grid
    u8g2.drawBox(0, 44, 240, 2);
    u8g2.drawBox(46, 44, 2, 20);
    u8g2.drawBox(92, 44, 2, 20);
    u8g2.drawBox(138, 44, 2, 20);
    u8g2.drawBox(184, 44, 2, 20);

    //distance field
    u8g2.setFont(u8g2_font_helvB12_tr);
    u8g2.setCursor(0, 61);
    printText(STR_DISTANCE);
#endif
  }

  void drawAmmoField() {
#if ENABLE_EASY_OLED == 1
    // Standard
    u8g2.setFont(u8g2_font_helvB12_tr);
    u8g2.setDrawColor(1);
    u8g2.setCursor(48, 61);
    formatAmmo(_buf, 0);
    u8g2.print(_buf);
    u8g2.setCursor(95, 61);
    formatAmmo(_buf, 1);
    u8g2.print(_buf);
    u8g2.setCursor(141, 61);
    formatAmmo(_buf, 2);
    u8g2.print(_buf);
    formatAmmo(_buf, 3);
    u8g2.setCursor(187, 61);
    u8g2.print(_buf);
    switch (_ammoSelection) {
      case 0:  // armor piercing
        u8g2.setDrawColor(1);
        u8g2.drawBox(46, 46, 46, 20);
        u8g2.setDrawColor(0);
        u8g2.setCursor(48, 61);
        formatAmmo(_buf, 0);
        u8g2.print(_buf);
        u8g2.setDrawColor(1);
        break;
      case 1:  // incendiary
      case 2:  // hotshot
        u8g2.setDrawColor(1);
        u8g2.drawBox(92, 46, 46, 20);
        u8g2.setDrawColor(0);
        u8g2.setCursor(95, 61);
        formatAmmo(_buf, 1);
        u8g2.print(_buf);
        u8g2.setDrawColor(1);
        break;
      case 3:  // high explosive
        u8g2.setDrawColor(1);
        u8g2.drawBox(138, 46, 46, 20);
        u8g2.setDrawColor(0);
        u8g2.setCursor(141, 61);
        formatAmmo(_buf, 2);
        u8g2.print(_buf);
        u8g2.setDrawColor(1);
        break;
      default:  // FMJ / STUN / RAPID
        u8g2.setDrawColor(1);
        u8g2.drawBox(184, 46, 56, 20);
        u8g2.setDrawColor(0);
        u8g2.setCursor(187, 61);
        formatAmmo(_buf, 3);
        u8g2.print(_buf);
        u8g2.setDrawColor(1);
        break;
    }
#endif
  }

  void drawAmmoMode() {
#if ENABLE_EASY_OLED == 1
    u8g2.setDrawColor(1);
    u8g2.setFont(u8g2_font_helvB14_tr);
    u8g2.setCursor(180, 42);
    if (_displayMode < DISPLAY_MAIN) {
      if (_displayMode == DISPLAY_DNA_CHK)
        printText(STR_RAPID);
      else
        printText(STR_SEMI);
    }

    if (_displayMode == DISPLAY_MAIN) {
      int ammoCount = _ammoCounts[_ammoIdx[_ammoSelection]];
      if (_ammoLow) {
        // low ammo
        printText(STR_SEMI);
      } else if (ammoCount == 0) {
        // empty clip
        printText(STR_SEMI);
      } else {
        switch (_ammoSelection) {
          case 1:  // incendiary
          case 2:  // hotshot
            printText(STR_CLEAR);
            break;
          case 6:  // FMJ
            printText(STR_RAPID);
            break;
          default:  // armor p / high ex / stun / FMJ
            printText(STR_SEMI);
            break;
        }
      }
    }
#endif
  }

  void drawAmmoName() {
#if ENABLE_EASY_OLED == 1
    int ammoCount = _ammoCounts[_ammoIdx[_ammoSelection]];
    u8g2.setDrawColor(1);
    u8g2.setFont(u8g2_font_helvB14_tr);
    if (_ammoLow) {
      u8g2.setCursor(0, 42);
      printText(STR_AMMUNITION_LOW);
    } else if (ammoCount == 0) {
      // Gun Empty - blink
      u8g2.setCursor(0, 42);
      printText(STR_EMPTY);
    } else {
      switch (_ammoSelection) {
        case 0:
          u8g2.setCursor(0, 42);
          printText(STR_ARMOR_PIERCING);
          break;
        case 1:
          u8g2.setCursor(0, 42);
          printText(STR_INCENDIARY);
          break;
        case 2:
          u8g2.setCursor(0, 42);
          printText(STR_HOT_SHOT);
          break;
        case 3:
          u8g2.setCursor(0, 42);
          printText(STR_HIGH_EX);
          break;
        case 4:
          u8g2.setCursor(0, 42);
          printText(STR_STUN);
          break;
        default:
          // FMJ / Rapid
          u8g2.setCursor(0, 42);
          printText(STR_CLEAR);
          break;
      }
    }
#endif
  }

  void formatInt(int i, int factor, char* strbuf) {
    uint8_t len = 0;
    while ((factor > 1) && (i >= factor)) {
      strbuf[len++] = ' ';
      factor /= 10;
    }
    itoa(i, &strbuf[len], 10);
  }

  void formatAmmo(char* strbuf, int idx) {
    memset(strbuf, 0, sizeof(char)*10);

    formatInt(_ammoCounts[idx], 10, strbuf);
    uint8_t len = strlen(strbuf);
    switch (idx) {
      case 0:  // armor piercing
        strbuf[len++] = 'a';
        strbuf[len++] = 'p';
        strbuf[len] = '\0';  // NUL-terminate the C string
        //sprintf (strbuf, "%dap", _ammoCounts[0]);
        break;
      case 1:  // incendiary
        strbuf[len++] = 'i';
        strbuf[len++] = 'n';
        strbuf[len] = '\0';  // NUL-terminate the C string
        //sprintf (strbuf, "%din", _ammoCounts[1]);
        break;
      case 2:  // highex
        strbuf[len++] = 'h';
        strbuf[len++] = 'e';
        strbuf[len] = '\0';  // NUL-terminate the C string
        //sprintf (strbuf, "%dhe", _ammoCounts[2]);
        break;
      case 3:  // full metal jacket
        strbuf[len++] = 'f';
        strbuf[len++] = 'm';
        strbuf[len++] = 'j';
        strbuf[len] = '\0';  // NUL-terminate the C string
        //sprintf (strbuf, "%dfmj", _ammoCounts[3]);
        break;
    }
  }

  void printText(const char* str) {
#if ENABLE_EASY_OLED == 1
    u8g2.print(getString(str));
#endif
  }

  char* getString(const char* str) {
    memset(_txtbuf, 0, sizeof(char)*80);
    strcpy_P(_txtbuf, (char*)str);
    return _txtbuf;
  }
};

#endif
