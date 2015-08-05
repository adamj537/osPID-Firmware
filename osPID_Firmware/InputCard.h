#ifndef INPUT_CARD_H
#define INPUT_CARD_H

#include <Arduino.h>
#include <stdint.h>

typedef enum							// status from functions
{
	INPUT_RESULT_OK,					// All is well!
	INPUT_RESULT_FAIL,					// It's the hardware's fault.
	INPUT_INVALID_SELECTION,			// It's your fault.
	INPUT_NOT_IMPLEMENTED,				// It's my fault.
} inputError_t;

typedef enum							// type of sensor used
{
	THERMOCOUPLE,
	THERMISTOR,
} inputSensor_t;

class InputCard
{
public:
	// Initialize the class.
	InputCard();
	
	// Set whether we use a thermocouple or thermistor.
	int16_t SetSensorType(inputSensor_t inputType);
	
	// Find if we're using a thermocouple (0) or thermistor (1).
	int16_t GetSensorType();
	
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
	
	// Ssend data from card to serial port.
	void SerialSend();
	
	// Send input card's hardware ID to serial port.
	void SerialID();
	
	// Read data from card.
	double ReadFromCard();
  
private:
	inputSensor_t inputType;			// type of sensor we're using
	double thermRes;					// thermistor's resistance at reference temperature
	double thermRefTemp;				// thermistor's reference temperature
	double thermBeta;					// thermistor's beta coefficient
	double refRes;						// value of resistor used for thermistor's voltage divider

	// Calculate the thermistor's temperature from a voltage.
	double ReadThermistorTemp(int voltage);
};

#endif
