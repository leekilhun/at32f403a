/*
 * hw.c
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */


#include "hw.h"



extern uint32_t _flash_tag_addr;
extern uint32_t __isr_vector_addr;





volatile const firm_ver_t firm_ver __attribute__((section(".version"))) = 
{
  .magic_number = VERSION_MAGIC_NUMBER,
  .version_str  = _DEF_FIRMWATRE_VERSION,
  .name_str     = _DEF_BOARD_NAME,
};

static const firm_ver_t *p_firm_ver = (firm_ver_t *)FLASH_ADDR_FW_VER;



void hwInit(void)
{
  bspInit();

  cliInit();
  logInit();
  ledInit();
  buttonInit();
  flashInit();
  usbInit();
  uartInit();
  uartOpen(_DEF_UART1, 115200);
  logOpen(_DEF_UART1, 115200);

  logPrintf("[ Bootloader Begin... ]\r\n");
  logPrintf("Boot Ver Addr\t\t: 0x%X\r\n", (int)&firm_ver);
  logPrintf("Boot Name \t\t: %s\r\n", firm_ver.name_str);
  logPrintf("Boot Ver  \t\t: %s\r\n", firm_ver.version_str);
  logPrintf("Core Clock \t\t: %d Mhz\r\n", system_core_clock/1000000);
  logPrintf("\n");

  logPrintf("Firm Ver Addr\t\t: 0x%X\r\n", (int)p_firm_ver);
  if (p_firm_ver->magic_number == VERSION_MAGIC_NUMBER)
  {
    logPrintf("Firm Name \t\t: %s\r\n", p_firm_ver->name_str);
    logPrintf("Firm Ver  \t\t: %s\r\n", p_firm_ver->version_str);
  }
  else
  {
    logPrintf("Firm Name \t\t: Empty\r\n");
    logPrintf("Firm Ver  \t\t: Empty\r\n");
  }

  logBoot(false);
}

void jumpToFw(void)
{
  void (**jump_func)(void) = (void (**)(void))(FLASH_ADDR_FW + 4); 


  usbDeInit();
  bspDeInit();
  __set_MSP(*(uint32_t *)(FLASH_ADDR_FW));
  SCB->VTOR = FLASH_ADDR_FW;


  //resetSetBootMode(RESET_MODE_FW);

  (*jump_func)();
}