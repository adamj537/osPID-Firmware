#ifndef OUTPUT_CARD_H
#define OUTPUT_CARD_H

#include <Arduino.h>

// UNCOMMENT THE APPROPRIATE DEFINE STATEMENT FOR THE CARD BEING USED.
//#define DIGITAL_OUTPUT_V120
#define DIGITAL_OUTPUT_V150

class OutputCard
{
public:
	OutputCard();							// Class initializer.
	void InitializeCard();					// Set up the card - do once when the program starts.
	int SelectRelay(byte relay);			// Select output 1 or output 2.
	int GetSelectedRelay();					// Fetch the selected output.
	void SetRelay(bool relay, bool state);	// Set the state of an output relay.
	int GetRelayState(bool relay);			// Fetch the state of a relay.
	void SetOutputWindow(double val);		// ???
	unsigned long GetOutputWindow();		// ???
	void SerialReceiveDuring(uint8_t val, uint8_t index);	// ???
	void SerialSend();						// Send data from card to serial port.
	void SerialID();						// Send input card's hardware ID to serial port.
	void WriteToCard(double value);			// Send output value to card.

private:
	union	// This union lets us easily convert a byte array to a float array.
	{
		uint8_t asBytes[32];
		float asFloat[8];
	} serialXfer;

	uint8_t outputRelay;		// 0 = mechanical relay; 1 = solid state relay 
//	uint32_t windowStartTime;
	double outWindowSec;
	uint32_t windowSize;
};

#endif
