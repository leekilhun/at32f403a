/*
 * rc100.h
 *
 *  Created on: 2022. 7. 22.
 *      Author: baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_RC100_H_
#define SRC_COMMON_HW_INCLUDE_RC100_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_RC100


#define RC100_BTN_U		(1)
#define RC100_BTN_D		(2)
#define RC100_BTN_L		(4)
#define RC100_BTN_R		(8)
#define RC100_BTN_1		(16)
#define RC100_BTN_2		(32)
#define RC100_BTN_3		(64)
#define RC100_BTN_4		(128)
#define RC100_BTN_5		(256)
#define RC100_BTN_6		(512)

#define RC100_BTN_MAX 10


bool rc100Init(void);
bool rc100IsInit(void);
bool rc100Update(uint8_t data);
uint16_t rc100GetData(void);


#endif


#ifdef __cplusplus
}
#endif

#endif 