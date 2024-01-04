/**
 * @file ice14.c
 * @author Lucas Franke (lmfranke@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-08-25
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../main.h"

#if defined(ICE) && (FILE_ID == 13)

#define EEPROM_TEST_SIZE 1024

char ICE_DESCRIPTION[] = "ECE353: ICE 13 - SPI EEPROM";
cyhal_trng_t trng_obj;
uint8_t Write_Values[EEPROM_TEST_SIZE];

/*****************************************************************************/
/* Peripheral Initilization                                                  */
/*****************************************************************************/

/**
 * @brief
 * This function will initialize all of the hardware resources for
 * the ICE
 */
void peripheral_init(void)
{
    /* Initialize the SPI interface */
    spi_init();

    /* Initialize the EEPROM CS line*/
    eeprom_cs_init();

    /* Initialize the hardware random number generator*/
    cyhal_trng_init(&trng_obj);
}

/*****************************************************************************/
/* Application Code                                                          */
/*****************************************************************************/
/**
 * @brief
 * This function implements the behavioral requirements for the ICE
 */
void main_app(void)
{
    uint16_t address = 0x0000;
    uint8_t read_byte;

    printf("Generating Random Numbers\n\r");
    for (int i = 0; i < EEPROM_TEST_SIZE; i++)
    {
        Write_Values[i] = (uint8_t)cyhal_trng_generate(&trng_obj);
    }

    printf("Writing Values to EEPROM\n\r");
    for (int i = 0; i < EEPROM_TEST_SIZE; i++)
    {
        eeprom_write_byte(address, Write_Values[i]);
        address++;
    }
    address = 0;

    printf("Reading Values From EEPROM\n\r");
    for (int i = 0; i < EEPROM_TEST_SIZE; i++)
    {
        read_byte = eeprom_read_byte(address);
        if (read_byte != Write_Values[i])
        {
            printf("EEPROM Read %d ERROR... %d != %d\n\r", i, Write_Values[i], read_byte);
            CY_ASSERT(0);
        }
        else
        {
            printf("EEPROM Read %d Passed.\n\r", i);
            address++;
        }
    }

    printf("EEPROM Test Passed\n\r");
    while (1)
    {
    }
}
#endif