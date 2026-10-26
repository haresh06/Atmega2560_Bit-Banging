#include <stdio.h>
#include "spi_bb.h"
#include "timer1.h"

void spi_init(void)
{
    //SETTING MOSI,SCK,CS AS OUTPUT
    DDRK |= (1<<MOSI) | (1<<SCK) | (1<<CS);

    //SETTING MISO AS INPUT
    DDRK &= ~(1<<MISO);

    //IDLE STATE OF CS
    PORTK |= (1<<CS);

    //IDLE STATE OF SCK IN MODE 0 IS LOW
    PORTK &= ~(1<<SCK);
}

void spi_transfer(uint8_t data)
{
    //SELECTING THE SLAVE DEVICE BY PULLING CS LOW
    PORTK &= ~(1<<CS);

    for(int i=7;i>=0;i--)
    {
        if(data & (1<<i))
          PORTK |= (1<<MOSI);
        else
          PORTK &= ~(1<<MOSI);

          PORTK |= (1 << SCK);   // SCK HIGH
          PORTK &= ~(1 << SCK);  // SCK LOW
    }
    //SETTING BACK THE CS HIGH 
    PORTK |= (1<<CS);


}