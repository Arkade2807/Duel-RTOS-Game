/**
 * @file ice02.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-08-25
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../main.h"

#if defined(ICE) && (FILE_ID == 2)

/*****************************************************************************/
/* Macros                                                                    */
/*****************************************************************************/

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/
char ICE_DESCRIPTION[] = "ECE353: ICE 02 - Memory Mapped IO";

/*****************************************************************************/
/* Function Declarations                                                     */
/*****************************************************************************/
bool is_button_pressed(uint32_t button_mask);

/*****************************************************************************/
/* Function Definitions                                                      */
/*****************************************************************************/

/**
 * @brief
 * This function returns a true if the specified button was pressed and then released.
 *
 * ALL of your code MUST be added inside this function.
 *
 * Each time you call this function, read the IN register a single time and examine the specified bit to
 * see if the button is being pressed.
 *
 * Compare the current state of the button with the previous state of the button to determine if the
 * button was pressed and then released.
 * (i.e current reading is not pressed AND previous reading was pressed, then return true)
 *
 * You can make use of static variables to keep track of the previous value of each push button.
 * Assume that you are only checking 1 button at a time.
 *
 * @param button_mask
 * A bit mask used to identify which bit of the IN register you are checking.  If the bitmask does not match
 * one of the 3 valid bitmasks, return false.
 * @return true
 * The button was pressed an then released
 * @return false
 * The button has not been released
 */
bool is_button_pressed(uint32_t button_mask)
{
    uint32_t reg_val;
    reg_val = REG_PUSH_BUTTON_IN;
    static bool previous1;
    bool current1;
    if (button_mask == SW1_MASK){
        if ((reg_val & button_mask) == 0){
            current1 = true;
        }
        else{
            current1 = false;
        }
        if (!current1 && previous1){
            previous1 = current1;
            return true;
        }
        previous1 = current1;
        return false;
    }
    static bool previous2;
    bool current2;
    if (button_mask == SW2_MASK){
        if ((reg_val & button_mask) == 0){
            current2 = true;
        }
        else{
            current2 = false;
        }
        if (!current2 && previous2){
            previous2 = current2;
            return true;
        }
        previous2 = current2;
        return false;
    }
    static bool previous3;
    bool current3;
    if (button_mask == SW3_MASK){
        if ((reg_val & button_mask) == 0){
            current3 = true;
        }
        else{
            current3 = false;
        }
        if (!current3 && previous3){
            previous3 = current3;
            return true;
        }
        previous3 = current3;
        return false;
    }

    return false;
}


/**
 * @brief
 * This function will initialize all of the hardware resources for
 * the ICE
 */
void peripheral_init(void)
{
    push_buttons_init();
}

/**
 * @brief
 * This function implements the behavioral requirements for the ICE
 */
void main_app(void)
{
    while (1)
    {
        if (is_button_pressed(SW1_MASK))
        {
            printf("SW1 was pressed\n\r");
        }

        if (is_button_pressed(SW2_MASK))
        {
            printf("SW2 was pressed\n\r");
        }

        if (is_button_pressed(SW3_MASK))
        {
            printf("SW3 was pressed\n\r");
        }

        /* Delay 50mS before checking the buttons again*/
        cyhal_system_delay_ms(50);
    }
}
#endif