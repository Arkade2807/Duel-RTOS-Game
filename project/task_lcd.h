/**
 * @file task_lcd.h
 * @author Lucas Franke (lmfranke@wisc.edu) & Aditya Kamasani (kamasani@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _TASK_LCD_H_
#define _TASK_LCD_H_

// yes we have to include all of these. no it's not efficient
#include "main.h"
#include "task_sw1.h"
#include "task_sw2.h"
#include "task_sw3.h"
#include "task_buzzer_victory.h"
#include "task_buzzer_damage.h"
#include "task_uart_rx.h"
#include "task_imu.h"
#include "task_ltr329.h"
#include "task_io_expander.h"
#include "task_eeprom.h"

#define Game_Won  0xFE
extern TaskHandle_t TaskHandle_LCD;

void task_lcd_init(void);

#endif