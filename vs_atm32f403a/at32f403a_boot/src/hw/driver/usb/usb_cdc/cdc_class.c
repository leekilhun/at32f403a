/**
  **************************************************************************
  * @file     cdc_class.c
  * @version  v2.0.6
  * @date     2021-12-31
  * @brief    usb cdc class type
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to 
  * download from Artery official website is the copyrighted work of Artery. 
  * Artery authorizes customers to use, copy, and distribute the BSP 
  * software and its related documentation for the purpose of design and 
  * development in conjunction with Artery microcontrollers. Use of the 
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */
#include "usbd_core.h"
#include "cdc_class.h"
#include "cdc_desc.h"
#include "cdc.h"
#include "qbuffer.h"


/** @addtogroup AT32F403A_407_middlewares_usbd_class
  * @{
  */
  
/** @defgroup USB_cdc_class
  * @brief usb device class cdc demo
  * @{
  */  

/** @defgroup USB_cdc_class_private_functions
  * @{
  */

#define USB_CDC_BUF_LEN   1024


usb_sts_type class_init_handler(void *udev);
usb_sts_type class_clear_handler(void *udev);
usb_sts_type class_setup_handler(void *udev, usb_setup_type *setup);
usb_sts_type class_ept0_tx_handler(void *udev);
usb_sts_type class_ept0_rx_handler(void *udev);
usb_sts_type class_in_handler(void *udev, uint8_t ept_num);
usb_sts_type class_out_handler(void *udev, uint8_t ept_num);
usb_sts_type class_sof_handler(void *udev);
usb_sts_type class_event_handler(void *udev, usbd_event_type event);

void usb_vcp_cmd_process(void *udev, uint8_t cmd, uint8_t *buff, uint16_t len);
/* usb rx and tx buffer */
static uint32_t alt_setting = 0;

static uint8_t g_rx_buff[USB_CDC_BUF_LEN];
static uint8_t g_tx_buff[USB_CDC_BUF_LEN];

static uint8_t g_cmd[USBD_CMD_MAXPACKET_SIZE];
static uint8_t g_req;
static uint16_t g_len;
static volatile bool is_tx_done = true;


linecoding_type linecoding =
{
  115200,
  0x00,
  0x00,
  0x08
};

/* static variable */


/* usb device class handler */
usbd_class_handler class_handler = 
{
  class_init_handler,
  class_clear_handler,
  class_setup_handler,
  class_ept0_tx_handler,
  class_ept0_rx_handler,
  class_in_handler,
  class_out_handler,
  class_sof_handler,
  class_event_handler,
};



static qbuffer_t q_rx;
static qbuffer_t q_tx;

static uint8_t q_rx_buf[1024];
static uint8_t q_tx_buf[1024];

static bool is_opened = false;
static bool is_rx_full = false;
static uint8_t cdc_type = 0;
static usbd_core_type *p_usb = NULL;

bool cdcIfInit(void)
{
  is_opened = false;
  qbufferCreate(&q_rx, q_rx_buf, USB_CDC_BUF_LEN);
  qbufferCreate(&q_tx, q_tx_buf, USB_CDC_BUF_LEN);

  return true;
}

uint32_t cdcIfAvailable(void)
{
  return qbufferAvailable(&q_rx);
}

uint8_t cdcIfRead(void)
{
  uint8_t ret = 0;

  qbufferRead(&q_rx, &ret, 1);

  return ret;
}

uint32_t cdcIfWrite(uint8_t *p_data, uint32_t length)
{
  uint32_t pre_time;
  uint32_t tx_len;
  uint32_t buf_len;
  uint32_t sent_len;


  if (cdcIfIsConnected() != true) return 0;


  sent_len = 0;

  pre_time = millis();
  while(sent_len < length)
  {
    buf_len = (q_tx.len - qbufferAvailable(&q_tx)) - 1;
    tx_len = length - sent_len;

    if (tx_len > buf_len)
    {
      tx_len = buf_len;
    }

    if (tx_len > 0)
    {
      qbufferWrite(&q_tx, p_data, tx_len);
      p_data += tx_len;
      sent_len += tx_len;
    }
    else
    {
      delay(1);
    }
    
    if (cdcIfIsConnected() != true)
    {
      break;
    }

    if (millis()-pre_time >= 100)
    {
      break;
    }
  }

  return sent_len;
}

uint32_t cdcIfGetBaud(void)
{
  return linecoding.bitrate;
}

bool cdcIfIsConnected(void)
{
  
  if (p_usb == NULL)
  {
    return false;
  }
  if (is_opened == false)
  {
    return false;
  }
  if (p_usb->conn_state != USB_CONN_STATE_CONFIGURED)
  {
    return false;
  }
  if (p_usb->dev_config == 0)
  {
    return false;
  }
  
  return true;
}

uint8_t cdcIfGetType(void)
{
  return cdc_type;
}


/**
  * @brief  initialize usb endpoint
  * @param  udev: to the structure of usbd_core_type
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_init_handler(void *udev)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  
  p_usb = pudev;
  

#ifndef USB_EPT_AUTO_MALLOC_BUFFER
  /* use user define buffer address */
  usbd_ept_buf_custom_define(pudev, USBD_CDC_INT_EPT, EPT2_TX_ADDR);
  usbd_ept_buf_custom_define(pudev, USBD_CDC_BULK_IN_EPT, EPT1_TX_ADDR);
  usbd_ept_buf_custom_define(pudev, USBD_CDC_BULK_OUT_EPT, EPT1_RX_ADDR);
#endif
  
  /* open in endpoint */
  usbd_ept_open(pudev, USBD_CDC_INT_EPT, EPT_INT_TYPE, USBD_CMD_MAXPACKET_SIZE);
  
  /* open in endpoint */
  usbd_ept_open(pudev, USBD_CDC_BULK_IN_EPT, EPT_BULK_TYPE, USBD_IN_MAXPACKET_SIZE);
  
  /* open out endpoint */
  usbd_ept_open(pudev, USBD_CDC_BULK_OUT_EPT, EPT_BULK_TYPE, USBD_OUT_MAXPACKET_SIZE);
  
  /* set out endpoint to receive status */
  usbd_ept_recv(pudev, USBD_CDC_BULK_OUT_EPT, g_rx_buff, USB_CDC_BUF_LEN);
  
  
  return status;
}

/**
  * @brief  clear endpoint or other state
  * @param  udev: to the structure of usbd_core_type
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_clear_handler(void *udev)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  
  /* close in endpoint */
  usbd_ept_close(pudev, USBD_CDC_INT_EPT);
  
  /* close in endpoint */
  usbd_ept_close(pudev, USBD_CDC_BULK_IN_EPT);
  
  /* close out endpoint */
  usbd_ept_close(pudev, USBD_CDC_BULK_OUT_EPT);
  
  return status;
}

/**
  * @brief  usb device class setup request handler
  * @param  udev: to the structure of usbd_core_type
  * @param  setup: setup packet
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_setup_handler(void *udev, usb_setup_type *setup)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;

  switch(setup->bmRequestType & USB_REQ_TYPE_RESERVED)
  {
    /* class request */
    case USB_REQ_TYPE_CLASS:
      if(setup->wLength)
      {
        if(setup->bmRequestType & USB_REQ_DIR_DTH)
        {
          usb_vcp_cmd_process(udev, setup->bRequest, g_cmd, setup->wLength);
          usbd_ctrl_send(pudev, g_cmd, setup->wLength);
        }
        else
        {
          g_req = setup->bRequest;
          g_len = setup->wLength;
          usbd_ctrl_recv(pudev, g_cmd, g_len);         
        }
      }
      else
      {
        usb_vcp_cmd_process(udev, setup->bRequest, (uint8_t *)setup, 0);
      }
      break;
    /* standard request */
    case USB_REQ_TYPE_STANDARD:
      switch(setup->bRequest)
      {
        case USB_STD_REQ_GET_DESCRIPTOR:
          usbd_ctrl_unsupport(pudev);
          break;
        case USB_STD_REQ_GET_INTERFACE:
          usbd_ctrl_send(pudev, (uint8_t *)&alt_setting, 1);
          break;
        case USB_STD_REQ_SET_INTERFACE:
          alt_setting = setup->wValue;
          break;
        default:
          break;
      }
      break;
    default:
      usbd_ctrl_unsupport(pudev);
      break;
  }
  return status;
}

/**
  * @brief  usb device class endpoint 0 in status stage complete
  * @param  udev: to the structure of usbd_core_type
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_ept0_tx_handler(void *udev)
{
  usb_sts_type status = USB_OK;
  
  /* ...user code... */
  
  return status;
}

/**
  * @brief  usb device class endpoint 0 out status stage complete
  * @param  udev: to the structure of usbd_core_type
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_ept0_rx_handler(void *udev)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  uint32_t recv_len = usbd_get_recv_len(pudev, 0);
  /* ...user code... */

  switch(g_req)
  {
    case SET_LINE_CODING:
    case GET_LINE_CODING:
    case SET_CONTROL_LINE_STATE:
      usb_vcp_cmd_process(udev, g_req, g_cmd, recv_len);
      break;
  }

  return status;
}

/**
  * @brief  usb device class transmision complete handler
  * @param  udev: to the structure of usbd_core_type
  * @param  ept_num: endpoint number
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_in_handler(void *udev, uint8_t ept_num)
{
  usb_sts_type status = USB_OK;
  
  /* ...user code...
    trans next packet data
  */
  
  is_tx_done = true;

  return status;
}

/**
  * @brief  usb device class endpoint receive data
  * @param  udev: to the structure of usbd_core_type
  * @param  ept_num: endpoint number
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_out_handler(void *udev, uint8_t ept_num)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  uint32_t rx_len;
  
  /* get endpoint receive data length  */
  rx_len = usbd_get_recv_len(pudev, ept_num);
  
  

  qbufferWrite(&q_rx, g_rx_buff, rx_len);


  #if 0
  uint32_t buf_len;

  buf_len = (q_rx.len - qbufferAvailable(&q_rx)) - 1;

  if (buf_len >= USBD_OUT_MAXPACKET_SIZE)
  {
    usbd_ept_recv(pudev, USBD_CDC_BULK_OUT_EPT, g_rx_buff, USB_CDC_BUF_LEN);
  }
  else
  {
    is_rx_full = true;
  }
  #else
  usbd_ept_recv(pudev, USBD_CDC_BULK_OUT_EPT, g_rx_buff, USBD_OUT_MAXPACKET_SIZE);
  #endif

  return status;
}

/**
  * @brief  usb device class sof handler
  * @param  udev: to the structure of usbd_core_type
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_sof_handler(void *udev)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  
  
  //-- RX
  //
  if (is_rx_full)
  {
    uint32_t buf_len;

    buf_len = (q_rx.len - qbufferAvailable(&q_rx)) - 1;

    if (buf_len >= USBD_OUT_MAXPACKET_SIZE)
    {
      usbd_ept_recv(pudev, USBD_CDC_BULK_OUT_EPT, g_rx_buff, USB_CDC_BUF_LEN);
      is_rx_full = false;
    }
  }


  //-- TX
  //
  uint32_t tx_len;
  tx_len = qbufferAvailable(&q_tx);

  if (tx_len%USBD_OUT_MAXPACKET_SIZE == 0)
  {
    if (tx_len > 0)
    {
      tx_len = tx_len - 1;
    }
  }

  if (tx_len > 0)
  {
    if (is_tx_done == true)    
    {
      is_tx_done = false;
      qbufferRead(&q_tx, g_tx_buff, tx_len);
      usbd_ept_send(pudev, USBD_CDC_BULK_IN_EPT, g_tx_buff, tx_len);
    }
  }
  
  return status;
}

/**
  * @brief  usb device class event handler
  * @param  udev: to the structure of usbd_core_type
  * @param  event: usb device event
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_event_handler(void *udev, usbd_event_type event)
{
  usb_sts_type status = USB_OK;
  switch(event)
  {
    case USBD_RESET_EVENT:
      
      /* ...user code... */
    
      break;
    case USBD_SUSPEND_EVENT:
      
      /* ...user code... */
    
      break;
    case USBD_WAKEUP_EVENT:
      /* ...user code... */
    
      break;
    default:
      break;
  }
  return status;
}



/**
  * @brief  usb device class request function
  * @param  udev: to the structure of usbd_core_type
  * @param  cmd: request number
  * @param  buff: request buffer
  * @param  len: buffer length
  * @retval none                            
  */
void usb_vcp_cmd_process(void *udev, uint8_t cmd, uint8_t *buff, uint16_t len)
{
  typedef  struct  usb_setup_req
  {
    uint8_t   bmRequest;
    uint8_t   bRequest;
    uint16_t  wValue;
    uint16_t  wIndex;
    uint16_t  wLength;
  } USBD_SetupReqTypedef;

  USBD_SetupReqTypedef *req = (USBD_SetupReqTypedef *)buff;
  uint32_t bitrate;

  switch(cmd)
  {
    case SET_LINE_CODING:
      bitrate = (uint32_t)(buff[0] | (buff[1] << 8) | (buff[2] << 16) | (buff[3] <<24));      
      linecoding.format = buff[4];
      linecoding.parity = buff[5];
      linecoding.data   = buff[6];
      linecoding.bitrate = bitrate & 0x0000000F;

      cdc_type = bitrate%10;
      break;
    
    case GET_LINE_CODING:
      buff[0] = (uint8_t)linecoding.bitrate;
      buff[1] = (uint8_t)(linecoding.bitrate >> 8);
      buff[2] = (uint8_t)(linecoding.bitrate >> 16);
      buff[3] = (uint8_t)(linecoding.bitrate >> 24);
      buff[4] = (uint8_t)(linecoding.format);
      buff[5] = (uint8_t)(linecoding.parity);
      buff[6] = (uint8_t)(linecoding.data);
      break;
    
    case SET_CONTROL_LINE_STATE:

      if (req->wValue & 0x01)
      {
        is_opened = true;
      }
      else
      {
        is_opened = false;    
      }
      break;

    default:
      break;
  }
}

/**
  * @}
  */ 

/**
  * @}
  */

/**
  * @}
  */

