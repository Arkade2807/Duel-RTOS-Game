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

#ifndef _TASK_EEPROM_H_
#define _TASK_EEPROM_H_

#include "main.h"
#include "task_sw3.h"

extern TaskHandle_t TaskHandle_EEPROM;
extern uint16_t address;
extern QueueHandle_t Queue_EEPROM;

void task_eeprom_init(void);

#endif