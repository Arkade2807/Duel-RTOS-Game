/*
 * opt3001.h
 *
 *  Created on: Dec 8, 2023
 *      Author: Lucas Franke
 */

#ifndef LTR329_H_
#define LTR329_H_

#include <stdint.h>
#include <stdbool.h>
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

/* ADD CODE */
#define LTR329_SUBORDINATE_WRITE_ADDR                 0x52
#define LTR329_SUBORDINATE_READ_ADDR                 0x53
#define LTR329_SUBORDINATE_ADDR                         0x29

/* ADD CODE */
#define LTR329_REG_CH_1_0                       0x88
#define LTR329_REG_CH_1_1                       0x89

#define LTR329_REG_CH_0_0                       0x8A
#define LTR329_REG_CH_0_1                       0x8B

#define LTR329_REG_CONTR                        0x80



/**
 *
 *
 *
 */
void LTR329_init(void);



/** Read the value from LTR329
 *
 * @param
 *
 */
uint16_t LTR329_get_value(void);

#endif /* LTR329_H_ */