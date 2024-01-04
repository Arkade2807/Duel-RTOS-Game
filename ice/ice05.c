/**
 * @file ice05.c
 * @author Lucas Franke
 * @brief
 * @version 0.1
 * @date 2023-08-25
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../main.h"

#if defined(ICE) && (FILE_ID == 5)

/*****************************************************************************/
/* Macros                                                                    */
/*****************************************************************************/

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/
char ICE_DESCRIPTION[] = "ECE353: ICE 05 - SysTick";

/*****************************************************************************/
/* Function Declarations                                                     */
/*****************************************************************************/

/*****************************************************************************/
/* Function Definitions                                                      */
/*****************************************************************************/

/**
 * @brief
 * This function will initialize all of the hardware resources for
 * the ICE
 */
void peripheral_init(void)
{
    /* Initialize the LEDs */
    leds_init();

    /* Initialize the push buttons */
    push_buttons_init();

    /* Set the SysTick Timer to that it  expires every 100uS*/
    systick_init(TICKS_US_100);
    /* ADD CODE */
}

/**
 * @brief
 * This function implements the behavioral requirements for the ICE
 */
void main_app(void)
{
    button_state_t button_state;
    uint8_t timer_count = 0;
    uint8_t on_threshold = 50;
    uint8_t ms_checker = 0;

    for (;;)
    {
        /* Check to see if the systick timer has expired */
        if (systick_expired())
        {
            timer_count = 0;
            ms_checker = ms_checker + 1;
            if (ms_checker % 50 == 0){
                ms_checker = 0;
                button_state_t button = get_buttons();
                if (button == BUTTON_SW1_RELEASED && on_threshold != 0){
                    on_threshold = on_threshold - 10;
                }
                else if (button == BUTTON_SW2_RELEASED && on_threshold != 100){
                    on_threshold = on_threshold + 10;
                }
                else if (button == BUTTON_SW3_RELEASED){
                    on_threshold = 50;
                }
            }
        }
        timer_count = timer_count + 1;

        if (timer_count<on_threshold){
            PORT_RGB_RED->OUT_SET = MASK_RGB_RED;
        }
        if (timer_count>=on_threshold){
            PORT_RGB_RED->OUT_CLR = MASK_RGB_RED;
        }
        
        /* Determine if the Red LED should be on or off */
        /* ADD CODE */
    }
}
#endif