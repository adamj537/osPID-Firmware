#ifndef OUTPUT_CARD_H
#define OUTPUT_CARD_H

#include <Arduino.h>

// UNCOMMENT THE APPROPRIATE DEFINE STATEMENT FOR THE CARD BEING USED.
//#define DIGITAL_OUTPUT_V120
#define DIGITAL_OUTPUT_V150

typedef enum							// status from functions
{
	OUTPUT_RESULT_OK,					// All is well!
	OUTPUT_RESULT_FAIL,					// It's the hardware's fault.
	OUTPUT_RESULT_INVALID,				// It's your fault.
	OUTPUT_RESULT_NOT_IMPLEMENTED,		// It's my fault.
} outputResult_t;

class OutputCard
{
public:
	// Class initializer.
	OutputCard(byte relay1Pin, byte relay2pin);
	
	// Set the state of an output relay.
	outputResult_t SetRelayState(bool relay, bool state);
	
	// Fetch the state of a relay.
	outputResult_t GetRelayState(bool relay, bool *state);
	
	// Output stuff
	void SetOutputWindow(double val);		// Set the output period.
	unsigned long GetOutputWindow();		// Get the output period.
	void SetOutputRelay(bool relay);		// Set which relay is used.
	void SetOutput(double value);			// Set % of output period relay is on.

private:
//	uint32_t windowStartTime;
	uint32_t windowSize;					// output period [milliseconds]
};

#endif
