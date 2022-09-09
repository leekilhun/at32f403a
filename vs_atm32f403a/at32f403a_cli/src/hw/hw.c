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


void hwInit(void)
{
  bspInit();

  cliInit();
  logInit();
  ledInit();
  flashInit();
  usbInit();
  uartInit();
  uartOpen(_DEF_UART1, 115200);
  logOpen(_DEF_UART1, 115200);

  logPrintf("[ Firmware Begin... ]\r\n");
  logPrintf("Firm Ver Addr\t\t: 0x%X\r\n", (int)&firm_ver);
  logPrintf("Firm Name \t\t: %s\r\n", firm_ver.name_str);
  logPrintf("Firm Ver  \t\t: %s\r\n", firm_ver.version_str);
  logPrintf("Core Clock \t\t: %d Mhz\r\n", system_core_clock/1000000);
  logPrintf("\n");

  usbBegin(USB_CDC_MODE);

  logBoot(false);
}
