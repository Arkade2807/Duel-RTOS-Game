/**
 * @file joystick.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "joystick.h"

/* Static Global Variables */
static cyhal_adc_t joystick_adc_obj;
static cyhal_adc_channel_t joystick_adc_chan_x_obj;
static cyhal_adc_channel_t joystick_adc_chan_y_obj;
const cyhal_adc_channel_config_t channel_config = { .enable_averaging = false, .min_acquisition_ns = 220, .enabled = true };

const cyhal_adc_config_t joystic_config = {
    .continuous_scanning = true, 
    .resolution = 12, 
    .average_count = 1, 
    .average_mode_flags = 0,
    .ext_vref_mv = 0,
    .vneg = CYHAL_ADC_VNEG_VREF,
    .vref = CYHAL_ADC_REF_VDDA_DIV_2,
    .ext_vref = NC,
    .is_bypassed = false,
    .bypass_pin = NC
};

/* Public API */

/** Initialize the ADC channels connected to the Joystick
 *
 * @param - None
 */
void joystick_init(void)
{
    cy_rslt_t rslt;

    /* Initialize ADC.*/
    rslt = cyhal_adc_init(&joystick_adc_obj, PIN_ANALOG_JOY_X, NULL);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS); // If the initialization, halt the MCU

    /* Set the Reference Voltage to be VDDA */
    rslt = cyhal_adc_configure(&joystick_adc_obj, &joystic_config);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS); // If the initialization, halt the MCU


    /* Initialize X direction */
    rslt = cyhal_adc_channel_init_diff(
        &joystick_adc_chan_x_obj, 
        &joystick_adc_obj, 
        PIN_ANALOG_JOY_X, 
        CYHAL_ADC_VNEG, 
        &channel_config
    );
    CY_ASSERT(rslt == CY_RSLT_SUCCESS); // If the initialization, halt the MCU


    /* Initialize Y direction */
    rslt = cyhal_adc_channel_init_diff(
        &joystick_adc_chan_y_obj, 
        &joystick_adc_obj, 
        PIN_ANALOG_JOY_Y, 
        CYHAL_ADC_VNEG, 
        &channel_config
    );
    CY_ASSERT(rslt == CY_RSLT_SUCCESS); // If the initialization, halt the MCU

}

/** Read X direction of Joystick 
 *
 * @param - None
 */
uint16_t  joystick_read_x(void)
{
    /* ADD CODE */
    return cyhal_adc_read_u16(&joystick_adc_chan_x_obj);
    }

/** Read Y direction of Joystick 
 *
 * @param - None
 */
uint16_t  joystick_read_y(void)
{
    /* ADD CODE */
    return cyhal_adc_read_u16(&joystick_adc_chan_y_obj);
}


/* ADD CODE */
/**
 * @brief 
 * Returns the current position of the joystick 
 * @return joystick_position_t 
 */
joystick_position_t joystick_get_pos(void)
{
    uint16_t x_pos = joystick_read_x();
    uint16_t y_pos = joystick_read_y();

    // If the joystick is left and up
    if (x_pos > JOYSTICK_THRESH_X_LEFT_2P475V && y_pos > JOYSTICK_THRESH_Y_UP_2P475V){
        return JOYSTICK_POS_UPPER_LEFT;
    }
    // If the joystick is left and down
    else if (x_pos > JOYSTICK_THRESH_X_LEFT_2P475V && y_pos < JOYSTICK_THRESH_Y_DOWN_0P825V){
        return JOYSTICK_POS_LOWER_LEFT;
    }
    // If the joystick is just left
    else if (x_pos > JOYSTICK_THRESH_X_LEFT_2P475V){
        return JOYSTICK_POS_LEFT;
    }
    // If the joystick is right and up
    else if (x_pos < JOYSTICK_THRESH_X_RIGHT_0P825V && y_pos > JOYSTICK_THRESH_Y_UP_2P475V){
        return JOYSTICK_POS_UPPER_RIGHT;
    }
    // If the joystick is right and down
    else if (x_pos < JOYSTICK_THRESH_X_RIGHT_0P825V && y_pos < JOYSTICK_THRESH_Y_DOWN_0P825V){
        return JOYSTICK_POS_LOWER_RIGHT;
    }
    // If the joystick is just right
    else if (x_pos < JOYSTICK_THRESH_X_RIGHT_0P825V){
        return JOYSTICK_POS_RIGHT;
    }
    // If the joystick is just up
    else if (y_pos > JOYSTICK_THRESH_Y_UP_2P475V){
        return JOYSTICK_POS_UP;
    }
    else if (y_pos < JOYSTICK_THRESH_Y_DOWN_0P825V){
        return JOYSTICK_POS_DOWN;
    }
    return JOYSTICK_POS_CENTER;
}

/* ADD CODE */
/**
 * @brief 
 * Prints out a string to the console based on parameter passed 
 * @param position 
 * The enum value to be printed.
 */
void joystick_print_pos(joystick_position_t position)
{
    if (position == JOYSTICK_POS_RIGHT){
        printf("RIGHT\n\r");
    }
    else if (position == JOYSTICK_POS_UP){
        printf("UP\n\r");
    }
    else if (position == JOYSTICK_POS_LEFT){
        printf("LEFT\n\r");
    }
    else if (position == JOYSTICK_POS_DOWN){
        printf("DOWN\n\r");
    }
    else if (position ==  JOYSTICK_POS_UPPER_RIGHT){
        printf("UPPER RIGHT\n\r");
    }
    else if (position ==  JOYSTICK_POS_UPPER_LEFT){
        printf("UPPER LEFT\n\r");
    }
    else if (position ==  JOYSTICK_POS_LOWER_LEFT){
        printf("LOWER LEFT\n\r");
    }
    else if (position ==  JOYSTICK_POS_LOWER_RIGHT){
        printf("LOWER RIGHT\n\r");
    }
    else {
        printf("CENTER\n\r");
    }
}