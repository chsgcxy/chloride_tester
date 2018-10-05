/*
*********************************************************************************************************
*	                                  
*	ģ������ : GUI����������
*	�ļ����� : MainTask.c
*	��    �� : V3.0
*	˵    �� : ����һ���򵥵�GUI���棬ʹ�õ��ǹٷ�ʵ��������ͻ���ⴥ����
*	�޸ļ�¼ :
*		�汾��   ����        ����     ˵��
*		V1.0    2013-09-11  Eric2013  �׷�
*       V2.0    2014-02-23  Eric2013  ����F4�̼��⵽1.3.0�汾
*	    V3.0    2015-03-27  Eric2013  1. �����̼��⵽V1.5.0
*                                     2. ����BSP�弶֧�ְ� 
*                                     3. ����STemWin��5.26
*                                     4. ����fatfs��V0.11
*                                     5. ����Ϊ�µ��ĵ㴥��У׼�㷨�����ز���Ŵ����������
*                                     6. ���7��800*480�ֱ��ʵ�����֧�֣����3.5��480*320��ILI9488֧�֡�
*									  7. ����FreeRTOS��V8.2.0
*	    V4.0    2016-06-20  Eric2013  1. ����STemWin��5.28
*                                     2. ����fatfs��V0.11a
*									  3. ����FreeRTOS��V8.3.0
*
*                                                                         
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "MainTask.h"


/*
*********************************************************************************************************
*	                       GUI_WIDGET_CREATE_INFO��������
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Counting...",     0,      30,  90, 260, 140, FRAMEWIN_CF_MOVEABLE },
  { TEXT_CreateIndirect,     "00",     GUI_ID_TEXT0,    10,  10, 120,  80 },
  { RADIO_CreateIndirect,    "",       GUI_ID_RADIO0,  150,  10, 100,  80, 0, 4 },
  { BUTTON_CreateIndirect,   "OK",     GUI_ID_OK,       10, 100,  60,  18 },
  { BUTTON_CreateIndirect,   "Cancel", GUI_ID_CANCEL,  180, 100,  60,  18 },
};

/*
*********************************************************************************************************
*	                                      ����
*********************************************************************************************************
*/
static const char * _apLabel[] = {
  "GUI_FontFD32",
  "GUI_FontFD48",
  "GUI_FontFD64",
  "GUI_FontFD80",
};

static const GUI_FONT * _apFont[] = {
  &GUI_FontD32,
  &GUI_FontD48,
  &GUI_FontD64,
  &GUI_FontD80
};

static const char * _asExplain[] = {
  "Please use the RADIO buttons to select",
  "the big digit font used for counting."
};

/*
*********************************************************************************************************
*	�� �� ��: _SetFont
*	����˵��: ������ʾ����		
*	��    ��: hDlg  �Ի�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _SetFont(WM_HWIN hDlg) 
{
	WM_HWIN hItem;
	int Index;
	hItem = WM_GetDialogItem(hDlg, GUI_ID_RADIO0);
	Index = RADIO_GetValue(hItem);
	hItem = WM_GetDialogItem(hDlg, GUI_ID_TEXT0);
	TEXT_SetFont(hItem, _apFont[Index]);
}

/*
*********************************************************************************************************
*	�� �� ��: _cbBkWindow
*	����˵��: ���洰�ڻص�����		
*	��    ��: pMsg  �ص����� 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbBkWindow(WM_MESSAGE * pMsg) 
{
	int i;
	
	switch (pMsg->MsgId) 
	{
		/* �ػ���Ϣ */
		case WM_PAINT:
			GUI_SetBkColor(GUI_BLUE);
			GUI_Clear();
			GUI_SetColor(GUI_WHITE);
			GUI_SetFont(&GUI_Font24_ASCII);
			GUI_DispStringHCenterAt("Counting Sample", 160, 5);
			GUI_SetFont(&GUI_Font8x16);
			for (i = 0; i < GUI_COUNTOF(_asExplain); i++) 
			{
				GUI_DispStringAt(_asExplain[i], 5, 40 + i * 16);
			}
			
		default:
			WM_DefaultProc(pMsg);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallback
*	����˵��: �Ի���ص�����	
*	��    ��: pMsg  �ص����� 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbCallback(WM_MESSAGE * pMsg) 
{
	int i;
	int NCode, Id;
	WM_HWIN hDlg, hItem;
	hDlg = pMsg->hWin;
	switch (pMsg->MsgId) 
	{
		case WM_INIT_DIALOG:
			hItem = WM_GetDialogItem(hDlg, GUI_ID_RADIO0);
			for (i = 0; i < GUI_COUNTOF(_apLabel); i++) 
			{
				RADIO_SetText(hItem, _apLabel[i], i);
			}
			_SetFont(hDlg);
			break;
			
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);    
			NCode = pMsg->Data.v;             
			switch (NCode) 
			{
				case WM_NOTIFICATION_VALUE_CHANGED: 
					_SetFont(hDlg);
					break;
				
				/* ������ť����Ϣ���� */
				case WM_NOTIFICATION_RELEASED:     
					if (Id == GUI_ID_OK) 
					{            
						GUI_EndDialog(hDlg, 0);
					}
					if (Id == GUI_ID_CANCEL) 
					{       
						GUI_EndDialog(hDlg, 1);
					}
					break;
			}
			break;
			
		default:
		WM_DefaultProc(pMsg);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: MainTask
*	����˵��: GUI������	
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MainTask(void) 
{
	int Value = 0;
	WM_HWIN hDlgFrame;

	/* ��ʼ�� */
	GUI_Init();
	//TOUCH_Calibration();
	GUI_CURSOR_Show();
	WM_SetCallback(WM_HBKWIN, _cbBkWindow);  
	WM_SetCreateFlags(WM_CF_MEMDEV);   
	hDlgFrame = 0;
	
	while(1) 
	{
		WM_HWIN hDlg, hText;
		char acText[3] = {0};
		
		GUI_Delay(100);
		/* ����Ի��򱻹رվ����µĽ����ٴ� */
		if (!WM_IsWindow(hDlgFrame)) 
		{
			Value = 0;
			hDlgFrame = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, 0, 0, 0);
		}
		
		Value = (Value + 1) % 100;
		acText[0] = '0' + Value / 10;
		acText[1] = '0' + Value % 10;
		hDlg = WM_GetClientWindow(hDlgFrame);
		hText = WM_GetDialogItem(hDlg, GUI_ID_TEXT0);
		TEXT_SetText(hText, acText);
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
