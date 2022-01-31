#ifndef easycounter_h
#define easycounter_h

#include "debug.h"

const static uint8_t STATE_ACTIVE      = 0;
const static uint8_t STATE_EMPTY       = 1;
const static uint8_t STATE_RELOAD      = 2;

const static int COUNTER_MODE_UP    =  1;
const static int COUNTER_MODE_DOWN  = -1;

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
    int _increment = COUNTER_MODE_DOWN;
    int _low;
    int _high;
    bool _resetOnEmpty;
    volatile int _currentCounter;
 
    // helper functions
    void setLow(int number) { this->_low = number; }
    void setHigh(int number) { this->_high = number; }
    void setIncrement(int value) { this->_increment = value; }
    void setCount(int value) { this->_currentCounter = value; }
    void setResetOnEmpty(bool value) { this->_resetOnEmpty = value; }
  public:
    EasyCounter(const char *label) : _name(label) {}

    void begin(int lowNumber, int highNumber, int increment) {
      begin(lowNumber, highNumber, increment, false);
    }
    void begin(int lowNumber, int highNumber, int increment, bool resetOnEmpty) {
      setLow(lowNumber);
      setHigh(highNumber);
      setIncrement(increment);
      increment == COUNTER_MODE_UP ? setCount(_low) : setCount(_high);
    }

    EasyCounter* EasyCounter::tick() {
      if (isEmpty()) {
        if (_resetOnEmpty) resetCount();
        return this;
      }
      if (_increment == COUNTER_MODE_UP) {
          _currentCounter = _currentCounter + 1;
      } else {
          _currentCounter = _currentCounter - 1;
      }
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

    // returns the reload state
    int resetCount() {
      if (_increment == COUNTER_MODE_UP)
        setCount(_low);
      if (_increment == COUNTER_MODE_DOWN)
        setCount(_high);
      return this->_currentCounter;
    }

    int getCount() {
      return this->_currentCounter;
    }

    uint8_t getState() {
      if (isEmpty()) return STATE_EMPTY;
      return STATE_ACTIVE;
    }
};

#endif
