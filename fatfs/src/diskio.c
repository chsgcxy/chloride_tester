/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "usb_conf.h"
#include "usbh_msc_core.h"
#include "w25xxx.h"
/*-----------------------------------------------------------------------*/
/* Correspondence between physical drive number and physical drive.      */

static volatile DSTATUS ata_stat = STA_NOINIT;	/* Disk status */
static volatile DSTATUS mmc_stat = STA_NOINIT;	/* Disk status */
static volatile DSTATUS usb_stat = STA_NOINIT;	/* Disk status */
static volatile DSTATUS spi_stat = STA_NOINIT;	/* Disk status */

extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
extern USBH_HOST                     USB_Host;

/*-----------------------------------------------------------------------*/
/* Inicializes a Drive                                                    */

DSTATUS disk_initialize (BYTE drv)    /* Physical drive nmuber (0..) */
{
	uint8_t vid, pid;

	switch (drv) {
	case ATA:
		return ata_stat;
	case MMC:
		return mmc_stat;
	case USB:
		if (HCD_IsDeviceConnected(&USB_OTG_Core))  
			usb_stat &= ~STA_NOINIT;
		return usb_stat;
	case SPI:
		w25xxx_read_id(&vid, &pid);
		if (vid == 0xEF && pid == 0x11)
			spi_stat &= ~STA_NOINIT;
		return spi_stat;
	default:
		return STA_NODISK;
	}
}

/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	switch (drv) {
	case ATA:
		return ata_stat;
	case MMC:
		return mmc_stat;
	case USB:
		return usb_stat;
	case SPI:
		return spi_stat;
	default:
		return STA_NODISK;
	}
}

static DRESULT ATA_disk_read(BYTE *buff, DWORD sector, BYTE count)
{
	return RES_PARERR;
}

static DRESULT MMC_disk_read(BYTE *buff, DWORD sector, BYTE count)
{
	return RES_PARERR;
}

static DRESULT USB_disk_read(BYTE *buff, DWORD sector, BYTE count)
{
	BYTE status = USBH_MSC_OK;

	if (!count)
  		return RES_PARERR;
  	if (usb_stat & STA_NOINIT)
		return RES_NOTRDY;
  
	if (HCD_IsDeviceConnected(&USB_OTG_Core)) {  
    	do {
			status = USBH_MSC_Read10(&USB_OTG_Core, buff, sector, 512 * count);
			USBH_MSC_HandleBOTXfer(&USB_OTG_Core, &USB_Host);
      		if (!HCD_IsDeviceConnected(&USB_OTG_Core))
        		return RES_ERROR;
    	} while (status == USBH_MSC_BUSY);
	}
  
	if(status == USBH_MSC_OK)
    	return RES_OK;
	return RES_ERROR;
}

static DRESULT SPI_disk_read(BYTE *buff, DWORD sector, BYTE count)
{
	while (count--)
		w25xxx_read_sector(buff, sector++, W25X20_SECTOR_SIZE);
	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	switch (drv) {
	case ATA:
		return ATA_disk_read(buff, sector, count);
	case MMC:
		return MMC_disk_read(buff, sector, count);
	case USB:
		return USB_disk_read(buff, sector, count);
	case SPI:
		return SPI_disk_read(buff, sector, count);
	default:
		return RES_PARERR;	
	}
}

static DRESULT ATA_disk_write(BYTE *buff, DWORD sector, BYTE count)
{
	return RES_PARERR;
}

static DRESULT MMC_disk_write(BYTE *buff, DWORD sector, BYTE count)
{
	return RES_PARERR;
}

static DRESULT USB_disk_write(BYTE *buff, DWORD sector, BYTE count)
{
	BYTE status = USBH_MSC_OK;

	if (!count)
		return RES_PARERR;
	if (usb_stat & STA_NOINIT)
		return RES_NOTRDY;
	if (usb_stat & STA_PROTECT)
		return RES_WRPRT;

	if (HCD_IsDeviceConnected(&USB_OTG_Core)) {  
		do {
			status = USBH_MSC_Write10(&USB_OTG_Core, (BYTE*)buff, sector, 512 * count);
			USBH_MSC_HandleBOTXfer(&USB_OTG_Core, &USB_Host);
			if (!HCD_IsDeviceConnected(&USB_OTG_Core))
				return RES_ERROR;
		} while (status == USBH_MSC_BUSY);
	}

	if (status == USBH_MSC_OK)
		return RES_OK;
	return RES_ERROR;
}

static DRESULT SPI_disk_write(BYTE *buff, DWORD sector, BYTE count)
{
	while (count--)
		w25xxx_write_sector(buff, sector++, W25X20_SECTOR_SIZE);
	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	switch (drv) {
	case ATA:
		return ATA_disk_write((BYTE *)buff, sector, count);
	case MMC:
		return MMC_disk_write((BYTE *)buff, sector, count);
	case USB:
		return USB_disk_write((BYTE *)buff, sector, count);
	case SPI:
		return SPI_disk_write((BYTE *)buff, sector, count);
	default:
		return RES_PARERR;
	}
}
#endif /* _READONLY */


static DRESULT ATA_disk_ioctl(BYTE ctrl, void *buff)
{
	return RES_PARERR;
}

static DRESULT MMC_disk_ioctl(BYTE ctrl, void *buff)
{
	return RES_PARERR;
}

static DRESULT USB_disk_ioctl(BYTE ctrl, void *buff)
{
	DRESULT res = RES_OK;

	if (usb_stat & STA_NOINIT)
		return RES_NOTRDY;

	switch (ctrl) {
	/* Make sure that no pending write process */
	case CTRL_SYNC:
		break;
	/* Get number of sectors on the disk (DWORD) */
	case GET_SECTOR_COUNT:
		*(DWORD*)buff = (DWORD)USBH_MSC_Param.MSCapacity;
		break;
	/* Get R/W sector size (WORD) */
	case GET_SECTOR_SIZE:
		*(WORD*)buff = 512;
		break;
	/* Get erase block size in unit of sector (DWORD) */
	case GET_BLOCK_SIZE:
		*(DWORD*)buff = 512;
		break;
	default:
		res = RES_PARERR;
	}

	return res;
}

/* 256KB spi-flash w25x20 */
static DRESULT SPI_disk_ioctl(BYTE ctrl, void *buff)
{
	DRESULT res = RES_OK;

	if (spi_stat & STA_NOINIT)
		return RES_NOTRDY;

	switch (ctrl) {
	/* Make sure that no pending write process */
	case CTRL_SYNC:
		break;
	/* Get number of sectors on the disk (DWORD) */
	case GET_SECTOR_COUNT:
		*(DWORD*)buff = (DWORD)W25X20_SECTOR_COUNT;
		break;
	/* Get R/W sector size (WORD) */
	case GET_SECTOR_SIZE:
		*(WORD*)buff = W25X20_SECTOR_SIZE;
		break;
	/* Get erase block size in unit of sector (DWORD) */
	case GET_BLOCK_SIZE:
		*(DWORD*)buff = W25X20_SECTOR_PER_BLOCK;
		break;
	default:
		res = RES_PARERR;
	}

	return res;
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	switch (drv) {
	case ATA:
		return ATA_disk_ioctl(ctrl, buff);
	case MMC:
		return MMC_disk_ioctl(ctrl, buff);
	case USB:
		return USB_disk_ioctl(ctrl, buff);
	case SPI:
		return SPI_disk_ioctl(ctrl, buff);
	default:
		return RES_PARERR;
	}
}

