/*
 * spi.c
 *
 * Created: 22/06/2016 22:28:40
 *  Author: Henning
 */ 

#include "global.h"

void SPI_MasterInit(void) {
	/* Enable SPI, Master, set clock rate F_CPU/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(0<<SPR0)|(0<<SPR1);
}