/**
 * @file task_imu.h
 * @author Lucas Franke (lmfranke@wisc.edu) & Aditya Kamasani (kamasani@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _TASK_IMU_H_
#define _TASK_IMU_H_

#include "main.h"
#include "imu.h"

extern TaskHandle_t TaskHandle_IMU;
extern QueueHandle_t QueueIMU;

void task_imu_init(void);

#endif