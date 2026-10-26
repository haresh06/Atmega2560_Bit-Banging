#include "i2c_bb.h"
#include "timer1.h"

void sda_low(void)
{
DDRF |= (1 << SDA_PIN);
PORTF &= ~(1 << SDA_PIN);
}

void sda_release(void)
{
PORTF &= ~(1 << SDA_PIN);
DDRF &= ~(1 << SDA_PIN);
}

void scl_low(void)
{
DDRF |= (1 << SCL_PIN);
PORTF &= ~(1 << SCL_PIN);
}

void scl_release(void)
{
PORTF &= ~(1 << SCL_PIN);
DDRF &= ~(1 << SCL_PIN);
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
if (bit)
sda_release();
else
sda_low();

delay_us(5);

scl_release();  // Allow the slave to read SDA
delay_us(5);

scl_low();      // End the clock pulse
delay_us(5);

}

uint8_t read_ack(void)
{
uint8_t ack;

sda_release();
delay_us(5);

scl_release();
delay_us(5);

ack = (PINF & (1 << SDA_PIN)) ? 1 : 0;  // Read ACK/NACK

scl_low();  // Finish the ninth clock pulse
delay_us(5);

return ack;

}

void trans_byte(uint8_t byte)
{
     for (int i = 7; i >= 0; i--)
    {
       write_bit((byte >> i) & 1);
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