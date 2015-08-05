/******************************************************************************
* Title:	MAX6675
*
* Source:	www.ladyada.net/learn/sensors/thermocouple
*
* Description:	This library is public domain (thanks LadyAda!).  It interfaces
*	with the MAX6675, a cold-junction-compensated interface for a K-type
*	thermocouple.  The MAX6675 uses SPI to communicate and gives a 12-bit
*	temperature reading (accurate to 0.25 degrees Celcius) up to 1024C.
*
******************************************************************************/

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>
#include "max6675_local.h"	//called "local" in case library is installed on IDE

/******************************************************************************
*
* Function:	Constructor
*
* Description:	Sets up the microcontroller's SPI pins to talk to the MAX6675.
*
* Parameters:
*	int8_t SCLK - SPI clock pin
*	int8_t MISO - SPI data pin (Master In Slave Out)
*	int8_t CS - SPI chip select pin
*
******************************************************************************/

MAX6675::MAX6675(uint8_t sclkPin, uint8_t misoPin, uint8_t csPin)
{
  sclk = sclkPin;		//store pin assignments to local variables
  cs = csPin;
  miso = misoPin;

  pinMode(cs, OUTPUT);		//define pin modes
  pinMode(sclk, OUTPUT); 
  pinMode(miso, INPUT);

  digitalWrite(cs, HIGH);	//initialize CS high
  //digitalWrite(sclk, LOW);	//should I add this?
}

/******************************************************************************
*
* Function:	ReadThermocouple
*
* Description:	Reads a temperature (either in Celcius or Fahrenheit) from the
*	MAX6675.  The data format is:
*
*	Bit:  S,T11,T10,T9,T8,T7,T6,T5,T4,T3,T2,T1,T0,D2,D1,D0
*
*	Bit S is a dummy sign bit and is always zero.
*	Bit T11-T0 is the temperature (T11 is the MSB; T0 is the LSB)
*	Bit D2 is normally low.  It goes high if a thermocouple is open.
*	Bit D1 is always zero (it's supposed to be a chip ID)
*	Bit D0 is always tri-stated.
*
* Parameters:
*	uint_t unit - CELSIUS or FAHRENHEIT
*
* Return Value:	double - measured temperature (in Celcius or Fahrenheit).  If
*	a fault is detected, FAULT_OPEN will be returned.  The fault value is
*	outside of the temperature range the MAX6675 is capable of.
*
******************************************************************************/

double MAX6675::ReadThermocouple(unit_t unit)
{
  uint16_t data;		//holds data from the chip
  double temperature = 0;	//holds temperature (initialized to 0)

  digitalWrite(cs, LOW);	//select the chip
  _delay_ms(1);			//wait for the chip to notice it was selected

  data = SpiRead();		//read 16 bits of data
  data <<= 8;
  data |= SpiRead();

  digitalWrite(cs, HIGH);	//deselect the chip

  if (data & 0x4) {		//check bit D2
    temperature = FAULT_OPEN;	// uh oh, no thermocouple attached!
  }
  else
  {
    data >>= 3;			//right-shift by 3 bits to get temperature
    temperature = data * 0.25;	//convert to degree Celsius
    
    if (unit == FAHRENHEIT)	//if Fahrenheit is desired...
    {				//...convert Celsius to Fahrenheit
    temperature = (temperature * 9.0/5.0) + 32;
    }
  }
  
  return temperature;		//return the temperature
}

/******************************************************************************
*
* Function:	SpiRead
*
* Description:	Reads data from an integrated circuit over SPI.
*
* Parameters:	none
*
* Return Value:	byte - data from the chip
*
******************************************************************************/

byte MAX6675::SpiRead(void)
{
  int i;			//counter variable
  byte data = 0;		//data variable (initialized to 0)

  for (i=7; i>=0; i--)		//read eight bits from the chip over SPI
  {
    digitalWrite(sclk, LOW);	//put the SPI clock pin low
    _delay_ms(1);		//wait for the chip to notice the low pin
    
    if (digitalRead(miso))	//read the state of the SPI data pin
    {				//if the data pin is high...
      data |= (1 << i);		//...set the bit to 0
    }

    digitalWrite(sclk, HIGH);	//put the SPI clock pin high
    _delay_ms(1);		//wait for the chip to notice the high pin
  }

  return data;			//return the byte read from the chip
}
