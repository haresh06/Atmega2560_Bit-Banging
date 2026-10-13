#include "i2c_bb.h"
#include "timer1.h"

void sda_low(void)
{
    DDRF |= (1<<PF0);
    PORTF &= ~(1<<PF0);
}

void sda_release(void)
{
    PORTF &= ~(1<<PF0);
    DDRF &= ~(1<<PF0);
    
}

void scl_low(void)
{
    DDRF |= (1<<PF1);
    PORTF &= ~(1<<PF1);
}

void scl_release(void)
{
    PORTF &= ~(1<<PF1);
    DDRF &= ~(1<<PF1);
    
}

void i2c_start(void)
{
    sda_release();
    scl_release();

    delay_us(5);

    sda_low();

    delay_us(5);

    scl_low();

}

void write_bit(int bit)
{
    if(bit)
      sda_release();
    else
      sda_low();
      
    delay_us(5);
    scl_release(); //allowing the slave to read the bit on SDA.
    delay_us(5);
    scl_low(); //This ends the clock pulse and prepares for the next bit.
    delay_us(5);
} 

uint8_t read_ack(void)
{
    uint8_t ack;

    sda_release();
    delay_us(5);

    scl_release();
    delay_us(5);

    ack = (PINF & (1<<0))?1:0; //generate 9th pulse 
    
    scl_low();                 // Finish 9th clock pulse
    delay_us(5);

    return ack;
}

void trans_byte(uint8_t byte)
{
    for(int i=7;i>=0;i--)
    {
        write_bit((byte>>i)&1);
    }
}

void i2c_stop(void)
{
    sda_low();
    delay_us(5);
    scl_release();
    delay_us(5);
    sda_release();
    delay_us(5);
}

