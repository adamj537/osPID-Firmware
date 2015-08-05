/*******************************************************************************
* Title:	MAX31855 Library
*
* Source:	Rocket Scream Electronics (www.rocketscream.com)
*
* Description:	This library is based on Rocketscream's MAX31855 library, which
*	is licensed under Creative Commons Attribution-ShareAlike 3.0 Unported
*	License.  It interfaces with the MAX31855, a cold-junction compensated
*	interface for thermocouples (there are different versions of the chip
*	for different types of thermocouple).  The MAX31855 uses SPI to
*	communicate and gives a 14-bit temperature reading (accurate to 0.25
*	degree Celsius).
*
*******************************************************************************/

#include <Arduino.h>
#include "MAX31855_local.h"	//called "local" in case library is installed on IDE

/******************************************************************************
*
* Function:	Constructor
*
* Description:	Sets up the microcontroller's SPI pins to talk to the MAX31855.
*
* Parameters:
*	int8_t SCLK - SPI clock pin
*	int8_t MISO - SPI data pin (Master In Slave Out)
*	int8_t CS - SPI chip select pin
*
******************************************************************************/

MAX31855::MAX31855(uint8_t sclkPin, uint8_t misoPin, uint8_t csPin)
{
  sclk = sclkPin;			//store pin assignments to local variables
  cs = csPin;
  miso = misoPin;
  
  pinMode(cs, OUTPUT);			//define pin modes
  pinMode(sclk, OUTPUT);
  pinMode(miso, INPUT);

  digitalWrite(cs, HIGH);		//default output pins states
  digitalWrite(sclk, LOW);
}

/*******************************************************************************
* Name:		ReadThermocouple
*
* Description:	Read the thermocouple temperature either in Degree Celsius or
*	Fahrenheit. Internally, the conversion takes place in the background
*	within 100 ms. Values are updated only when the CS line is high.
*
* Parameters:
*	unit_t unit - CELSIUS or FAHRENHEIT
*
* Return Value:	double - temperature of the thermocouple either in Celsius or
*	Fahrenheit. If fault is detected, FAULT_OPEN, FAULT_SHORT_GND or
*	FAULT_SHORT_VCC will be returned. These fault values are outside
*	of the temperature range the MAX31855 is capable of.
*******************************************************************************/	
double	MAX31855::ReadThermocouple(unit_t unit)
{
  unsigned long data;
  double temperature = 0;	//holds data from the chip (initialized to 0)

  data = SpiRead();		//shift in 32-bit of data from chip

  if (data & 0x00010000)	//if fault is detected...
  {
    switch (data & 0x00000007)	//check for fault type (3 LSB)
    {
      case 0x01:		//open circuit
        temperature = FAULT_OPEN;
        break;
	
      case 0x02:		//thermocouple short to GND
        temperature = FAULT_SHORT_GND;
        break;
	
      case 0x04:		//thermocouple short to VCC
        temperature = FAULT_SHORT_VCC;
        break;
    }
  }
  else				//no fault detected...yay!
  {
    data = data >> 18;		//strip data to get the thermocouple temp
    temperature = (data & 0x00001FFF);//bit-14 is the sign

    if (data & 0x00002000)	//check for negative temperature
    {				//if negative, find 2's complement
      data = ~data; 		//invert
      data = data & 0x00001FFF;	//ensure operation involves lower 13-bit only
      data += 1;		//add 1 to obtain the positive number
      data *= -1;	 	//make temperature negative
    }
    
    temperature = data * 0.25;	//convert to degree Celsius
    
    if (unit == FAHRENHEIT)	//if Fahrenheit is desired...
    {				//...convert Celsius to Fahrenheit
      temperature = (temperature * 9.0/5.0)+ 32; 
    }
  }
  
  return (temperature);		//return the temperature
}

/*******************************************************************************
* Name:		ReadJunction
*
* Description:	Read the temperature of the chip's reference junction either in
*	Celsius or Fahrenheit. This is the value used by the chip to compensate
*	the thermocouple for the temperature of the controller itself.
*	Internally, the conversion takes place in the background within 100 ms.
*	Values are updated only when the CS line is high.
*
* Parameters:
*	unit_t unit - CELSIUS or FAHRENHEIT
*
* Return Value:	double - measured temperature of the cold junction either in
*	Degree Celsius or Fahrenheit. 
*
*******************************************************************************/
double	MAX31855::ReadJunction(unit_t unit)
{
  double temperature;			//temperature from chip
  unsigned long data;			//data variable

  data = SpiRead();			//read 32-bits of data from chip

  data = data >> 4;			//strip fault data bits & reserved bit
  temperature = (data & 0x000007FF);	//bit-12 is the sign

  if (data & 0x00000800)		//check for negative temperature
  {					//if negative, find 2's complement
    data = ~data; 			//invert
    data = data & 0x000007FF;		//ensure operation involves lower 11-bit only
    data += 1;				//add 1 to obtain the positive number
    data *= -1; 			//make temperature negative
  }
  
  temperature = data * 0.0625;		//convert to degree Celsius

  if (unit == FAHRENHEIT)		// If Fahrenheit is desired...
  {					// ...convert Celsius to Fahrenheit
    temperature = (temperature * 9.0/5.0)+ 32; 	
  }

  return (temperature);			//return the temperature
}

/*******************************************************************************
*
* Function:	SpiRead
*
* Description:	Shift in 32-bit of data from MAX31855 chip. Minimum clock pulse
*	width is 100 ns. No delay is required in this case.
*
* Parameters:	none
*
* Return Value:	unsigned long - 32-bit of data acquired from the chip
*				
*******************************************************************************/

unsigned long MAX31855::SpiRead()
{
  int i;			//counter variable
  unsigned long data = 0;	//data variable (initialized to 0)

  digitalWrite(cs, LOW);	//select the chip
	
  for (i = 31; i >= 0; i--)	//read 32 bits of data from the chip over SPI
  {
    digitalWrite(sclk, HIGH);	//put the SPI clock pin high

    if (digitalRead(miso))	//read the state of the SPI data pin
    {
      //If the data bit is high set the bit to 0.  We need to type cast data
      //type to unsigned long, else compiler will truncate to 16-bit
      data |= ((unsigned long)1 << i);
    }	

    digitalWrite(sclk, HIGH);	//put the SPI clock pin high
  }

  digitalWrite(cs, HIGH);	//deselect the chip

  return data;			//return the data read from the chip
}
