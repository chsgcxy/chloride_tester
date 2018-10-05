/*
*********************************************************************************************************
*	                                  
*	ģ������ : GUI����������
*	�ļ����� : MainTask.c
*	��    �� : V1.0
*	˵    �� : GUI����������
*		�汾��   ����         ����            ˵��
*		v1.0    2013-11-17  Eric2013  	 1. ST�̼���V1.0.2�汾
*       v1.1    2014-03-22  Eric2013     1. ����ST�̼��⵽V1.3.0�汾
*                                        2. ���uCOS-II����FPU������
*	    V1.2    2015-03-27   Eric2013    1. �����̼��⵽V1.5.0
*                                        2. ����BSP�弶֧�ְ� 
*                                        3. ����STemWin��5.26
*                                        4. ����Ϊ�µ��ĵ㴥��У׼�㷨�����ز���Ŵ����������
*                                        5. ���7��800*480�ֱ��ʵ�����֧�֣����3.5��480*320��ILI9488֧�֡�
*                                     
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __MainTask_H
#define __MainTask_H

#include "GUI.h"
#include "DIALOG.h"
#include "WM.h"
#include "BUTTON.h"
#include "CHECKBOX.h"
#include "DROPDOWN.h"
#include "EDIT.h"
#include "FRAMEWIN.h"
#include "LISTBOX.h"
#include "MULTIEDIT.h"
#include "RADIO.h"
#include "SLIDER.h"
#include "TEXT.h"
#include "PROGBAR.h"
#include "SCROLLBAR.h"
#include "LISTVIEW.h"
#include "GRAPH.h"
#include "MENU.h"
#include "MULTIPAGE.h"
#include "ICONVIEW.h"
#include "TREEVIEW.h"
#include "ff.h"

/*
************************************************************************
*						  FatFs
************************************************************************
*/
extern FRESULT result;
extern FIL file;
extern FILINFO finfo;
extern DIR DirInf;
extern UINT bw;
extern FATFS fs;

extern void _WriteByte2File(U8 Data, void * p); 

/*
************************************************************************
*						emWin����
************************************************************************
*/
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_Song_12;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_FangSong_16;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_Song_16;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_Hei_24;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_Kai_24;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_Song_24;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_SimSun_1616;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_SimSun_2424;

#endif

/*****************************(END OF FILE) *********************************/
