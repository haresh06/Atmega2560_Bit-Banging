#ifndef TIMER1_H
#define TIMER1_H

#include <stdint.h>

//====================== TIMER1 REGISTERS ======================

#define TCCR1A (*(volatile uint8_t*)0x80)
#define TCCR1B (*(volatile uint8_t*)0x81)
#define TCNT1  (*(volatile uint16_t*)0x84)
#define OCR1A  (*(volatile uint16_t*)0x88)
#define TIMSK1 (*(volatile uint8_t*)0x6F)
#define TIFR1  (*(volatile uint8_t*)0x36)
#define ICR1   (*(volatile uint16_t*)0x86)

//====================== BIT POSITIONS ======================

// Waveform Generation Mode bits
#define WGM10  0
#define WGM11  1
#define WGM12  3
#define WGM13  4

// Compare Output Mode bits
#define COM1A1 7

// Clock Select bits
#define CS10 0
#define CS11 1
#define CS12 2

// Timer Flags
#define TOV1  0
#define OCF1A 1

// Interrupt Enable bits
#define TOIE1  0
#define OCIE1A 1

//====================== PRESCALER ENUM ======================

typedef enum
{
    TIMER1_NOCLK = 0,
    TIMER1_PRES1,
    TIMER1_PRES8,
    TIMER1_PRES64,
    TIMER1_PRES256,
    TIMER1_PRES1024

} timer1_clk;

//====================== TIMER CONTROL ======================

void timer1_stop(void);
void timer1_start(timer1_clk clock);
void timer1_reset(void);

//====================== REGISTER SET FUNCTIONS ======================

void set_count1(uint16_t val);
void set_compare1(uint16_t val);

//====================== TIMER MODES ======================

void timer1_normal_mode(timer1_clk val);
void timer1_ctc_mode(uint16_t value, timer1_clk val);

//====================== POLLING DELAYS ======================

void normal_delay(uint16_t x);
void ctc_delay(uint16_t x);

//====================== INTERRUPTS ======================

void en_ovf_intr(void);
void dis_ovf_intr(void);

void en_com_intr(void);
void dis_com_intr(void);

//====================== USER DELAY FUNCTIONS ======================

void delay_ms(uint16_t value);
void delay_s(uint16_t value);

//====================== PWM ======================

void pwm_init(timer1_clk prescaler, uint16_t top);
void duty_cycle(uint8_t duty_percent);

#endif