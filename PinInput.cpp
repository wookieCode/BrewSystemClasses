/*
 * PinInput.cpp - Implementation file for PinInput.h
 *
 *  Created on: Dec 8, 2013
 *      Author: Jeff H.
 */

#include "PinInput.h"

PinInput::PinInput(unsigned int newPin) :
	pin(newPin)
{
	// Create an out file stream
	std::ofstream fileGPIO("/sys/class/gpio/export");

	// Export the GPIO pin as long as the file opened correctly.
	if (fileGPIO) {
		fileGPIO << pin;
		fileGPIO.close();
	} else {
		throw std::ofstream::failure("Unable to export GPIO.");
	}

	// Open the direction file, tell it to be an input, and then close the file
	std::string setDirStr = "/sys/class/gpio/gpio" + std::to_string(pin) + "/direction";
	fileGPIO.open(setDirStr.c_str());

	if(fileGPIO) {
		fileGPIO << "in";
		fileGPIO.close();
	} else {
		throw std::ofstream::failure("Unable to set GPIO direction.");
	}

	// Setup the string used to read the value
	pinValueFileString = "/sys/class/gpio/gpio" + std::to_string(pin) + "/value";
}

PinInput::~PinInput() {
	// Create an out file stream and unexport the GPIO pin
	std::ofstream fileGPIO("/sys/class/gpio/unexport");

	if(fileGPIO) {
		fileGPIO << pin;
		fileGPIO.close();
	} else {
		throw std::ofstream::failure("Unable to unexport GPIO.");
	}
}

PinInput::PIN_VALUE PinInput::getValue() const {
	// Create an in file stream.
	std::ifstream valueFile(pinValueFileString.c_str());

	// Get the current value from the in file stream
	std::string value;
	if(valueFile) {
		valueFile >> value;
		valueFile.close();
	} else {
		throw std::ifstream::failure("Unable to read GPIO pin.");
	}

	// Determine the value and return it
	if(value != "0") {
		return HIGH;
	} else {
		return LOW;
	}
}

