#pragma once
#ifndef BCM2835_DRIVER_H
#define BCM2835_DRIVER_H

/* BCM2835.h Provided by AirSpayce
* https://www.airspayce.com/mikem/bcm2835/
*/
#include <bcm2835.h>

#define ADS_RST_PIN 18
#define ADS_CS_PIN 22
#define ADS_DRDY_PIN 17

/**
 * Begin SPI operations and assign pins required
*/
static void BCM2835_GPIO_INIT() {
	// Set pins to output
	bcm2835_gpio_fsel(ADS_RST_PIN,  BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(ADS_CS_PIN,   BCM2835_GPIO_FSEL_OUTP);

	// Set pins to input
	bcm2835_gpio_fsel(ADS_DRDY_PIN, BCM2835_GPIO_FSEL_INPT);
}

/**
 * End SPI operations and return assigned pins to default input
 * Close BCM2835 library
*/
static void BCM2835_GPIO_EXIT() {
	bcm2835_spi_end();
	bcm2835_close();
}

/**
 * Initialize BCM2835 library and SPI protocol
 * @return 0 if successful otherwise 1
*/
uint8_t BCM2835_INIT() {
	if (bcm2835_init()) {
		BCM2835_GPIO_INIT();

		bcm2835_spi_begin();
		bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);	// SPI Mode 0 only supports MSB bit order
		bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);					// Set SPI Mode 0, hardware SPI chips
		bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64);	// 6.250 Mhz SPI frequency

		return 0;
	}
	
	return 1;
}
#endif