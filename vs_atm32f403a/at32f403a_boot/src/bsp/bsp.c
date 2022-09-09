/*
 * bsp.c
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */


#include "bsp.h"
#include "uart.h"



static volatile uint32_t systick_counter = 0;

void SysTick_Handler(void)
{
  systick_counter++;
}



void bspInit(void)
{  
  system_clock_config();

  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4); 
  SysTick_Config(system_core_clock/1000);  
  NVIC_SetPriority(SysTick_IRQn, 0);

  __enable_irq();    
}

void bspDeInit(void)
{
   __disable_irq();

  // Disable Interrupts
  //
  for (int i=0; i<1; i++)
  {
    NVIC->ICER[i] = 0xFFFFFFFF;
    __DSB();
    __ISB();
  }
  SysTick->CTRL = 0;
}

void delay(uint32_t ms)
{
#ifdef _USE_HW_RTOS
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
    osDelay(ms);
  }
  else
  {
    HAL_Delay(ms);
  }
#else
  uint32_t pre_time = systick_counter;

  while(systick_counter-pre_time < ms);   
#endif
}

uint32_t millis(void)
{
  return systick_counter;
}

int __io_putchar(int ch)
{
  //uartWrite(_DEF_UART1, (uint8_t *)&ch, 1);
  return 1;
}


