/**
 * @file pwm-buzzer.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "pwm-buzzer.h"

cyhal_pwm_t pwm_buzzer_obj;

// Initializes the PWM
void pwm_buzzer_init(void)
{
	cy_rslt_t rslt;
    // Initialize PWM on the supplied pin and assign a new clock
	cyhal_pwm_init(&pwm_buzzer_obj, PIN_IO_BUZZER, NULL); // initialize the pwm on the buzzer pin
	// CY_ASSERT(rslt==CY_RSLT_SUCCESS); // check if the initialization works
	rslt = cyhal_pwm_set_duty_cycle(&pwm_buzzer_obj, PWM_BUZZER_DUTY, PWM_BUZZER_FREQ);
	// CY_ASSERT(rslt = CY_RSLT_SUCCESS);
}

// Start the buzzer
void pwm_buzzer_start(void)
{
	cy_rslt_t rslt;
    rslt = cyhal_pwm_start(&pwm_buzzer_obj);
}

// Stop the buzzer
void pwm_buzzer_stop(void)
{
	cy_rslt_t rslt;
    rslt = cyhal_pwm_stop(&pwm_buzzer_obj);
}

void pwm_buzzer_freq_set(uint16_t freq){
	cyhal_pwm_set_duty_cycle(&pwm_buzzer_obj, PWM_BUZZER_DUTY, freq);
}
void pwm_damage(){
	pwm_buzzer_stop();
	pwm_buzzer_freq_set(D_SHARP_4);

	pwm_buzzer_start();
	vTaskDelay(100);
	pwm_buzzer_stop();

	pwm_buzzer_freq_set(B_FLAT_5);

	pwm_buzzer_start();
	vTaskDelay(100);
	pwm_buzzer_stop();

	pwm_buzzer_freq_set(D_SHARP_4);

	pwm_buzzer_start();
	vTaskDelay(100);
	pwm_buzzer_stop();

	pwm_buzzer_freq_set(B_FLAT_5);

	pwm_buzzer_start();
	vTaskDelay(100);
	pwm_buzzer_stop();



	pwm_buzzer_stop();
}



