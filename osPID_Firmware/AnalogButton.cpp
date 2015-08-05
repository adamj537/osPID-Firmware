/******************************************************************************
 *
 *	Filename:		AnalogButton.cpp
 *
 *	Author:			Adam Johnson
 *
 *	Description:	Contains functions to access a keypad using a single
 *					analog pin.
 *
 *****************************************************************************/

#include <Arduino.h>
#include <stdint.h>
#include "AnalogButton_local.h"	//called "local" in case library is installed on IDE

/******************************************************************************
 *
 *	Function:		Initializer
 *
 *	Description:	Sets up an analog pin to sense buttons.
 *
 *	Parameters:		uint8_t analogPin - the pin used to sense button presses
 *					int buttonValueReturn - ADC value for "Return" button
 *					int buttonValueUp - ADC value for "Up" button
 *					int buttonValueDown - ADC value for "Down" button
 *					int buttonValueOk - ADC value for "Ok" button
 *
 * Return Value:	button_t - the button that was pressed
 *
 *****************************************************************************/

AnalogButton::AnalogButton(uint8_t analogPin, int buttonValueReturn, 
  int buttonValueUp, int buttonValueDown, int buttonValueOk)
{
  // Store analog pin used to multiplex push buttons.
  buttonPin = analogPin;

  // Add upper bound of tolerance for variation againts resistor values,
  // temperature, and other possible drift.
  buttonValueThresholdReturn = TOLERANCE * buttonValueReturn;
  buttonValueThresholdUp = TOLERANCE * buttonValueUp;
  buttonValueThresholdDown = TOLERANCE * buttonValueDown;
  buttonValueThresholdOk = TOLERANCE * buttonValueOk;
}

/******************************************************************************
 *
 *	Function:		Read
 *
 *	Description:	This function is used by the "Get" function below.  It
 *					reads an analog pin and decides which button is most likely
 *					pressed.  Does not debounce (which is what "Get" does).
 *
 *	Return Value:	button_t - the button most likely pressed
 *
 *****************************************************************************/

button_t AnalogButton::Read(void)
{
	int buttonValue;						// ADC value
	button_t result;						// the button most likely pressed

	// Read the ADC.
	buttonValue = analogRead(buttonPin);

	// Compare the ADC value to see what button it's closest to.
	if (buttonValue >= BUTTON_NONE_THRESHOLD)		{ result = BUTTON_NONE; }
	if (buttonValue <= buttonValueThresholdReturn)	{ result =  BUTTON_BACK; }
	if (buttonValue <= buttonValueThresholdUp)		{ result =  BUTTON_UP; }
	if (buttonValue <= buttonValueThresholdDown)	{ result =  BUTTON_DOWN; }
	if (buttonValue <= buttonValueThresholdOk)		{ result =  BUTTON_OK; }
	else											{ result = BUTTON_NONE; }
	
	return result;
}

/******************************************************************************
 *
 *	Function:		Get
 *
 *	Description:	This function must be called at regular intervals, either
 *					in a loop or with an interrupt. Fetches the currently
 *					pressed button and debounces.  Note that because these are
 *					analog buttons, we can't detect multiple button presses.
 *
 * Return Value:	button_t - the button being pressed
 *
 *****************************************************************************/

button_t AnalogButton::Get(void)
{
	static button_t pastKeys;			// flags for detected keypresses
	static buttonState_t buttonState;	// state machine variable
	static unsigned long debounceTimer;	// debouncing timer
	button_t currentKeys;				// button currently being pressed
	button_t buttonStatus;				// button flags (debounced)

	// Initialize button status.
	buttonStatus = BUTTON_NONE;

	// Retrieve current button value.
	currentKeys = Read();
	
	// This is a state machine.
	switch (buttonState)
	{
	case BUTTON_STATE_SCAN:
		// If a button is detected...
		if (currentKeys != BUTTON_NONE)
		{
			// Store current button press value.
			pastKeys = currentKeys;
			
			// Compute the timestamp when the button will be debounced.
			debounceTimer = millis() + DEBOUNCE_PERIOD;
			
			// Proceed to button debounce state.
			buttonState = BUTTON_STATE_DEBOUNCE;
		}
		break;

	case BUTTON_STATE_DEBOUNCE:
		// If the current keypress matches the past keypress...
		if (currentKeys == pastKeys)
		{
			// If debounce periodis complete...
			if (millis() >= debounceTimer)
			{
				// This is now considered a "real" keypress.  Store it!
				buttonStatus = pastKeys;
				
				// Proceed to wait for button to be released.  Otherwise we'll
				// accidentally send the same keypress over and over.
				buttonState = BUTTON_STATE_RELEASE;
			}
		}
		break;

	case BUTTON_STATE_RELEASE:
		// If the button is released...
		if (currentKeys == BUTTON_NONE)
		{
			// Reset the past keypress variable.
			pastKeys = BUTTON_NONE;
			
			// Reset the state machine.
			buttonState = BUTTON_STATE_SCAN;
		}
		break;
	}

	return (buttonStatus);
}

