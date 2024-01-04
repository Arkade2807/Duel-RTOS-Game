/**
 * @file task_uart_tx.c
 * @author Lucas Franke (lmfranke@wisc.edu) & Aditya Kamasani (kamasani@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "task_uart_rx.h"

TaskHandle_t TaskHandle_UART_RX;
QueueHandle_t Queue_UART_RX;

void task_rx(void *Parameters){

    while(1){
        // /* Sleep for 50mS -- DO NOT use any cyhal_ functions to delay */
        uint8_t char_read[1];

        // reads a char from the uart
        bool successful_read = remote_uart_rx_char_polling(char_read);

        // sends to the uart queue if the read was successful
        if (successful_read){
            xQueueSend(Queue_UART_RX, &char_read, portMAX_DELAY);
        }

        vTaskDelay(1);
        
    }
}

void task_rx_init(){
    Queue_UART_RX = xQueueCreate(1, sizeof(uint8_t));

     xTaskCreate(
        task_rx,
        "Task RX",
        configMINIMAL_STACK_SIZE,
        NULL,
        4,
        &TaskHandle_UART_RX
    );
}
