#include "boot.h"



#define BOOT_CMD_READ_BOOT_VERSION      0x00
#define BOOT_CMD_READ_BOOT_NAME         0x01
#define BOOT_CMD_READ_FIRM_VERSION      0x02
#define BOOT_CMD_READ_FIRM_NAME         0x03
#define BOOT_CMD_FLASH_ERASE            0x04
#define BOOT_CMD_FLASH_WRITE            0x05
#define BOOT_CMD_FLASH_READ             0x06
#define BOOT_CMD_JUMP_TO_FW             0x08




static void bootCmdReadBootVersion(cmd_t *p_cmd);
static void bootCmdReadBootName(cmd_t *p_cmd);
static void bootCmdReadFirmVersion(cmd_t *p_cmd);
static void bootCmdReadFirmName(cmd_t *p_cmd);
static void bootCmdFlashErase(cmd_t *p_cmd);
static void bootCmdFlashWrite(cmd_t *p_cmd);
static void bootCmdFlashRead(cmd_t *p_cmd);
static void bootCmdJumpToFw(cmd_t *p_cmd);


extern uint32_t __version_addr;

firm_ver_t *p_boot_ver = (firm_ver_t *)&__version_addr;
firm_ver_t *p_firm_ver = (firm_ver_t *)(FLASH_ADDR_FW_VER);






void bootInit(void)
{

}

void bootProcessCmd(cmd_t *p_cmd)
{
  switch(p_cmd->rx_packet.cmd)
  {
    case BOOT_CMD_READ_BOOT_VERSION:
      bootCmdReadBootVersion(p_cmd);
      break;

    case BOOT_CMD_READ_BOOT_NAME:
      bootCmdReadBootName(p_cmd);
      break;

    case BOOT_CMD_READ_FIRM_VERSION:
      bootCmdReadFirmVersion(p_cmd);
      break;
    
    case BOOT_CMD_READ_FIRM_NAME:
      bootCmdReadFirmName(p_cmd);
      break;

    case BOOT_CMD_FLASH_ERASE:
      bootCmdFlashErase(p_cmd);
      break;

    case BOOT_CMD_FLASH_WRITE:
      bootCmdFlashWrite(p_cmd);
      break;

    case BOOT_CMD_FLASH_READ:
      bootCmdFlashRead(p_cmd);
      break;

    case BOOT_CMD_JUMP_TO_FW:
      bootCmdJumpToFw(p_cmd);
      break;

    default:
      cmdSendResp(p_cmd, p_cmd->rx_packet.cmd, ERR_BOOT_WRONG_CMD, NULL, 0);
      break;
  }
}

void bootCmdReadBootVersion(cmd_t *p_cmd)
{
  uint8_t resp_cmd = p_cmd->rx_packet.cmd;

  cmdSendResp(p_cmd, resp_cmd, CMD_OK, (uint8_t *)_DEF_FIRMWATRE_VERSION, strlen(_DEF_FIRMWATRE_VERSION));
}

void bootCmdReadBootName(cmd_t *p_cmd)
{
  uint8_t resp_cmd = p_cmd->rx_packet.cmd;

  cmdSendResp(p_cmd, resp_cmd, CMD_OK, (uint8_t *)_DEF_BOARD_NAME, strlen(_DEF_BOARD_NAME));
}

void bootCmdReadFirmVersion(cmd_t *p_cmd)
{
  uint8_t resp_cmd = p_cmd->rx_packet.cmd;
  firm_ver_t ver;
        
  memcpy(&ver, (const void *)FLASH_ADDR_FW_VER, sizeof(firm_ver_t)); 
  if (ver.magic_number != VERSION_MAGIC_NUMBER)
  {
    ver.version_str[0] = 0;
  }
  cmdSendResp(p_cmd, resp_cmd, CMD_OK, (uint8_t *)ver.version_str, strlen(ver.version_str));
}

void bootCmdReadFirmName(cmd_t *p_cmd)
{
  uint8_t resp_cmd = p_cmd->rx_packet.cmd;
  firm_ver_t ver;
        
  memcpy(&ver, (const void *)FLASH_ADDR_FW_VER, sizeof(firm_ver_t)); 
  if (ver.magic_number != VERSION_MAGIC_NUMBER)
  {
    ver.name_str[0] = 0;
  }  
  cmdSendResp(p_cmd, resp_cmd, CMD_OK, (uint8_t *)ver.name_str, strlen(ver.name_str));
}

void bootCmdFlashErase(cmd_t *p_cmd)
{
  uint8_t resp_cmd = p_cmd->rx_packet.cmd;
  cmd_packet_t *p_packet = &p_cmd->rx_packet;
  uint32_t addr = 0;
  uint32_t length = 0;
  uint8_t err_code = CMD_OK;


  addr  = ((uint32_t)p_packet->data[0] <<  0);
  addr |= ((uint32_t)p_packet->data[1] <<  8);
  addr |= ((uint32_t)p_packet->data[2] << 16);
  addr |= ((uint32_t)p_packet->data[3] << 24);

  length  = ((uint32_t)p_packet->data[4] <<  0);
  length |= ((uint32_t)p_packet->data[5] <<  8);
  length |= ((uint32_t)p_packet->data[6] << 16);
  length |= ((uint32_t)p_packet->data[7] << 24);


  if (flashInRange(addr, length) == true)
  {    
    if (flashErase(addr, length) != true)
    {
      err_code = ERR_BOOT_FLASH_ERASE;
    }
  }
  else
  {
    err_code = ERR_BOOT_WRONG_RANGE;
  }  

  cmdSendResp(p_cmd, resp_cmd, err_code, NULL, 0);  
}

void bootCmdFlashWrite(cmd_t *p_cmd)
{
  uint8_t resp_cmd = p_cmd->rx_packet.cmd;
  cmd_packet_t *p_packet = &p_cmd->rx_packet;
  uint32_t addr = 0;
  uint32_t length = 0;
  uint8_t err_code = CMD_OK;


  addr  = ((uint32_t)p_packet->data[0] <<  0);
  addr |= ((uint32_t)p_packet->data[1] <<  8);
  addr |= ((uint32_t)p_packet->data[2] << 16);
  addr |= ((uint32_t)p_packet->data[3] << 24);

  length  = ((uint32_t)p_packet->data[4] <<  0);
  length |= ((uint32_t)p_packet->data[5] <<  8);
  length |= ((uint32_t)p_packet->data[6] << 16);
  length |= ((uint32_t)p_packet->data[7] << 24);


  if (flashInRange(addr, length) == true)
  {    
    if (flashWrite(addr, &p_packet->data[8], length) != true)
    {
      err_code = ERR_BOOT_FLASH_WRITE;
    }
  }
  else
  {
    err_code = ERR_BOOT_WRONG_RANGE;
  }  

  cmdSendResp(p_cmd, resp_cmd, err_code, NULL, 0);  
}

void bootCmdFlashRead(cmd_t *p_cmd)
{
  uint8_t resp_cmd = p_cmd->rx_packet.cmd;
  cmd_packet_t *p_packet = &p_cmd->rx_packet;
  uint32_t addr = 0;
  uint32_t length = 0;
  uint8_t tx_buf[CMD_MAX_DATA_LENGTH];
  uint8_t *p_data;


  addr  = ((uint32_t)p_packet->data[0] <<  0);
  addr |= ((uint32_t)p_packet->data[1] <<  8);
  addr |= ((uint32_t)p_packet->data[2] << 16);
  addr |= ((uint32_t)p_packet->data[3] << 24);

  length  = ((uint32_t)p_packet->data[4] <<  0);
  length |= ((uint32_t)p_packet->data[5] <<  8);
  length |= ((uint32_t)p_packet->data[6] << 16);
  length |= ((uint32_t)p_packet->data[7] << 24);


  if (flashInRange(addr, length) != true || length >= CMD_MAX_DATA_LENGTH)
  {    
    cmdSendResp(p_cmd, resp_cmd, ERR_BOOT_WRONG_RANGE, NULL, 0);
    return;
  }
  
  p_data = (uint8_t *)addr;
  for (int i=0; i<(int)length; i++)
  {
    tx_buf[i] = p_data[i];
  }

  cmdSendResp(p_cmd, resp_cmd, CMD_OK, tx_buf, length);
}

void bootCmdJumpToFw(cmd_t *p_cmd)
{
  uint8_t resp_cmd = p_cmd->rx_packet.cmd;

  
  cmdSendResp(p_cmd, resp_cmd, CMD_OK, NULL, 0);
  delay(10);
  //jumpToFw();
}


