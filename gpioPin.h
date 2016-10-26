/*
 * 	gpioPin.h - Allows the use of GPIO pins on the raspberry pi.  This uses the BCM2835 GPIO numbering
 * 			scheme.
 *
 * 			@throws - std::runtime_error upon failure to initialize.
 *
 *  Created on: Jan 11, 2014
 *      Author: Jeff H.
 */

#ifndef GPIOPIN_H_
#define GPIOPIN_H_

#include <stdexcept>
#include <sstream>
#include <cstdint>
#include <cstdio>
#include <fcntl.h>
#include <sys/mman.h>

// Need for Direct Memory Access on the Raspberry PI
#define BCM2708_PERI_BASE	0x20000000
#define GPIO_BASE	(BCM2708_PERI_BASE + 0x200000)
#define PAGE_SIZE	(4*1024)
#define BLOCK_SIZE	(4*1024)

class gpioPin {
public:
	/*
	 *	VALUE - enum describing the value of the pin.
	 *
	 */
	enum VALUE {OFF=0, LOW=0, ON=1, HIGH=1};

	/*
	 *	DIRECTION - enum used to set and get the currently selected direction for
	 *			the pin.
	 */
	enum DIRECTION {IN=0, OUT=1};

	gpioPin(uint32_t newPin, DIRECTION dir=IN);
	virtual ~gpioPin();

	/*
	 * 	direction() - sets the current direction of the pin.
	 * 	@params - dir - the direction to set the pin.
	 * 	@post - the pin will be set to the correct direction (IN/OUT).
	 */
	void direction(DIRECTION dir);

	/*
	 * 	getDirection() - returns the current direction of the pin.
	 * 	@return - the current direction of the pin.
	 */
	DIRECTION getDirection() const;

	/*
	 * 	On() - Sets the pin to ON/HIGH as long as it is set as an output.  Has no
	 * 			effect if set as an input.
	 * 	@pre - pin should be set as an output.
	 * 	@post - the output pin will be set ON/HIGH.  Has no effect on input pins.
	 */
	void On();

	/*
	 * 	Off() - Sets the pin to OFF/LOW as long as it is set as an output.  Does
	 * 			nothing if the pin is set as an input.
	 * 	@pre - pin should be set as an output.
	 * 	@post - the output pin will be set OFF/LOW.  Has no effect on input pins.
	 */
	void Off();

	/*
	 * 	Value() - Returns the current value of the pin.
	 * 	@return - the current value of the pin.
	 */
	VALUE Value() const;

private:
	/*
	 * 	gpioMap - stores the mmap return for future direct access.  Initialized to null.
	 */
	static void *gpioMap;

	/*
	 * 	gpio - once mapped, provides direct pin I/O access.  Initialized to null.
	 */
	static volatile uint32_t *gpio;

	/*
	 *	SET_OFFSET - the memory offset from gpio needed for setting a pin(pin HIGH).
	 */
	const uint32_t SET_OFFSET;

	/*
	 *	CLR_OFFSET - the memory offset from gpio needed for clearing a pin(pin LOW).
	 */
	const uint32_t CLR_OFFSET;

	/*
	 * 	LVL_OFFSET - the memory offset from gpio needed for reading a pins level(HIGH/LOW).
	 */
	const uint32_t LVL_OFFSET;

	/*
	 *	pin - the pin which is currently being represented.
	 */
	uint32_t pin;

	/*
	 * 	pinBitShift - pre-bit shifted value for the current pin.
	 */
	uint32_t pinBitShift;

	/*
	 * 	currentDirection - stores the pins currently selected direction.
	 */
	DIRECTION currentDirection;
};

#endif /* GPIOPIN_H_ */
