/**
 * @file task_sw3.h
 * @author Lucas Franke (lmfranke@wisc.edu) & Aditya Kamasani (kamasani@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _TASK_SW3_H_
#define _TASK_SW3_H_

#include "main.h"

extern TaskHandle_t TaskHandle_SW3;

extern QueueHandle_t Queue_SW3;

void task_sw3_init(void);

#endif