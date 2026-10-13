#include "i2c_bb.h"

// ==================== PIN CONTROL MACROS ====================
// Standard I2C Open-Drain style:
// HIGH -> Line released (Pin configured as INPUT, internal/external pull-up brings it HIGH)
// LOW  -> Line driven LOW (Pin configured as OUTPUT, driven to 0)
#define SCL_HIGH()  do { DDRF &= ~(1 << SCL_PIN); PORTF |= (1 << SCL_PIN); } while(0)
#define SCL_LOW()   do { DDRF |= (1 << SCL_PIN);  PORTF &= ~(1 << SCL_PIN); } while(0)

#define SDA_HIGH()  do { DDRF &= ~(1 << SDA_PIN); PORTF |= (1 << SDA_PIN); } while(0)
#define SDA_LOW()   do { DDRF |= (1 << SDA_PIN);  PORTF &= ~(1 << SDA_PIN); } while(0)

#define SDA_READ()  ((PINF & (1 << SDA_PIN)) ? 1 : 0)

// ==================== BIT DELAY ====================
// Small delay for I2C clock half-period (~50 kHz to 100 kHz at 16 MHz)
static void i2c_delay(void)
{
    for (volatile uint8_t i = 0; i < 20; i++)
    {
        // Delay loop for I2C timing
    }
}

// ==================== CORE I2C FUNCTIONS ====================

// Initialize I2C lines to idle state (both SCL and SDA released HIGH)
void i2c_init(void)
{
    SDA_HIGH();
    SCL_HIGH();
    i2c_delay();
}

// Generate I2C START condition:
// High-to-Low transition on SDA while SCL is HIGH
void i2c_start(void)
{
    SDA_HIGH();
    SCL_HIGH();
    i2c_delay();

    SDA_LOW();      // SDA transitions LOW while SCL is HIGH
    i2c_delay();

    SCL_LOW();      // Pull SCL LOW to begin clocking
    i2c_delay();
}

// Generate I2C REPEATED START condition
void i2c_repeated_start(void)
{
    SDA_HIGH();     // Release SDA while SCL is LOW
    i2c_delay();

    SCL_HIGH();     // SCL goes HIGH
    i2c_delay();

    SDA_LOW();      // SDA transitions LOW while SCL is HIGH
    i2c_delay();

    SCL_LOW();      // Pull SCL LOW
    i2c_delay();
}

// Generate I2C STOP condition:
// Low-to-High transition on SDA while SCL is HIGH
void i2c_stop(void)
{
    SDA_LOW();      // Ensure SDA is LOW first
    i2c_delay();

    SCL_HIGH();     // SCL goes HIGH
    i2c_delay();

    SDA_HIGH();     // SDA transitions HIGH while SCL is HIGH
    i2c_delay();
}

// Transmit 1 byte (MSB first) and return received ACK/NACK
// Returns: 0 (I2C_ACK) on success, 1 (I2C_NACK) on failure
uint8_t i2c_write(uint8_t data)
{
    uint8_t ack;

    // Transmit 8 bits, MSB first
    for (uint8_t i = 0; i < 8; i++)
    {
        if (data & 0x80)
        {
            SDA_HIGH();
        }
        else
        {
            SDA_LOW();
        }
        i2c_delay();

        SCL_HIGH();
        i2c_delay();

        SCL_LOW();
        i2c_delay();

        data <<= 1;
    }

    // 9th clock cycle: Read ACK/NACK bit from receiver
    SDA_HIGH();     // Release SDA line for slave to drive ACK
    i2c_delay();

    SCL_HIGH();
    i2c_delay();

    ack = SDA_READ(); // 0 = ACK, 1 = NACK

    SCL_LOW();
    i2c_delay();

    return ack;
}

// Receive 1 byte (MSB first) and send ACK/NACK to slave
// Param: ack -> I2C_ACK (0) to request more bytes, I2C_NACK (1) for last byte
// Returns: 8-bit received data
uint8_t i2c_read(uint8_t ack)
{
    uint8_t data = 0;

    // Release SDA so slave can transmit data
    SDA_HIGH();
    i2c_delay();

    // Read 8 bits (MSB first)
    for (uint8_t i = 0; i < 8; i++)
    {
        data <<= 1;

        SCL_HIGH();
        i2c_delay();

        if (SDA_READ())
        {
            data |= 0x01;
        }

        SCL_LOW();
        i2c_delay();
    }

    // 9th clock cycle: Master sends ACK/NACK to slave
    if (ack == I2C_ACK)
    {
        SDA_LOW();      // Drive LOW to ACK
    }
    else
    {
        SDA_HIGH();     // Release HIGH to NACK
    }
    i2c_delay();

    SCL_HIGH();
    i2c_delay();

    SCL_LOW();
    i2c_delay();

    SDA_HIGH();         // Release SDA line after ACK/NACK

    return data;
}

// ==================== HIGH-LEVEL UTILITY FUNCTIONS ====================

// Send START condition and 7-bit slave address with R/W direction bit
// Param: slave_addr (7-bit address, e.g. 0x50), rw (I2C_WRITE or I2C_READ)
// Returns: 0 (I2C_ACK) if slave responded, 1 (I2C_NACK) if no response
uint8_t i2c_start_addr(uint8_t slave_addr, uint8_t rw)
{
    i2c_start();
    return i2c_write((uint8_t)((slave_addr << 1) | (rw & 0x01)));
}

// Write a buffer of bytes to a slave device
// Returns: 0 on success, 1 on address NACK, 2 on data byte NACK
uint8_t i2c_write_buffer(uint8_t slave_addr, uint8_t *data, uint16_t len)
{
    if (i2c_start_addr(slave_addr, I2C_WRITE) != I2C_ACK)
    {
        i2c_stop();
        return 1;
    }

    for (uint16_t i = 0; i < len; i++)
    {
        if (i2c_write(data[i]) != I2C_ACK)
        {
            i2c_stop();
            return 2;
        }
    }

    i2c_stop();
    return 0;
}

// Read a buffer of bytes from a slave device
// Returns: 0 on success, 1 on address NACK
uint8_t i2c_read_buffer(uint8_t slave_addr, uint8_t *data, uint16_t len)
{
    if (len == 0)
    {
        return 0;
    }

    if (i2c_start_addr(slave_addr, I2C_READ) != I2C_ACK)
    {
        i2c_stop();
        return 1;
    }

    for (uint16_t i = 0; i < len; i++)
    {
        if (i == (len - 1))
        {
            data[i] = i2c_read(I2C_NACK); // Send NACK on final byte
        }
        else
        {
            data[i] = i2c_read(I2C_ACK);  // Send ACK on intermediate bytes
        }
    }

    i2c_stop();
    return 0;
}

// Write 1 byte to a specific register inside a slave device
// Returns: 0 on success, non-zero on failure
uint8_t i2c_write_reg(uint8_t slave_addr, uint8_t reg_addr, uint8_t data)
{
    if (i2c_start_addr(slave_addr, I2C_WRITE) != I2C_ACK)
    {
        i2c_stop();
        return 1;
    }

    if (i2c_write(reg_addr) != I2C_ACK)
    {
        i2c_stop();
        return 2;
    }

    if (i2c_write(data) != I2C_ACK)
    {
        i2c_stop();
        return 3;
    }

    i2c_stop();
    return 0;
}

// Read 1 byte from a specific register inside a slave device
// Returns: 0 on success, non-zero on failure
uint8_t i2c_read_reg(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data)
{
    // Step 1: Send register address to read from
    if (i2c_start_addr(slave_addr, I2C_WRITE) != I2C_ACK)
    {
        i2c_stop();
        return 1;
    }

    if (i2c_write(reg_addr) != I2C_ACK)
    {
        i2c_stop();
        return 2;
    }

    // Step 2: Send repeated start with READ mode
    if (i2c_start_addr(slave_addr, I2C_READ) != I2C_ACK)
    {
        i2c_stop();
        return 3;
    }

    // Step 3: Read single byte with NACK
    *data = i2c_read(I2C_NACK);

    i2c_stop();
    return 0;
}
