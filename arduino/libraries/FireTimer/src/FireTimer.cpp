#include "FireTimer.h"




/*
 void FireTimer::begin(const ulong &timeout, const bool &micros)

 Description:
 ------------
  * Constructor for the FireTimer Class

 Inputs:
 -------
  * const uint8_t &timeout- Timeout in ms (or us if micros flag is set)
  * const bool &micros - Set timeout units to us

 Return:
 -------
  * void
*/
void FireTimer::begin(const ulong &timeout, const bool &micros)
{
	us = micros;
	update(timeout);
}




/*
 bool FireTimer::update(const ulong &timeout)

 Description:
 ------------
  * Update timer's timeout and reinitialize the timer

 Inputs:
 -------
  * const ulong &timeout - Number of ms/us to fire after

 Return:
 -------
  * void
*/
void FireTimer::update(const ulong &timeout)
{
	period = timeout;
	start();
}




/*
 void FireTimer::start()

 Description:
 ------------
  * Initializes/reinitializes timer

 Inputs:
 -------
  * void

 Return:
 -------
  * void
*/
void FireTimer::start()
{
	if (us)
		timeBench = micros();
	else
		timeBench = millis();
}




/*
 void FireTimer::reset()

 Description:
 ------------
  * Resets timer

 Inputs:
 -------
  * void

 Return:
 -------
  * void
*/
void FireTimer::reset()
{
	timeBench += period;
}




/*
 bool FireTimer::fire(const bool &_reset)

 Description:
 ------------
  * Determine if enough time has elapsed - handles
  millis() and micros() rollover

 Inputs:
 -------
  * const bool &_reset - Allows user to specify if
  the timer should reset if enough time has elapsed

 Return:
 -------
  * bool - Whether or not enough time has elapsed
*/
bool FireTimer::fire(const bool &_reset)
{
	ulong currentTime;

	// Update current time in user specified units
	if (us)
		currentTime = micros();
	else
		currentTime = millis();

	// Handle overflow
	if (currentTime < timeBench)
		timeDiff = (UL_MAX - timeBench) + currentTime;
	else
		timeDiff = currentTime - timeBench;

	// Determine if enough time has passed
	if (timeDiff >= period)
	{
		if (_reset)
			reset();
		return true;
	}

	return false;
}
