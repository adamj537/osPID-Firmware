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
#include "InputCard.h"

#ifdef TEMP_INPUT_V110
const char inputCardVersion[5] = "IID1";
#else ifdefined TEMP_INPUT_V120
const char inputCardVersion[5] = "IID2";
#endif

uint8_t thermistorPin = A6;
uint8_t thermocoupleCS = 10;
uint8_t thermocoupleMISO = 12;
uint8_t thermocoupleCLK = 13;

#ifdef TEMP_INPUT_V120
#include "MAX31855.h"
MAX31855 thermocouple(thermocoupleMISO, thermocoupleCS, thermocoupleCLK);
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
 *	Parameters:		pinThermistor - analog pin for thermistor
 *					pinCS - SPI chip select pin for thermocouple chip
 *					pinMISO - SPI data pin for thermocouple chip
 *					pinCLK - SPI clock pin for thermocouple chip
 *
 *****************************************************************************/

InputCard::InputCard(byte pinThermistor, byte pinCS, byte pinMISO, byte pinCLK)
{
	// Remember the pins we're using.
	thermistorPin = pinThermistor;
	thermocoupleCS = pinCS;
	thermocoupleMISO = pinMISO;
	thermocoupleCLK = pinCLK;
	
	// Set default coefficients.
	inputType = INPUT_SENSOR_THERMISTOR;// default input type is thermocouple
	thermRes = 10000;					// default thermistor is 10 kOhm @ 25 C
	thermRefTemp = 25;
	thermBeta = 3575;					// default thermistor beta is 3575
	refRes = 10000;						// default thermistor has ref resistor 10 Ohm
}

#if (defined(TEMP_INPUT_V110) || defined(TEMP_INPUT_V120))

/******************************************************************************
 *
 *	Function:		SetSensorType
 *
 *	Description:	Sets whether we use a thermocouple or thermistor.
 *
 *	Parameters:		sensorType - selected type of sensor
 *
 *	Return Value:	enumerated error code
 *
 *****************************************************************************/

inputResult_t InputCard::SetSensorType(inputSensor_t sensorType)
{
	// Start as a pessimist :(
	inputResult_t result = INPUT_RESULT_INVALID;
	
	// If the sensor is valid...
	if ((sensorType == INPUT_SENSOR_THERMOCOUPLE) ||
		(sensorType == INPUT_SENSOR_THERMISTOR))
	{
		// Remember the input type.
		inputType = sensorType;
		
		// If we got here, we're ok :)
		result = INPUT_RESULT_OK;
	}
	
	// Return the status.
	return result;
}

/******************************************************************************
*
*	Function:		GetSensorType
*
*	Description:	Indicates whether we're use a thermocouple or thermistor.
*
*	Return Value:	enumerated sensor type
*
******************************************************************************/

inputSensor_t InputCard::GetSensorType()
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
	  thermRefTemp = temp;	//thermistor's reference temperature
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

double InputCard::CalcSteinhart(float R)
{
	float steinhart;	// eventually this will be temperature [C]

	steinhart = R / thermRes;					// (R/Ro)
	steinhart = log(steinhart);					// ln(R/Ro)
	steinhart /= thermBeta;						// 1/B * ln(R/Ro)
	steinhart += 1.0 / (thermRefTemp + 273.15);	// + (1/To)
	steinhart = 1.0 / steinhart;				// Invert
	steinhart -= 273.15;						// convert to C

	return steinhart;
}

double InputCard::ReadFromCard()
{
	double temp = NAN;	// sensor temperature
	
	// If we're using a thermocouple...
	if(inputType == INPUT_SENSOR_THERMOCOUPLE)
	{
		// Read temperature from the thermocouple chip...
		temp = thermocouple.readThermocouple(CELSIUS);

		// If there was an error...
		if (temp == FAULT_OPEN || temp == FAULT_SHORT_GND || temp == FAULT_SHORT_VCC)
		{
			// Set an error value.
			temp = NAN;
		}
	}
	// If we're using a thermistor...
	else if(inputType == INPUT_SENSOR_THERMISTOR)
	{
		// Read the analog pin.
		int counts = analogRead(thermistorPin);

		// Convert to resistance.
		int R = refRes / (1024.0/(float)counts - 1);

		// Convert to temperature.
		temp = CalcSteinhart(R);
	}

	return temp;
}

#endif /*TEMP_INPUT_V110 || TEMP_INPUT_V120*/
