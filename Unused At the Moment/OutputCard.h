#ifndef OUTPUT_CARD_H
#define OUTPUT_CARD_H

#include <Arduino.h>

// UNCOMMENT THE APPROPRIATE DEFINE STATEMENT FOR THE CARD BEING USED.
//#define DIGITAL_OUTPUT_V120
#define DIGITAL_OUTPUT_V150

class OutputCard
{
public:
	typedef enum							// status from functions
	{
		OUTPUT_RESULT_OK,					// All is well!
		OUTPUT_RESULT_FAIL,					// It's the hardware's fault.
		OUTPUT_RESULT_INVALID,				// It's your fault.
		OUTPUT_RESULT_NOT_IMPLEMENTED,		// It's my fault.
	} outputResult_t;

	// Class initializer.
	OutputCard();
	
	// Set the state of an output relay.
	outputResult_t SetRelayState(bool relay, bool state);
	
	// Fetch the state of a relay.
	outputResult_t GetRelayState(bool relay, bool *state);
	
	// Output stuff
	void SetOutputWindow(double val);		// ???
	unsigned long GetOutputWindow();		// ???
	void SetOutput(double value);			// Send output value to card.

private:
//	uint32_t windowStartTime;
	double outWindowSec;
	uint32_t windowSize;
};

#endif
