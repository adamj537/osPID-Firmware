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

const uint8_t pinRelay1 = 6;				// relay on Digital Pin 6
const uint8_t pinRelay2 = 5;				// relay on Digital Pin 5

const char outputVersion[5] = "OID1";

byte outputRelay = 1;

#if defined(DIGITAL_OUTPUT_V120) || defined(DIGITAL_OUTPUT_V150)

/******************************************************************************
 *
 *	Function:		OutputCard (Class Initializer)
 *
 *	Description:	Sets up the output card with default values.
 *
 *****************************************************************************/
OutputCard::OutputCard(void)
{
	outputRelay = 0;					// Default to use relay 1.
	outWindowSec = 5.0;					// Set window size for 5 seconds.
	windowSize = 5000;					// Set window size for 5 seconds.
  
	pinMode(relay1, OUTPUT);			// Set relay pins as outputs.
	pinMode(relay2, OUTPUT);
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
		digitalWrite(relay1, state);
	}
	else if(relay == 1)
	{
		digitalWrite(relay2, state);
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
		*state = digitalRead(relay1, state);
	}
	else if (relay == 1)
	{
		*state = digitalRead(relay2, state);
	}
	else
	{
		result = OUTPUT_RESULT_INVALID;
	}

	return result;
}

void OutputCard::SetOutputWindow(double val)
{
	uint32_t mSec;						// holds val in milliseconds
  
	mSec = (uint32_t)(val * 1000);		// Convert from sec to milliseconds.
  
	if (mSec < 500)
	{
		mSec = 500;						// Minimum size is 500 milliseconds.
	}
  
	outWindowSec = (double)mSec / 1000;	// Convert to seconds.
	
	if(mSec != windowSize)				// Store the new value (if necessary).
	{
		windowSize = mSec;
	}
}

unsigned long OutputCard::GetOutputWindow()
{
	return windowSize;
}

void OutputCard::SetOutput(double value)
{
	unsigned long wind;	// window of time
	unsigned long oVal;	// output value (0 - 100%)

	wind  = millis() % windowSize;
	/*
	wind = (millis() - windowStartTime);
	if(wind > windowSize)
	{ 
		wind -= windowSize;
		windowStartTime += windowSize;
	}
	*/

	oVal = (unsigned long)(value*(double)windowSize / 100.0);

	if (outputRelay == 0)		//activate selected relay
	{
		digitalWrite(relay1 ,(oVal > wind) ? HIGH : LOW);
	}
	else if (outputRelay == 1)
	{
		digitalWrite(relay2 ,(oVal > wind) ? HIGH : LOW);
	}
}

#endif /* DIGITAL_OUTPUT_V120 & DIGITAL_OUTPUT_V150 */
