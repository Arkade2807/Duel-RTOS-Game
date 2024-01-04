/**
 * @file ice01.c
 * @author Lucas Franke (lmfranke@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-08-25
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../main.h"

#if defined(ICE) && (FILE_ID == 14)

char ICE_DESCRIPTION[] = "ECE353: ICE 14 - IO Expander";

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
    i2c_init();

    io_expander_set_configuration(0x80); 	// Set bit 7 as input, bits 6-0 as outputs 
	io_expander_set_output_port(0x00); 		// Turn OFF all LEDs
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
    uint8_t active_led = 0x01;
    uint8_t io_sw = 0xFF;

    while(1)
    {
        /* Need to read the IO Expander due to the way Interrupts from the IO Expander work */
        io_sw = io_expander_get_input_port();

        /* Set the IO Expander Outputs to the active LED */ 
	    io_expander_set_output_port(active_led); 	

        cyhal_system_delay_ms(500);

        /* Rotate the active LED */
        if(active_led == 0x40)
        {
            active_led = 0x01;
        }
        else
        {
            active_led = active_led << 1;
        }
    }
}
#endif