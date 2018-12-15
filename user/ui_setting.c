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
#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"
#include "main.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_BUTTON_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_1 (GUI_ID_USER + 0x02)
#define ID_SPINBOX_0 (GUI_ID_USER + 0x04)
#define ID_TEXT_0 (GUI_ID_USER + 0x05)
#define ID_SPINBOX_1 (GUI_ID_USER + 0x06)
#define ID_TEXT_1 (GUI_ID_USER + 0x07)
#define ID_SPINBOX_2 (GUI_ID_USER + 0x08)
#define ID_TEXT_2 (GUI_ID_USER + 0x09)
#define ID_SPINBOX_3 (GUI_ID_USER + 0x0A)
#define ID_SPINBOX_4 (GUI_ID_USER + 0x0B)
#define ID_TEXT_3 (GUI_ID_USER + 0x0C)
#define ID_TEXT_4 (GUI_ID_USER + 0x0D)
#define ID_SPINBOX_5 (GUI_ID_USER + 0x0F)
#define ID_TEXT_5 (GUI_ID_USER + 0x10)
#define ID_TEXT_6 (GUI_ID_USER + 0x11)
#define ID_BUTTON_2 (GUI_ID_USER + 0x12)
#define ID_BUTTON_3 (GUI_ID_USER + 0x13)

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
    {BUTTON_CreateIndirect, "触摸屏校准", ID_BUTTON_0, 10, 13, 769, 55, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "保存设置", ID_BUTTON_1, 589, 300, 190, 35, 0, 0x0, 0},
    {SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_0, 10, 300, 70, 35, 0, 0x0, 0},
    {TEXT_CreateIndirect, "年", ID_TEXT_0, 87, 300, 35, 35, 0, 0x64, 0},
    {SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_1, 126, 300, 70, 35, 0, 0x0, 0},
    {TEXT_CreateIndirect, "月", ID_TEXT_1, 200, 300, 35, 35, 0, 0x64, 0},
    {SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_2, 240, 300, 70, 35, 0, 0x0, 0},
    {TEXT_CreateIndirect, "日", ID_TEXT_2, 312, 300, 35, 35, 0, 0x64, 0},
    {SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_3, 352, 300, 70, 35, 0, 0x0, 0},
    {SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_4, 470, 300, 70, 35, 0, 0x0, 0},
    {TEXT_CreateIndirect, "时", ID_TEXT_3, 428, 300, 35, 35, 0, 0x64, 0},
    {TEXT_CreateIndirect, "分", ID_TEXT_4, 546, 300, 35, 35, 0, 0x64, 0},
    {SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_5, 144, 160, 107, 35, 0, 0x0, 0},
    {TEXT_CreateIndirect, "越变电位", ID_TEXT_5, 10, 160, 140, 38, 0, 0x64, 0},
    {TEXT_CreateIndirect, "mV", ID_TEXT_6, 258, 160, 43, 35, 0, 0x64, 0},
    {BUTTON_CreateIndirect, "保存设置", ID_BUTTON_2, 349, 160, 428, 35, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "返回", ID_BUTTON_3, 638, 384, 146, 34, 0, 0x0, 0},
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
        FRAMEWIN_SetTextColor(hItem, GUI_BLUE);
        FRAMEWIN_SetClientColor(hItem, GUI_GRAY);
        FRAMEWIN_SetFont(hItem, &GUI_FontHZ_Arial);
        FRAMEWIN_SetText(hItem, "系统设置");
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetFont(hItem, &GUI_FontHZ_Consolas);
        BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
        BUTTON_SetFont(hItem, &GUI_FontHZ_Consolas);
        BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
        //
        // Initialization of 'Spinbox'
        // year
        hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_0);
        SPINBOX_SetFont(hItem, GUI_FONT_32_ASCII);
        SPINBOX_SetRange(hItem, 0, 99);
        SPINBOX_SetValue(hItem, 18);
        //
        // Initialization of 'Text'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
        TEXT_SetFont(hItem, &GUI_FontHZ_Consolas);
        //
        // Initialization of 'Spinbox'
        // month
        hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_1);
        SPINBOX_SetFont(hItem, GUI_FONT_32_ASCII);
        SPINBOX_SetRange(hItem, 1, 12);
        SPINBOX_SetValue(hItem, 6);
        //
        // Initialization of 'Text'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
        TEXT_SetFont(hItem, &GUI_FontHZ_Consolas);
        //
        // Initialization of 'Spinbox'
        // day
        hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_2);
        SPINBOX_SetFont(hItem, GUI_FONT_32_ASCII);
        SPINBOX_SetRange(hItem, 1, 31);
        SPINBOX_SetValue(hItem, 18);
        //
        // Initialization of 'Text'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
        TEXT_SetFont(hItem, &GUI_FontHZ_Consolas);
        //
        // Initialization of 'Spinbox'
        // hour
        hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_3);
        SPINBOX_SetFont(hItem, GUI_FONT_32_ASCII);
        SPINBOX_SetRange(hItem, 0, 23);
        SPINBOX_SetValue(hItem, 10);
        //
        // Initialization of 'Spinbox'
        // minute
        hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_4);
        SPINBOX_SetFont(hItem, GUI_FONT_32_ASCII);
        SPINBOX_SetRange(hItem, 0, 59);
        SPINBOX_SetValue(hItem, 58);
        //
        // Initialization of 'Text'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
        TEXT_SetFont(hItem, &GUI_FontHZ_Consolas);
        //
        // Initialization of 'Text'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
        TEXT_SetFont(hItem, &GUI_FontHZ_Consolas);
        //
        // Initialization of 'Spinbox'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_5);
        SPINBOX_SetFont(hItem, GUI_FONT_32_ASCII);
        SPINBOX_SetRange(hItem, 190, 230);
        SPINBOX_SetValue(hItem, 210);
        SPINBOX_SetStep(hItem, 5);
        //
        // Initialization of 'Text'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
        TEXT_SetFont(hItem, &GUI_FontHZ_Consolas);
        //
        // Initialization of 'Text'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
        TEXT_SetFont(hItem, GUI_FONT_32_ASCII);
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
        BUTTON_SetFont(hItem, &GUI_FontHZ_Consolas);
        BUTTON_SetTextColor(hItem, 0, GUI_BLUE);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
        BUTTON_SetFont(hItem, &GUI_FontHZ_Consolas);
        BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
        // USER START (Optionally insert additional code for further widget initialization)
        // USER END
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id)
        {
        case ID_BUTTON_0: // Notifications sent by 'Button'
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // touch calc
                printf("sss\r\n");
                g_ui_msg.msg = MSG_LOAD_UI_TOUCH_CALC;
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
        case ID_BUTTON_1: // Notifications sent by 'Button'
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
        case ID_SPINBOX_0: // Notifications sent by 'Spinbox'
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
            case WM_NOTIFICATION_MOVED_OUT:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_VALUE_CHANGED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        case ID_SPINBOX_1: // Notifications sent by 'Spinbox'
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
            case WM_NOTIFICATION_MOVED_OUT:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_VALUE_CHANGED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        case ID_SPINBOX_2: // Notifications sent by 'Spinbox'
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
            case WM_NOTIFICATION_MOVED_OUT:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_VALUE_CHANGED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        case ID_SPINBOX_3: // Notifications sent by 'Spinbox'
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
            case WM_NOTIFICATION_MOVED_OUT:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_VALUE_CHANGED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        case ID_SPINBOX_4: // Notifications sent by 'Spinbox'
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
            case WM_NOTIFICATION_MOVED_OUT:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_VALUE_CHANGED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        case ID_SPINBOX_5: // Notifications sent by 'Spinbox'
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
            case WM_NOTIFICATION_MOVED_OUT:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_VALUE_CHANGED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        case ID_BUTTON_2: // Notifications sent by 'Button'
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
        case ID_BUTTON_3: // Notifications sent by 'Button'
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                g_ui_msg.msg = MSG_LOAD_UI_MENU;
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
WM_HWIN ui_setting_creat(void)
{
    WM_HWIN hWin;

    hWin = GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
