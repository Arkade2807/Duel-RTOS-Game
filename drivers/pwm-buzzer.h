/**
 * @file pwm-buzzer.h
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-09-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef ECE353_PWM_BUZZER_H_
#define ECE353_PWM_BUZZER_H_

#include <stdint.h>
#include <stdbool.h>
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

#define PIN_IO_BUZZER           P6_2 // Set the buzzer to the proper pin

#define PWM_BUZZER_FREQ			2500 // Set the buzzer frequency to 2.5 kHz
#define PWM_BUZZER_DUTY			50 // Set the buzzer duty cycle to 50%

#define E4                      330

#define D4                      294

#define C4                      262

#define D_SHARP_4               311

#define B_FLAT_5                466

void  pwm_buzzer_init(void);
void  pwm_buzzer_start(void);
void  pwm_buzzer_stop(void);

#endif /* ECE353_PWM_BUZZER_H_ */
