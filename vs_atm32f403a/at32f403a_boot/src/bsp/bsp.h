/*
 * bsp.h
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */

#ifndef SRC_BSP_BSP_H_
#define SRC_BSP_BSP_H_

#ifdef __cplusplus
 extern "C" {
#endif


#include "def.h"




#include "at32f403a_407.h"
#include "at32f403a_407_clock.h"


void bspInit(void);
void bspDeInit(void);

void delay(uint32_t ms);
uint32_t millis(void);



#ifdef __cplusplus
 }
#endif

#endif /* SRC_BSP_BSP_H_ */
