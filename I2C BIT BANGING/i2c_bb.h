#ifndef I2C_BB_H
#define I2C_BB_H

#include <stdint.h>

#define DDRF (*(volatile uint8_t *)0x30)
#define PORTF (*(volatile uint8_t *)0x31)
#define PINF (*(volatile uint8_t *)0x2F)

/* Port F bit positions */
#define SDA_PIN 1 // PF1 - SDA
#define SCL_PIN 2 // PF2 - SCL

void sda_low(void);
void sda_release(void);
void scl_low(void);
void scl_release(void);

void i2c_start(void);
void write_bit(int bit);
uint8_t read_ack(void);
void trans_byte(uint8_t byte);
void i2c_stop(void);

#endif