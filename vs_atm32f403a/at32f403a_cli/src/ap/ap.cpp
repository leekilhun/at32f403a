/*
 * ap.c
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */


#include "ap.h"




void cliInfo(cli_args_t *args);



void apInit(void)
{
  cliOpen(_DEF_UART1, 115200);

  cliAdd("info", cliInfo);
}

void apMain(void)
{
  uint32_t pre_time;

  
  while(1)
  {
    if (millis()-pre_time >= 500)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
    }
    
    cliMain();
  }
}

void cliInfo(cli_args_t *args)
{
  bool ret = false;



  if (args->argc == 1 && args->isStr(0, "usb_tx"))
  {
    while(cliKeepLoop())
    {
      cliPrintf("123456789012345678901234567890123456789012345678901234567890\n");
    }

    ret = true;   
  }

  if (args->argc == 1 && args->isStr(0, "usb_rx"))
  {   
    uint8_t rx_buf[2048];

    while(1)
    {
      int rx_len;

      rx_len = uartAvailable(_DEF_UART1);
      for (int i=0; i<rx_len; i++)
      {
        rx_buf[i] = uartRead(_DEF_UART1);
      }
      if (rx_len > 0)
      {
        uartWrite(_DEF_UART1, rx_buf, rx_len);
      }
    }

    ret = true;   
  }

  if (ret != true)
  {
    cliPrintf("info usb_tx\n");
    cliPrintf("info usb_rx\n");
  }
}