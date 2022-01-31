#ifndef easyoled_h
#define easyoled_h

#include <U8g2lib.h>
#include "debug.h"

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

template <int CL_PIN, int DA_PIN, int CS_PIN, int DC_PIN, int RESET_PIN>
class EasyOLED
{

  private:
    const uint8_t _progressBarIncrement = 10;
    // index of ammo selections and ammo counters based on the config.h
    uint8_t _ammoIdx[8] = {0, 1, 1, 2, 3, 3, 3, 3};

    // See the instructions for optimizing the U8g2 lib.
    U8G2_SH1122_256X64_2_4W_SW_SPI u8g2;

    String _name;
    uint8_t _displayMode = 0;
    uint8_t _progressBar = 0;

    uint8_t _ammoSelection = 0;
    bool _blink = false;
    uint8_t _ammoCounts[4];
    char _buf[10];

  public:
    const static uint8_t DISPLAY_LOGO       = 1;
    const static uint8_t DISPLAY_COMM_CHK   = 2;
    const static uint8_t DISPLAY_DNA_CHK    = 3;
    const static uint8_t DISPLAY_DNA_PRG    = 4;
    const static uint8_t DISPLAY_ID_OK      = 5;
    const static uint8_t DISPLAY_ID_NAME    = 6;
    const static uint8_t DISPLAY_MAIN       = 7;
    const static uint8_t DISPLAY_ID_FAIL    = 8;

    EasyOLED(const char *name) : u8g2(U8G2_R0, /* clock=*/CL_PIN, /* data=*/DA_PIN, /* cs=*/CS_PIN, /* dc=*/DC_PIN, /* reset=*/RESET_PIN),
      _name(name) {
    }

    /**
       In the setup, initialize the display count and brightness.
    */
    void begin(int ammoSelection, uint8_t ammoCounts[]) {
#ifdef ENABLE_EASY_OLED
      debugLog("Initializing OLED display");
      u8g2.begin();
      u8g2.setBusClock(8000000);
      _ammoSelection = ammoSelection;
      memcpy(_ammoCounts, ammoCounts, sizeof(_ammoCounts));
#endif
    }

    /**
     * 
     */
    void setDisplayMode(uint8_t displayMode) {
      _displayMode = displayMode;
    }

    /**
     * 
     */
    uint8_t getDisplayMode() {
      return _displayMode;
    }

    /**
     * Refresh the component. This MUST be called continuously from
     * the main program loop. For this reason, don't add any debug logging in this method.
     *  
     */
    void startupDisplay(uint8_t displayMode, uint8_t progress) {
      _displayMode = displayMode;
      _progressBar = progress * _progressBarIncrement;
      _blink = (millis() % 1000) < 500;

      drawDisplay(_displayMode, _progressBar);
    }
    
    /**
     * Refresh the component. This MUST be called continuously from
     *  the main program loop
     *  
     *  For this reason, don't add any debug logging in this method.
     */
    void updateDisplay(int ammoSelection, uint8_t ammoCounts[]) {
      _ammoSelection = ammoSelection;
      memcpy(_ammoCounts, ammoCounts, sizeof(_ammoCounts));
      drawDisplay(_displayMode, _progressBar);
      delay(10);
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
      u8g2.print(F("Props3D Pro"));
#endif
    }

    void drawProgress(int progress) {
#ifdef ENABLE_EASY_OLED
      if (progress > 0) {
        u8g2.drawBox(0, 0, progress , 7);
        u8g2.drawDisc(progress, 3, 3);
      }
#endif
    }

    void drawCommOk(int progress) {
#ifdef ENABLE_EASY_OLED
      drawProgress(progress);
      u8g2.setFont(u8g2_font_helvB14_tr);
      u8g2.setCursor(0, 42);
      u8g2.print(F("COMM OK"));
      drawAmmoMode();
      drawGrid();
#endif
    }

    void drawDNACheck(int progress) {
#ifdef ENABLE_EASY_OLED
      drawProgress(progress);
      u8g2.setFont(u8g2_font_helvB14_tr);
      u8g2.setCursor(0, 42);
      u8g2.print(F("DNA CHECK"));
      drawAmmoMode();
      drawGrid();
#endif
    }

    void drawIDOk(int progress) {
#ifdef ENABLE_EASY_OLED
      drawProgress(progress);
      u8g2.setFont(u8g2_font_helvB14_tr);
      u8g2.setCursor(0, 42);
      if (_blink) {
        u8g2.print(F("I.D. OK"));
      } else {
        u8g2.print(F(""));
      }
      drawAmmoMode();
      drawGrid();
#endif
    }

    void drawIDFail(int progress) {
#ifdef ENABLE_EASY_OLED
      drawProgress(progress);
      u8g2.setFont(u8g2_font_helvB14_tr);
      u8g2.setCursor(0, 42);
      if (_blink) {
        u8g2.print(F("I.D. FAIL"));
      } else {
        u8g2.print(F(""));
      }
      drawAmmoMode();
      drawGrid();
#endif
    }

    void drawIDName(int progress) {
#ifdef ENABLE_EASY_OLED
      drawProgress(progress);
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
          case DISPLAY_ID_NAME:
            // ID NAME
            drawIDName(progress);
            break;
          case DISPLAY_ID_FAIL:
            // ID FAIL
            drawIDFail(progress);
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
      u8g2.drawBox(44, 44, 2, 20);
      u8g2.drawBox(92, 44, 2, 20);
      u8g2.drawBox(138, 44, 2, 20);
      u8g2.drawBox(184, 44, 2, 20);

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
      u8g2.setCursor(47, 61);
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
        case 0: // armor piercing
          u8g2.setDrawColor(1);
          u8g2.drawBox(44, 46, 46, 20);
          u8g2.setDrawColor(0);
          u8g2.setCursor(47, 61);
          formatAmmo(_buf, 0);
          u8g2.print(_buf);
          u8g2.setDrawColor(1);
          break;
        case 1: // incendiary
        case 2: // hotshot
          u8g2.setDrawColor(1);
          u8g2.drawBox(92, 46, 46, 20);
          u8g2.setDrawColor(0);
          u8g2.setCursor(95, 61);
          formatAmmo(_buf, 1);
          u8g2.print(_buf);
          u8g2.setDrawColor(1);
          break;
        case 3: // high explosive
          u8g2.setDrawColor(1);
          u8g2.drawBox(138, 46, 46, 20);
          u8g2.setDrawColor(0);
          u8g2.setCursor(141, 61);
          formatAmmo(_buf, 2);
          u8g2.print(_buf);
          u8g2.setDrawColor(1);
          break;
        default: // FMJ / STUN / RAPID
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
            case 2: // hotshot
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
      int ammoCount = _ammoCounts[_ammoIdx[_ammoSelection]];
      u8g2.setDrawColor(1);
      u8g2.setFont(u8g2_font_helvB14_tr);
      if (ammoCount < 4 && ammoCount > 0) {
        u8g2.setCursor(0, 42);
        u8g2.print(F("AMMUNITION LOW"));
      } else if (ammoCount == 0 ) {
        // Gun Empty - blink
        u8g2.setCursor(0, 42);
        u8g2.print(F("EMPTY"));
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
            u8g2.print(F("HOT SHOT"));
            break;
          case 3:
            u8g2.setCursor(0, 42);
            u8g2.print(F("HIGH EX"));
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

    void formatInt( int i, int factor, char* _buffer)
    {
      uint8_t len        = 0;
      while ((factor > 1) && (i >= factor)) {
        _buffer[ len++ ] = ' ';
        factor /= 10;
      }
      itoa( i, &_buffer[ len ], 10 );
    }

    void formatAmmo(char* _buffer, int idx) {
      memset(_buffer, 0, sizeof(_buffer));
      formatInt(_ammoCounts[idx], 10, _buffer);
      uint8_t len = strlen(_buffer);
      switch (idx) {
        case 0: // armor piercing
          _buffer[ len++ ] = 'a';
          _buffer[ len++ ] = 'p';
          _buffer[ len   ] = '\0'; // NUL-terminate the C string
          //sprintf (_buf, "%dap", _ammoCounts[0]);
          break;
        case 1: // incendiary
          _buf[ len++ ] = 'i';
          _buf[ len++ ] = 'n';
          _buf[ len   ] = '\0'; // NUL-terminate the C string
          //sprintf (_buf, "%din", _ammoCounts[1]);
          break;
        case 2: // highex
          _buf[ len++ ] = 'h';
          _buf[ len++ ] = 'e';
          _buf[ len   ] = '\0'; // NUL-terminate the C string
          //sprintf (_buf, "%dhe", _ammoCounts[2]);
          break;
        case 3: // full metal jacket
          _buf[ len++ ] = 'f';
          _buf[ len++ ] = 'm';
          _buf[ len++ ] = 'j';
          _buf[ len   ] = '\0'; // NUL-terminate the C string
          //sprintf (_buf, "%dfmj", _ammoCounts[3]);
          break;
      }
    }

};

#endif
