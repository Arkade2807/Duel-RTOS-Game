/**
 * @file project.c
 * @author Lucas Franke (lmfranke@wisc.edu) & Aditya Kamasani (kamasani@wisc.edu)

 * @brief 
 * @version 0.1
 * @date 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "project.h"

char PROJECT_DESCRIPTION[] = "Final ECE 353 Project for Lucas Franke and Aditya Kamasani";

/**
 * @brief 
 * This function will initialize all of the hardware resources for
 * the project.  
 * 
 */
void project_peripheral_init(void){
    /* Init I2C and SPI*/
    i2c_init();

    spi_init();

    /* Init EEPROM */
    task_eeprom_init();

    /* Init Light Sensor */
    task_ltr329_init();

    /* Init the buttons */
    push_buttons_init();
    task_sw1_init();
    task_sw2_init();
    task_sw3_init();

    /* Init IMU */
    task_imu_init();

    /* Init the LCD */
    task_lcd_init();

    /* Init the Buzzer */
    pwm_buzzer_init();
    task_buzzer_damage_init();
    task_buzzer_vic_init();

    /* Init UART */
    remote_uart_init();

    task_rx_init();

    /* Init IO Expander */
    task_io_expander_init();
}

/*****************************************************************************/
/* Application Code                                                          */
/*****************************************************************************/
/**
 * @brief 
 * This function implements the behavioral requirements for the ICE
 * 
 * This function is implemented in the iceXX.c file for the ICE you are 
 * working on.
 */
void project_main_app(void){
    vTaskStartScheduler();

    while(1){
        
    }
}