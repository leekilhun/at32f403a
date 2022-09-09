/*
 * flash.c
 *
 *  Created on: 2020. 12. 14.
 *      Author: baram
 */


#include "flash.h"
#include "cli.h"


#ifdef _USE_HW_FLASH



#define FLASH_BANK_MAX              1
#define FLASH_ADDR                  0x08000000
#define FLASH_SIZE                  (1024*1024)
#define FLASH_SECTOR_SIZE           2048
#define FLASH_SECTOR_MAX            (FLASH_SIZE/FLASH_SECTOR_SIZE)
#define FLASH_PAGE_SIZE             4






static bool flashInSector(uint16_t sector_num, uint32_t addr, uint32_t length);


#ifdef _USE_HW_CLI
static void cliFlash(cli_args_t *args);
#endif


bool flashInit(void)
{

#ifdef _USE_HW_CLI
  cliAdd("flash", cliFlash);
#endif

  return true;
}

bool flashInRange(uint32_t addr_begin, uint32_t length)
{
  bool ret = false;

  for (int i=0; i<FLASH_SECTOR_MAX; i++)
  {
    if (flashInSector(i, addr_begin, length) == true)
    {
      ret = true;
    }
  }
  return ret;
}

bool flashErase(uint32_t addr, uint32_t length)
{
  bool ret = false;


  flash_unlock();
  for (int i=0; i<FLASH_SECTOR_MAX; i++)
  {
    bool update = false;
    uint32_t start_addr;
    uint32_t end_addr;


    start_addr = FLASH_ADDR + (i*FLASH_SECTOR_SIZE);
    end_addr   = start_addr + FLASH_SECTOR_SIZE - 1;

    if (start_addr >= addr && start_addr < (addr+length))
    {
      update = true;
    }
    if (end_addr >= addr && end_addr < (addr+length))
    {
      update = true;
    }

    if (addr >= start_addr && addr <= end_addr)
    {
      update = true;
    }
    if ((addr+length-1) >= start_addr && (addr+length-1) <= end_addr)
    {
      update = true;
    }


    if (update == true)
    {
      if (flash_sector_erase(start_addr) == FLASH_OPERATE_DONE)
      {
        ret = true;        
      }
      else
      {
        ret = false;
        break;
      }
    }
  }

  flash_lock();

  return ret;
}

bool flashWrite(uint32_t addr, uint8_t *p_data, uint32_t length)
{
  bool ret = true;
  uint32_t index;
  uint32_t write_length;
  uint32_t write_addr;
  data_t  buf;
  uint32_t offset;

  if (flashInRange(addr, length) != true)
  {
    return false;
  }

  index = 0;
  offset = addr%FLASH_PAGE_SIZE;

  flash_unlock();
  if (offset != 0 || length < FLASH_PAGE_SIZE)
  {
    write_addr = addr - offset;
    memcpy(&buf.u8Data[0], (void *)write_addr, FLASH_PAGE_SIZE);
    memcpy(&buf.u8Data[offset], &p_data[0], constrain(FLASH_PAGE_SIZE-offset, 0, length));

    if (flash_word_program(write_addr, buf.u32D) != FLASH_OPERATE_DONE)
    {
      flash_lock();
      return false;
    }

    if (offset == 0 && length < FLASH_PAGE_SIZE)
    {
      index += length;
    }
    else
    {
      index += (FLASH_PAGE_SIZE - offset);
    }
  }


  while(index < length)
  {
    write_length = constrain(length - index, 0, FLASH_PAGE_SIZE);

    if (write_length >= FLASH_PAGE_SIZE)
    {
      memcpy(&buf.u8Data[0], &p_data[index], FLASH_PAGE_SIZE);
      if (flash_word_program(addr + index, buf.u32D) != FLASH_OPERATE_DONE)
      {
        flash_lock();
        return false;
      }
      index += write_length;
    }

    if ((length - index) > 0 && (length - index) < FLASH_PAGE_SIZE)
    {
      offset = length - index;
      write_addr = addr + index;
      memcpy(&buf.u8Data[0], (void *)write_addr, FLASH_PAGE_SIZE);
      memcpy(&buf.u8Data[0], &p_data[index], offset);

      if (flash_word_program(write_addr, buf.u32D) != FLASH_OPERATE_DONE)
      {
        flash_lock();
        return false;
      }      
      break;
    }
  }
  flash_lock();

  return ret;
}

bool flashRead(uint32_t addr, uint8_t *p_data, uint32_t length)
{
  bool ret = true;
  uint8_t *p_byte = (uint8_t *)addr;


  for (int i=0; i<length; i++)
  {
    p_data[i] = p_byte[i];
  }

  return ret;
}

bool flashInSector(uint16_t sector_num, uint32_t addr, uint32_t length)
{
  bool ret = false;

  uint32_t sector_start;
  uint32_t sector_end;
  uint32_t flash_start;
  uint32_t flash_end;


  sector_start = FLASH_ADDR + (sector_num*FLASH_SECTOR_SIZE);
  sector_end   = sector_start + FLASH_SECTOR_SIZE - 1;
  flash_start  = addr;
  flash_end    = addr + length - 1;


  if (sector_start >= flash_start && sector_start <= flash_end)
  {
    ret = true;
  }

  if (sector_end >= flash_start && sector_end <= flash_end)
  {
    ret = true;
  }

  if (flash_start >= sector_start && flash_start <= sector_end)
  {
    ret = true;
  }

  if (flash_end >= sector_start && flash_end <= sector_end)
  {
    ret = true;
  }

  return ret;
}






#ifdef _USE_HW_CLI
void cliFlash(cli_args_t *args)
{
  bool ret = false;



  if (args->argc == 1 && args->isStr(0, "info") == true)
  {
    {
      cliPrintf("Flash : 0x%X , %dKB\n", FLASH_ADDR, FLASH_SIZE/1024);
    }
    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "read") == true)
  {
    uint32_t addr;
    uint32_t length;

    addr   = (uint32_t)args->getData(1);
    length = (uint32_t)args->getData(2);

    for (int i=0; i<length; i++)
    {
      cliPrintf("0x%X : 0x%X\n", addr+i, *((uint8_t *)(addr+i)));
    }

    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "erase") == true)
  {
    uint32_t addr;
    uint32_t length;

    addr   = (uint32_t)args->getData(1);
    length = (uint32_t)args->getData(2);

    if (flashErase(addr, length) == true)
    {
      cliPrintf("Erase OK\n");
    }
    else
    {
      cliPrintf("Erase Fail\n");
    }
    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "write") == true)
  {
    uint32_t addr;
    uint32_t data;

    addr   = (uint32_t)args->getData(1);
    data   = (uint32_t)args->getData(2);

    if (flashWrite(addr, (uint8_t *)&data, 4) == true)
    {
      cliPrintf("Write OK\n");
    }
    else
    {
      cliPrintf("Write Fail\n");
    }

    ret = true;
  }

  if (ret != true)
  {
    cliPrintf("flash info\n");
    cliPrintf("flash read  addr length\n");
    cliPrintf("flash erase addr length\n");
    cliPrintf("flash write addr data\n");
  }
}
#endif

#endif