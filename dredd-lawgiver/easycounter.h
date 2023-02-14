#ifndef easycounter_h
#define easycounter_h

namespace easy_counter {
    static const uint8_t STATE_ACTIVE     = 0;
    static const uint8_t STATE_EMPTY      = 1;
    static const uint8_t STATE_RESET      = 2;

    static const int COUNTER_MODE_UP   =  1;
    static const int COUNTER_MODE_DOWN = -1;

}
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
    int _increment = easy_counter::COUNTER_MODE_DOWN;
    int _low;
    int _high;
    bool _resetOnEmpty;
    volatile int _state;
    volatile int _currentCounter;
 
    // helper functions
    void setLow(int number) { this->_low = number; }
    void setHigh(int number) { this->_high = number; }
    void setIncrement(int value) { this->_increment = value; }
    void setCount(int value) { this->_currentCounter = value; }
    void setResetOnEmpty(bool value) { this->_resetOnEmpty = value; }
  public:
  
    EasyCounter() {}

    void begin(int lowNumber, int highNumber, int increment) {
      begin(lowNumber, highNumber, increment, false);
    }
    void begin(int lowNumber, int highNumber, int increment, bool resetOnEmpty) {
      setLow(lowNumber);
      setHigh(highNumber);
      setIncrement(increment);
      increment == easy_counter::COUNTER_MODE_UP ? setCount(_low) : setCount(_high);
      _resetOnEmpty = resetOnEmpty;
      _state = easy_counter::STATE_ACTIVE;
    }

    bool EasyCounter::tick() {
      if (isEmpty()) {
        _state = easy_counter::STATE_EMPTY;
        if (_resetOnEmpty) resetCount();
        return false;
      }
      if (_increment == easy_counter::COUNTER_MODE_UP) {
          _currentCounter = _currentCounter + 1;
      } else {
          _currentCounter = _currentCounter - 1;
      }
      _state = easy_counter::STATE_ACTIVE;
      return true;
    }

    bool isEmpty() {
      if (_increment == easy_counter::COUNTER_MODE_UP)
        return (_currentCounter == _high);
      if (_increment == easy_counter::COUNTER_MODE_DOWN)
        return (_currentCounter == _low);
      return false;
    }

    bool isFull() {
      if (_increment == easy_counter::COUNTER_MODE_UP)
        return (_currentCounter == _low);
      if (_increment == easy_counter::COUNTER_MODE_DOWN)
        return (_currentCounter == _high);
      return false;
    }

    // returns the current count
    int resetCount() {
      if (_increment == easy_counter::COUNTER_MODE_UP)
        setCount(_low);
      if (_increment == easy_counter::COUNTER_MODE_DOWN)
        setCount(_high);
      _state = easy_counter::STATE_RESET;
      return this->_currentCounter;
    }

    int getCount() {
      return this->_currentCounter;
    }

    uint8_t getState() {
      return _state;
    }
};

#endif
