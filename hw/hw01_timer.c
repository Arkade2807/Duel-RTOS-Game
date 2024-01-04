/**
 * @file hw01_timer.c
 * @author Lucas Franke (lmfranke@wisc.edu) & Aditya Kamasani (kamasani@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-09-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "hw01_timer.h"
/*****************************************************************************/
/*  Function Declaration                                                     */
/*****************************************************************************/
void Handler_HW01_Timer(void *handler_arg, cyhal_timer_event_t event);

/*****************************************************************************/
/*  Global Variables                                                         */
/*****************************************************************************/

hw01_timer_alerts_t ALERT_SW1_LONG = HW01_ALERT_NONE;
hw01_timer_alerts_t ALERT_SW2_LONG = HW01_ALERT_NONE;
hw01_timer_alerts_t ALERT_SW3_LONG = HW01_ALERT_NONE;
hw01_timer_alerts_t ALERT_SW1_SHORT = HW01_ALERT_NONE;
hw01_timer_alerts_t ALERT_SW2_SHORT = HW01_ALERT_NONE;
hw01_timer_alerts_t ALERT_SW3_SHORT = HW01_ALERT_NONE;
hw01_timer_alerts_t ALERT_UPDATE_SECOND = HW01_ALERT_NONE;
hw01_timer_alerts_t ALERT_UPDATE_MINUTE = HW01_ALERT_NONE;
hw01_timer_alerts_t ALERT_TIME_BLINK = HW01_ALERT_NONE;
hw01_timer_alerts_t ALERT_ALARM_BLINK = HW01_ALERT_NONE;

/* Timer object and timer_cfg object */
cyhal_timer_t timer_obj;

cyhal_timer_cfg_t timer_cfg =
    {
        .compare_value = 0,              /* Timer compare value, not used */
        .period = 0,                     /* number of timer ticks */
        .direction = CYHAL_TIMER_DIR_UP, /* Timer counts up */
        .is_compare = false,             /* Don't use compare mode */
        .is_continuous = true,           /* Run timer indefinitely */
        .value = 0                       /* Initial value of counter */
};

/*****************************************************************************/
/*  Interrupt Handlers                                                       */
/*****************************************************************************/

/**
 * @brief
 *  Function used as Timer Handler
 */
void Handler_HW01_Timer(void *handler_arg, cyhal_timer_event_t event)
{
    CY_UNUSED_PARAMETER(handler_arg);
    CY_UNUSED_PARAMETER(event);
    /* Static Vars */
    // Tell us if the switch has been pressed
    volatile static uint8_t handler_called_sw1 = 0;
    volatile static uint8_t handler_called_sw2 = 0;
    volatile static uint8_t handler_called_sw3 = 0;

    // Tells us if a second has passed
    volatile static uint8_t handler_called_time_second = 0;
    
    // When it's in the start mode, blink the time
    if (handler_called_time_second < 5){
        ALERT_TIME_BLINK = HW01_ALERT_NONE;
    }
    else if (handler_called_time_second >= 5){
        ALERT_TIME_BLINK = HW01_ALERT_BLINK;
    }

    // If 10 ticks have passed (each tick is 0.1 sec, a second has passed)
    if (handler_called_time_second >= 9){
        ALERT_UPDATE_SECOND = HW01_ALERT_TIME_UPDATE;
    }

    // Increment the second checker
    handler_called_time_second = (handler_called_time_second + 1)%10;

    // Define the port where we get our buttonsa
    uint32_t buttons_reg_val = PORT_BUTTONS->IN;

    // Tells us if the button is being pressed (and how long)
    if (((buttons_reg_val & SW1_MASK) == 0)){
        handler_called_sw1 = handler_called_sw1 + 1;
    }

    // Upon release or not being pressed:
    else{
        // If SW1 was long-pressed, tell us that SW1 has been long-pressed
        if (handler_called_sw1 >= 20){
            printf("handler1: %d\n\r",handler_called_sw1);
            ALERT_SW1_LONG = HW01_ALERT_BUTTON_GT_2S;
        }
        // If SW1 was short-pressed, tell us it's been short-pressed
        else if (handler_called_sw1 > 0 && handler_called_sw1 < 20){
            printf("handler1: %d\n\r",handler_called_sw1);
            ALERT_SW1_SHORT = HW01_ALERT_BUTTON_LT_2S;
        }
        // If SW1 hasn't been pressed, clear the alerts
        else{
            ALERT_SW1_LONG = HW01_ALERT_NONE;
            ALERT_SW1_SHORT = HW01_ALERT_NONE;
        }
        // Reset the SW1 checker to 0
        handler_called_sw1 = 0;
    }

    // Tells us if SW2 is being pressed (and how long)
    if (((buttons_reg_val & SW2_MASK) == 0)){
        handler_called_sw2 = handler_called_sw2 + 1;
    }

    // Upon release or not being pressed:
    else{
        // If SW2 long-pressed, tell us
        if (handler_called_sw2 >= 20){
            printf("handler2: %d\n\r",handler_called_sw2);
            ALERT_SW2_LONG = HW01_ALERT_BUTTON_GT_2S;
        }
        // If SW2 short-pressed, tell us
        else if (handler_called_sw2 > 0 && handler_called_sw2 < 20){
            printf("handler2: %d\n\r",handler_called_sw2);
            ALERT_SW2_SHORT = HW01_ALERT_BUTTON_LT_2S;
        }
        // If SW2 not pressed, clear the alerts
        else{
            ALERT_SW2_LONG = HW01_ALERT_NONE;
            ALERT_SW2_SHORT = HW01_ALERT_NONE;
        }
        // Reset the SW2 checker
        handler_called_sw2 = 0;
    }

    // Tells us if SW3 is being pressed (and how long)
    if (((buttons_reg_val & SW3_MASK) == 0)){
        handler_called_sw3 = handler_called_sw3 + 1;
    }
    // Upon release or not being pressed:
    else{
        // Tell us if it's been long-pressed
        if (handler_called_sw3 >= 20){
            printf("handler3: %d\n\r",handler_called_sw3);
            ALERT_SW3_LONG = HW01_ALERT_BUTTON_GT_2S;
        }
        // Tell us if it's been short-pressed
        else if (handler_called_sw3 > 0 && handler_called_sw3 < 20){
            printf("handler3: %d\n\r",handler_called_sw3);
            ALERT_SW3_SHORT = HW01_ALERT_BUTTON_LT_2S;
        }
        // If it's not been pressed, reset the alerts
        else{
            ALERT_SW3_LONG = HW01_ALERT_NONE;
            ALERT_SW3_SHORT = HW01_ALERT_NONE;
        }
        handler_called_sw3 = 0;
    }
}

/*****************************************************************************/
/*  TCPWM Initialization Function                                            */
/*****************************************************************************/
bool timer_interrupt_flag = false; // tells us if an interrupt happened
void hw01_timer_init(uint32_t ticks)
{
    cy_rslt_t rslt; // declaring rslt to check if inits work
    timer_cfg.period = ticks;

    rslt = cyhal_timer_init(&timer_obj, NC, NULL); // initialize timer
    // CY_ASSERT(rslt = CY_RSLT_SUCCESS); // check init worked

    rslt = cyhal_timer_configure(&timer_obj, &timer_cfg); // timer configuration
    // CY_ASSERT(rslt = CY_RSLT_SUCCESS); // check config worked

    rslt = cyhal_timer_set_frequency(&timer_obj, 100000000); // set the timer frequency
    // CY_ASSERT(rslt = CY_RSLT_SUCCESS); // check freq set worked

    cyhal_timer_register_callback(&timer_obj,Handler_HW01_Timer,NULL); // Assign the ISR to execute on timer interrupt
    
    cyhal_timer_enable_event(&timer_obj, CYHAL_TIMER_IRQ_TERMINAL_COUNT,3, true); // Set the event on which timer interrupt occurs and enable it
    
    rslt = cyhal_timer_start(&timer_obj);
    // CY_ASSERT(rslt = CY_RSLT_SUCCESS);
}
