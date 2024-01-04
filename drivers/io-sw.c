/**
 * @file io-sw.c
 * @author Lucas Franke (lmfranke@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-09-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "io-sw.h"

/*****************************************************************************/
/* ICE 02 START */
/*****************************************************************************/
void push_buttons_init(void)
{
    cy_rslt_t rslt1;
    cy_rslt_t rslt2;
    cy_rslt_t rslt3;

    rslt1 = cyhal_gpio_init(PIN_SW1, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, 0);
    rslt2 = cyhal_gpio_init(PIN_SW2, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, 0);
    rslt3 = cyhal_gpio_init(PIN_SW3, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, 0);
    CY_ASSERT(rslt1==CY_RSLT_SUCCESS && rslt2==CY_RSLT_SUCCESS && rslt3==CY_RSLT_SUCCESS);
} 
/*****************************************************************************/
/* ICE 02 END */
/*****************************************************************************/


/*****************************************************************************/
/* ICE 03 START */
/*****************************************************************************/
/**
 * @brief
 * Returns which button has been pressed and then released. 
 *  
 * This function should examine the current state of each button and
 * compare it with the previous state of the button.  If the current
 * state is not-pressed and the previous state was pressed, return that
 * the button has been released.
 * 
 * If none of the buttons have been released, check to see if the button is 
 * currently being pressed.  If it is, return that the button is being pressed
 * 
 * If multiple buttons are pressed/released, return the button state of the button
 * with the highest SW number (SW3 has highest priority, then SW2, then SW1)
 * 
 * This function should only read the IN register once.
 * 
 * @return button_state_t 
 * 
 * Returns which of the buttons is currently being pressed.  
 */
button_state_t get_buttons(void)
{
    static bool previous1;
    bool current1 = false;
    if ((PORT_BUTTONS->IN & SW1_MASK)==0){
        current1 = true;
    }
    else{
        current1 = false;
    }

    static bool previous2 = false;
    bool current2 = false;
    if ((PORT_BUTTONS->IN & SW2_MASK)==0){
        current2 = true;
    }
    else{
        current2 = false;
    }

    static bool previous3 = false;
    bool current3 = false;
    if ((PORT_BUTTONS->IN & SW3_MASK)==0){
        current3 = true;
    }
    else{
        current3 = false;
    }

    if (previous3 && !current3){
        previous1 = current1;
        previous2 = current2;
        previous3 = current3;
        return BUTTON_SW3_RELEASED;
    }

    if (previous2 && !current2){
        previous1 = current1;
        previous2 = current2;
        previous3 = current3;
        return BUTTON_SW2_RELEASED;
    }

    if (previous1 && !current1){
        previous1 = current1;
        previous2 = current2;
        previous3 = current3;
        return BUTTON_SW1_RELEASED;
    }

    if (current3 && !previous3){
        previous1 = current1;
        previous2 = current2;
        previous3 = current3;
        return BUTTON_SW3_PRESSED;
    }

    if (current2 && !previous2){
        previous1 = current1;
        previous2 = current2;
        previous3 = current3;
        return BUTTON_SW2_PRESSED;
    }

    if (current1 && !previous1){
        previous1 = current1;
        previous2 = current2;
        previous3 = current3;
        return BUTTON_SW1_PRESSED;
    }

    previous1 = current1;
    previous2 = current2;
    previous3 = current3;
    return BUTTON_INACTIVE;
}

/*****************************************************************************/
/* ICE 03 END */
/*****************************************************************************/