#ifndef BOOT_H_
#define BOOT_H_

#include "ap_def.h"


bool bootInit(uint8_t ch, char *port_name, uint32_t baud);
bool bootInitNoRunBoot(uint8_t ch, char *port_name, uint32_t baud);
bool bootDeInit(uint8_t ch);


uint8_t bootCmdReadBootVersion(char *p_str);
uint8_t bootCmdReadBootName(char *p_str);
uint8_t bootCmdReadFirmVersion(char *p_str);
uint8_t bootCmdReadFirmName(char *p_str);
uint8_t bootCmdFlashErase(uint32_t addr, uint32_t length, uint32_t timeout);
uint8_t bootCmdFlashWrite(uint32_t addr, uint8_t *p_data, uint32_t length, uint32_t timeout);
uint8_t bootCmdFlashRead(uint32_t addr, uint8_t *p_data, uint32_t length, uint32_t timeout);
uint8_t bootCmdJumpToFw(void);
uint8_t bootCmdTagRead(firm_tag_t *p_tag);
uint8_t bootCmdTagWrite(firm_tag_t *p_tag);
uint8_t bootCmdTagVerify(uint32_t timeout);
uint8_t bootCmdTagErase(uint32_t timeout);

#endif