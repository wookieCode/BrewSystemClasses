/*
 * gpioPin.cpp - implementation file for gpioPin.h.
 *
 *  Created on: Jan 11, 2014
 *      Author: Jeff H.
 */

#include "gpioPin.h"

// Initialize default values for static members.
void *gpioPin::gpioMap = NULL;
volatile uint32_t *gpioPin::gpio = NULL;

gpioPin::gpioPin(uint32_t newPin, DIRECTION dir) :
		SET_OFFSET(7),
		CLR_OFFSET(10),
		LVL_OFFSET(13),
		pin(newPin),
		pinBitShift(1<<pin),
		currentDirection(dir)
{
	// See if gpioMap & gpio have been initialized.  If not, do it!
	if (gpioMap == NULL) {
		// Open /dev/mem
		int mem_fd;
		if ((mem_fd = open("/dev/mem",  O_RDWR|O_SYNC)) < 0) {
			throw std::runtime_error("Can't Open /dev/mem.");
		}

		// Map GPIO
		gpioMap = mmap(
				NULL,					// Any address in our space will do
				BLOCK_SIZE,				// Map Length
				PROT_READ|PROT_WRITE,	// Enable reading & writing to mapped memory
				MAP_SHARED,				// Shared with other processes
				mem_fd,					// File to map
				GPIO_BASE				// Offset to GPIO Peripheral
		);

		// Close mem_fd
		close(mem_fd);

		// Make sure the mapping succeeded.
		if (gpioMap == MAP_FAILED) {
			std::ostringstream stream;
			stream << "mmap error " << (int)gpioMap;
			throw std::runtime_error(stream.str());
		}

		// Set up our volatile pointer
		gpio = (volatile uint32_t *)gpioMap;
	}

	// Set the direction
	this->direction(currentDirection);
}

gpioPin::~gpioPin() {

}

void gpioPin::direction(DIRECTION dir) {
	// Always set IN before OUT.  So set in.
	currentDirection = IN;
	*(gpio+(pin/10)) &= ~(7<<((pin%10)*3));

	// See if we then need to set out
	if (dir == OUT) {
		currentDirection = OUT;
		*(gpio+(pin/10)) |= (1<<((pin%10)*3));
	}
}

gpioPin::DIRECTION gpioPin::getDirection() const {
	return currentDirection;
}

void gpioPin::On() {
	// Make sure direction is set to output.  If not, do nothing.
	if (currentDirection == OUT) {
		// Set the pin (turn it on)
		*(gpio + SET_OFFSET) = pinBitShift;
	}
}

void gpioPin::Off() {
	// Make sure direction is set to output.  If not, do nothing.
	if (currentDirection == OUT) {
		// Clear the pin (turn it off)
		*(gpio + CLR_OFFSET) = pinBitShift;
	}
}

gpioPin::VALUE gpioPin::Value() const {
	// variable to hold the return value
	VALUE pinValue;

	// Read the pins value and see if it is high or low
	if ( (*(gpio + LVL_OFFSET) & pinBitShift) != 0 ) {
		pinValue = HIGH;
	} else {
		pinValue = LOW;
	}

	return pinValue;
}

