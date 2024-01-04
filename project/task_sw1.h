/**
 * @file task_sw1.h
 * @author Lucas Franke (lmfranke@wisc.edu) & Aditya Kamasani (kamasani@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-13s
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _TASK_SW1_H_
#define _TASK_SW1_H_

#include "main.h"

extern TaskHandle_t TaskHandle_SW1;

extern QueueHandle_t Queue_SW1;

void task_sw1_init(void);

#endif