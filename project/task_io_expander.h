/**
 * @file task_io_expander.h
 * @author Lucas Franke (lmfranke@wisc.edu) & Aditya Kamasani (kamasani@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _TASK_IO_EXPANDER_H_
#define _TASK_IO_EXPANDER_H_

#include "main.h"

extern TaskHandle_t TaskHandle_IO_Expander;
extern QueueHandle_t Queue_IO_Expander;

void task_ltr329_init(void);

#endif