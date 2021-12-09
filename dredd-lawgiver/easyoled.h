#ifndef easyoled_h
#define easyoled_h

//#include <Arduino.h>
#include <U8g2lib.h>
#include "debug.h"

//#ifdef U8X8_HAVE_HW_SPI
//#include <SPI.h>
//#endif


/**
   A simple class for managing an LED display. It's mainly based on
   OLED 2.08 display

   The main configuration is specified on declaration:
   eg. EasyOLED<8, 9, 10> oledDisplay;

   In the setup, you can use the begin() function to initialize the display count and brightness.
   eg. display.begin();

   The display count can be updated using the updateDisplay() function
   eg. display.updateDisplay(ammoSelection, ammoCounter);

   REQUIRED LIBRARY: U8g2lib
*/

template <int CS_PIN, int DC_PIN, int RESET_PIN>
class EasyOLED
{

  private:
    // After downloading the library, Don't forget to Enable U8G2_16BIT in u8g2.h
    U8G2_SH1122_256X64_2_4W_HW_SPI u8g2;

    String _name;
    uint8_t _displayMode = 0;
    uint8_t _progressBar = 0;

    long _progressBarInterval = 100;
    long _lastProgressBarUpdate = 0;
    uint8_t _ammoSelection = 0;
    bool _blink = false;
    uint8_t _ammoCounts[4];
    char _buf[10];

  public:
    const static uint8_t DISPLAY_LOGO       = 0;
    const static uint8_t DISPLAY_COMM_CHK   = 1;
    const static uint8_t DISPLAY_DNA_CHK    = 2;
    const static uint8_t DISPLAY_DNA_PRG    = 3;
    const static uint8_t DISPLAY_ID_OK      = 4;
    const static uint8_t DISPLAY_ID_FAIL    = 5;
    const static uint8_t DISPLAY_ID_NAME    = 6;
    const static uint8_t DISPLAY_MAIN       = 7;

    EasyOLED(const char *name) : u8g2(U8G2_R2, /* clock=13*/ /* data=11*/ /* cs=*/CS_PIN, /* dc=*/DC_PIN, /* reset=*/RESET_PIN),
                           _name(name){
    }

    /**
       In the setup, initialize the display count and brightness.
    */
    void begin(int ammoSelection, uint8_t ammoCounts[]) {
#ifdef ENABLE_EASY_OLED
      debugLog("Initialing OLED display");
      u8g2.begin();
      u8g2.setBusClock(8000000);
      _ammoSelection = ammoSelection;
      memcpy(_ammoCounts, ammoCounts, sizeof(_ammoCounts));
#endif
    }

    /**
    */
    void setDisplayMode(uint8_t displayMode) {
      _displayMode = displayMode;
    }

    /**
    */
    uint8_t currentDisplayMode() {
      return _displayMode;
    }

    /**
        Refresh the component. This MUST be called continuously from
        the main program loop

        For this reason, don't add any debug logging in this method.
    */
    void updateDisplay(int ammoSelection, int ammoCount) {
#ifdef ENABLE_EASY_OLED
      // Comm OK Sequence
      if (_displayMode == DISPLAY_COMM_CHK) {
        if (millis() > _progressBarInterval + _lastProgressBarUpdate) {
          //debugLog("OLED display - COMM OK Progress");
          _progressBar = _progressBar + 10;
          _lastProgressBarUpdate = millis();
        }
      }

      // DNA Progress Sequence
      if (_displayMode == DISPLAY_DNA_PRG) {
        if (millis() > _progressBarInterval + _lastProgressBarUpdate) {
          //debugLog("OLED display - DNA Check Progress");
          _progressBar = _progressBar + 10;
          _lastProgressBarUpdate = millis();
        }
      }

      if (_ammoSelection != ammoSelection || _ammoCounts[ammoSelection] != ammoCount) {
        _ammoSelection = ammoSelection;
        _ammoCounts[_ammoSelection] = ammoCount;
      }

      drawDisplay(_displayMode, _progressBar);
      delay(10);
#endif
    }


    void drawFiringMode() {
#ifdef ENABLE_EASY_OLED
      // Standard
      drawGrid();
      drawAmmoMode();
      drawAmmoName();
      drawAmmoField();
#endif
    }

    void drawLogo() {
#ifdef ENABLE_EASY_OLED
      u8g2.setFont(u8g2_font_helvB18_tr);
      u8g2.setCursor(40, 42);
      u8g2.print(F("3DProps Pro"));
#endif
    }

    void drawCommOk(int progress) {
#ifdef ENABLE_EASY_OLED
      u8g2.drawBox(0, 0, progress , 7);
      u8g2.drawDisc(progress, 3, 3);
      u8g2.setFont(u8g2_font_helvB14_tr);
      u8g2.setCursor(0, 42);
      u8g2.print(F("COMM OK"));
      drawAmmoMode();
      drawGrid();
#endif
    }

    void drawDNACheck(int progress) {
#ifdef ENABLE_EASY_OLED
      u8g2.drawBox(0, 0, progress , 7);
      u8g2.drawDisc(progress, 3, 3);
      u8g2.setFont(u8g2_font_helvB14_tr);
      u8g2.setCursor(0, 42);
      u8g2.print(F("DNA CHECK"));
      drawAmmoMode();
      drawGrid();
#endif
    }

    void drawIDOk(int progress) {
#ifdef ENABLE_EASY_OLED
      u8g2.drawBox(0, 0, progress , 7);
      u8g2.drawDisc(progress, 3, 3);
      u8g2.setFont(u8g2_font_helvB14_tr);
      u8g2.setCursor(0, 42);
      _blink = (millis() % (500 + 500) < 500);
      if (_blink) {
          u8g2.print(F(""));
      } else {
          u8g2.print(F("I.D. OK"));
      }
      drawAmmoMode();
      drawGrid();
#endif
    }

    void drawIDFail(int progress) {
#ifdef ENABLE_EASY_OLED
      u8g2.drawBox(0, 0, progress , 7);
      u8g2.drawDisc(progress, 3, 3);
      u8g2.setFont(u8g2_font_helvB14_tr);
      u8g2.setCursor(0, 42);
      if (_blink) {
          u8g2.print(F(""));
      } else {
          u8g2.print(F("I.D. FAIL"));
      }
      drawAmmoMode();
      drawGrid();
#endif
    }

    void drawIDName(void) {
#ifdef ENABLE_EASY_OLED
      u8g2.setFont(u8g2_font_helvB14_tr);
      u8g2.setCursor(0, 42);
      u8g2.print(_name);
      drawAmmoMode();
      drawGrid();
#endif
    }

    void drawDisplay(int displayMode, int progress) {
#ifdef ENABLE_EASY_OLED
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
          case DISPLAY_ID_FAIL:
            // ID OK
            drawIDFail(progress);
            break;
          case DISPLAY_ID_NAME:
            // ID NAME
            drawIDName();
            break;
          default:
            break;
        }
      } while ( u8g2.nextPage() );
#endif
    }

    void drawGrid() {
#ifdef ENABLE_EASY_OLED
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
      u8g2.drawBox(47, 44, 2, 20);
      u8g2.drawBox(95, 44, 2, 20);
      u8g2.drawBox(143, 44, 2, 20);
      u8g2.drawBox(191, 44, 2, 20);

      //distance field
      u8g2.setFont(u8g2_font_helvB12_tr);
      u8g2.setCursor(0, 61);
      u8g2.print(F("D:0.0"));
#endif
    }

    void drawAmmoField() {
#ifdef ENABLE_EASY_OLED
      // Standard
      u8g2.setFont(u8g2_font_helvB12_tr);
      u8g2.setDrawColor(1);
      u8g2.setCursor(54, 61);
      memset(_buf, 0, sizeof(_buf));
      sprintf (_buf, "%dap", _ammoCounts[0]);
      u8g2.print(_buf);
      u8g2.setCursor(104, 61);
      memset(_buf, 0, sizeof(_buf));
      sprintf (_buf, "%din", _ammoCounts[1]);
      u8g2.print(_buf);
      u8g2.setCursor(150, 61);
      memset(_buf, 0, sizeof(_buf));
      sprintf (_buf, "%dhe", _ammoCounts[2]);
      u8g2.print(_buf);
      memset(_buf, 0, sizeof(_buf));
      sprintf (_buf, "%dfmj", _ammoCounts[3]);
      u8g2.setCursor(196, 61);
      u8g2.print(_buf);
      switch (_ammoSelection) {
        case 0: // armor piercing
          u8g2.setDrawColor(1);
          u8g2.drawBox(47, 46, 48, 20);
          u8g2.setDrawColor(0);
          u8g2.setCursor(54, 61);
          memset(_buf, 0, sizeof(_buf));
          sprintf (_buf, "%dap", _ammoCounts[0]);
          u8g2.print(_buf);
          u8g2.setDrawColor(1);
          break;
        case 1: // incendiary
        case 3: // hotshot
          u8g2.setDrawColor(1);
          u8g2.drawBox(95, 46, 48, 20);
          u8g2.setDrawColor(0);
          u8g2.setCursor(104, 61);
          memset(_buf, 0, sizeof(_buf));
          sprintf (_buf, "%din", _ammoCounts[1]);
          u8g2.print(_buf);
          u8g2.setDrawColor(1);
          break;
        case 2: // high explosive
          u8g2.setDrawColor(1);
          u8g2.drawBox(143, 46, 48, 20);
          u8g2.setDrawColor(0);
          u8g2.setCursor(150, 61);
          memset(_buf, 0, sizeof(_buf));
          sprintf (_buf, "%dhe", _ammoCounts[2]);
          u8g2.print(_buf);
          u8g2.setDrawColor(1);
          break;
        default: // FMJ / STUN / RAPID
          u8g2.setDrawColor(1);
          u8g2.drawBox(191, 46, 48, 20);
          u8g2.setDrawColor(0);
          u8g2.setCursor(196, 61);
          memset(_buf, 0, sizeof(_buf));
          sprintf (_buf, "%dfmj", _ammoCounts[3]);
          u8g2.print(_buf);
          u8g2.setDrawColor(1);
          break;
      }
#endif
    }

    void drawAmmoMode() {
#ifdef ENABLE_EASY_OLED
      u8g2.setFont(u8g2_font_helvB14_tr);
      u8g2.setCursor(180, 42);
      if (_displayMode < DISPLAY_MAIN) {
          if (_displayMode == DISPLAY_DNA_CHK)
            u8g2.print(F("RAPID"));
          else
            u8g2.print(F("SEMI"));
      }
      
      if (_displayMode == DISPLAY_MAIN) {
        int ammoCount = _ammoCounts[_ammoSelection];
        if (ammoCount < 4 && ammoCount > 0) {
          // low ammo
          u8g2.print(F("SEMI"));
        } else if (ammoCount == 0 ) {
          // empty clip
          u8g2.print(F("SEMI"));
        }
        else
        {
          switch (_ammoSelection) {
            case 1: // incendiary
            case 3: // hotshot
              u8g2.print(F(""));
              break;
            case 6: // FMJ
              u8g2.print(F("RAPID"));
              break;
            default: // armor p / high ex / stun / FMJ
              u8g2.print(F("SEMI"));
              break;
          }
        }
      }
#endif
    }

    void drawAmmoName() {
#ifdef ENABLE_EASY_OLED
      int ammoCount = _ammoCounts[_ammoSelection];
      u8g2.setDrawColor(1);
      u8g2.setFont(u8g2_font_helvB14_tr);
      if (ammoCount < 4 && ammoCount > 0) {
        u8g2.setCursor(0, 42);
        u8g2.print(F("AMMUNITION LOW"));
      } else if (ammoCount == 0 ) {
      // Gun Empty - blink
        _blink = (millis() % (500 + 500) < 500);
        if (_blink) {
            u8g2.setCursor(0, 42);
            u8g2.print(F(""));
        } else {
            u8g2.setCursor(0, 42);
            u8g2.print(F("EMPTY"));
        }
      }
      else
      {
        switch (_ammoSelection) {
          case 0:
            u8g2.setCursor(0, 42);
            u8g2.print(F("ARMOR PIERCING"));
            break;
          case 1:
            u8g2.setCursor(0, 42);
            u8g2.print(F("INCENDIARY"));
            break;
          case 2:
            u8g2.setCursor(0, 42);
            u8g2.print(F("HIGH EX"));
            break;
          case 3:
            u8g2.setCursor(0, 42);
            u8g2.print(F("HOT SHOT"));
            break;
          case 4:
            u8g2.setCursor(0, 42);
            u8g2.print(F("STUN"));
            break;
          default:
            // FMJ / Rapid
            u8g2.setCursor(0, 42);
            u8g2.print(F(""));
            break;
        }
      }
#endif
    }

};

#endif
