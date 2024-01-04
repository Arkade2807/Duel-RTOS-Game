/**
 * @file task_io_expander.c
 * @author Lucas Franke (lmfranke@wisc.edu) & Aditya Kamasani (kamasani@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "task_io_expander.h"

TaskHandle_t TaskHandle_IO_Expander;
QueueHandle_t Queue_IO_Expander;

/**
 * @brief All this task does is decide how many leds to turn on based on what it receives from the queue
 * 
 * @param Parameters 
 */
void task_io_expander(void *Parameters){
    static uint8_t number_leds_on = 7;

    while(1){
        
        vTaskDelay(50);

        xQueueReceive(Queue_IO_Expander, &number_leds_on, 0);

        switch(number_leds_on){
            case(7):
                io_expander_set_output_port(0x7F);
                break;
            case(6):
                io_expander_set_output_port(0x7E);
                break;
            case(5):
                io_expander_set_output_port(0x7C);
                break;
            case(4):
                io_expander_set_output_port(0x78);
                break;
            case(3):
                io_expander_set_output_port(0x70);
                break;
            case(2):
                io_expander_set_output_port(0x60);
                break;
            case(1):
                io_expander_set_output_port(0x40);
                break;
            case(0):
                io_expander_set_output_port(0x00);
                break;

        }

    }
}

void task_io_expander_init(){

    io_expander_set_configuration(0x80); 

    Queue_IO_Expander = xQueueCreate(1, sizeof(uint8_t));

    xTaskCreate(
        task_io_expander,
        "Task IO Expander",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &TaskHandle_IO_Expander
    );
}