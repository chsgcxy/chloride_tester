#include "w25xxx.h"
#include "stm32_spi.h"

#define WriteEnable               0x06       //写使能，设置状态寄存器
#define WriteDisable              0x04       //写禁止
#define ReadStatusRegister        0x05       //读状态寄存器
#define WriteStatusRegister       0x01       //写状态寄存器
#define Read_Data                 0x03       //读取存储器数据
#define FastReadData              0x0B       //快速读取存储器数据
#define FastReadDualOutput        0x3B       //快速双端口输出方式读取存储器数据
#define Page_Program              0x02       //页面编程--写数据
#define BlockErace                0xD8       //块擦除
#define SectorErace               0x20       //扇区擦除
#define ChipErace                 0xC7       //片擦除
#define Power_Down                0xB9       //掉电模式
#define ReleacePowerDown          0xAB       //退出掉电模式
#define ReadDeviceID              0xAB       //获取设备ID信息

#define ReadDeviceID              0xAB       //退出掉电模式、设备ID信息
#define ReadManuIDDeviceID        0x90       //读取制造厂商ID信息和设备ID信息
#define ReadJedec_ID              0x9F       //JEDEC的ID信息

#define Dummy_Byte                0xA5

#define SPI_FLASH_CS_LOW()
#define SPI_FLASH_CS_HIGH()
#define SPI_Flash_WP_LOW()
#define SPI_Flash_WP_HIGH()
#define SPI_Flash_HOLD_LOW()
#define SPI_Flash_HOLD_HIGH()

u8 DataByte=0;
u8 Tx_Buffer[] = {0x72,0x62,0x02,0x78,0x60,0x96,0x86,0x79,0x85,0x24,0x36,0x48};
u8 Rx_Buffer[BufferSize];
vu32 FLASH_ID = 0;

  //SPI_FLASH_Init();
  
  //SPI_FLASH_ByteWrite(0x72, 0x01F01F);
  //DataByte = SPI_FLASH_ByteRead(0x01F01F);
  
  //DataByte = SPI_Flash_ReadStatusRegister();
  
  //SPI_FLASH_SectorErase(0x01F01F);
  //SPI_FLASH_BulkErase(0x01F01F);
  //SPI_FLASH_ChipErase();
    
  //DataByte = SPI_FLASH_FasttRead(0x01F01F);
  //DataByte = SPI_Flash_ReadStatusRegister();
  //DataByte = SPI_FLASH_ReadDeviceID();
  //SPI_FLASH_ReadManuID_DeviceID(0x000000);
  //SPI_FLASH_ReadJedecID();
  
  
  //SPI_FLASH_PageWrite(Tx_Buffer, 0x01F01F, 10);
  //SPI_FLASH_BufferRead(Rx_Buffer, 0x01F01F, 4);
  

#define SPI_FLASH_PageSize 256

static u8 SPI_FLASH_SendByte(u8 byte)
{
    return (spi_send_byte(SPI2, byte, SPI_MODE_LIB) & 0xff);
}

void SPI_FLASH_WriteEnable(void)
{
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(WriteEnable);
  SPI_FLASH_CS_HIGH();
}
void SPI_FLASH_WriteDisable(void)
{
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(WriteDisable);
  SPI_FLASH_CS_HIGH();
}
u8 SPI_Flash_ReadStatusRegister(void)
{
  u8 StatusRegister = 0;
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(ReadStatusRegister);
  StatusRegister = SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_CS_HIGH();
  return StatusRegister;
}
void SPI_Flash_WriteStatusRegister(u8 Byte)
{
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(WriteStatusRegister);
  SPI_FLASH_SendByte(Byte);
  SPI_FLASH_CS_HIGH();
}

u8 SPI_FLASH_ByteRead(u32 ReadAddr)
{
  u8 Temp = 0;
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(Read_Data);
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
  
  Temp = SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_CS_HIGH();
  return Temp;
}
u8 SPI_FLASH_FasttRead(u32 ReadAddr)
{
  u8 Temp = 0;
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(FastReadData);
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
  SPI_FLASH_SendByte(Dummy_Byte);
  
  Temp = SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_CS_HIGH();
  return Temp;
}

void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(Read_Data);
    SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
    SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
    SPI_FLASH_SendByte(ReadAddr & 0xFF);
    while (NumByteToRead--)
        *pBuffer++ = SPI_FLASH_SendByte(Dummy_Byte);
    SPI_FLASH_CS_HIGH();
}

void SPI_FLASH_SectorErase(u32 SectorAddr)
{
    SPI_FLASH_WriteEnable();
    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(SectorErace);
    SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
    SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
    SPI_FLASH_SendByte(SectorAddr & 0xFF);
    SPI_FLASH_CS_HIGH();
    SPI_FLASH_WaitForWriteEnd();
}

void SPI_FLASH_BulkErase(u32 BlockAddr)
{
    SPI_FLASH_WriteEnable();
    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(BlockErace);

    SPI_FLASH_SendByte((BlockAddr & 0xFF0000) >> 16);
    SPI_FLASH_SendByte((BlockAddr & 0xFF00) >> 8);
    SPI_FLASH_SendByte(BlockAddr & 0xFF);
    SPI_FLASH_CS_HIGH();
    SPI_FLASH_WaitForWriteEnd();
}

void SPI_FLASH_ChipErase(void)
{
    SPI_FLASH_WriteEnable();
    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(ChipErace);
    SPI_FLASH_CS_HIGH();
    SPI_FLASH_WaitForWriteEnd();
}

void SPI_FLASH_PowerDown()
{
  SPI_FLASH_WriteEnable();
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(Power_Down);
  SPI_FLASH_CS_HIGH();
  SPI_FLASH_WaitForWriteEnd();
}

void SPI_FLASH_ReleasePowerDown()
{
  SPI_FLASH_WriteEnable();
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(ReleacePowerDown);
  SPI_FLASH_CS_HIGH();
  SPI_FLASH_WaitForWriteEnd();
}

u8 SPI_FLASH_ReadDeviceID(void)
{
    u8 DeviceID = 0;
    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(ReadDeviceID);
    SPI_FLASH_SendByte(Dummy_Byte);
    SPI_FLASH_SendByte(Dummy_Byte);
    SPI_FLASH_SendByte(Dummy_Byte);

    DeviceID = SPI_FLASH_SendByte(Dummy_Byte);
    SPI_FLASH_CS_HIGH();      
    return DeviceID;
}
u16 SPI_FLASH_ReadManuID_DeviceID(u32 ReadManu_DeviceID_Addr)
{
    u16 ManuID_DeviceID = 0;
    u8 ManufacturerID = 0,  DeviceID = 0;
    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(ReadManuIDDeviceID);

    SPI_FLASH_SendByte((ReadManu_DeviceID_Addr & 0xFF0000) >> 16);
    SPI_FLASH_SendByte((ReadManu_DeviceID_Addr & 0xFF00) >> 8);
    SPI_FLASH_SendByte(ReadManu_DeviceID_Addr & 0xFF);

    if (ReadManu_DeviceID_Addr == 1) {
        DeviceID = SPI_FLASH_SendByte(Dummy_Byte);
        ManufacturerID = SPI_FLASH_SendByte(Dummy_Byte);
    } else {
        ManufacturerID = SPI_FLASH_SendByte(Dummy_Byte);
        DeviceID = SPI_FLASH_SendByte(Dummy_Byte);
    }

    ManuID_DeviceID = ((ManufacturerID << 8) | DeviceID);
    SPI_FLASH_CS_HIGH();      
    return ManuID_DeviceID;
}

u32 SPI_FLASH_ReadJedecID(void)
{
    u32 JEDECID = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(ReadJedec_ID);
    Temp0 = SPI_FLASH_SendByte(Dummy_Byte);
    Temp1 = SPI_FLASH_SendByte(Dummy_Byte);
    Temp2 = SPI_FLASH_SendByte(Dummy_Byte);
    SPI_FLASH_CS_HIGH();  
    JEDECID = (Temp0 << 16) | (Temp1 << 8) | Temp2;
    return JEDECID;
}

void SPI_FLASH_ByteWrite(u8 Byte, u32 WriteAddr)
{
    SPI_FLASH_WriteEnable(); 
    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(Page_Program);
    SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
    SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);  
    SPI_FLASH_SendByte(WriteAddr & 0xFF);

    SPI_FLASH_SendByte(Byte); 
    SPI_FLASH_CS_HIGH();
    SPI_FLASH_WaitForWriteEnd();
}
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    SPI_FLASH_WriteEnable(); 
    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(Page_Program);
    SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
    SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);  
    SPI_FLASH_SendByte(WriteAddr & 0xFF);

    while (NumByteToWrite--)
        SPI_FLASH_SendByte(*pBuffer++);

    SPI_FLASH_CS_HIGH();
    SPI_FLASH_WaitForWriteEnd();
}
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
    Addr = WriteAddr % SPI_FLASH_PageSize;
    count = SPI_FLASH_PageSize - Addr;
    NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
    NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
  
  if(Addr == 0) /* WriteAddr is SPI_FLASH_PageSize aligned  */
  {
    if(NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */ 
    {
      while(NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;  
      }    
     
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
   }
  }
  else /* WriteAddr is not SPI_FLASH_PageSize aligned  */
  {
    if(NumOfPage== 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      if(NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
      {
        temp = NumOfSingle - count;
      
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count; 
        
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
      
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;  
     
      while(NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
      
      if(NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

void SPI_FLASH_WaitForWriteEnd(void)
{
    u8 FLASH_Status = 0;

    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(ReadStatusRegister);
    
    do {
        FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);
    } while((FLASH_Status & WriteStatusRegister) == SET);

    SPI_FLASH_CS_HIGH();
}
