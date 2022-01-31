#pragma once 

#ifndef debuglog_h
#define debuglog_h

/**
 * Debug logging that can be disabled, See config.h to disable.
 */
template<bool ENABLE_LOGGING>
class DebugLog
{
  private:
    const bool ENABLED = ENABLE_LOGGING;

  public:
    void log(const String &msg) {
      if (ENABLED) Serial.println(msg);
    }

    void log(const char *msg) {
      if (ENABLED) Serial.println(msg);
    }
    
    void log(int msg, int base) {
      if (ENABLED) Serial.println(msg, base);
    }
    
    void log(long msg, int base) {
      if (ENABLED) Serial.println(msg, base);
    }
    
    void log(double msg, int base) {
      if (ENABLED) Serial.println(msg, base);
    }
    
    void log(const Printable& msg) {
      if (ENABLED) Serial.println(msg);
    }
};


#ifdef ENABLE_DEBUG
static DebugLog<true> debug;
#else
static DebugLog<false> debug;
#endif

#define debugLog(msg) debug.log(F(msg));
#define debugLogInt(msg, base) debug.log(msg, base);


#endif
