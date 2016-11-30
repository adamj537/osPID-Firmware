/******************************************************************************
 *
 *	Filename:		EEPROMAnything.h
 *
 *	Source:			playground.arduino.cc/Code/EEPROMWriteAnything
 *
 *	Description:	In the Arduino library versions through 0013, the only way
 *					to read and write the built-in EEPROM memory was through
 *					functions that only support one byte of data at a time.
 *					When saving a number from 0-255, this is sufficient. When
 *					saving a larger number, you have to call the EEPROM
 *					routines more than once, to save a "high byte" and a "low
 *					byte" for 16-bit numbers, or even more often for bigger
 *					numbers or other data types that cannot fit in one byte.
 *
 *					With the following code, you can write any data structure
 *					or variable, using any number of bytes of EEPROM to do it
 *					(of course, only as many bytes as your chip holds), all in
 *					a single call.
 *
 *					A common use of this would be to have a group of related
 *					"persistent" variables that you want to save for future
 *					sessions. This could be a high-score list in a game, or a
 *					set of configuration choices in a device like an alarm
 *					clock.
 *
 ******************************************************************************/

#ifndef EEPROM_ANYTHING_H
#define EEPROM_ANYTHING_H

#include <EEPROM.h>
#include <stdint.h>

// These are the EEPROM addresses where important stuff is stored.  Note that
// we need to leave enough room so that nothing overlaps, or else stuff will be
// overwritten.  Also note that our processor (Atmega328P) has 1024 bytes of
// EEPROM, so the addresses cannot exceed that.  Also, the address type is a
// uint16_t (since uint8_t can't exceed 255).

// EEPROM management variable
#define RESET_ADDR			0	// 1 byte

// Tuning parameters
#define DIR_ADDR			1	// 1 byte - char
#define KP_ADDR				2	// 4 bytes - double
#define KI_ADDR				6	// 4 bytes - double
#define KD_ADDR				10	// 4 bytes - double

// Dashboard settings
#define MODE_ADDR			14	// 1 byte - char
#define SP_ADDR				15	// 4 bytes - double
#define OUTPUT_ADDR			19	// 4 bytes - double

// Tuning variables
#define TUNE_STEP_ADDR		23	// 4 bytes - double
#define TUNE_NOISE_ADDR		27	// 4 bytes - double
#define TUNE_LOOKBACK_ADDR	31	// 4 bytes - double

// Profile variables (136 bytes)
#define PROF_NAME_ADDR		35	// string - 8 bytes (including NULL)
#define PROF_TYPES_ADDR		43	// 24 bytes of profile data
#define PROF_VALS_ADDR		67	// 85 bytes of profile data
#define PROF_TIMES_ADDR		152	// 20 bytes of profile data

// Input Card variables (# bytes depends on the card)
#define INPUT_TYPE_ADDR		172	// 1 byte - char
#define INPUT_REFRES_ADDR	173	// 4 bytes - double
#define INPUT_BETA_ADDR		176	// 4 bytes - double
#define INPUT_REFTEMP_ADDR	180	// 4 bytes - double
#define INPUT_DIV_ADDR		184	// 4 bytes - double

// Output Card variables (# bytes depends on the card)
#define OUTPUT_RELAY_ADDR	300	// 1 byte - char
#define OUTPUT_WINDOW_ADDR	301	// 4 bytes - unsigned long

// EEPROM address 0 holds a value used to automatically trigger an EEPROM reset
// If this value is not equal to a certain value (defined by EEPROM_RESET
// below), then this module will reload all variables from EEPROM.  This is
// usually done after a firmware update or reset.  Note that values stored in
// EEPROM are of type uint8_t.
#define EEPROM_RESET		0	// "Please reset me to default values."
#define EEPROM_NO_RESET		2	// "Don't reset me."

/******************************************************************************
 *
 *	Function:		EEPROM_writeAnything
 *
 *	Description:	Writes data to EEPROM.  Any data type is accepted.
 *
 *	Parameters:		int address - the EEPROM address where the first byte of
 *						data should go
 *					const T& data - a list of data to be stored in EEPROM
 *
 *	Return Value:	uint16_t - the number of bytes written to EEPROM
 *
 ******************************************************************************/

template <class T> int EEPROM_writeAnything(int address, const T& data)
{
	const byte* ptr = (const byte*)(const void*)&data;
	uint16_t i;

	// Cycle through all the data we wish to write.
	for (i = 0; i < sizeof(data); i++)
	{
		// If the data is different from what's there...
		if (EEPROM.read(address) != *ptr)
		{
			// Write the data to EEPROM.
			EEPROM.write(address, *ptr);
		}
		
		// Increment the address.
		address++;
		
		// Increment the pointer.
		*ptr++;
	}
	
	// Return the number of bytes written to EEPROM.
	return i;
}

/******************************************************************************
*
* Function:	EEPROM_readAnything
*
* Description:	Reads data from EEPROM.  Any data type is supported.
*
* Parameters:
*	int address - the EEPROM address where the first byte of data is
*	const T& data - pointer to a list of data to be read from EEPROM
*
* Return Value:	unsigned int - the number of bytes read from EEPROM
*
******************************************************************************/

template <class T> int EEPROM_readAnything(int address, T& data)
{
	byte* ptr = (byte*)(void*)&data;
	unsigned int i;

	// Cycle through all the data we wish to write.
	for (i = 0; i < sizeof(data); i++)
	{
		// Read the data.
		*ptr = EEPROM.read(address++);
		
		// Increment the pointer.
		*ptr++;
		
		// Increment the address.
		address++;
	}
	
	// Return the number of bytes read from EEPROM.
	return i;
}

#endif
