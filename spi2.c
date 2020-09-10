#include "stm32f4xx.h"
#define WHO_AM_I 0x0F


void delay (int a)
{
    volatile int i,j;

    for (i=0 ; i < a ; i++)
    {
        j++;
    }

    return;
}


void SPI_init(void){
	// GPIOA
	RCC->AHB1ENR |= (1<<0); // enable clock for GPIOA
	
	GPIOA->MODER |= (1<<15) | (1<<13) | (1<<11);    // set PA7 (MOSI), PA6 (MISO) and PA5 (SCK) to Alternate Function (AF)
	GPIOA->OTYPER &= ~( (1<<7) | (1<<6) | (1<<5) ); // set PA7, PA6 and PA5 to push pull mode
	GPIOA->OSPEEDR |= (1<<15) | (1<<13) | (1<<11);  // set PA7, PA6 and PA5 speed to 50MHz fast speed
	GPIOA->PUPDR |= (1<<14) | (1<<12) | (1<<10);    // enable pull-ups on PA7, PA6 and PA5
	
	// connect PA7, PA6 and PA5 to AF5
	GPIOA->AFR[0] |= (1<<30) | (1<<28); 
	GPIOA->AFR[0] |= (1<<26) | (1<<24); 
	GPIOA->AFR[0] |= (1<<22) | (1<<20); 
	
	// GPIOE
	RCC->AHB1ENR |= (1<<4); // enable clock for GPIOE
	
	GPIOE->MODER |= (1<<6);   // set PE3 to GPIO (output)
	GPIOE->OTYPER &= ~(1<<3); // set PE3 to push-pull ouput type
	GPIOE->OSPEEDR |= (1<<7); // set PE3 speed to 50MHz fast speed
	GPIOE->PUPDR |= (1<<6);   // enable pull-up on PE3
	
	// configure SPI1
	RCC->APB2ENR |= (1<<12); // enable clock for SPI1
	SPI1->CR1 |= (1<<4) | (1<<3); // BR, set SPI clock to f_PCLK / 16
	SPI1->CR1 |= (1<<9) | (1<<8); // SSM(Software slave management) and SSI(Internal slave select) to high
	SPI1->CR1 |= (1<<11); // DFF: Data frame format, 16-bit data frame format is selected for transmission/reception
	SPI1->CR1 |= (1<<2); // MSTR - master configuration
	SPI1->CR1 |= (1<<1) | (1<<0); // set CPOL high = CK to 1 when idle(bezczynny), set CPHA high = second clock transition is the first data capture edge
	SPI1->CR1 |= (1<<6); //SPE, enable the SPI 
}
void push_slave_LOW(void)
{
while( SPI1->SR & (1<<7) );
GPIOE->BSRR |= (1 << 3);
}

void push_slave_HIGH(void)
{
while( SPI1->SR & (1<<7) );
GPIOE->BSRR |= (1 << 19);
}

short SPI_Transmitt(short data){	

	SPI1->DR = data; // write value to transmit into the data register
	while( !(SPI1->SR & (1<<1)) ); // wait until TXE(writing) flag is set --> transmit buffer empty
	while( SPI1->SR & (1<<7) );    // wait until SPI1 is not busy anymore
	while( !(SPI1->SR & (1<<0)) ); // wait until RXE(reading) flag is set --> receive buffer not empty

	return SPI1->DR; // read received data from data register and return it
}



int main()
{
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIODEN; //Enable GPIOD
	GPIOD -> MODER |=  (1<<24);

	SPI_init();
	GPIOE->ODR |= (1<<3); // set PE3 high

	GPIOD->ODR &= ~(1<<3); // set PE3 low
	SPI_Transmitt(0x80 | WHO_AM_I); 
	short whoami=SPI_Transmitt(0x00);
	GPIOE->ODR |= (1<<3); // set PE3 high

 	if(whoami == (unsigned int)0xFFFFFFFF) 
	{
		GPIOD->ODR ^= GPIO_ODR_ODR_12;
	}
	
return(1);

}