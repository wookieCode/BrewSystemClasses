/*
 * PinInput.h - used to control a pin as an input.  It has a very simple user interface of getValue().
 *
 * 			Example usage:	- PinInput gpio(25);
 * 							- if(gpio.getValue()) {//Its on do stuff!};
 *
 * 	Requires C++11 (-std=c++0x command line option)
 *
 *  Created on: Dec 8, 2013
 *      Author: Jeff H.
 */

#ifndef PININPUT_H_
#define PININPUT_H_

#include <fstream>
#include <string>

class PinInput {
public:
	/*
	 * PIN_VALUE - an enum which set values for whether the pin is HIGH(on) or LOW(off).
	 */
	enum PIN_VALUE {LOW=0, HIGH=1};

	/*
	 * 	Both the constructor and destructor throw std::ofstream::failure if there are any errors.
	 */
	PinInput(unsigned int newPin);
	virtual ~PinInput();

	/*
	 * 	getValue() - returns the value of the pin.
	 * 	@return - the value of the pin.
	 * 	@throws - std::ifstream::failure
	 */
	PIN_VALUE getValue() const;

private:
	/*
	 * 	pin - holds the pin that this class controls.
	 */
	unsigned int pin;

	/*
	 * 	pinValueFileString - stores the file name used for reading the pin's value.
	 */
	std::string pinValueFileString;
};

#endif /* PININPUT_H_ */
