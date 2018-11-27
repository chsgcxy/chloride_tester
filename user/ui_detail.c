/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.32                          *
*        Compiled Oct  8 2015, 11:59:02                              *
*        (c) 2015 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

// USER START (Optionally insert additional includes)
// USER END

#include "DIALOG.h"
#include "main.h"
#include "experiment.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_LISTVIEW_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_RETURN (GUI_ID_USER + 0x03)
#define ID_BUTTON_EXP (GUI_ID_USER + 0x04)
#define ID_BUTTON_PRINT (GUI_ID_USER + 0x05)
#define ID_TEXT_0 (GUI_ID_USER + 0x06)
#define ID_TEXT_1 (GUI_ID_USER + 0x07)
#define ID_TEXT_2 (GUI_ID_USER + 0x08)
#define ID_TEXT_5 (GUI_ID_USER + 0x09)
#define ID_TEXT_6 (GUI_ID_USER + 0x0A)
#define ID_TEXT_7 (GUI_ID_USER + 0x0B)
#define ID_TEXT_8 (GUI_ID_USER + 0x0C)

// USER START (Optionally insert additional defines)
extern const GUI_FONT GUI_FontHZ_Consolas;
extern const GUI_FONT GUI_FontHZ_Arial;
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    {FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 800, 480, 0, 0x0, 0},
    {LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0, 10, 96, 630, 319, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "Button", ID_BUTTON_RETURN, 660, 370, 120, 40, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "Button", ID_BUTTON_EXP, 660, 200, 120, 40, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "Button", ID_BUTTON_PRINT, 660, 96, 120, 40, 0, 0x0, 0},
    {TEXT_CreateIndirect, "Text", ID_TEXT_0, 10, 5, 60, 25, 0, 0x64, 0},
    {TEXT_CreateIndirect, "Text", ID_TEXT_1, 100, 5, 80, 25, 0, 0x64, 0},
    {TEXT_CreateIndirect, "Text", ID_TEXT_2, 260, 5, 250, 25, 0, 0x64, 0},
    {TEXT_CreateIndirect, "Text", ID_TEXT_5, 10, 35, 250, 25, 0, 0x64, 0},
    {TEXT_CreateIndirect, "Text", ID_TEXT_6, 260, 35, 80, 25, 0, 0x64, 0},
    {TEXT_CreateIndirect, "Text", ID_TEXT_7, 10, 65, 250, 25, 0, 0x64, 0},
    {TEXT_CreateIndirect, "Text", ID_TEXT_8, 260, 65, 81, 25, 0, 0x64, 0},
    // USER START (Optionally insert additional widgets)
    // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE *pMsg)
{
    WM_HWIN hItem;
    int NCode;
    int Id;
    HEADER_Handle hHeader;
    // USER START (Optionally insert additional variables)
    // USER END

    switch (pMsg->MsgId)
    {
    case WM_INIT_DIALOG:
        //
        // Initialization of 'Framewin'
        //
        hItem = pMsg->hWin;
        FRAMEWIN_SetTitleHeight(hItem, 50);
        FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
        FRAMEWIN_SetFont(hItem, &GUI_FontHZ_Arial);
        FRAMEWIN_SetTextColor(hItem, GUI_BLUE);
        FRAMEWIN_SetText(hItem, "实验数据");
        FRAMEWIN_SetClientColor(hItem, GUI_GRAY);
        //
        // Initialization of 'Listview'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
        LISTVIEW_SetRowHeight(hItem, 29);
        LISTVIEW_SetBkColor(hItem, 0, GUI_GRAY);
        LISTVIEW_SetGridVis(hItem, 1);
        LISTVIEW_AddRow(hItem, NULL);
        LISTVIEW_AddRow(hItem, NULL);
        LISTVIEW_AddRow(hItem, NULL);
        LISTVIEW_AddRow(hItem, NULL);
        LISTVIEW_AddRow(hItem, NULL);
        LISTVIEW_AddRow(hItem, NULL);
        LISTVIEW_AddRow(hItem, NULL);
        LISTVIEW_AddRow(hItem, NULL);
        LISTVIEW_AddRow(hItem, NULL);
        LISTVIEW_AddRow(hItem, NULL);
        LISTVIEW_SetFont(hItem, GUI_FONT_24_ASCII);

        hHeader = LISTVIEW_GetHeader(hItem);
        HEADER_SetFont(hHeader, GUI_FONT_24_ASCII);
        HEADER_SetTextColor(hHeader, GUI_BLUE);
        HEADER_SetHeight(hHeader, 29);
        HEADER_SetBkColor(hHeader, GUI_GRAY);
        LISTVIEW_AddColumn(hItem, 105, "AgNO3", GUI_TA_HCENTER | GUI_TA_VCENTER);
        LISTVIEW_AddColumn(hItem, 105, "E/Mv", GUI_TA_HCENTER | GUI_TA_VCENTER);
        LISTVIEW_AddColumn(hItem, 105, "E/Mv", GUI_TA_HCENTER | GUI_TA_VCENTER);
        LISTVIEW_AddColumn(hItem, 105, "v_ml", GUI_TA_HCENTER | GUI_TA_VCENTER);
        LISTVIEW_AddColumn(hItem, 105, "Col", GUI_TA_HCENTER | GUI_TA_VCENTER);
        LISTVIEW_AddColumn(hItem, 105, "Col", GUI_TA_HCENTER | GUI_TA_VCENTER);
        
        LISTVIEW_SetTextColor(hItem, 0, GUI_WHITE);
        LISTVIEW_SetItemText(hItem, 0, 0, "0.00");
        LISTVIEW_SetItemText(hItem, 0, 1, "0.01");
        LISTVIEW_SetItemText(hItem, 5, 1, "0.01");
        LISTVIEW_SetTextColor(hItem, 0, GUI_RED);
        LISTVIEW_SetItemText(hItem, 0, 2, "0.03");
        LISTVIEW_SetItemText(hItem, 1, 2, "0.03");
        LISTVIEW_SetItemText(hItem, 2, 2, "0.10");
        LISTVIEW_SetItemText(hItem, 3, 2, "0.10");
        LISTVIEW_SetItemText(hItem, 4, 2, "0.10");
        LISTVIEW_SetItemText(hItem, 5, 2, "0.10");
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_RETURN);
        BUTTON_SetFont(hItem, &GUI_FontHZ_Consolas);
        BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
        BUTTON_SetText(hItem, "返回");
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_EXP);
        BUTTON_SetFont(hItem, &GUI_FontHZ_Consolas);
        BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
        BUTTON_SetText(hItem, "导出");
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_PRINT);
        BUTTON_SetFont(hItem, &GUI_FontHZ_Consolas);
        BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
        BUTTON_SetText(hItem, "打印");
        //
        // Initialization of 'Text'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
        TEXT_SetFont(hItem, &GUI_FontHZ_Consolas);
        TEXT_SetTextColor(hItem, GUI_WHITE);
        TEXT_SetText(hItem, "序号");
        //
        // Initialization of 'Text'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
        TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
        TEXT_SetTextColor(hItem, GUI_WHITE);
        TEXT_SetText(hItem, "001");
        //
        // Initialization of 'Text'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
        TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
        TEXT_SetTextColor(hItem, GUI_WHITE);
        TEXT_SetText(hItem, "2018-11-23  19:23");
        //
        // Initialization of 'Text'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
        TEXT_SetFont(hItem,  &GUI_FontHZ_Consolas);
        TEXT_SetTextColor(hItem, GUI_WHITE);
        TEXT_SetText(hItem, "硝酸银用量");
        //
        // Initialization of 'Text'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
        TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
        TEXT_SetTextColor(hItem, GUI_WHITE);
        TEXT_SetText(hItem, "0.01mL");
        //
        // Initialization of 'Text'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_7);
        TEXT_SetFont(hItem,  &GUI_FontHZ_Consolas);
        TEXT_SetTextColor(hItem, GUI_WHITE);
        TEXT_SetText(hItem, "每克试样含氯离子");
        //
        // Initialization of 'Text'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_8);
        TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
        TEXT_SetTextColor(hItem, GUI_WHITE);
        TEXT_SetText(hItem, "0.12%");
        // USER START (Optionally insert additional code for further widget initialization)
        // USER END
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id)
        {
        case ID_LISTVIEW_0: // Notifications sent by 'Listview'
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_SEL_CHANGED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        case ID_BUTTON_RETURN: // Notifications sent by 'Button'
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                printf("run data\r\n");
                g_ui_msg.msg = MSG_LOAD_UI_DATA;
                GUI_EndDialog(pMsg->hWin, 0);
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        case ID_BUTTON_EXP: // Notifications sent by 'Button'
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        case ID_BUTTON_PRINT: // Notifications sent by 'Button'
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
            // USER START (Optionally insert additional code for further Ids)
            // USER END
        }
        break;
    // USER START (Optionally insert additional message handling)
    // USER END
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateFramewin
*/

WM_HWIN data_detail_creat(void)
{
    WM_HWIN hWin;

    hWin = GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
