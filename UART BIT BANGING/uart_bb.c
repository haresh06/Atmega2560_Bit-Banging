#include <stdio.h>
#include "uart_bb.h"

void bit_duration(void)
{
    timer1_ctc_mode(24, TIMER1_PRES64);

    ctc_delay(1);

    timer1_stop();
}
void uart_init(void)
{
    //MAKING PORTF PIN AS OUTPUT
    DDRF = 0x01;
    
    //IDLE STATE -> 1 ALWAYS HIGH UNTIL START BIT HAS ARRIVED
    PORTF = 0X01;
    // delay_ms(1);

}

void uart_transmit(uint8_t data)
{
    bit_duration();
// delay_ms(1);
    PORTF &= (~(0x01));

     bit_duration();
// delay_ms(1);
    for(int i=0;i<8;i++)
    {
        if(data & (1<<0))
        {
            PORTF |= (1<<0);
        }
        else
        {
            PORTF &= (~(1<<0));
        }

        data = (data>>1);
// delay_ms(1);
        bit_duration();
    }

    PORTF |= (1<<0);

    bit_duration();
// delay_ms(2);
}

void string_transmit(char *str)
{
    while(*str != '\0')
    {
        uart_transmit(*str);
        str++;
    }
}
