/*
 *	TemperatureProbe.cpp - implementation file for TemperatureProbe.h
 *
 *  Created on: Nov 30, 2013
 *      Author: Jeff H.
 */

#include "TemperatureProbe.h"

TemperatureProbe::TemperatureProbe(unsigned int newChipSelect, UNIT newUnit) :
	temperature(0), currentUnit(newUnit), currentChipSelect(newChipSelect), mode(SPI_MODE_1),
	bitsPerWord(8), speed(1000000), spifd(-1), spiDRDY(DRDY_PIN)
{
	// Using currentChipSelect, open the correct SPI device.
	if (currentChipSelect == SPI_CE0) {
		spiOpen(std::string("/dev/spidev0.0"));
	} else {
		spiOpen(std::string("/dev/spidev0.1"));
	}

	// Set-up the configuration register of the MAX31865, ready it for 1-shot conversion using 3-wire RTD, and
	// clear the fault register
	unsigned char initConfig[2] = {0x80, 0x92};
	this->spiWriteRead(initConfig, 2);

	// Delay to allow the RC network to settle 10ms
	usleep(10000);
}

TemperatureProbe::~TemperatureProbe() {
	spiClose();
}

double TemperatureProbe::getTemperature() {
	// Send 1 shot start: 10110000 = 0xB0
	unsigned char oneShotStart[2] = {0x80, 0xB0};
	this->spiWriteRead(oneShotStart, 2);

	// Wait for DRDY to go low
	while(spiDRDY.getValue()) {
		usleep(10000); // sleep for 10 ms
	}

	// Read RTD Registers: send 01, read a byte, read a byte
	unsigned char getTempData[3] = {0x01, 0x00, 0x00};
	this->spiWriteRead(getTempData, 3);

	// See if the RTD Registers fault bit is set.
	if (getTempData[2] & 0x01) {
		// Fault bit is set, throw exception!
		throw std::runtime_error("Fault bit set on temperature read.");
	}

	// Get the ADC code from the received data
	unsigned int adcCode = (getTempData[1] << 7) + (getTempData[2] >> 1);

	// Convert this to the RTD adc value
	double adcValue =(adcCode*385)/pow(2, 15);

	// Store needed values for accurate conversion
	double Z1 = -3.9083e-3;
	double Z2 = 17.58480889e-6;
	double Z3 = -23.10e-9;
	double Z4 = -1.155e-6;

	// Convert reading to temperature
	temperature = (Z1 + sqrt(Z2+(Z3*adcValue)))/Z4;

	// Check and see if it needs to be converted to degrees F
	if (currentUnit == FAHRENHEIT) {
		temperature = (temperature*1.8) + 32.0;
	}

	return temperature;
}

void TemperatureProbe::setUnit(const UNIT newUnit) {
	currentUnit = newUnit;
}

TemperatureProbe::UNIT TemperatureProbe::getUnit() const {
	return currentUnit;
}

void TemperatureProbe::setChipSelect(const unsigned int newChipSelect) {
	// If new chip select is the same as current chip select, do nothing. Otherwise
	// close the current SPI dev and open a new one.
	if(currentChipSelect != newChipSelect) {
		currentChipSelect = newChipSelect;

		spiClose();

		if(currentChipSelect == SPI_CE0) {
			spiOpen(std::string("/dev/spidev0.0"));
		} else {
			spiOpen(std::string("/dev/spidev0.1"));
		}
	}
}

unsigned int TemperatureProbe::getChipSelect() const {
	return currentChipSelect;
}

unsigned char TemperatureProbe::getFaultStatusRegister() const {
	// Read the fault register and return it's value
	unsigned char faultRead[2] = {0x07, 0x00};
	this->spiWriteRead(faultRead, 2);

	return faultRead[1];
}

void TemperatureProbe::clearFaultStatusRegister() const {
	// Clear the Fault Register while maintaining our current settings.
	unsigned char initConfig[2] = {0x80, 0x92};
	this->spiWriteRead(initConfig, 2);
}

int TemperatureProbe::spiOpen(std::string devspi) {
	int statusVal = -1;

	spifd = open(devspi.c_str(), O_RDWR);
	if(spifd < 0){
		throw std::runtime_error("could not open SPI device");
	}

	statusVal = ioctl(spifd, SPI_IOC_WR_MODE, &(mode));
	if(statusVal < 0){
		throw std::runtime_error("Could not set SPIMode (WR)...ioctl fail");
	}

	statusVal = ioctl(spifd, SPI_IOC_RD_MODE, &(mode));
	if(statusVal < 0) {
		throw std::runtime_error("Could not set SPIMode (RD)...ioctl fail");
	}

	statusVal = ioctl(spifd, SPI_IOC_WR_BITS_PER_WORD, &(bitsPerWord));
	if(statusVal < 0) {
		throw std::runtime_error("Could not set SPI bitsPerWord (WR)...ioctl fail");
	}

	statusVal = ioctl(spifd, SPI_IOC_RD_BITS_PER_WORD, &(bitsPerWord));
	if(statusVal < 0) {
		throw std::runtime_error("Could not set SPI bitsPerWord(RD)...ioctl fail");
	}

	statusVal = ioctl(spifd, SPI_IOC_WR_MAX_SPEED_HZ, &(speed));
	if(statusVal < 0) {
		throw std::runtime_error("Could not set SPI speed (WR)...ioctl fail");
	}

	statusVal = ioctl(spifd, SPI_IOC_RD_MAX_SPEED_HZ, &(speed));
	if(statusVal < 0) {
		throw std::runtime_error("Could not set SPI speed (RD)...ioctl fail");
	}

	return statusVal;
}

int TemperatureProbe::spiWriteRead( unsigned char *data, int length) {
	struct spi_ioc_transfer spi[length];
	int i = 0;
	int retVal = -1;

	// one spi transfer for each byte
	for (i = 0 ; i < length ; i++){
		spi[i].tx_buf        = (unsigned long)(data + i); // transmit from "data"
		spi[i].rx_buf        = (unsigned long)(data + i) ; // receive into "data"
		spi[i].len           = sizeof(*(data + i)) ;
		spi[i].delay_usecs   = 0 ;
		spi[i].speed_hz      = speed ;
		spi[i].bits_per_word = bitsPerWord ;
		spi[i].cs_change = 0;
	}

	retVal = ioctl (spifd, SPI_IOC_MESSAGE(length), &spi) ;

	if(retVal < 0){
		throw std::runtime_error("Problem transmitting spi data..ioctl");
	}

	return retVal;
}

int TemperatureProbe::spiClose() {
	int statusVal = -1;
	statusVal = close(spifd);

	if(statusVal < 0) {
		throw std::runtime_error("Could not close SPI device");
	}

	return statusVal;
}
