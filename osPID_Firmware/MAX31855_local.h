#ifndef	MAX31855_H
#define MAX31855_H

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#define FAULT_OPEN      10000
#define FAULT_SHORT_GND 10001
#define FAULT_SHORT_VCC 10002

enum unit_t
{
  CELSIUS,
  FAHRENHEIT
};

class MAX31855
{
public:
  MAX31855(uint8_t sclkPin, uint8_t misoPin, uint8_t csPin);	//class initializer
  double ReadThermocouple(unit_t unit);				//reads thermocouple temperature
  double ReadJunction(unit_t unit);				//reads chip's junction temperature

private:
  int8_t sclk;						//SPI clock pin
  int8_t miso;						//SPI data pin
  int8_t cs;						//SPI chip select pin
  unsigned long SpiRead();				//reads data over SPI
};

#endif
