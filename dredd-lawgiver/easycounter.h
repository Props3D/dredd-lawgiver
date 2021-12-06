#ifndef easycounter_h
#define easycounter_h

#include <Arduino.h>
#include "debug.h"

const static uint8_t STATE_ACTIVE      = 0;
const static uint8_t STATE_EMPTY       = 1;
const static uint8_t STATE_FULL        = 2;

const static uint8_t COUNTER_MODE_UP = 1;
const static uint8_t COUNTER_MODE_DOWN = -1;

/**
 * Simple class for tracking counts up or down.
 * Use the constructor to set the min, max, and increment.
 * eg. EasyCounter countZeroToOneHundredByOne(0, 100, 1);
 * eg. EasyCounter countZeroToOneHundredByTwo(0, 100, 2);
 * eg. EasyCounter countOneHundredToZeroByOne(0, 100, -1);
 * eg. EasyCounter countOneHundredToZeroByTwo(0, 100, -2);
 * 
 * You can move the counter by using the toggleCount function:
 * eg. int value = countZeroToOneHundredByOne.toggleCount();
 * 
 * You can reset the counter to the original state:
 * eg. int value = countZeroToOneHundredByOne.resetCount();

 */
class EasyCounter
{
  private:
    String _name;
    int8_t _increment = COUNTER_MODE_DOWN;
    uint8_t _low;
    uint8_t _high;
    bool _resetOnEmpty;
    volatile uint8_t _currentCounter;
    // file indexes for different states
    //uint8_t _activeTrackIdx;
    //uint8_t _emptyTrackIdx;
    //uint8_t _resetTrackIdx;
 
    // helper functions
    void setLow(int number) { this->_low = number; }
    void setHigh(int number) { this->_high = number; }
    void setIncrement(int value) { this->_increment = value; }
    void setCount(int value) { this->_currentCounter = value; }
    void setResetOnEmpty(bool value) { this->_resetOnEmpty = value; }
  public:
    EasyCounter(const char *label) : _name(label) {}
    //EasyCounter(const char *label, uint8_t activeTrack, uint8_t emptyTrack, uint8_t resetTrack) : _name(label) {
    //  this->_activeTrackIdx = activeTrack;
    //  this->_emptyTrackIdx = emptyTrack;
    //  this->_resetTrackIdx = resetTrack;
    //}

    void begin(uint8_t lowNumber, uint8_t highNumber, int8_t increment, bool resetOnEmpty) {
      //Serial.print("Initialing counter: ");
      //Serial.println(_name);
      setLow(lowNumber);
      setHigh(highNumber);
      setIncrement(increment);
      resetCount();
    }

//    void setTracks(uint8_t activeTrack, uint8_t emptyTrack, uint8_t resetTrack) {
//      this->_activeTrackIdx = activeTrack;
//      this->_emptyTrackIdx = emptyTrack;
//      this->_resetTrackIdx = resetTrack;
//    }

    EasyCounter* EasyCounter::tick() {
      if (isEmpty()) {
        if (_resetOnEmpty) resetCount();
        return this;
      }
      if (_increment == COUNTER_MODE_UP) 
          _currentCounter = _currentCounter + 1;
       else
          _currentCounter = _currentCounter - 1;
      return this;
    }

    bool isEmpty() {
      if (_increment == COUNTER_MODE_UP)
        return (_currentCounter == _high);
      if (_increment == COUNTER_MODE_DOWN)
        return (_currentCounter == _low);
      return false;
    }

    bool isFull() {
      if (_increment == COUNTER_MODE_UP)
        return (_currentCounter == _low);
      if (_increment == COUNTER_MODE_DOWN)
        return (_currentCounter == _high);
      return false;
    }

    uint8_t resetCount() {
      if (_increment == COUNTER_MODE_UP)
        setCount(_low);
      if (_increment == COUNTER_MODE_DOWN)
        setCount(_high);
      //Serial.print(_name);
      //Serial.print(" reset counter: ");
      //Serial.println(_currentCounter);
      return this->_currentCounter;
    }

    uint8_t getCount() {
      return this->_currentCounter;
    }

    uint8_t getState() {
      if (isEmpty()) return STATE_EMPTY;
      if (isFull()) return STATE_FULL;
      return STATE_ACTIVE;
    }
};

#endif
