/**
 * @file ice16.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-11-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../main.h"

#if defined(ICE) && (FILE_ID == 16)

char ICE_DESCRIPTION[] = "ECE353: ICE 16 - FreeRTOS Semaphores";

/*****************************************************************************/
/*  FreeRTOS Handles
/*****************************************************************************/
TaskHandle_t Task_Handle_Button_SW1 = NULL;
TaskHandle_t Task_Handle_Button_SW2 = NULL;
TaskHandle_t Task_Handle_RGB_LED = NULL;
TaskHandle_t Task_Handle_Buzzer = NULL;

/* ADD CODE */
/* Allocate a semaphore that will be used to control the RGB LED */
SemaphoreHandle_t Sem_RGB;

/* ADD CODE */
/* Allocate a semaphore that will be used to control the Buzzer */
SemaphoreHandle_t Sem_Buzzer;

/*****************************************************************************/
/* Function Declarations                                                     */
/*****************************************************************************/
void task_button_sw1(void *pvParameters);
void task_button_sw2(void *pvParameters);
void task_RGB_LED(void *pvParameters);
void task_buzzer(void *pvParameters);

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/

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
    /* Initialize the RGB LEDs */
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
    /* Create Sempaphores */
    Sem_RGB = xSemaphoreCreateBinary();
    Sem_Buzzer = xSemaphoreCreateBinary();

    xSemaphoreGive(Sem_RGB);
    xSemaphoreGive(Sem_Buzzer);

    /* Create SW1 Task */
    xTaskCreate(task_button_sw1,
                "SW1 Task",
                configMINIMAL_STACK_SIZE,
                NULL,
                1,
                &Task_Handle_Button_SW1);

    /* Create SW1 Task */
    xTaskCreate(task_button_sw2,
                "SW2 Task",
                configMINIMAL_STACK_SIZE,
                NULL,
                1,
                &Task_Handle_Button_SW2);

    /* Create RGB LED Task */
    xTaskCreate(task_RGB_LED,
                "RGB Task",
                configMINIMAL_STACK_SIZE,
                NULL,
                1,
                &Task_Handle_RGB_LED);

    /* Create Buzzer Task */
    xTaskCreate(task_buzzer,
                "Buzzer Task",
                configMINIMAL_STACK_SIZE,
                NULL,
                1,
                &Task_Handle_Buzzer);

    /* Start the Scheduler */
    vTaskStartScheduler();

    while (1)
    {
    };
}

/**
 * @brief
 * This task will monitor SW1.  When SW1 is pressed, the task will give a 
 * semeaphore to the RGB LED task to update the currently acitve color.
 *  @param pvParameters
 */
void task_button_sw1(void *pvParameters)
{
    /* Allocate any local variables used in this task */
    bool state_button_prev = false;
    bool state_button_curr = false;

    while (1)
    {
        /* Sleep for 50mS -- DO NOT use any cyhal_ functions to delay */
        vTaskDelay(50);

        /* Check the current state of the button */
        if ((PORT_BUTTONS->IN & SW1_MASK) == 0x00)
        {
            state_button_curr = true;
        }
        else
        {
            state_button_curr = false;
        }

        /* Detect if the button has been pressed */
        if (state_button_curr && !state_button_prev)
        {
            /* ADD CODE */
            /* Give the semaphore to the RGB Task */
            xSemaphoreGive(Sem_RGB);

            printf("SW1 Detected \n\r");
        }

        /* Update the previous button state */
        state_button_prev = state_button_curr;
    }
}

/**
 * @brief
 * This task will detect when SW2 is pressed.  When SW2 is pressed, it will give a semaphore
 * to the Buzzer task indicating that the frequency of the buzzer should be updated.
 * @param pvParameters
 */
void task_button_sw2(void *pvParameters)
{
    /* Allocate any local variables used in this task */
    bool state_button_prev = false;
    bool state_button_curr = false;

    while (1)
    {
        /* Sleep for 50mS -- DO NOT use any cyhal_ functions to delay */
        vTaskDelay(50);

        /* Check the current state of the button */
        if ((PORT_BUTTONS->IN & SW2_MASK) == 0x00)
        {
            state_button_curr = true;
        }
        else
        {
            state_button_curr = false;
        }

        /* Detect if the button has been pressed */
        if (state_button_curr && !state_button_prev)
        {
            /* ADD CODE */
            /* Give the semaphore to the Buzzer Task */
            xSemaphoreGive(Sem_Buzzer);
            
            printf("SW2 Detected \n\r");
        }

        /* ADD CODE */
        /* Update the previous button state */
        state_button_prev = state_button_curr;
    }
}

/**
 * @brief 
 * Controls the color displayed on the RGB LED.  The active color changes everytime 
 * the taks successfully takes a semaphore 
 * @param pvParameters 
 */
void task_RGB_LED(void *pvParameters)
{
    uint8_t color = 0;

    /* Turn all of the LEDs off */
    PORT_RGB_RED->OUT_CLR = MASK_RGB_RED;
    PORT_RGB_GRN->OUT_CLR = MASK_RGB_GRN;
    PORT_RGB_BLU->OUT_CLR = MASK_RGB_BLU;

    while (1)
    {
        /* ADD CODE */
        /* Take the RGB Color Semaphore */
        xSemaphoreTake(Sem_RGB, portMAX_DELAY);

        /* ADD CODE to cycle through the colors.  The colors
         * should change everytime a semaphore is successfully
         * taken.  
         * 
         * All OFF->Red     Semaphore Taken
         * Red->Green       Semaphore Taken
         * Green->Blue      Semaphore Taken
         * Blue->ALL OFF    Semaphore Taken
         * All OFF->red     Semaphore Taken
         * Repeat...
         * 
         */
        if (color == 0){
            PORT_RGB_RED->OUT_CLR = MASK_RGB_RED;
            PORT_RGB_GRN->OUT_CLR = MASK_RGB_GRN;
            PORT_RGB_BLU->OUT_CLR = MASK_RGB_BLU;
        }
        else if (color == 1){
            PORT_RGB_RED->OUT_SET = MASK_RGB_RED;
            PORT_RGB_GRN->OUT_CLR = MASK_RGB_GRN;
            PORT_RGB_BLU->OUT_CLR = MASK_RGB_BLU;
        }
        else if (color == 2){
            PORT_RGB_RED->OUT_CLR = MASK_RGB_RED;
            PORT_RGB_GRN->OUT_SET = MASK_RGB_GRN;
            PORT_RGB_BLU->OUT_CLR = MASK_RGB_BLU;
        }
        else if (color == 3){
            PORT_RGB_RED->OUT_CLR = MASK_RGB_RED;
            PORT_RGB_GRN->OUT_CLR = MASK_RGB_GRN;
            PORT_RGB_BLU->OUT_SET = MASK_RGB_BLU;
        }
        color = (color+1)%4;

        printf("Changing Color... ");
    }
}
/**
 * @brief 
 *  The task controls the frequency of the buzzer.  The frequency of the buzzer changes everytime
 *  the task sucessfully takes a semaphore. 
 * @param pvParameters 
 */
void task_buzzer(void *pvParameters)
{
    uint16_t buzz_freq = 0;

    /* Stop the buzzer */
    pwm_buzzer_stop();
    while (1)
    {
        /* ADD CODE */
        /* wait for a semaphore to change the state of the buzzer */
        xSemaphoreTake(Sem_Buzzer, portMAX_DELAY);

        /* ADD CODE */
        /* The buzzer should start OFF.  Each time a semaphore is taken, increment the current
         * frequency of the buzzer by 1Khz.  The supported frequencies are 0, 1Khz, 2Khz, 3Khz.
         *
         * 0 Hz         Semaphore Taken
         * 1000 Hz      Semaphore Taken
         * 2000 Hz      Semaphore Taken
         * 3000 Hz      Semaphore Taken
         * 0 Hz         Semaphore Taken
         * Repeat...
         * 
         * */
        uint16_t frequency = buzz_freq * 1000;
        pwm_buzzer_stop();
        pwm_buzzer_freq_set(frequency);
        if (buzz_freq == 0){
            pwm_buzzer_stop();
        }
        else{
            pwm_buzzer_start();
        }

        printf("Changing Freq...%d\n\r", buzz_freq);
        buzz_freq = (buzz_freq + 1)%4;

        // Victory mode
        // if (buzz_freq == 0){
        //     pwm_buzzer_stop();
        //     buzz_freq = 1;
        // }
        // else{
        //     pwm_buzzer_victory(); 
        // }

         



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