/*
 * hw_def.h
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */

#ifndef SRC_HW_HW_DEF_H_
#define SRC_HW_HW_DEF_H_


#include "def.h"
#include "bsp.h"



#define _DEF_FIRMWATRE_VERSION    "V220822R1"
#define _DEF_BOARD_NAME           "AT32F403A_BOOT"



#define _USE_HW_FLASH


#define _USE_HW_LED
#define      HW_LED_MAX_CH          1

#define _USE_HW_CDC
#define _USE_HW_USB
#define      HW_USE_CDC             1

#define _USE_HW_UART
#define      HW_UART_MAX_CH         1

#define _USE_HW_LOG
#define      HW_LOG_CH              _DEF_UART1
#define      HW_LOG_BOOT_BUF_MAX    1024
#define      HW_LOG_LIST_BUF_MAX    1024

#define _USE_HW_CLI
#define      HW_CLI_CMD_LIST_MAX    32
#define      HW_CLI_CMD_NAME_MAX    16
#define      HW_CLI_LINE_HIS_MAX    4
#define      HW_CLI_LINE_BUF_MAX    64

#define _USE_HW_CMD
#define      HW_CMD_MAX_DATA_LENGTH (1024+8)

#define _USE_HW_BUTTON
#define      HW_BUTTON_MAX_CH       1

#define FLASH_BOOT_ADDR             0x08000000
#define FLASH_BOOT_SIZE             (64*1024)           

#define FLASH_ADDR_TAG              0x08010000
#define FLASH_ADDR_FW               0x08010400
#define FLASH_ADDR_FW_VER           0x08010800

#define FLASH_ADDR_START            0x08010000
#define FLASH_ADDR_END              (FLASH_ADDR_START + (1024-64)*1024)



#endif /* SRC_HW_HW_DEF_H_ */
