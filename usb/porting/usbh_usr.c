/**
  ******************************************************************************
  * @file    usbh_usr.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   This file includes the usb host library user callbacks
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "usbh_usr.h"
#include "ff.h"       /* FATFS */
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"
#include "diskio.h"
#include "main.h"

/** @addtogroup USBH_USER
* @{
*/

/** @addtogroup USBH_MSC_DEMO_USER_CALLBACKS
* @{
*/

/** @defgroup USBH_USR 
* @brief    This file includes the usb host stack user callbacks
* @{
*/ 

/** @defgroup USBH_USR_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Defines
* @{
*/
//#define USB_USR_DBG

#ifdef USB_USR_DBG 
    #define USB_DBG_PRINT(fmt, args...)     printf(fmt, ##args)
#else
    #define USB_DBG_PRINT(fmt, args...)
#endif

static struct data_usb_cmd usb_cmd;
FATFS fatfs;

/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_cb =
{
    USBH_USR_Init,
    USBH_USR_DeInit,
    USBH_USR_DeviceAttached,
    USBH_USR_ResetDevice,
    USBH_USR_DeviceDisconnected,
    USBH_USR_OverCurrentDetected,
    USBH_USR_DeviceSpeedDetected,
    USBH_USR_Device_DescAvailable,
    USBH_USR_DeviceAddressAssigned,
    USBH_USR_Configuration_DescAvailable,
    USBH_USR_Manufacturer_String,
    USBH_USR_Product_String,
    USBH_USR_SerialNum_String,
    USBH_USR_EnumerationDone,
    USBH_USR_UserInput,
    USBH_USR_MSC_Application,
    USBH_USR_DeviceNotSupported,
    USBH_USR_UnrecoveredError
};

/**
* @}
*/

/** @defgroup USBH_USR_Private_Constants
* @{
*/ 
/*--------------- LCD Messages ---------------*/
const uint8_t MSG_HOST_INIT[]        = "> Host Library Initialized";
const uint8_t MSG_DEV_ATTACHED[]     = "> Device Attached";
const uint8_t MSG_DEV_DISCONNECTED[] = "> Device Disconnected";
const uint8_t MSG_DEV_ENUMERATED[]   = "> Enumeration completed";
const uint8_t MSG_DEV_HIGHSPEED[]    = "> High speed device detected";
const uint8_t MSG_DEV_FULLSPEED[]    = "> Full speed device detected";
const uint8_t MSG_DEV_LOWSPEED[]     = "> Low speed device detected";
const uint8_t MSG_DEV_ERROR[]        = "> Device fault";
const uint8_t MSG_USB_RESET[]        = "> USB reset";

const uint8_t MSG_MSC_CLASS[]        = "> Mass storage device connected";
const uint8_t MSG_HID_CLASS[]        = "> HID device connected";
const uint8_t MSG_DISK_SIZE[]        = "> Size of the disk in MBytes:";
const uint8_t MSG_LUN[]              = "> LUN Available in the device:";
const uint8_t MSG_ROOT_CONT[]        = "> Exploring disk flash ...";
const uint8_t MSG_WR_PROTECT[]       = "> The disk is write protected";
const uint8_t MSG_UNREC_ERROR[]      = "> UNRECOVERED ERROR STATE";

/**
* @brief  USBH_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBH_USR_Init(void)
{
    USB_DBG_PRINT("%s:%s\r\n", __FUNCTION__, MSG_HOST_INIT);
    usb_cmd.cmd = USB_INVALID;
}

/**
* @brief  USBH_USR_DeviceAttached 
*         Displays the message on LCD on device attached
* @param  None
* @retval None
*/
void USBH_USR_DeviceAttached(void)
{
    USB_DBG_PRINT("%s:%s\r\n", __FUNCTION__, MSG_DEV_ATTACHED);
}


/**
* @brief  USBH_USR_UnrecoveredError
* @param  None
* @retval None
*/
void USBH_USR_UnrecoveredError(void)
{
    USB_DBG_PRINT("%s:%s\r\n", __FUNCTION__, MSG_UNREC_ERROR);
}


/**
* @brief  USBH_DisconnectEvent
*         Device disconnect event
* @param  None
* @retval Staus
*/
void USBH_USR_DeviceDisconnected(void)
{
    USB_DBG_PRINT("%s:%s\r\n", __FUNCTION__, MSG_DEV_DISCONNECTED);
}
/**
* @brief  USBH_USR_ResetUSBDevice 
* @param  None
* @retval None
*/
void USBH_USR_ResetDevice(void)
{
    USB_DBG_PRINT("%s:%s\r\n", __FUNCTION__, MSG_DEV_DISCONNECTED);
}


/**
* @brief  USBH_USR_DeviceSpeedDetected 
*         Displays the message on LCD for device speed
* @param  Device speed
* @retval None
*/
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
    if(DeviceSpeed == HPRT0_PRTSPD_HIGH_SPEED) {
        USB_DBG_PRINT("%s:%s\r\n", __FUNCTION__, MSG_DEV_HIGHSPEED);
    } else if(DeviceSpeed == HPRT0_PRTSPD_FULL_SPEED) {
        USB_DBG_PRINT("%s:%s\r\n", __FUNCTION__, MSG_DEV_FULLSPEED);
    } else if(DeviceSpeed == HPRT0_PRTSPD_LOW_SPEED) {
        USB_DBG_PRINT("%s:%s\r\n", __FUNCTION__, MSG_DEV_LOWSPEED);
    } else {
        USB_DBG_PRINT("%s:%s\r\n", __FUNCTION__, MSG_DEV_ERROR);
    }
}

/**
* @brief  USBH_USR_Device_DescAvailable 
*         Displays the message on LCD for device descriptor
* @param  device descriptor
* @retval None
*/
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{ 
    USBH_DevDesc_TypeDef *hs;
    hs = DeviceDesc;
  
    USB_DBG_PRINT("%s: VID: %04Xh\n" , __FUNCTION__, (uint32_t)(*hs).idVendor); 
    USB_DBG_PRINT("%s: PID: %04Xh\n" , __FUNCTION__, (uint32_t)(*hs).idProduct); 
}

/**
* @brief  USBH_USR_DeviceAddressAssigned 
*         USB device is successfully assigned the Address 
* @param  None
* @retval None
*/
void USBH_USR_DeviceAddressAssigned(void)
{
    USB_DBG_PRINT("%s:\r\n", __FUNCTION__);
}


/**
* @brief  USBH_USR_Conf_Desc 
*         Displays the message on LCD for configuration descriptor
* @param  Configuration descriptor
* @retval None
*/
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
    USBH_InterfaceDesc_TypeDef *id;

    id = itfDesc;  

    if ((*id).bInterfaceClass == 0x08) {
        USB_DBG_PRINT("%s:%s\r\n", __FUNCTION__, MSG_MSC_CLASS);
    } else if ((*id).bInterfaceClass == 0x03) {
        USB_DBG_PRINT("%s:%s\r\n", __FUNCTION__, MSG_HID_CLASS);
    }
}

/**
* @brief  USBH_USR_Manufacturer_String 
*         Displays the message on LCD for Manufacturer String 
* @param  Manufacturer String 
* @retval None
*/
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
    USB_DBG_PRINT("%s:Manufacturer: %s\r\n", __FUNCTION__, (char *)ManufacturerString);
}

/**
* @brief  USBH_USR_Product_String 
*         Displays the message on LCD for Product String
* @param  Product String
* @retval None
*/
void USBH_USR_Product_String(void *ProductString)
{
    USB_DBG_PRINT("%s:Product : %s\r\n", __FUNCTION__, (char *)ProductString);  
}

/**
* @brief  USBH_USR_SerialNum_String 
*         Displays the message on LCD for SerialNum_String 
* @param  SerialNum_String 
* @retval None
*/
void USBH_USR_SerialNum_String(void *SerialNumString)
{
    USB_DBG_PRINT("%s:Serial Number : %s\r\n", __FUNCTION__, (char *)SerialNumString);    
} 



/**
* @brief  EnumerationDone 
*         User response request is displayed to ask application jump to class
* @param  None
* @retval None
*/
void USBH_USR_EnumerationDone(void)
{
    USB_DBG_PRINT("%s:%s\r\n", __FUNCTION__, MSG_DEV_ENUMERATED);  
} 


/**
* @brief  USBH_USR_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
void USBH_USR_DeviceNotSupported(void)
{
    USB_DBG_PRINT("> Device not supported.\r\n"); 
}  


/**
* @brief  USBH_USR_UserInput
*         User Action for application state entry
* @param  None
* @retval USBH_USR_Status : User response for key button
*/
USBH_USR_Status USBH_USR_UserInput(void)
{
    USBH_USR_Status usbh_usr_status;

    //usbh_usr_status = USBH_USR_NO_RESP;  
    usbh_usr_status = USBH_USR_RESP_OK;

    return usbh_usr_status;
}  

/**
* @brief  USBH_USR_OverCurrentDetected
*         Over Current Detected on VBUS
* @param  None
* @retval Staus
*/
void USBH_USR_OverCurrentDetected(void)
{
    USB_DBG_PRINT("Overcurrent detected.\r\n");
}

/**
* @brief  USBH_USR_MSC_Application 
*         Demo application for mass storage
* @param  None
* @retval Staus
*/


int USBH_USR_MSC_Application(void)
{
    switch (usb_cmd.cmd) {
    case USB_INVALID:
        f_mount(USB, &fatfs);
        usb_cmd.cmd = USB_READY;
        break;
    case USB_READY:
        break;
    case USB_MKDIR:
        if (!HCD_IsDeviceConnected(&USB_OTG_Core)) {
            USB_DBG_PRINT("%s > HCD is not connected!\r\n", __FUNCTION__);
            usb_cmd.cmd = USB_INVALID;
            break;
        }
        
        if (USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED) {
            USB_DBG_PRINT( "%s > Disk flash is write protected\r\n", __FUNCTION__);
            usb_cmd.cmd = USB_INVALID;
            break;
        }
        
        if (data_mkdir())
            usb_cmd.cmd = USB_ERROR;
        else
            usb_cmd.cmd = USB_READY;
        break;        
    case USB_EXPORT:
        if (!HCD_IsDeviceConnected(&USB_OTG_Core)) {
            USB_DBG_PRINT("%s > HCD is not connected!\r\n", __FUNCTION__);
            usb_cmd.cmd = USB_INVALID;
            break;
        }
        
        if (USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED) {
            USB_DBG_PRINT( "%s > Disk flash is write protected\r\n", __FUNCTION__);
            usb_cmd.cmd = USB_INVALID;
            break;
        }
        
        data_export(usb_cmd.table, usb_cmd.len);
        usb_cmd.cmd = USB_READY;
        break;
    case USB_ERROR:
        break;
    case USB_QUIT:
        usb_cmd.cmd = USB_INVALID;
        return 1;
    default:
        usb_cmd.cmd = USB_INVALID;
        break;
    } 
    //printf("usb status = %d\r\n", usb_cmd.cmd);
    return 0;
}


int usb_cmd_set(struct data_usb_cmd *cmd)
{
    memcpy(&usb_cmd, cmd, sizeof(struct data_usb_cmd));
    return 0;
}

int usb_cmd_get(void)
{
    return usb_cmd.cmd;
}

int usb_wait_ready(int ms)
{
    int i = 0;
    
    while (ms--) {
        if (usb_cmd.cmd == USB_READY)
            return 1;
        for (i = 0; i < 24000; i++);
    }
        
    return 0;
}

/**
* @brief  USBH_USR_DeInit
*         Deint User state and associated variables
* @param  None
* @retval None
*/
void USBH_USR_DeInit(void)
{
    usb_cmd.cmd = USB_INVALID;
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

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

