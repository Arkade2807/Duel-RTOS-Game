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

#if defined(ICE) && (FILE_ID == 15)

char ICE_DESCRIPTION[] = "ECE353: ICE 15 - FreeRTOS";

TaskHandle_t Task_Handle_Button_SW1 = NULL;
TaskHandle_t Task_Handle_Button_SW2 = NULL;

/*****************************************************************************/
/* Function Declarations                                                     */
/*****************************************************************************/
void task_button_sw1(void *pvParameters);
void task_button_sw2(void *pvParameters);

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
    /* Initialize the LEDs that will be controlled by the FreeRTOS Tasks*/
    leds_init();

    /* Initialize the buttons */
    push_buttons_init();

    /* Initialize the Buzzer */
    pwm_buzzer_init();
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
    /* ADD CODE */
    /* Create SW1 Task */
    xTaskCreate(
        task_button_sw1,
        "SW1 Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        NULL
    );

    /* ADD CODE */
    /* Create SW1 Task */
    xTaskCreate(
        task_button_sw2,
        "SW2 Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        NULL
    );

    /* Start the Scheduler */
    vTaskStartScheduler();

    while (1)
    {
    };
}

/**
 * @brief
 * This task will turn the buzzer on/off by detecting when SW1 has been pressed. This task
 * should sleep for 50mS.  When it is active, check the current state of SW1 and compare it with
 * the previous value of SW1.  If you detect that the button has been pressed, then toggle the
 * buzzer On/Off
 * @param pvParameters
 */
void task_button_sw1(void *pvParameters)
{
    /* ADD CODE */
    /* Allocate any local variables used in this task */
    static uint8_t sw1_current = 0;
    static uint8_t sw1_previous = 0;
    static uint8_t buzzer_on = 0;

    while (1)
    {
        /* ADD CODE */
        /* Sleep for 50mS -- DO NOT use any cyhal_ functions to delay */
        vTaskDelay(50);

        /* ADD CODE */
        /* Check the current state of the button */
        if ((PORT_BUTTONS->IN & SW1_MASK) == 0x00)
        {
            sw1_current = 1;
        }
        else
        {
            sw1_current = 0;
        }

        /* ADD CODE */
        /* Detect if the button has been pressed */
        if (sw1_current == 0 && sw1_previous == 1)
        {
            /* ADD CODE */
            /* Determine if the buzzer should be started or stopped*/
            if (buzzer_on == 0){
                buzzer_on = 1;
                pwm_buzzer_start();
            }
            else {
                buzzer_on = 0;
                pwm_buzzer_stop();
            }
        }

        /* ADD CODE */
        /* Update the previous button state */
        sw1_previous = sw1_current;
    }
}

/**
 * @brief
 * This task will turn the green on/off by detecting when SW2 has been pressed. This task
 * should sleep for 50mS.  When it is active, check the current state of SW1 and compare it with
 * the previous value of SW2.  If you detect that the button has been pressed, then toggle the
 * Green LED On/Off
 * @param pvParameters
 */

void task_button_sw2(void *pvParameters)
{
    /* ADD CODE */
    /* Allocate any local variables used in this task */
    static uint8_t sw2_current = 0;
    static uint8_t sw2_previous = 0;
    static uint8_t green_led_on = 0;

    while (1)
    {
        /* ADD CODE */
        /* Sleep for 50mS -- DO NOT use any cyhal_ functions to delay */
        vTaskDelay(50);

        /* ADD CODE */
        /* Check the current state of the button */
        if ((PORT_BUTTONS->IN & SW2_MASK) == 0x00)
        {
            sw2_current = 1;
        }
        else
        {
            sw2_current = 0;
        }

        /* ADD CODE */
        /* Detect if the button has been pressed */
        if (sw2_current == 0 && sw2_previous == 1)
        {
            /* ADD CODE */
            /* Toggle the Green LED */
            if (green_led_on == 0){
                green_led_on = 1;
                PORT_RGB_GRN->OUT_SET = MASK_RGB_GRN;
            }
            else{
                green_led_on = 0;
                PORT_RGB_GRN->OUT_CLR = MASK_RGB_GRN;
            }
        }

        /* ADD CODE */
        /* Update the previous button state */
        sw2_previous = sw2_current;
    }
}

//*****************************************************************************
//
//! \brief Application defined malloc failed hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationMallocFailedHook()
{
    /* Handle Memory Allocation Errors */
    while (1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    // Handle FreeRTOS Stack Overflow
    while (1)
    {
    }
}

#endif