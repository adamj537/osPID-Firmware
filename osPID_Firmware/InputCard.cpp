/******************************************************************************
 *
 *	Filename:		InputCard.cpp
 *
 *	Description:	The osPID Kit comes with swappable IO cards which are
 *					supported by different device drivers & libraries. For the
 *					osPID firmware to correctly communicate with your
 *					configuration, you must uncomment the appropriate "define"
 *					statements in InputCard.h.  Please take note that only one
 *					input card can be used at a time.
 *
 *					List of available input cards:
 *					------------------------------
 *					1.	TEMP_INPUT_V110 - Temperature Basic V1.10 with 1 
 *						thermistor & 1 type-K thermocouple (MAX6675) interface.
 *
 *					2.	TEMP_INPUT_V120 - Temperature Basic V1.20 with 1
 *						thermistor & 1 type-K thermocouple MAX31855KASA)
 *						interface.
 *
 *****************************************************************************/

#include <Arduino.h>
#include <stdint.h>
#include <EEPROM.h>
#include "InputCard.h"

//UNCOMMENT THE APPROPRIATE DEFINE STATEMENT FOR THE CARD BEING USED
//#define TEMP_INPUT_V110
#define TEMP_INPUT_V120

const uint8_t thermistorPin = A6;
const uint8_t thermocoupleCS = 10;
const uint8_t thermocoupleMISO = 12;
const uint8_t thermocoupleCLK = 13;

#ifdef TEMP_INPUT_V120
#include "MAX31855_local.h"
MAX31855 thermocouple(thermocoupleCLK, thermocoupleMISO, thermocoupleCS);
#endif

#ifdef TEMP_INPUT_V110
#include "MAX6675_local.h"
MAX6675 thermocouple(thermocoupleCLK, thermocoupleMISO, thermocoupleCS);
#endif

/******************************************************************************
 *
 *	Function:		Initializer
 *
 *	Description:	Sets up the input card with default values.  The default
 *					values for a thermistor were taken from thermistor.com's
 *					thermistor calculators, and are calculated for a S/F
 *					(-4.0%/degree C @ 25C) Mil Ratio M thermistor from 0C to 
 *					50C.  The default reference resistor (for the voltage
 *					divider) is assumed to be the same value as the
 *					thermistor's nominal value.
 *
 *					Development was done on a S2-15 thermistor from Dwyer
 *					Instruments, which I think matches the default values
 *					below.  See Dwyer part number 391-9700, which may be a
 *					Dwyer type "B" curve, and 391-9702, which may be a Dwyer
 *					type "A" curve.
 *
 *****************************************************************************/

InputCard::InputCard(void)
{
	inputType = 0;						//default input type is thermocouple
	thermRes = 10000;					//default thermistor is 10 kOhm @ 25 C
	thermRefTemp = 25;
	thermBeta = 3575;					//default thermistor beta is 3575
	refRes = 10000;						//default thermistor has ref resistor 10 Ohm
}

#if (defined(TEMP_INPUT_V110) || defined(TEMP_INPUT_V120))

/******************************************************************************
 *
 *	Function:		SetSensorType
 *
 *	Description:	Sets whether we use a thermocouple or thermistor.
 *
 *	Parameters:		uint8_t sensorType - 0 = thermocouple; 1 = thermistor
 *
 *	Return Value:	non-zero values indicate errors
 *
 *****************************************************************************/

int16_t InputCard::SetSensorType(uint8_t sensorType)
{
	int16_t error = 0;						// an optimistic return value :)
  
	if (sensorType > 1)						// Check for invalid sensor types.
	{
		error = sensorType;					// Set an error.
	}
	else
	{
		inputType = sensorType;				// Remember the input type.
	}
  
	return error;							// Return the status.
}

/******************************************************************************
*
* Function:	GetSensorType
*
* Description:	Indicates whether we're use a thermocouple or thermistor.
*
* Return Value:	0 = thermocouple; 1 = thermistor; other values indicate errors
*
******************************************************************************/

int InputCard::GetSensorType()
{
  return inputType;
}

/******************************************************************************
*
* Function:	SetThermistorCoeffs
*
* Description:	Sets the thermistor's calibration coefficients.
*
******************************************************************************/

void InputCard::SetThermistorCoeffs(double res, double temp, double beta, double divider)
{
  thermRes = res;		//thermistor's resistance at reference temperature
  thermRefTemp = temp;		//thermistor's reference temperature
  thermBeta = beta;		//thermistor's beta coefficient
  refRes = divider;		//value of resistor used for thermistor's voltage divider
}

double InputCard::GetThermistorRefRes()
{
  return thermRes;
}

double InputCard::GetThermistorRefTemp()
{
  return thermRefTemp;
}

double InputCard::GetThermistorBeta()
{
  return thermBeta;
}

double InputCard::GetThermistorDiv()
{
  return refRes;
}

void InputCard::SerialSend()
{
  Serial.print((int)inputType); 
  Serial.print(" "); 
  Serial.print(thermRes); 
  Serial.print(" ");  
  Serial.print(thermBeta); 
  Serial.print(" ");  
  Serial.print(thermRefTemp);   
  Serial.print(" ");  
  Serial.println(refRes);
}

void InputCard::SerialID()
{
#ifdef TEMP_INPUT_V110
  Serial.print(" IID1");
#else ifdefined TEMP_INPUT_V120
  Serial.print(" IID2");
#endif
}

double InputCard::ReadThermistorTemp(int voltage)
{
  float R;
  float steinhart;
  
  R = refRes / (1024.0/(float)voltage - 1);	// convert ADC value to voltage
  steinhart = R / thermRes;			// (R/Ro)
  steinhart = log(steinhart);			// ln(R/Ro)
  steinhart /= thermBeta;			// 1/B * ln(R/Ro)
  steinhart += 1.0 / (thermRefTemp + 273.15);	// + (1/To)
  steinhart = 1.0 / steinhart;			// Invert
  steinhart -= 273.15;				// convert to C

  return steinhart;
}

double InputCard::ReadFromCard()
{
  if(inputType == 0)
  {
    double val = thermocouple.ReadThermocouple(CELSIUS);
    if (val == FAULT_OPEN || val == FAULT_SHORT_GND || val == FAULT_SHORT_VCC)
    {
      val = NAN;
    }
    return val;
  }
  else if(inputType == 1)
  {
    return ReadThermistorTemp(analogRead(thermistorPin));
  }
}

#endif /*TEMP_INPUT_V110 || TEMP_INPUT_V120*/