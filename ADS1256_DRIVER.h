#pragma once
#ifndef ADS1256_DRIVER_H
#define ADS1256_DRIVER_H

/* BCM2835.h Provided by AirSpayce
* https://www.airspayce.com/mikem/bcm2835/
*/
#include <bcm2835.h>
#include <chrono>
#include <thread>
#include "BCM2835_DRIVER.h"

/**
* Register map as defined in the datasheet
* https://www.ti.com/lit/ds/symlink/ads1256.pdf (Page 30)
*/
typedef enum {
	REG_STATUS	= 0x00,
	REG_MUX		= 0x01,
	REG_ADCON	= 0x02,
	REG_DRATE	= 0x03,
	REG_IO		= 0x04,
	REG_OFC0	= 0x05,
	REG_OFC1	= 0x06,
	REG_OFC2	= 0x07,
	REG_FSC0	= 0x08,
	REG_FSC1	= 0x09,
	REG_FSC2	= 0x0A,
} ADS1256_REGISTERS;

/**
* Command definitions as defined in the datasheet
* https://www.ti.com/lit/ds/symlink/ads1256.pdf (Page 34)
*/
typedef enum {
	CMD_WAKEUP	= 0x00,		// Completes SYNC and exits standby mode
	CMD_RDATA	= 0x01,		// Read Data
	CMD_RDATAC	= 0x03,		// Read Data Continuously
	CMD_SDATAC	= 0x0F,		// Stop Read Data Continuously
	CMD_RREG	= 0x10,		// Read from REG rrrr
	CMD_WREG	= 0x50,		// Write from REG rrrr
	CMD_SELFCAL	= 0xF0,		// Offset and Gain Self-Calibration
	CMD_SELFOCAL= 0xF1,		// Offset Self-Calibration
	CMD_SELFGCAL= 0xF2,		// Gain Self-Calibration
	CMD_SYSOCAL	= 0xF3,		// System Offset Calibration
	CMD_SYSGCAL	= 0xF4,		// System Gain Calibration
	CMD_SYNC	= 0xFC,		// Synchronize the A/D Conversion
	CMD_STANDBY	= 0xFD,		// Begin Standby Mode
	CMD_RESET	= 0xFE,		// Reset to Power-Up Values
	CMD_WAKEUP_1= 0xFF,		// Completes SYNC and exits standby mode
} ADS1256_COMMANDS;



/**
* Resets the ADS1256 chip
*/
static void ADS1256_RST() {
	bcm2835_gpio_write(ADS_RST_PIN, 1);
	std::this_thread::sleep_for(std::chrono::microseconds(200));
	bcm2835_gpio_write(ADS_RST_PIN, 0);
	std::this_thread::sleep_for(std::chrono::microseconds(200));
	bcm2835_gpio_write(ADS_RST_PIN, 1);
}


/**
* Send a command to the ADS1256
* 
* Pulls the CS pin low then sends the command and raises the CS pin high
* 
* @param CMD. The command defined in the ADS1256_COMMANDS enum
*/
static void ADS1256_SENDCMD(uint8_t CMD) {
	bcm2835_gpio_write(ADS_CS_PIN, 0);
	bcm2835_spi_transfer(CMD);
	bcm2835_gpio_write(ADS_CS_PIN, 1);
}

/**
* Write to a register on the ADS1256
*
* Pulls the CS pin low then sends the WREG command 
* followed by the data and then raises the CS pin high
*
* @param uint8_t REG. The register defined in the ADS1256_REGISTERS enum
* @param uint8_t DATA. The value to set the register
*/
static void ADS1256_WREG(uint8_t REG, uint8_t DATA) {
	bcm2835_gpio_write(ADS_CS_PIN, 0);

	bcm2835_spi_transfer(CMD_WREG | REG);
	bcm2835_spi_transfer((0x0 << 4) | DATA);

	bcm2835_gpio_write(ADS_CS_PIN, 1);
}

/**
* Write to a register on the ADS1256
*
* Pulls the CS pin low then sends the WREG command
* followed by the data and then raises the CS pin high
*
* @param uint8_t REG. The register defined in the ADS1256_REGISTERS enum
* @return uint8_t The value from REG
*/
static uint8_t ADS1256_RREG(uint8_t REG) {
	uint8_t value = 0;

	bcm2835_gpio_write(ADS_CS_PIN, 0);
	bcm2835_spi_transfer(CMD_RREG | REG);	// 1-ST Control Byte: Read from REG
	bcm2835_spi_transfer(0x00);				// 2-ND Control Byte: Read from a single register

	std::this_thread::sleep_for(std::chrono::microseconds(10));

	value = bcm2835_spi_transfer(0xFF);		// SYNC and Wakeup
	bcm2835_gpio_write(ADS_CS_PIN, 1);

	return value;
}

/**
* 
*
*/
static void ADS1256_SETCHANNEL(uint8_t CHANNEL) {

}

/**
* Reads the ADS1256 chip ID via RREG
* @return uint8_t Chip ID
*/
uint8_t ADS1256_READID() {
	return ADS1256_RREG(REG_STATUS) >> 4;
}


/**
* Initialize the ADS1256 chip
*/
uint8_t ADS1256_INIT() {
	// Raise RST pin high before initializing
	bcm2835_gpio_write(ADS_RST_PIN, 1);
	
}
#endif