#ifndef easyqueue_h
#define easyqueue_h


/**
 * A templated class for a FIFO queue, with a maximum size.
 * Uses a fixed array with revolving indices to track the
 * contents of the queue.
 */
template<class T>
class EasyQueue {
  private:
    uint8_t _head;
    uint8_t _tail;
    uint8_t _count;
    uint8_t _size;
    T *_data;
  public:
    EasyQueue(int maxSize = 256) {
      _head = 0;
      _tail = 0;
      _count = 0;
      _size = maxSize;
      _data = new T[_size + 1];
    }

    ~EasyQueue() {
      delete[] _data;  
    }

    /**
     * Returns the number of items in the queue.
     */
    inline uint8_t count() 
    {
      return _count;
    }

    /**
     * Returns whether the queue is empty or not.
     */
    inline bool empty() 
    {
      return _count <= 0;
    }

    /**
     * Put a new item at the end of the queue.
     */
    void push(const T &item)
    {
      // Only queue items when not full
      if(_count < _size) { 
        _data[_tail++]=item;
        ++_count;
        // Check wrap around
        if (_tail > _size)
          _tail -= (_size + 1);
      }
    }

    /**
     * remove and return the next item in the queue.
     */
    T pop() {
      if(empty()) return T(); // Returns empty

      T result = _data[_head];
      _head++;
      --_count;
      // Check wrap around
      if (_head > _size) 
        _head -= (_size + 1);
      return result; 
    }

    /**
     * check the next item without removing from the queue.
     */
    T peek() {
      if(_count <= 0)
        return T();
      return _data[_head];
    }

    /**
     * clear the queue
     */
    void clear() 
    {
      _head = _tail;
      _count = 0;
    }
};

#endif
