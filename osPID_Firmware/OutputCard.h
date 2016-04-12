#ifndef OUTPUT_CARD_H
#define OUTPUT_CARD_H

#include <Arduino.h>

// UNCOMMENT THE APPROPRIATE DEFINE STATEMENT FOR THE CARD BEING USED.
//#define DIGITAL_OUTPUT_V120
#define DIGITAL_OUTPUT_V150

typedef enum							// status from functions
{
	INPUT_RESULT_OK,					// All is well!
	INPUT_RESULT_FAIL,					// It's the hardware's fault.
	INPUT_RESULT_INVALID,				// It's your fault.
	INPUT_RESULT_NOT_IMPLEMENTED,		// It's my fault.
} outputResult_t;

class OutputCard
{
public:
	// Class initializer.
	OutputCard();
	int SelectRelay(byte relay);			// Select output 1 or output 2.
	int GetSelectedRelay();					// Fetch the selected output.
	void SetRelay(bool relay, bool state);	// Set the state of an output relay.
	int GetRelayState(bool relay);			// Fetch the state of a relay.
	void SetOutputWindow(double val);		// ???
	unsigned long GetOutputWindow();		// ???
	void WriteToCard(double value);			// Send output value to card.

private:
	uint8_t outputRelay;		// 0 = mechanical relay; 1 = solid state relay 
//	uint32_t windowStartTime;
	double outWindowSec;
	uint32_t windowSize;
};

#endif
