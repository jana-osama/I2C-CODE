#define F_CPU 16000000UL
#define SCL_FREQ 400000UL

#include <avr/io.h>
#include "I2C.h"


/* =========================================================
   MASTER INIT
   ========================================================= */

void I2C_Master_Init(void)
{
    /*
     * ATmega328P:
     * PC4 -> SDA
     * PC5 -> SCL
     *
     * TWI controls the pins.
     * SDA and SCL are not configured as normal GPIO outputs.
     */

  //  DDRC &= ~((1 << PC4) | (1 << PC5));

  //  /* Disable internal pull-up */
//    PORTC &= ~((1 << PC4) | (1 << PC5));


    /*
     * Prescaler = 1
     */
    TWSR = 0x00;

    TWBR = ((F_CPU / SCL_FREQ) - 16) / 2;


    /* Enable TWI */
    TWCR = (1 << TWEN);
}


/* =========================================================
   START CONDITION
   ========================================================= */

uint8_t I2C_Start_condition(void)
{
    /*
     * Generate START condition
     */

    TWCR = (1 << TWINT) |
           (1 << TWSTA) |
           (1 << TWEN);


    /*
     * Wait until START is transmitted
     */

    while (!(TWCR & (1 << TWINT)));


    /*
     * TWI Status Register
     *
     * 0x08 = START transmitted
     */

    if ((TWSR & 0xF8) == 0x08)
    {
        return I2C_OK;
    }

    return I2C_ERROR;
}


/* =========================================================
   SEND SLA + WRITE
   ========================================================= */

uint8_t I2C_Send_SLA_W(uint8_t address)
{
    /*
     * 7-bit address
     *
     * address << 1
     *
     * R/W = 0 -> WRITE
     */

    TWDR = (address << 1) | 0;


    /*
     * Start transmission
     */

    TWCR = (1 << TWINT) |
           (1 << TWEN);


    /*
     * Wait
     */

    while (!(TWCR & (1 << TWINT)));


    /*
     * 0x18 =
     *
     * SLA+W transmitted
     * ACK received
     */

    if ((TWSR & 0xF8) == 0x18)
    {
        return I2C_OK;
    }

    return I2C_ERROR;
}


/* =========================================================
   SEND SLA + READ
   ========================================================= */

uint8_t I2C_Send_SLA_R(uint8_t address)
{
    /*
     * 7-bit address
     *
     * address << 1
     *
     * R/W = 1 -> READ
     */

    TWDR = (address << 1) | 1;


    /*
     * Start transmission
     */

    TWCR = (1 << TWINT) |
           (1 << TWEN);


    /*
     * Wait
     */

    while (!(TWCR & (1 << TWINT)));


    /*
     * 0x40 =
     *
     * SLA+R transmitted
     * ACK received
     */

    if ((TWSR & 0xF8) == 0x40)
    {
        return I2C_OK;
    }

    return I2C_ERROR;
}


/* =========================================================
   WRITE DATA
   ========================================================= */

uint8_t I2C_Write(uint8_t data)
{

    TWDR = data;
   
    TWCR = (1 << TWINT) |
           (1 << TWEN);
		   
    while (!(TWCR & (1 << TWINT)));

    if ((TWSR & 0xF8) == 0x28)
    {
        return I2C_OK;
    }

    return I2C_ERROR;
}


/* =========================================================
   READ DATA + ACK
   ========================================================= */

uint8_t I2C_Read_ACK(void)
{
    /*
     * TWEA = 1
     *
     * After receiving data:
     * send ACK
     */

    TWCR = (1 << TWINT) |
           (1 << TWEN) |
           (1 << TWEA);


    /*
     * Wait
     */

    while (!(TWCR & (1 << TWINT)));

    if ((TWSR & 0xF8) == 0x50)
    {
	    return TWDR;
    }


    return I2C_ERROR;
}


/* =========================================================
   READ DATA + NACK
   ========================================================= */

uint8_t I2C_Read_NACK(void)
{
    /*
     * TWEA = 0
     *
     * After receiving data:
     * send NACK
     */

    TWCR = (1 << TWINT) |
           (1 << TWEN);


    /*
     * Wait
     */

    while (!(TWCR & (1 << TWINT)));


    /*
     * Return received data
     */

    return TWDR;
}


/* =========================================================
   STOP CONDITION
   ========================================================= */

void I2C_Stop(void)
{
    /*
     * Generate STOP condition
     */

    TWCR = (1 << TWINT) |
           (1 << TWSTO) |
           (1 << TWEN);
}


/* =========================================================
   SLAVE INIT
   ========================================================= */

void I2C_Slave_Init(uint8_t address)
{
  

    TWAR = (address << 1);

   // TWAMR = 0x00;

    TWCR = (1 << TWEN) | (1 << TWEA);
}


/* =========================================================
   SLAVE RECEIVE
   ========================================================= */

uint8_t I2C_Slave_Receive(void)
{
    /*
     * Enable TWI
     * Enable ACK
     * Clear TWINT
     */

    TWCR = (1 << TWINT) |
           (1 << TWEN) |
           (1 << TWEA);


    /*
     * Wait until something is received
     */

    while (!(TWCR & (1 << TWINT)));


    /*
     * Return received data
     */

    return TWDR;
}
uint8_t I2C_Repeated_Start(void)
{
    /*
     * Generate REPEATED START condition
     */

    TWCR = (1 << TWINT) |
           (1 << TWSTA) |
           (1 << TWEN);

    /*
     * Wait until START is transmitted
     */

    while (!(TWCR & (1 << TWINT)));

    /*
     * 0x10 =
     * Repeated START transmitted
     */

    if ((TWSR & 0xF8) == 0x10)
    {
        return I2C_OK;
    }

    return I2C_ERROR;
}

