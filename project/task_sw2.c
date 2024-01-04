/**
 * @file task_sw2.c
 * @author Lucas Franke (lmfranke@wisc.edu) & Aditya Kamasani (kamasani@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "task_sw2.h"

TaskHandle_t TaskHandle_SW2;

QueueHandle_t Queue_SW2;

cyhal_gpio_callback_data_t sw2_callback_data;

void task_sw2(void *Parameters);

/*****************************************************************************/
/* Interrupt Handlers                                                        */
/*****************************************************************************/

/* Interrupt handler callback function */
void sw2_handler(void *handler_arg, cyhal_gpio_event_t event)
{
    BaseType_t xHigherPriorityTaskWoken;

    /* The xHigherPriorityTaskWoken parameter must be initialized to pdFALSE as
     it will get set to pdTRUE inside the interrupt safe API function if a
     context switch is required. */
    xHigherPriorityTaskWoken = pdFALSE;

    /* ADD CODE */
    /* Send a notification directly to the task to which interrupt processing
     * is being deferred.
     */
    vTaskNotifyGiveFromISR(TaskHandle_SW2, &xHigherPriorityTaskWoken);

    /* ADD CODE */
    /* Call the function that will force the task that was running prior to the
     * interrupt to yield*/
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/*****************************************************************************/
/* Peripheral Initialization                                                 */
/*****************************************************************************/
void sw2_irq_enable(void)
{
    // push_buttons_init();
    sw2_callback_data.callback = sw2_handler;
    cyhal_gpio_register_callback(PIN_SW2, &sw2_callback_data);
    cyhal_gpio_enable_event(PIN_SW2, CYHAL_GPIO_IRQ_FALL, 3, true);
}

/**
 * @brief This task just takes a task notification from the ISR and sends to a queue that it's been pressed
 * 
 * @param Parameters 
 */
void task_sw2(void *Parameters){
    static uint8_t sw2_current = 0;
    static uint8_t sw2_previous = 0;

    while(1){

         ulTaskNotifyTake(true, portMAX_DELAY);

        /* Check the current state of the button */
        // if ((PORT_BUTTONS->IN & SW1_MASK) == 0x00)
        // {
        //     sw1_current = 1;
        // }
        // else
        // {
        //     sw1_current = 0;
        // }

         uint8_t sw2_pressed = 0;
        // /* Detect if the button has been pressed */
        // if (sw1_current == 0 && sw1_previous == 1)
        // {   
        //     sw1_pressed = 1;
        //     // printf("SW1 pressed\n");
        // }

        sw2_pressed =1;
        xQueueSend(Queue_SW2, &sw2_pressed, portMAX_DELAY);
        /* Update the previous button state */
        sw2_previous = sw2_current;
        vTaskDelay(30);
    }
}

/**
 * @brief 
 * 
 */
void task_sw2_init(){

    // push_buttons_init();
    sw2_irq_enable();

    Queue_SW2 = xQueueCreate(1, sizeof(uint8_t));

    xTaskCreate(
        task_sw2,
        "Task SW2",
        configMINIMAL_STACK_SIZE,
        NULL,
        4,
        &TaskHandle_SW2
    );
}