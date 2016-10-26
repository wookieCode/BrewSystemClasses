/*
 * PinOutput.cpp
 *
 *  Created on: Nov 30, 2013
 *      Author: ibanus
 */

#include "PinOutput.h"

PinOutput::PinOutput(unsigned int newPin) :
	pin(newPin), isHigh(false)
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

	// Open the direction file, tell it to be an output, and then close the file
	std::string setDirStr = "/sys/class/gpio/gpio" + std::to_string(pin) + "/direction";
	fileGPIO.open(setDirStr.c_str());

	if(fileGPIO) {
		fileGPIO << "out";
		fileGPIO.close();
	} else {
		throw std::ofstream::failure("Unable to set GPIO direction.");
	}

	// Construct the file name used for turning the pin on & off.
	onOffFileString = "/sys/class/gpio/gpio" + std::to_string(pin) + "/value";

	// Make sure the pin is currently off
	this->Off();
}

PinOutput::~PinOutput() {
	// Create an out file stream and unexport the GPIO pin
	std::ofstream fileGPIO("/sys/class/gpio/unexport");

	if(fileGPIO) {
		fileGPIO << pin;
		fileGPIO.close();
	} else {
		throw std::ofstream::failure("Unable to unexport GPIO.");
	}
}

void PinOutput::On() {
	// Create an out file stream, open the value file, and tell it to turn the pin on.
	std::ofstream onFile(onOffFileString.c_str());

	if(onFile) {
		onFile << "1";
		onFile.close();

		// Update isHigh
		isHigh = true;
	} else {
		throw std::ofstream::failure("Unable to turn on GPIO pin.");
	}
}

void PinOutput::Off() {
	// Create an out file stream, open the value file, and tell it to turn the pin off.
	std::ofstream offFile(onOffFileString.c_str());

	if(offFile) {
		offFile << "0";
		offFile.close();

		// Update isHigh
		isHigh = false;
	} else {
		throw std::ofstream::failure("Unable to turn off GPIO pin.");
	}
}

bool PinOutput::isOn() const {
	return isHigh;
}

bool PinOutput::isOff() const {
	return !isHigh;
}

