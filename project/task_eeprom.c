/**
 * @file task_eeprom.c
 * @author Lucas Franke (lmfranke@wisc.edu) & Aditya Kamasani (kamasani@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "task_eeprom.h"

TaskHandle_t TaskHandle_EEPROM;
extern QueueHandle_t Queue_IO_Expander;
QueueHandle_t Queue_EEPROM;
uint16_t address = 0x0000;

void task_eeprom(void *Parameters){

    while(1){
        vTaskDelay(100);
        // uint8_t reset_eeprom = 0;
        // xQueueReceive(Queue_SW3, &reset_eeprom, 1);
        // if (reset_eeprom == 1){
        //     printf("reset eeprom\n\r");
        //     eeprom_write_byte(address, 0x00);
        // }

        uint8_t our_health;
        
        // Waits to receive information from task_lcd
        BaseType_t received = xQueueReceive(Queue_EEPROM, &our_health, portMAX_DELAY);
        // If we get something from the lcd
        if (received == pdPASS){
            printf("we got %d from the queue in eeprom\n\r", our_health);
            // if we lost
            if (our_health == 0){
                // set our win streak to 0
                uint8_t lose_byte = 0x00;
                eeprom_write_byte(address, lose_byte);
                xQueueSend(Queue_IO_Expander, &lose_byte, portMAX_DELAY);
            }
            // if we get the reset byte from the lcd, set the info in the eeprom to 0
            else if (our_health == 0xEE){
                eeprom_write_byte(address, 0x00);
                printf("eeprom reset\n\r");
            }
            // otherwise
            else{
                // read in what's in the eeprom (our current streak)
                printf("trying to read byte\n\r");
                uint8_t read_byte = eeprom_read_byte(address);
                printf("read byte from eeprom: %d\n\r", read_byte);
                
                // increment our current streak %8 so that it has a max of 7
                read_byte = (read_byte + 1)%8;
                
                // send the new streak to the io expander
                xQueueSend(Queue_IO_Expander, &read_byte, portMAX_DELAY);

                // write the new streak to the eeprom
                eeprom_write_byte(address, read_byte);
            }
        }
        
    }
}

void task_eeprom_init(){
    /* Initialize the EEPROM CS line*/
    eeprom_cs_init();
    printf("initialized eeprom cs\n\r");

    Queue_EEPROM = xQueueCreate(1, sizeof(uint8_t));

    xTaskCreate(
        task_eeprom,
        "Task EEPROM",
        1024,
        NULL,
        3,
        &TaskHandle_EEPROM
    );
}