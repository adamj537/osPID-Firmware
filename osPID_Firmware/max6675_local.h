#ifndef MAX6675_H
#define MAX6675_H

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#define FAULT_OPEN	NAN	//-100?
// FAULT_SHORT_GND is not supported by MAX6675
// FAULT_SHORT_VCC is not supported by MAX6675

enum unit_t
{
  CELSIUS,
  FAHRENHEIT
};

class MAX6675
{
public:
  MAX6675(uint8_t sclkPin, uint8_t misoPin, uint8_t csPin);	//class initializer
  double ReadThermocouple(unit_t unit);				//reads thermocouple temperature
  //readJunction is not supported by MAX6675
  
private:
  int8_t sclk;					//SPI clock pin
  int8_t miso;					//SPI data pin
  int8_t cs;					//SPI chip select pin
  uint8_t SpiRead(void);			//reads data over SPI
};

#endif
