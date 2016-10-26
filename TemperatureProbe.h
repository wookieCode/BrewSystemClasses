/*
 * TemperatureProbe.h - Reads the Temperature from a RTD PT100 sensor using a MAX31865 RTD-to-Digital Converter chip through the
 * 				SPI interface.  Up to 2 different temperature sensors may be used using the built in Chip Select of the Raspberry
 * 				Pi.  These two Chip Select pins are defined in PinAssignments.h as SPI_CE0 & SPI_CE1.  SPI_CE0 is chosen as default
 * 				but can be changed using the setChipSelect() method.  The currently selected Chip Select can be found using the method
 * 				getChipSelect().  This value may also be chosen using the constructor.
 *
 * 				The unit in which the temperature is stored and returned is determined by the currentUnit variable which is of type
 * 				enum UNIT.  This enum defines all the units it is possible to store temperature in.  The currently used unit can be
 * 				set using setUnit() and retrieved using getUnit().
 *
 * 				TemperatureProbe throws std::runtime_error upon exceptions.
 *
 *  Created on: Nov 30, 2013
 *      Author: Jeff H.
 */

#ifndef TEMPERATUREPROBE_H_
#define TEMPERATUREPROBE_H_

#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <cmath>
#include <exception>
#include <stdexcept>
#include "PinAssignments.h"
#include "PinInput.h"

class TemperatureProbe {
public:
	/*
	 * 	Unit - an enum describing the units temperature can be stored in.
	 */
	enum UNIT {CELSIUS, FAHRENHEIT};

	/*
	 * FAULT_BITS - an enum defining each of the fault bits that can be set in the Fault Register
	 * 		of the MAX31865.  & against the fault register to determine if the bit is set.
	 */
	enum FAULT_BITS {RTD_HIGH_THRESHOLD=0x80,
					RTD_LOW_THRESHOLD=0x40,
					REFIN_GREATER_THAN=0x20,
					REFIN_LESS_THAN=0x10,
					RTDIN_LESS_THAN=0x08,
					OVER_UNDER_VOLTAGE=0x04};

	TemperatureProbe(unsigned int newChipSelect = SPI_CE0, UNIT newUnit = FAHRENHEIT);
	virtual ~TemperatureProbe();

	/*
	 *	getTemperature() - returns the current temperature.
	 *	@return - current temperature as a double.
	 *	@throws - std::runtime_error when a fault bit in the MAX31865 is set.
	 */
	double getTemperature();

	/*
	 *	setUnit() - changes how the temperature is stored and returned.
	 *	@post - the temperature will be stored and returned as degrees newUnit.
	 *	@params - Unit newUnit - the unit the temperature should be stored and returned as.
	 */
	void setUnit(const UNIT newUnit);

	/*
	 * 	getUnit() - returns the current unit used to store and return temperature.
	 * 	@return - the unit currently used to store and return the temperature.
	 */
	UNIT getUnit() const;

	/*
	 * 	setChipSelect() - changes the currently used chip select for SPI communication.
	 * 	@post - newChipSelect will be used for SPI communications.
	 * 	@params - newChipSelect - the new chip select pin that should be used for SPI communications. This
	 * 						value is defined in PinAssignments.h.
	 * 	@throws - std::runtime_error
	 */
	void setChipSelect(const unsigned int newChipSelect);

	/*
	 * 	getChipSelect() - returns the chip select currently being used.  This value is defined in PinAssignment.h.
	 * 	@return - the currently selected chip select.
	 */
	unsigned int getChipSelect() const;

	/*
	 * 	getFaultStatusRegister() - retrieves the fault status register of the MAX31865.  This can be &'d against the
	 * 			fault bit enum to determine which fault bits are set.
	 * 	@return - the fault status register of the MAX31865.
	 */
	unsigned char getFaultStatusRegister() const;

	/*
	 * 	clearFaultStatusRegister() - this can be used to clear the fault status register of the MAX31865.
	 * 	@post - the fault status register of the MAX31865 will be cleared.
	 */
	void clearFaultStatusRegister() const;

private:
	/*
	 *	temperature - stores the current temperature.  The units temperature can be stored as are defined in the enum UNIT.  The
	 *			current unit temperature is being stored and returned as is stored in currentUnit.
	 */
	double temperature;

	/*
	 * 	currentUnit - stores the unit that temperature should be stored and returned as.
	 */
	UNIT currentUnit;

	/*
	 * 	currentChipSelect - stores which chip select the SPI interface is currently using.
	 */
	unsigned int currentChipSelect;

	/*
	 *****	SPI INTERFACE DEFINITIONS	*****
	 *	The variables are set to default values
	 *	in the constructor.
	 *
	 *	Thanks to HalHerta for the tutorial where
	 *	I got most of this code.
	 */
	unsigned char mode;	// SPI Mode to use.  We use SPI_MODE_0.
	unsigned char bitsPerWord;	// bit with of data transmitted.  Default is 8.
	unsigned int speed;	// SPI Clock Freq.  We will use 1 MHz.
	int spifd;	// SPI file descriptor.

	/*
	 *	spiDRDY - input pin which goes low when the temperature conversion is ready to be read.
	 */
	PinInput spiDRDY;

	int spiOpen(std::string devspi);	// Opens an SPI device for comms. @throws - std::runtime_error
	int spiWriteRead( unsigned char *data, int length);	// writes data of length to the SPI device.  Recieved data is written back to data. @throws - std::runtime_error
	int spiClose();	// Closes the SPI device. @throws - std::runtime_error
};

#endif /* TEMPERATUREPROBE_H_ */
