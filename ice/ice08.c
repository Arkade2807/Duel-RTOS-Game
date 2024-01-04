/**
 * @file ice08.c
 * @author Lucas Franke (lmfranke@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-08-25
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../main.h"

#if defined(ICE) && (FILE_ID == 8)

#define TICKS_MS_100 (10000000)

typedef enum {
    ALERT_TIMER_NONE,
    ALERT_TIMER_100MS
}alert_timer_t;

char ICE_DESCRIPTION[] = "ECE353: ICE 08 - ADC";

/*****************************************************************************/
/*  Function Declaration                                                     */
/*****************************************************************************/
void Handler_Timer(void *handler_arg, cyhal_timer_event_t event);

/*****************************************************************************/
/*  Global Variables                                                         */
/*****************************************************************************/
volatile alert_timer_t ALERT_TIMER = ALERT_TIMER_NONE; 

/* Timer object used */
cyhal_timer_t timer_obj;
cyhal_timer_cfg_t timer_cfg;

// cyhal_timer_cfg_t timer_cfg =
//     {
//         .compare_value = 0,              /* Timer compare value, not used */
//         .period = 0,                     /* number of timer ticks */
//         .direction = CYHAL_TIMER_DIR_UP, /* Timer counts up */
//         .is_compare = false,             /* Don't use compare mode */
//         .is_continuous = true,           /* Run timer indefinitely */
//         .value = 0                       /* Initial value of counter */
// };

/*****************************************************************************/
/*  Interrupt Handlers                                                       */
/*****************************************************************************/
/**
 * @brief
 *  Function used as Timer Handler
 */
void Handler_Timer(void *handler_arg, cyhal_timer_event_t event)
{
    ALERT_TIMER = ALERT_TIMER_100MS;
    /* ADD CODE*/
}

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
    /* ADD CODE */

    /* Initialize a timer to generate an interrupt every 100mS*/
    cy_rslt_t rslt;
    timer_init(&timer_obj, &timer_cfg, TICKS_MS_100, Handler_Timer);

    /* Initialize the X and Y directions of the joystick to be ADC pins*/
    joystick_init();

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
    joystick_position_t curr_position = JOYSTICK_POS_CENTER;
    joystick_position_t prev_position = JOYSTICK_POS_CENTER;

    while (1)
    {
        if (ALERT_TIMER == ALERT_TIMER_100MS)
        {
            /* Set Interrupt flag back to false*/
            ALERT_TIMER = ALERT_TIMER_NONE;

            /* Save the previous position */
            prev_position = curr_position;

            /* Get the current position */
            curr_position = joystick_get_pos();

            /* If the joystick was moved, print its new position*/
            if (curr_position != prev_position)
            {
                printf("Joystick now in ");
                joystick_print_pos(curr_position);
                printf(" position\n\r");
            }
        }
    }
}
#endif