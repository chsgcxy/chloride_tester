/*********************************************************************
*          Portions COPYRIGHT 2016 STMicroelectronics                *
*          Portions SEGGER Microcontroller GmbH & Co. KG             *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2015  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.32 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : LCDConf_FlexColor_Template.c
Purpose     : Display controller configuration (single layer)
---------------------------END-OF-HEADER------------------------------
*/

/**
  ******************************************************************************
  * @attention
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

#include "GUI.h"
#include "GUIDRV_FlexColor.h"
#include "stm32f2xx_gpio.h"
#include "stm32f2xx_fsmc.h"
#include "stdio.h"
#include "delay.h"
/*********************************************************************
*
*       Layer configuration (to be modified)
*
**********************************************************************
*/

//
// Physical display size
//
#define XSIZE_PHYS  800 // To be adapted to x-screen size
#define YSIZE_PHYS  480 // To be adapted to y-screen size

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   VXSIZE_PHYS
  #define VXSIZE_PHYS XSIZE_PHYS
#endif
#ifndef   VYSIZE_PHYS
  #define VYSIZE_PHYS YSIZE_PHYS
#endif
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   GUICC_565
  #error Color conversion not defined!
#endif
#ifndef   GUIDRV_FLEXCOLOR
  #error No display driver defined!
#endif


#define LCD_RAM    *(__IO uint16_t *) (0x60020000)
#define LCD_REG    *(__IO uint16_t *) (0x60000000)
	
#define LCD_WRITE_REG(value)     (LCD_REG = value)
#define LCD_WRITE_RAM(value)    (LCD_RAM = value)	
	
#define  HDP  799  //Horizontal Display Period
#define  HT   1000 //Horizontal Total
#define  HPS  51  //LLINE Pulse Start Position
#define  LPS  3   //	Horizontal Display Period Start Position
#define  HPW  8   //	LLINE Pulse Width


#define  VDP  479	//Vertical Display Period
#define  VT   530	//Vertical Total
#define  VPS  24	//	LFRAME Pulse Start Position
#define  FPS  23	//Vertical Display Period Start Positio
#define  VPW  3 	// LFRAME Pulse Width

#define   Black        0x0000
#define   Navy         0x000F
#define   Dgreen       0x03E0
#define   Dcyan        0x03EF
#define   Marooon      0x7800
#define   Purple       0x780F
#define   Olive        0x7BE0
#define   Lgray        0xC618
#define   Dgray        0x7BEF
#define   Blue         0x001F
#define   Green        0x07E0
#define   Cyan         0x07FF
#define   Red          0xF800
#define   Magenta      0xF81F
#define   Yellow       0xFFE0
#define   White        0xFFFF

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/

static void lcd_delay(int cnt)
{
	unsigned int dl;
	
	while (cnt--)
		for (dl = 0; dl < 500; dl++);
}

void lcd_io_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 |
                                  GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 |
                                  GPIO_Pin_15 | GPIO_Pin_4 | GPIO_Pin_5 |
                                  GPIO_Pin_7 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
                                 GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 |
                                 GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);

    /* force reset */ 
#if 1  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_11);
#endif
}

static void lcd_fsmc_init(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef p;

    /* Enable FSMC clock */
#if 1
    delay_ms(500);
    GPIO_SetBits(GPIOB, GPIO_Pin_11);
#endif    
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    p.FSMC_AddressSetupTime = 0x2;
    p.FSMC_AddressHoldTime = 0x1;
    p.FSMC_DataSetupTime = 0x09;
    p.FSMC_BusTurnAroundDuration = 0;
    p.FSMC_CLKDivision = 1;
    p.FSMC_DataLatency = 0;
    p.FSMC_AccessMode = FSMC_AccessMode_A;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
    
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

void lcd_draw_point(int x, int y, int color)
{
    printf("draw point...\r\n");
    LCD_WRITE_REG(0x002A);	
    LCD_WRITE_RAM(x >> 8);	    
    LCD_WRITE_RAM(x & 0x00ff);
    LCD_WRITE_RAM(HDP >> 8);	    
    LCD_WRITE_RAM(HDP & 0x00ff);
    LCD_WRITE_REG(0x002b);	
    LCD_WRITE_RAM(y >> 8);	    
    LCD_WRITE_RAM(y & 0x00ff);
    LCD_WRITE_RAM(VDP >> 8);	    
    LCD_WRITE_RAM(VDP & 0x00ff);
    LCD_WRITE_REG(0x002c);	
    LCD_WRITE_RAM(color); 
}  
 
#if 0
static void lcd_clear(unsigned short Color)
{
    unsigned int count;

	LCD_WRITE_REG(0x002A);	
	LCD_WRITE_RAM(0);	    
	LCD_WRITE_RAM(0);
	LCD_WRITE_RAM(HDP>>8);	    
	LCD_WRITE_RAM(HDP&0x00ff);
    LCD_WRITE_REG(0x002b);	
	LCD_WRITE_RAM(0);	    
	LCD_WRITE_RAM(0);
	LCD_WRITE_RAM(VDP>>8);	    
	LCD_WRITE_RAM(VDP&0x00ff);
	LCD_WRITE_REG(0x002c);

	for (count = 0; count < (HDP + 1) * (VDP + 1); count++)		
		LCD_WRITE_RAM(Color);
}
#endif

static void lcd_ssd1963_config(void)
{  
  	LCD_WRITE_REG(0x002b);	
	LCD_WRITE_RAM(0);
	lcd_delay(50);
	
    LCD_WRITE_REG(0x00E2);					//PLL multiplier, set PLL clock to 120M
	LCD_WRITE_RAM(0x0023);					//N=0x36 for 6.5M, 0x23 for 10M crystal
	LCD_WRITE_RAM(0x0002);
	LCD_WRITE_RAM(0x0004);
	
	LCD_WRITE_REG(0x00E0);					//PLL enable
	LCD_WRITE_RAM(0x0001);
	lcd_delay(1);

	LCD_WRITE_REG(0x00E0);
	LCD_WRITE_RAM(0x0003);
	lcd_delay(5);
	
    LCD_WRITE_REG(0x0001);  					//software reset
	lcd_delay(5);

	LCD_WRITE_REG(0x00E6);					//PLL setting for PCLK, depends on resolution
	LCD_WRITE_RAM(0x0004);
	LCD_WRITE_RAM(0x0093);
	LCD_WRITE_RAM(0x00E0);

	LCD_WRITE_REG(0x00B0);					//LCD SPECIFICATION
	LCD_WRITE_RAM(0x0020);
	LCD_WRITE_RAM(0x0000);
	LCD_WRITE_RAM((HDP>>8)&0X00FF);			//Set HDP
	LCD_WRITE_RAM(HDP&0X00FF);
    LCD_WRITE_RAM((VDP>>8)&0X00FF);			//Set VDP
	LCD_WRITE_RAM(VDP&0X00FF);
    LCD_WRITE_RAM(0x0000);
	lcd_delay(5);

	LCD_WRITE_REG(0x00B4);					//HSYNC
	LCD_WRITE_RAM((HT>>8)&0X00FF); 			//Set HT
	LCD_WRITE_RAM(HT&0X00FF);
	LCD_WRITE_RAM((HPS>>8)&0X00FF);			//Set HPS
	LCD_WRITE_RAM(HPS&0X00FF);
	LCD_WRITE_RAM(HPW);						//Set HPW
	LCD_WRITE_RAM((LPS>>8)&0X00FF); 			//Set HPS
	LCD_WRITE_RAM(LPS&0X00FF);
	LCD_WRITE_RAM(0x0000);

	LCD_WRITE_REG(0x00B6);					//VSYNC
	LCD_WRITE_RAM((VT>>8)&0X00FF);   		//Set VT
	LCD_WRITE_RAM(VT&0X00FF);
	LCD_WRITE_RAM((VPS>>8)&0X00FF); 			//Set VPS
	LCD_WRITE_RAM(VPS&0X00FF);
	LCD_WRITE_RAM(VPW);						//Set VPW
	LCD_WRITE_RAM((FPS>>8)&0X00FF);			//Set FPS
	LCD_WRITE_RAM(FPS&0X00FF);
	lcd_delay(5);

	LCD_WRITE_REG(0x00BA);
	LCD_WRITE_RAM(0x0005);//0x000F);    //GPIO[3:0] out 1

	LCD_WRITE_REG(0x00B8);
	LCD_WRITE_RAM(0x0007);    //GPIO3=input, GPIO[2:0]=output
	LCD_WRITE_RAM(0x0001);    //GPIO0 normal

	LCD_WRITE_REG(0x0036); //rotation
	LCD_WRITE_RAM(0x0000);
	lcd_delay(50);

	LCD_WRITE_REG(0x00BE); //set PWM for B/L
	LCD_WRITE_RAM(0x0006);
	LCD_WRITE_RAM(0x0080);
	LCD_WRITE_RAM(0x0001);
	LCD_WRITE_RAM(0x00f0);
	LCD_WRITE_RAM(0x0000);
	LCD_WRITE_RAM(0x0000);

	LCD_WRITE_REG(0x00d0);
	LCD_WRITE_RAM(0x000d);
   
	LCD_WRITE_REG(0x00F0); //pixel data interface
	LCD_WRITE_RAM(0x0003); //03:16?   02:24?

	LCD_WRITE_REG(0x0029); //display on
}
	

/********************************************************************
*
*       LcdWriteReg
*
* Function description:
*   Sets display register
*/
static void LcdWriteReg(U16 Data) {
    // ... TBD by user
    LCD_WRITE_REG(Data);
}

/********************************************************************
*
*       LcdWriteData
*
* Function description:
*   Writes a value to a display register
*/
static void LcdWriteData(U16 Data) {
    // ... TBD by user
    LCD_WRITE_RAM(Data);
}

/********************************************************************
*
*       LcdWriteDataMultiple
*
* Function description:
*   Writes multiple values to a display register.
*/
static void LcdWriteDataMultiple(U16 * pData, int NumItems) {
    while (NumItems--) {
        // ... TBD by user
        LCD_WRITE_RAM(*pData++);
    }
}

/********************************************************************
*
*       LcdReadDataMultiple
*
* Function description:
*   Reads multiple values from a display register.
*/
static void LcdReadDataMultiple(U16 * pData, int NumItems) {
    while (NumItems--) {
        // ... TBD by user
        *pData++ = LCD_RAM;
    }
}

/*********************************************************************
*
*       Public functions
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Config
*
* Function description:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*
*/
void LCD_X_Config(void) {
    GUI_DEVICE * pDevice;
    CONFIG_FLEXCOLOR Config = {0};
    GUI_PORT_API PortAPI = {0};
    //
    // Set display driver and color conversion
    //
    pDevice = GUI_DEVICE_CreateAndLink(GUIDRV_FLEXCOLOR, GUICC_565, 0, 0);
    //
    // Display driver configuration, required for Lin-driver
    //
    LCD_SetSizeEx(0, XSIZE_PHYS , YSIZE_PHYS);
    LCD_SetVSizeEx(0, VXSIZE_PHYS, VYSIZE_PHYS);
    //
    // Orientation
    //
    // the hardware have gpio to set flush mode
    //Config.Orientation = GUI_SWAP_XY | GUI_MIRROR_Y;
    //Config.Orientation = GUI_MIRROR_Y; //| GUI_MIRROR_X;
    GUIDRV_FlexColor_Config(pDevice, &Config);
    //
    // Set controller and operation mode
    //
    PortAPI.pfWrite16_A0  = LcdWriteReg;
    PortAPI.pfWrite16_A1  = LcdWriteData;
    PortAPI.pfWriteM16_A1 = LcdWriteDataMultiple;
    PortAPI.pfReadM16_A1  = LcdReadDataMultiple;
    GUIDRV_FlexColor_SetFunc(pDevice, &PortAPI, GUIDRV_FLEXCOLOR_F66709, GUIDRV_FLEXCOLOR_M16C0B16);
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Function description:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
    (void) LayerIndex;
    (void) pData;

    switch (Cmd) {
    case LCD_X_INITCONTROLLER:
        //
        // Called during the initialization process in order to set up the
        // display controller and put it into operation. If the display
        // controller is not initialized by any external routine this needs
        // to be adapted by the customer...
        //
        // ...
        //printf("gui init................\r\n");
        //lcd_io_init();
        lcd_fsmc_init();
        lcd_ssd1963_config();
        return 0;
    default:
        return -1;
    }
}

/*************************** End of file ****************************/
