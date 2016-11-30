/******************************************************************************
 *
 *	Filename:		OutputCard
 *
 *	Description:	The osPID Kit comes with swappable IO cards which are
 *					supported by different device drivers & libraries. For the
 *					osPID firmware to correctly communicate with your
 *					configuration, you must uncomment the appropriate "define"
 *					statements in OutputCard.h.  Please take note that only one
 *					output card can be used at a time.
 *
 *					List of available output cards:
 *					-------------------------------
 *					1.	DIGITAL_OUTPUT_V120: 
 *						Output card with 1 SSR & 2 relay output.
 *					2.	DIGITAL_OUTPUT_V150: 
 *						Output card with 1 SSR & 2 relay output. Similar to
 *						V1.20 except LED mount orientation.
 *
 *****************************************************************************/

#include <Arduino.h>
#include <stdint.h>
#include "OutputCard.h"

byte pinRelay1;				// pin attached to relay 1
byte pinRelay2;				// pin attached to relay 2
byte outputRelay;			// relay used for output functions

const char outputVersion[5] = "OID1";

#if defined(DIGITAL_OUTPUT_V120) || defined(DIGITAL_OUTPUT_V150)

/******************************************************************************
 *
 *	Function:		OutputCard (Class Initializer)
 *
 *	Description:	Sets up the output card with default values.
 *
 *****************************************************************************/
OutputCard::OutputCard(byte relay1Pin, byte relay2pin)
{
	outputRelay = 0;					// Default to use relay 2.
	windowSize = 10000;					// Set window size for 10 seconds.
	
	pinRelay1 = relay1Pin;				// Remember the relay pins.
	pinRelay2 = relay2pin;
	
	pinMode(pinRelay1, OUTPUT);			// Set relay pins as outputs.
	pinMode(pinRelay2, OUTPUT);
}

/******************************************************************************
 *
 *	Function:		SetRelayState
 *
 *	Description:	Turn a relay on or off.
 *
 *	Parameters:		relay - which relay to change
 *					state - desired state of relay (true = on; false = off)
 *
 *****************************************************************************/
outputResult_t OutputCard::SetRelayState(bool relay, bool state)
{
	outputResult_t result = OUTPUT_RESULT_OK;
	
	if (relay == 0)
	{
		digitalWrite(pinRelay1, state);
	}
	else if (relay == 1)
	{
		digitalWrite(pinRelay2, state);
	}
	else
	{
		result = OUTPUT_RESULT_INVALID;
	}

	return result;
}

/******************************************************************************
 *
 *	Function:		SetRelayState
 *
 *	Description:	Turn a relay on or off.
 *
 *	Parameters:		relay - which relay to change
 *					state - desired state of relay (true = on; false = off)
 *
 *****************************************************************************/
outputResult_t  GetRelayState(bool relay, bool *state)
{
	outputResult_t result = OUTPUT_RESULT_OK;
	
	if (relay == 0)
	{
		*state = digitalRead(pinRelay1);
	}
	else if (relay == 1)
	{
		*state = digitalRead(pinRelay2);
	}
	else
	{
		result = OUTPUT_RESULT_INVALID;
	}

	return result;
}

/******************************************************************************
 *
 *	Function:		SetOutputWindow
 *
 *	Description:	Sets the output period time.
 *
 *	Parameters:		seconds - time of a single output period [seconds]
 *
 *****************************************************************************/
void OutputCard::SetOutputWindow(double seconds)
{
	uint32_t mSec;						// holds val in milliseconds
  
	mSec = (uint32_t)(seconds * 1000);	// Convert from sec to milliseconds.
  
	if (mSec < 500)
	{
		mSec = 500;						// Minimum size is 500 milliseconds.
	}
	
	if (mSec != windowSize)				// Store the new value (if necessary).
	{
		windowSize = mSec;
	}
}

unsigned long OutputCard::GetOutputWindow()
{
	return windowSize;
}

/******************************************************************************
 *
 *	Function:		SetOutputRelay
 *
 *	Description:	Set which relay is used by the output functions.
 *
 *	Parameters:		relay - which relay is used
 *
 *****************************************************************************/
void SetOutputRelay(bool relay)
{
	outputRelay = relay;
}

void OutputCard::SetOutput(double value)
{
	unsigned long wind;	// time relay is on + time relay is off [milliseconds]
	unsigned long oVal;	// time relay is on [milliseconds]

	wind  = millis() % windowSize;
/*
	wind = (millis() - windowStartTime);
	if (wind > windowSize)
	{ 
		wind -= windowSize;
		windowStartTime += windowSize;
	}
*/
	oVal = (unsigned long)(value * (double)windowSize / 100.0);

	if (outputRelay == 0)		//activate selected relay
	{
		digitalWrite(pinRelay1 ,(oVal > wind) ? HIGH : LOW);
	}
	else if (outputRelay == 1)
	{
		digitalWrite(pinRelay2 ,(oVal > wind) ? HIGH : LOW);
	}
}

#endif /* DIGITAL_OUTPUT_V120 & DIGITAL_OUTPUT_V150 */
