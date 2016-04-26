#ifndef INPUT_CARD_H
#define INPUT_CARD_H

#include <Arduino.h>
#include <stdint.h>

// UNCOMMENT THE APPROPRIATE DEFINE STATEMENT FOR THE CARD BEING USED.
//#define TEMP_INPUT_V110
#define TEMP_INPUT_V120

typedef enum							// status from functions
{
	INPUT_RESULT_OK,					// All is well!
	INPUT_RESULT_FAIL,					// It's the hardware's fault.
	INPUT_RESULT_INVALID,				// It's your fault.
	INPUT_RESULT_NOT_IMPLEMENTED,		// It's my fault.
} inputResult_t;

typedef enum							// type of sensor used
{
	INPUT_SENSOR_THERMOCOUPLE = 0,		// thermocouple
	INPUT_SENSOR_THERMISTOR,			// thermistor
} inputSensor_t;

class InputCard
{
public:
	// Initialize the class.
	InputCard(byte pinThermistor, byte pinCS, byte pinMISO, byte pinCLK);
	
	// Set whether we use a thermocouple or thermistor.
	inputResult_t SetSensorType(inputSensor_t inputType);
	
	// Find if we're using a thermocouple or thermistor.
	inputSensor_t GetSensorType();
	
	// Set coefficients for thermistor.
	void SetThermistorCoeffs(double res, double temp, double beta, double divider);
	
	// Fetch the thermistor's resistance at reference temperature.
	double GetThermistorRefRes();
	
	// Fetch the thermistor's reference temperature.
	double GetThermistorRefTemp();
	
	// Fetch the thermistor's beta coefficient.
	double GetThermistorBeta();
	
	// Fetch the value of resistor used for thermistor's voltage divider.
	double GetThermistorDiv();
	
	// Read data from card.
	double ReadFromCard();
  
private:
	inputSensor_t inputType;			// type of sensor we're using
	double thermRes;					// thermistor's resistance at reference temperature
	double thermRefTemp;				// thermistor's reference temperature
	double thermBeta;					// thermistor's beta coefficient
	double refRes;						// value of resistor used for thermistor's voltage divider

	// Calculate the thermistor's temperature from a resistance.
	double CalcSteinhart(float R);
};

#endif
