/*
 * 	PinOutput.h - used to control a pin as an output.  It has a very simple user interface of On(), Off(), isOn(), and isOff().  It is
 * 			not meant to be used for rapid switching applications.
 *
 * 			Example usage: Pump control - PinOutput pump(PUMP_PIN);
 * 										- pump.On();
 * 										- pump.Off();
 *
 * 	Requires C++11 (-std=c++0x command line option)
 *
 *  Created on: Nov 30, 2013
 *      Author: Jeff H.
 */

#ifndef PINOUTPUT_H_
#define PINOUTPUT_H_

#include <fstream>
#include <string>

class PinOutput {
public:
	/*
	 * 	Both the constructor and destructor throw std::ofstream::failure if there are any errors.
	 */
	PinOutput(unsigned int newPin);
	virtual ~PinOutput();

	/*
	 *	On() - turns the pin to the on/high position.
	 *	@post - pin will be on/high.
	 *	@throws - std::ofstream::failure
	 */
	void On();

	/*
	 *	Off() - turns the pin to the off/low position.
	 *	@post - pin will be off/low.
	 *	@throws - std::ofstream::failure
	 */
	void Off();

	/*
	 *	isOn() - returns whether the pin is on or not.
	 *	@return - true if the pin is on, false if not.
	 */
	bool isOn() const;

	/*
	 *	isOff() - returns whether the pin is off or not.
	 *	@return - true if the pin is off, false if not.
	 */
	bool isOff() const;

private:
	/*
	 * 	pin - holds the pin that this class controls.
	 */
	unsigned int pin;

	/*
	 * 	onOffFileString - stores the file name used for turning the pin on and off.
	 */
	std::string onOffFileString;

	/*
	 * 	isHigh - stores true if the pin is high/on and false if it is low/off.
	 */
	bool isHigh;
};

#endif /* PINOUTPUT_H_ */
