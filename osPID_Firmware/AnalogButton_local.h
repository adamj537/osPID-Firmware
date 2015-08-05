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
 
 #ifndef ANALOG_BUTTON_H
#define ANALOG_BUTTON_H

#include <stdint.h>

enum button_t  							// the names of each button
{
	BUTTON_NONE,
	BUTTON_BACK,	
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_OK
};

enum buttonState_t						// states for state machine
{
	BUTTON_STATE_SCAN,
	BUTTON_STATE_DEBOUNCE,	
	BUTTON_STATE_RELEASE
};

#define	BUTTON_NONE_THRESHOLD	1000	// maximum ADC value
#define	TOLERANCE				1.1		// % tolerance for ADC values
#define	DEBOUNCE_PERIOD			100		// how long to debounce

class AnalogButton
{
public:
	AnalogButton (uint8_t analogPin,	// class initializer
		int buttonValueReturn,
		int buttonValueUp,
		int buttonValueDown,
		int buttonValueOk);

	button_t Get(void);					// fetches a buttonpress

private:
	button_t Read(void);

	uint8_t buttonPin;					// analog pin used as button multiplexer
	int buttonValueThresholdReturn;		// upper bound ADC value for RETURN button
	int buttonValueThresholdUp;			// upper bound ADC value for UP button
	int buttonValueThresholdDown;		// upper bound ADC value for DOWN button
	int buttonValueThresholdOk;			// upper bound ADC value for OK button
};

#endif

