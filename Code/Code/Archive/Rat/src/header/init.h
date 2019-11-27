// Public function declarations


#ifndef INIT_H_
#define INIT_H_

//#define F_CPU 8000000UL
//#define BAUD 250000
//#define BAUD_CALC F_CPU/16/BAUD-1

void IO_Init(void);
void SPI_MasterInit(void);
void timer_init(void);

#endif /* INIT_H_ */

