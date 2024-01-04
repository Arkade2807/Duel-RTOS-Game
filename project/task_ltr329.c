/**
 * @file task_ltr329.c
 * @author Lucas Franke (lmfranke@wisc.edu) & Aditya Kamasani (kamasani@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "task_ltr329.h"

TaskHandle_t TaskHandle_LTR329;
QueueHandle_t Queue_LTR329;

void task_ltr329(void *Parameters){
    static uint8_t light_sensor_value = 0;

    while(1){
        /* Sleep for 150mS */
        vTaskDelay(150);

        // read in the value given by the light sensor and send it to the queue to be read by the lcd
        light_sensor_value = LTR329_get_value();

        xQueueSend(Queue_LTR329, &light_sensor_value, portMAX_DELAY);

        // printf("light sensor value: %d\n\r", light_sensor_value);
    }
}

void task_ltr329_init(){
    LTR329_init();

    Queue_LTR329 = xQueueCreate(1, sizeof(uint8_t));

    xTaskCreate(
        task_ltr329,
        "Task LTR329",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &TaskHandle_LTR329
    );
}