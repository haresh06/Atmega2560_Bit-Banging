#include "timer1.h"
#include <avr/interrupt.h>

//====================== TIMER CONTROL ======================

// Stop Timer1
void timer1_stop(void)
{
    TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12));
}

// Start Timer1 with selected prescaler
void timer1_start(timer1_clk clock)
{
    timer1_stop();

    switch(clock)
    {
        case TIMER1_PRES1:
            TCCR1B |= (1 << CS10);
            break;

        case TIMER1_PRES8:
            TCCR1B |= (1 << CS11);
            break;

        case TIMER1_PRES64:
            TCCR1B |= ((1 << CS10) | (1 << CS11));
            break;

        case TIMER1_PRES256:
            TCCR1B |= (1 << CS12);
            break;

        case TIMER1_PRES1024:
            TCCR1B |= ((1 << CS10) | (1 << CS12));
            break;

        case TIMER1_NOCLK:
        default:
            break;
    }
}

// Reset Timer Counter
void timer1_reset(void)
{
    TCNT1 = 0;
}

//====================== REGISTER SET FUNCTIONS ======================

// Load value into TCNT1
void set_count1(uint16_t val)
{
    TCNT1 = val;
}

// Load value into OCR1A
void set_compare1(uint16_t val)
{
    OCR1A = val;
}

//====================== NORMAL MODE ======================

void timer1_normal_mode(timer1_clk val)
{
    timer1_stop();

    TCCR1A = 0;
    TCCR1B = 0;

    timer1_reset();

    timer1_start(val);
}

//====================== CTC MODE ======================

void timer1_ctc_mode(uint16_t value, timer1_clk val)
{
    timer1_stop();

    TCCR1A = 0;
    TCCR1B = 0;

    timer1_reset();

    OCR1A = value;

    // CTC Mode
    TCCR1B |= (1 << WGM12);

    timer1_start(val);
}

//====================== POLLING DELAYS ======================

// Wait for x overflows
void normal_delay(uint16_t x)
{
    for(uint16_t i = 0; i < x; i++)
    {
        while(!(TIFR1 & (1 << TOV1)));

        // Clear overflow flag
        TIFR1 |= (1 << TOV1);
    }
}

// Wait for x compare matches
void ctc_delay(uint16_t x)
{
    for(uint16_t i = 0; i < x; i++)
    {
        while(!(TIFR1 & (1 << OCF1A)));

        // Clear compare match flag
        TIFR1 |= (1 << OCF1A);
    }
}

//====================== INTERRUPTS ======================

// Enable Timer1 Overflow Interrupt
void en_ovf_intr(void)
{
    TIMSK1 |= (1 << TOIE1);
    sei();
}

// Disable Timer1 Overflow Interrupt
void dis_ovf_intr(void)
{
    TIMSK1 &= ~(1 << TOIE1);
}

// Enable Timer1 Compare Match Interrupt
void en_com_intr(void)
{
    TIMSK1 |= (1 << OCIE1A);
    sei();
}

// Disable Timer1 Compare Match Interrupt
void dis_com_intr(void)
{
    TIMSK1 &= ~(1 << OCIE1A);
}

//====================== USER DELAY FUNCTIONS ======================

// Delay in milliseconds
// F_CPU = 16 MHz
// OCR1A = 249
// Prescaler = 64
// 1 Compare Match = 1 ms

void delay_ms(uint16_t value)
{
    timer1_ctc_mode(24, TIMER1_PRES64);

    ctc_delay(value);

    timer1_stop();
}

// Delay in seconds
// F_CPU = 16 MHz
// OCR1A = 15624
// Prescaler = 1024
// 1 Compare Match = 1 second

void delay_s(uint16_t value)
{
    timer1_ctc_mode(15624, TIMER1_PRES1024);

    ctc_delay(value);

    timer1_stop();
}

//====================== PWM ======================

void pwm_init(timer1_clk prescaler, uint16_t top)
{
    // Fast PWM Mode 14
    // Non-inverting PWM on OC1A

    TCCR1A |= ((1 << WGM11) | (1 << COM1A1));

    TCCR1B |= ((1 << WGM12) |
               (1 << WGM13) |
               (prescaler & 0x07));

    ICR1 = top;

    OCR1A = 0;
}

void duty_cycle(uint8_t duty_percent)
{
    uint16_t duty_value;

    duty_value = ((uint32_t)duty_percent * ICR1) / 100;

    OCR1A = duty_value;
}