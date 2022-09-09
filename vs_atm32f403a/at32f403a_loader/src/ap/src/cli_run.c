#include "cli_run.h"
#include "boot.h"


void cliRunInfo(cli_args_t *args);
void cliRunUart(cli_args_t *args);
void cliRunBoot(cli_args_t *args);


static int32_t getFileSize(char *file_name);


void cliRunInit(void)
{
  cliAdd("info", cliRunInfo);  
  cliAdd("uart", cliRunUart);  
  cliAdd("boot", cliRunBoot);  
}

void cliRunInfo(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 0)
  {
    logPrintf("This is info\n");
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("info\n");
  }
}

void cliRunUart(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info"))
  {
    if (uartIsOpen(_USE_UART_CMD))
    {
      cliPrintf("Uart  : Opened\n");
      cliPrintf("Port  : %s\n", uartGetPortName(_USE_UART_CMD));
      cliPrintf("Baud  : %d bps\n", uartGetBaud(_USE_UART_CMD));
    }
    else
    {
      cliPrintf("Uart  : Closed\n");
    }
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "list"))
  {
    char port_str[128];

    for (int i=0; i<100; i++)
    {
      snprintf(port_str, 128, "com%d", i);
      if (uartOpenPort(_USE_UART_CMD, port_str, 115200) == true)
      {
        cliPrintf("COM%d \t: Found\n", i);
        uartClose(_USE_UART_CMD);
      }
    }

    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "open"))
  {
    char *port_str;
    uint32_t port_baud;
    bool uart_ret;

    port_str = args->getStr(1);
    port_baud = args->getData(2);

    uart_ret = uartOpenPort(_USE_UART_CMD, port_str, port_baud);

    cliPrintf("Port : %s\n", port_str);
    cliPrintf("Baud : %d bps\n", port_baud);
    if (uart_ret)
      cliPrintf("Open : True\n");
    else
      cliPrintf("Open : False\n");

    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "test"))
  {
    bool uart_ret = false;

    if (uartIsOpen(_USE_UART_CMD))
    {
      cliPrintf("Uart  : Opened\n");
      cliPrintf("Port  : %s\n", uartGetPortName(_USE_UART_CMD));
      cliPrintf("Baud  : %d bps\n", uartGetBaud(_USE_UART_CMD));
      uart_ret = true;
    }
    else
    {
      cliPrintf("Uart  : Closed\n");
      cliPrintf("Need to run uart open\n");
    }

    while(cliKeepLoop() && uart_ret == true)
    {
      if (uartAvailable(_USE_UART_CMD) > 0)
      {
        uint8_t rx_data;

        rx_data = uartRead(_USE_UART_CMD);
        uartWrite(_USE_UART_CLI, &rx_data, 1);
      }  
    }

    ret = true;
  }


  if (ret == false)
  {
    cliPrintf("uart info\n");
    cliPrintf("uart list\n");
    cliPrintf("uart open comport baud\n");
    cliPrintf("uart test \n");
  }
}

int32_t getFileSize(char *file_name)
{
  int32_t ret = -1;
  FILE *fp;

  if ((fp = fopen( file_name, "rb")) == NULL)
  {
    fprintf( stderr, "Unable to open %s\n", file_name );
    return -1;
  }
  else
  {
    fseek( fp, 0, SEEK_END );
    ret = ftell( fp );
    fclose(fp);
  }

  return ret;
}

void cliRunBoot(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 2 && args->isStr(0, "cmd") == true)
  {
    if (uartIsOpen(_USE_UART_CMD))
    {
      uint8_t cmd;
      cmd_t cmd_boot;

      cmd = args->getData(1);

      cmdInit(&cmd_boot);
      cmdOpen(&cmd_boot, _USE_UART_CMD, uartGetBaud(_USE_UART_CMD));

      cliPrintf("cmd  : 0x%02X\n", cmd);
      if (cmdSendCmdRxResp(&cmd_boot, cmd, NULL, 0, 100) == true)
      {
        cmd_packet_t *p_packet = &cmd_boot.rx_packet;

        p_packet->data[p_packet->length] = 0;
        cliPrintf("resp : %s\n", p_packet->data);
      } 
      else
      {
        cliPrintf("not resp\n");
      } 
    }
    else
    {
      cliPrintf("Uart  : Closed\n");
      cliPrintf("Need to run uart open\n");      
    }
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "boot_version"))
  {
    if (uartIsOpen(_USE_UART_CMD))
    {
      bootInit(_USE_UART_CMD, NULL, uartGetBaud(_USE_UART_CMD));
      
      char str[32];
      uint8_t err_code;

      err_code = bootCmdReadBootVersion(str);      
      if (err_code == CMD_OK)
      {
        cliPrintf("boot_version : %s\n", str);
      }
      else
      {
        cliPrintf("\t err : %d\n", err_code);
      }
    }
    else
    {
      cliPrintf("Uart  : Closed\n");
      cliPrintf("Need to run uart open\n");      
    }
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "boot_name"))
  {
    if (uartIsOpen(_USE_UART_CMD))
    {
      bootInit(_USE_UART_CMD, NULL, uartGetBaud(_USE_UART_CMD));
      
      char str[32];
      uint8_t err_code;

      err_code = bootCmdReadBootName(str);      
      if (err_code == CMD_OK)
      {
        cliPrintf("boot_name : %s\n", str);
      }
      else
      {
        cliPrintf("\t err : %d\n", err_code);
      }
    }
    else
    {
      cliPrintf("Uart  : Closed\n");
      cliPrintf("Need to run uart open\n");      
    }
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "firm_version"))
  {
    if (uartIsOpen(_USE_UART_CMD))
    {
      bootInit(_USE_UART_CMD, NULL, uartGetBaud(_USE_UART_CMD));
      
      char str[32];
      uint8_t err_code;

      err_code = bootCmdReadFirmVersion(str);      
      if (err_code == CMD_OK)
      {
        cliPrintf("firm_version : %s\n", str);
      }
      else
      {
        cliPrintf("\t err : %d\n", err_code);
      }
    }
    else
    {
      cliPrintf("Uart  : Closed\n");
      cliPrintf("Need to run uart open\n");      
    }
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "firm_name"))
  {
    if (uartIsOpen(_USE_UART_CMD))
    {
      bootInit(_USE_UART_CMD, NULL, uartGetBaud(_USE_UART_CMD));
      
      char str[32];
      uint8_t err_code;

      err_code = bootCmdReadFirmName(str);      
      if (err_code == CMD_OK)
      {
        cliPrintf("firm_version : %s\n", str);
      }
      else
      {
        cliPrintf("\t err : %d\n", err_code);
      }
    }
    else
    {
      cliPrintf("Uart  : Closed\n");
      cliPrintf("Need to run uart open\n");      
    }
    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "flash_read"))
  {
    if (uartIsOpen(_USE_UART_CMD))
    {
      uint8_t err_code;
      uint32_t addr;
      uint32_t length;
      uint8_t rx_buf[1024];

      addr = args->getData(1);
      length = args->getData(2);

      if (length > 1024)
        length = 1024;

      bootInit(_USE_UART_CMD, NULL, uartGetBaud(_USE_UART_CMD));

      cliPrintf("flash read 0x%X, %d\n", addr, length);

      err_code = bootCmdFlashRead(addr, rx_buf, length, 500);
      if (err_code == CMD_OK)
      {
        for (int i=0; i<length; i++)
        {
          cliPrintf("  0x%X : 0x%X\n", addr + i, rx_buf[i]);
        }
      }

      cliPrintf("error : %d \n", err_code);      
    }
    else
    {
      cliPrintf("Uart  : Closed\n");
      cliPrintf("Need to run uart open\n");      
    }
    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "flash_write"))
  {
    if (uartIsOpen(_USE_UART_CMD))
    {
      uint8_t err_code;
      uint32_t addr;
      uint32_t data;

      addr = args->getData(1);
      data = args->getData(2);

      bootInit(_USE_UART_CMD, NULL, uartGetBaud(_USE_UART_CMD));

      cliPrintf("flash write 0x%X, 0x%d\n", addr, data);

      err_code = bootCmdFlashWrite(addr, (uint8_t *)&data, 4, 500);

      cliPrintf("error : %d \n", err_code);
    }
    else
    {
      cliPrintf("Uart  : Closed\n");
      cliPrintf("Need to run uart open\n");      
    }
    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "flash_erase"))
  {
    if (uartIsOpen(_USE_UART_CMD))
    {
      uint8_t err_code;
      uint32_t addr;
      uint32_t length;

      addr = args->getData(1);
      length = args->getData(2);

      bootInit(_USE_UART_CMD, NULL, uartGetBaud(_USE_UART_CMD));

      cliPrintf("flash erase 0x%X, %d\n", addr, length);

      err_code = bootCmdFlashErase(addr, length, 500);

      cliPrintf("error : %d \n", err_code);
    }
    else
    {
      cliPrintf("Uart  : Closed\n");
      cliPrintf("Need to run uart open\n");      
    }
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "jump_fw"))
  {
    if (uartIsOpen(_USE_UART_CMD))
    {
      uint8_t err_code;

      bootInit(_USE_UART_CMD, NULL, uartGetBaud(_USE_UART_CMD));

      cliPrintf("jump to fw\n");

      err_code = bootCmdJumpToFw();
      
      cliPrintf("error : %d \n", err_code);
    }
    else
    {
      cliPrintf("Uart  : Closed\n");
      cliPrintf("Need to run uart open\n");      
    }
    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "down_fw"))
  {
    if (uartIsOpen(_USE_UART_CMD))
    {
      uint8_t err_code;
      char *file_name;
      uint32_t addr;
      int32_t file_len;

      file_name = args->getStr(1);
      addr = args->getData(2);

      bootInit(_USE_UART_CMD, NULL, uartGetBaud(_USE_UART_CMD));

      cliPrintf("down_fw \n");

      file_len = getFileSize(file_name);

      cliPrintf("file_name : %s \n", file_name);
      cliPrintf("file_addr : 0x%X \n", addr);
      cliPrintf("file_len  : %d Bytes\n", file_len);


      FILE *fp;

      if ((fp = fopen(file_name, "rb")) == NULL)
      {
        cliPrintf("Unable to open %s\n", file_name);
        apExit();
      }

      while(1)
      {
        if (file_len <= 0) break;
      
        // 1. Flash Erase
        //
        err_code = bootCmdFlashErase(addr, file_len, 5000);
        if (err_code != CMD_OK)
        {
          cliPrintf("bootCmdFlashErase Fail : %d\n", err_code);
          break;
        }
        cliPrintf("bootCmdFlashErase OK : 0x%X, %d Bytes\n", addr, file_len);


        // 2. Flash Write
        //
        uint32_t tx_block_size = 256;
        uint8_t  tx_buf[256];
        uint32_t tx_len;
        int32_t  len_to_send;
        bool write_done = false;

        tx_len = 0;
        while(tx_len < file_len)
        {
          len_to_send = fread(tx_buf, 1, tx_block_size, fp);
          if (len_to_send <= 0)
          {
            break;
          }

          err_code = bootCmdFlashWrite(addr + tx_len, tx_buf, len_to_send, 500);
          if (err_code != CMD_OK)
          {
            cliPrintf("bootCmdFlashWrite Fail : %d, 0x%X\n", err_code, addr + tx_len);
            break;
          }
          else
          {
            cliPrintf("bootCmdFlashWrite %d%%\r", ((tx_len+len_to_send)*100/file_len));
          }

          tx_len += len_to_send;    

          if (tx_len == file_len)
          {
            write_done = true;
            break;
          }      
        }

        cliPrintf("\n");

        if (write_done == true)
        {
          cliPrintf("bootCmdFlashWrite OK\n");

          err_code = bootCmdJumpToFw();
          cliPrintf("bootCmdJumpToFw   OK\n");
        }
        break;
      }

      fclose(fp);
    }
    else
    {
      cliPrintf("Uart  : Closed\n");
      cliPrintf("Need to run uart open\n");      
    }
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("boot cmd cmd[0x00~0xFF] \n");
    cliPrintf("boot boot_version\n");
    cliPrintf("boot boot_name\n");
    cliPrintf("boot firm_version\n");
    cliPrintf("boot firm_name\n");
    cliPrintf("boot flash_read addr[0x4000~] length[~256]\n");
    cliPrintf("boot flash_write addr[0x4000~] data\n");
    cliPrintf("boot flash_erase addr[0x4000~] length\n");
    cliPrintf("boot jump_fw\n");
    cliPrintf("boot down_fw fw.bin addr\n");
  }
}