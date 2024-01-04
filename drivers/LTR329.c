/**
 * @file LTR329.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-09-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "i2c.h"
#include "LTR329.h"

void LTR329_init(void){

	uint8_t write_init[2] = {LTR329_REG_CONTR, 0x01};

	/* Use cyhal_i2c_master_write to write the required data to the device. 
	 * Send the register address and then generate a stop condition 
	 */
	cyhal_i2c_master_write(&i2c_monarch_obj, LTR329_SUBORDINATE_ADDR, write_init, 2, 0, true);
}

/** Read a register on the LTR329
 *
 * @param reg The reg address to read
 *
 */
static uint8_t LTR329_read_reg(uint8_t reg)
{
	uint8_t return_value;
	cy_rslt_t rslt;

	uint8_t write_data[1] = {reg};
	uint8_t read_data[4];

	/* Use cyhal_i2c_master_write to write the required data to the device. 
	 * Send the register address and then generate a stop condition 
	 */
	rslt = cyhal_i2c_master_write(&i2c_monarch_obj, LTR329_SUBORDINATE_ADDR, write_data, 1, 0, true);

	/* Use cyhal_i2c_master_read to read the required data from the device. 
	 * The register address has already been set in the write above, so read two bytes
	 * of data.
	 */
	rslt = cyhal_i2c_master_read(&i2c_monarch_obj, LTR329_SUBORDINATE_ADDR, read_data, 4, 0, true);

    return_value = read_data[0];
	
	CY_ASSERT(rslt == CY_RSLT_SUCCESS); /* If the read failed, halt the CPU */

	// /* Return an 16-bit signed number that represents the temp. */
	// return_value = read_data[0]<<1;

	// if (read_data[1] & 0x80){
	// 	return_value = return_value | 0x1;
	// }

	// if (read_data[0] & 0x80){
	// 	return_value = return_value | 0xFE00;
	// }
	
	return return_value;
}

/** Read the value of the input port
 *
 * @param reg The reg address to read
 *
 */
uint16_t LTR329_get_value(void)
{
	uint8_t light_value = LTR329_read_reg(LTR329_REG_CH_1_0);

	// printf("light sensor all bits: %d\n\r", light_value);
	return light_value;
}
