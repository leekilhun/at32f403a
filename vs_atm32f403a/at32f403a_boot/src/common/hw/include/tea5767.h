/*
 * tea5767.h
 *
 *  Created on: 2022. 8. 6.
 *      Author: baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_TEA5767_H_
#define SRC_COMMON_HW_INCLUDE_TEA5767_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_TEA5767



bool tea5767Init(void);
bool tea5767IsInit(void);
void tea5767SetMute(bool enable);
bool tea5767GetMute(void);

bool tea5767SetFreq(float freq_mhz);
float tea5767GetFreq(void);
bool tea5767Update(void);
uint8_t tea5767GetLevel(void);

#endif

#ifdef __cplusplus
}
#endif

#endif 