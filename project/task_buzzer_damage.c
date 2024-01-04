/**
 * @file task_buzzer_damage.c
 * @author Lucas Franke (lmfranke@wisc.edu) & Aditya Kamasani (kamasani@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "task_buzzer_damage.h"

TaskHandle_t TaskHandle_Buzzer_Damage;

void task_buzzer_damage(void *Parameters){

    while(1){
		
    // Gets a task notification and plays the damage sound. only happens when we get shot
        
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        pwm_damage(); 
    //     vTaskDelay(50);
    //     pwm_buzzer_stop();
        
    }
}


void task_buzzer_damage_init(){



	 xTaskCreate(
        task_buzzer_damage,
        "Task Buzzer",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &TaskHandle_Buzzer_Damage
    );
}