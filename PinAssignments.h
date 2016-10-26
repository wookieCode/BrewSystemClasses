/*
 * PinAssignments.h - defines what is connected to which pin on the Raspberry Pi.
 *
 *  Created on: Nov 30, 2013
 *      Author: Jeff H.
 */

#ifndef PINASSIGNMENTS_H_
#define PINASSIGNMENTS_H_

/*	SPI PIN'S	*/
const static unsigned int SPI_CLK = 11;
const static unsigned int SPI_MOSI = 10;
const static unsigned int SPI_MISO = 9;
const static unsigned int SPI_CE0	= 8;
const static unsigned int SPI_CE1	= 7;
const static unsigned int DRDY_PIN = 25;

/*	HEATING ELEMENT PWM PIN	*/
const static unsigned int HE_PIN = 18;

/*	PUMP GPIO PIN	*/
const static unsigned int PUMP_PIN = 23;

#endif /* PINASSIGNMENTS_H_ */
