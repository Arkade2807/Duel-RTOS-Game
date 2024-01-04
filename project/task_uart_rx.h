/**
 * @file task_uart_tx.h
 * @author Lucas Franke (lmfranke@wisc.edu) & Aditya Kamasani (kamasani@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _TASK_UART_RX_H_
#define _TASK_UART_RX_H_

#include "main.h"


extern TaskHandle_t TaskHandle_UART_RX;
extern QueueHandle_t Queue_UART_RX;

void task_rx_init(void);

#endif