/*
 * usb.c
 *
 *  Created on: 2020. 12. 9.
 *      Author: baram
 */


#include "usb.h"
#include "cdc.h"

#ifdef _USE_HW_USB
#include "usbd_core.h"

#if HW_USE_CDC == 1
#include "cdc_class.h"
#include "cdc_desc.h"
#include "usbd_int.h"
#endif

#if HW_USE_MSC == 1
#include "usbd_msc.h"
#include "usbd_storage_if.h"
#endif


static bool is_init = false;
static UsbMode is_usb_mode = USB_NON_MODE;

usbd_core_type usb_core_dev;






bool usbInit(void)
{
  bool ret = true;


  crm_usb_clock_source_select(CRM_USB_CLOCK_SOURCE_HICK);

  /* enable the acc calibration ready interrupt */
  crm_periph_clock_enable(CRM_ACC_PERIPH_CLOCK, TRUE);

  /* update the c1\c2\c3 value */
  acc_write_c1(7980);
  acc_write_c2(8000);
  acc_write_c3(8020);

  /* open acc calibration */
  acc_calibration_mode_enable(ACC_CAL_HICKTRIM, TRUE);


  return ret;
}

void usbDeInit(void)
{
  if (is_init == true)
  {
    usbd_disconnect(&usb_core_dev);
    delay(100);
  }  
}

UsbMode usbGetMode(void)
{
  return is_usb_mode;
}

bool usbBegin(UsbMode usb_mode)
{
  bool ret = false;


#if HW_USE_CDC == 1

  if (usb_mode == USB_CDC_MODE)
  {
    cdcInit();

    /* enable usb clock */
    crm_periph_clock_enable(CRM_USB_PERIPH_CLOCK, TRUE);

    /* enable usb interrupt */
    nvic_irq_enable(USBFS_L_CAN1_RX0_IRQn, 0, 0);

    /* usb core init */
    usbd_core_init(&usb_core_dev, USB, &class_handler, &desc_handler, 0);

    /* enable usb pull-up */
    usbd_connect(&usb_core_dev);
    
    is_usb_mode = USB_CDC_MODE;
    ret = true;
  }
#endif

#if HW_USE_MSC == 1

  if (usb_mode == USB_MSC_MODE)
  {
    if (USBD_Init(&hUsbDeviceFS, &MSC_Desc, DEVICE_FS) != USBD_OK)
    {
      return false;
    }
    if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_MSC) != USBD_OK)
    {
      return false;
    }
    if (USBD_MSC_RegisterStorage(&hUsbDeviceFS, &USBD_Storage_Interface_fops_FS) != USBD_OK)
    {
      return false;
    }
    if (USBD_Start(&hUsbDeviceFS) != USBD_OK)
    {
      return false;
    }

    is_usb_mode = USB_MSC_MODE;
    ret = true;
  }
#endif


  is_init = ret;

  return ret;
}


void USBFS_L_CAN1_RX0_IRQHandler(void)
{
  usbd_irq_handler(&usb_core_dev);
}

void usb_delay_ms(uint32_t ms)
{
  delay(ms);
}

#endif
