# FireTimer
[![GitHub version](https://badge.fury.io/gh/PowerBroker2%2FFireTimer.svg)](https://badge.fury.io/gh/PowerBroker2%2FFireTimer) [![arduino-library-badge](https://www.ardu-badge.com/badge/FireTimer.svg?)](https://www.ardu-badge.com/FireTimer)<br /><br />
Simple and non-blocking Arduino library used to "fire-off" processes at specific intervals

# Example
```C++
#include "FireTimer.h"

FireTimer msTimer;
FireTimer usTimer;

void setup()
{
  Serial.begin(115200);

  msTimer.begin(1000);
  usTimer.begin(1000000, MICRO_SECONDS);
}

void loop()
{
  if(msTimer.fire())
    Serial.println("ms");

  if(usTimer.fire())
    Serial.println("us");
}
```
