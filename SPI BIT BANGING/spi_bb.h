#ifndef SPI_BB_H
#define SPI_BB_H

#define DDRK   (*(volatile uint8_t *)0x107)
#define PORTK  (*(volatile uint8_t *)0x108)
#define PINK   (*(volatile uint8_t *)0x106)

#define MOSI 3
#define MISO 2
#define SCK  1
#define CS   0

#endif