#include "timer1.h"
#include<stdint.h>

//GPIO REG 
#define DDRF (*(volatile uint8_t*)0x30)
#define PORTF (*(volatile uint8_t*)0x31)



//void bit_duration(void);
void uart_init(void);
void uart_transmit(uint8_t data);
void string_transmit(char *str);
