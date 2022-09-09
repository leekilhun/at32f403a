/*
 * hw.h
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */

#ifndef SRC_HW_HW_H_
#define SRC_HW_HW_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"


#include "led.h"
#include "cdc.h"
#include "usb.h"
#include "uart.h"
#include "log.h"
#include "cli.h"
#include "flash.h"
#include "cmd.h"
#include "util.h"
#include "button.h"


void hwInit(void);

void jumpToFw(void);

#ifdef __cplusplus
 }
#endif

#endif /* SRC_HW_HW_H_ */
