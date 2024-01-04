/**
 * @file task_buzzer_victory.c
 * @author Lucas Franke (lmfranke@wisc.edu) & Aditya Kamasani (kamasani@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "task_buzzer_victory.h"

TaskHandle_t TaskHandle_Buzzer_Victory;

void pwm_buzzer_victory(){
	pwm_buzzer_stop();
	pwm_buzzer_freq_set(E4);

	pwm_buzzer_start();
	vTaskDelay(100);

	pwm_buzzer_stop();
	vTaskDelay(50);

	pwm_buzzer_start();
	vTaskDelay(100);

	pwm_buzzer_stop();
	vTaskDelay(50);

	pwm_buzzer_start();
	vTaskDelay(100);

	pwm_buzzer_stop();
	vTaskDelay(50);

	pwm_buzzer_start();
	vTaskDelay(300);

	pwm_buzzer_stop();
	pwm_buzzer_freq_set(C4);
	vTaskDelay(50);

	pwm_buzzer_start();
	vTaskDelay(300);

	pwm_buzzer_stop();
	pwm_buzzer_freq_set(D4);
	vTaskDelay(50);

	pwm_buzzer_start();
	vTaskDelay(300);

	pwm_buzzer_stop();
	pwm_buzzer_freq_set(E4);
	vTaskDelay(50);

	pwm_buzzer_start();
	vTaskDelay(200);

	pwm_buzzer_stop();
	pwm_buzzer_freq_set(D4);
	vTaskDelay(50);

	pwm_buzzer_start();
	vTaskDelay(120);

	pwm_buzzer_stop();
	pwm_buzzer_freq_set(E4);
	vTaskDelay(50);

	pwm_buzzer_start();
	vTaskDelay(400);

	pwm_buzzer_stop();   
}


void task_buzzer_victory(void *Parameters){

    while(1){
		
		// gets a task notification and plays the victory sound. only happens when we win
        
ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        pwm_buzzer_victory(); 
        
    }
}



void task_buzzer_vic_init(){

    xTaskCreate(
        task_buzzer_victory,
        "Task Buzzer",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &TaskHandle_Buzzer_Victory
    );


}