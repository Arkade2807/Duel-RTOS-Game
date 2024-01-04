/**
 * @file project.h
 * @author  * @author Lucas Franke (lmfranke@wisc.edu) & Aditya Kamasani (kamasani@wisc.edu)

 * @brief 
 * @version 0.1
 * @date 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _PROJECT_H_
#define _PROJECT_H_

#include "main.h"
extern char PROJECT_DESCRIPTION[];

/**
 * @brief 
 * This function will initialize all of the hardware resources for
 * the project.  
 * 
 */
void peripheral_init(void);

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
void main_app(void);


#endif