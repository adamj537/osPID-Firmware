/******************************************************************************
 *
 *	Project Name:	osPID
 *	Designer:	Adam Johnson
 *	Language:	C++
 *	Environment:	Arduino
 *
 *	Description:	The osPID is a PID controller.  This is the main file.
 *
 *****************************************************************************/

// Libraries
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "AnalogButton_local.h"
#include "EEPROMAnything.h"
#include "InputCard.h"
//#include "OutputCard.h"
#include "PID_v1_local.h"
#include "PID_AutoTune_v0_local.h"

#define PROJECT			" osPID"		// project name
#define FVN				" alpha"		// firmware version

// Pins
const byte pinBuzzer	= 3;			// buzzer pin
const byte pinLCDd4		= 4;			// LCD data 4
const byte pinRelay2	= 5;			// second output relay
const byte pinRelay1	= 6;			// first output relay
const byte pinLCDd5		= 7;			// LCD data 5
const byte pinLCDd6		= 8;			// LCD data 6
const byte pinLCDd7		= 9;			// LCD data 7
const byte pinCS		= 10;			// SPI CS pin for thermocouple chip
const byte pinMISO		= 12;			// SPI data pin for thermocouple chip
const byte pinCLK		= 13;			// SPI clock pin for thermocouple chip
const byte pinLCDen		= A0;			// LCD enable pin
const byte pinLCDrs		= A1;			// LCD RS pin
const byte pinSystemLED = A2;			// LED pin
const byte pinKeys		= A3;			// analog buttons pin
const byte pinTherm		= A6;			// thermistor pin

// Button hardware settings
const byte key0Level = 0;				// ADC level for button 0
const byte key1Level = 253;				// ADC level for button 1
const byte key2Level = 454;				// ADC level for button 2
const byte key3Level = 657;				// ADC level for button 3

// Other settings
const int baudRate = 9600;				// USB serial port baud rate
const byte lcdRows = 2;					// LCD's number of lines
const byte lcdColumns = 8;				// LCD's number of characters per line

// Objects
LiquidCrystal lcd(pinLCDrs, pinLCDen, pinLCDd4, pinLCDd5, pinLCDd6, pinLCDd7);
AnalogButton button(pinKeys, key0Level, key1Level, key2Level, key3Level);
InputCard myInput(pinTherm, pinCS, pinMISO, pinCLK);
//OutputCard myOutput(pinRelay1, pinRelay2);

/******************************************************************************
 *
 *	Function:		setup
 *
 *	Description:	This function runs once when the osPID starts up.  It
 *					initializes the peripheral devices on the microcontroller.
 *
 *****************************************************************************/

void setup(void)
{
	// Initialize UART.
	Serial.begin(baudRate);

	// Initialize LCD (8 chars wide, 2 chars tall).
	lcd.begin(lcdColumns, lcdRows);

	// Display firmware version.
	lcd.setCursor(0,0);
	lcd.print(F(PROJECT));
	lcd.setCursor(0,1);
	lcd.print(F(FVN));
	delay(1000);
	
	// Set up PID library.
//	myPID.SetSampleTime(1000);
//	myPID.SetOutputLimits(0, 100);
//	myPID.SetTunings(kp, ki, kd);
//	myPID.SetControllerDirection(ctrlDirection);
//	myPID.SetMode(modeIndex);
}

/******************************************************************************
*
* Function:	loop
*
* Description:	This function runs over and over after the setup function
*		finishes.
*
******************************************************************************/

void loop()
{
	double temperature;
	
	// Test the input card.
	temperature = myInput.ReadFromCard();

	// Report the temperature to the user.
	Serial.print(temperature);
	Serial.println(" C");

	// Display the temperature.
	lcd.setCursor(0,1);
	lcd.print(temperature);

	// Wait a second.
	delay(1000);
}

