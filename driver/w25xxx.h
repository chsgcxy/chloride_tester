#ifndef __W25XXX_H__
#define __W25XXX_H__

#include "stm32f2xx.h"

#define  BufferSize               (countof(Tx_Buffer)-1)
#define countof(a)                (sizeof(a) / sizeof(*(a)))
#define SPI_FLASH_PageSize        256

extern void SPI_FLASH_WriteEnable(void);
extern void SPI_FLASH_WriteDisable(void);
extern u8 SPI_Flash_ReadStatusRegister(void);
extern void SPI_Flash_WriteStatusRegister(u8 Byte);
extern u8 SPI_FLASH_SendByte(u8 byte);
extern u8 SPI_FLASH_ReceiveByte(void);
extern u8 SPI_FLASH_ByteRead(u32 ReadAddr);
extern u8 SPI_FLASH_FasttRead(u32 ReadAddr);
extern void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
extern void SPI_FLASH_SectorErase(u32 SectorAddr);
extern void SPI_FLASH_BulkErase(u32 BlockAddr);
extern void SPI_FLASH_ChipErase(void);
extern void SPI_FLASH_PowerDown(void);
extern void SPI_FLASH_ReleasePowerDown(void);
extern u8 SPI_FLASH_ReadDeviceID(void);
extern u16 SPI_FLASH_ReadManuID_DeviceID(u32 ReadManu_DeviceID_Addr);
extern u32 SPI_FLASH_ReadJedecID(void);
extern void SPI_FLASH_ByteWrite(u8 Byte, u32 WriteAddr);
extern void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
extern void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
extern void SPI_FLASH_WaitForWriteEnd(void);

#endif 
