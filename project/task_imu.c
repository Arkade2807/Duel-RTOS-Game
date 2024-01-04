/**
 * @file task_imu.c
 * @author Lucas Franke (lmfranke@wisc.edu) & Aditya Kamasani (kamasani@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "task_imu.h"

TaskHandle_t TaskHandle_IMU;

void task_imu(void *Parameters){

    while(1){
        
        // this function just reads in the accelerometer data from the imu and sends it to a queue to be received by the lcd
        lsm6dsm_orientation1();


        vTaskDelay(5);
        
    }
}

void task_imu_init(){

    xTaskCreate(
        task_imu,
        "Task IMU",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        &TaskHandle_IMU
    );
}