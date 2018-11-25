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
#include "stdio.h"
#include "main.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_LISTBOX_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_LOOK (GUI_ID_USER + 0x02)
#define ID_BUTTON_DEL (GUI_ID_USER + 0x03)
#define ID_BUTTON_EXP (GUI_ID_USER + 0x04)
#define ID_BUTTON_RETURN (GUI_ID_USER + 0x05)
#define ID_BUTTON_EXPALL (GUI_ID_USER + 0x06)
#define ID_PROGBAR_0 (GUI_ID_USER + 0x07)

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
    {LISTBOX_CreateIndirect, "Listbox", ID_LISTBOX_0, 5, 5, 485, 380, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "Button", ID_BUTTON_LOOK, 565, 10, 170, 40, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "Button", ID_BUTTON_DEL, 565, 80, 170, 40, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "Button", ID_BUTTON_EXP, 565, 150, 170, 40, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "Button", ID_BUTTON_RETURN, 565, 370, 170, 40, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "Button", ID_BUTTON_EXPALL, 565, 220, 170, 40, 0, 0x0, 0},
    {PROGBAR_CreateIndirect, "Progbar", ID_PROGBAR_0, 5, 390, 485, 25, 0, 0x0, 0},
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
        FRAMEWIN_SetText(hItem, "���ݴ���");
        FRAMEWIN_SetFont(hItem, &GUI_FontHZ_Arial);
        //
        // Initialization of 'Listbox'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_0);
        LISTBOX_SetFont(hItem, GUI_FONT_32_ASCII);
        LISTBOX_AddString(hItem, "data_000_20180910_1231");
        LISTBOX_AddString(hItem, "data_001_20180910_1232");
        LISTBOX_AddString(hItem, "data_002_20180910_1233");
        LISTBOX_AddString(hItem, "data_003_20180911_0918");
        LISTBOX_AddString(hItem, "data_004_20181011_0920");
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_LOOK);
        BUTTON_SetFont(hItem, &GUI_FontHZ_Consolas);
        BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
        BUTTON_SetText(hItem, "�鿴");
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_DEL);
        BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
        BUTTON_SetFont(hItem, &GUI_FontHZ_Consolas);
        BUTTON_SetText(hItem, "ɾ��");
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_EXP);
        BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
        BUTTON_SetFont(hItem, &GUI_FontHZ_Consolas);
        BUTTON_SetText(hItem, "����");
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_RETURN);
        BUTTON_SetFont(hItem, &GUI_FontHZ_Consolas);
        BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
        BUTTON_SetText(hItem, "����");
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_EXPALL);
        BUTTON_SetFont(hItem, &GUI_FontHZ_Consolas);
        BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
        BUTTON_SetText(hItem, "ȫ������");

        hItem = WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_0);
        PROGBAR_SetFont(hItem, GUI_FONT_20B_ASCII);
        PROGBAR_SetBarColor(hItem, 0, GUI_GREEN);
        PROGBAR_SetMinMax(hItem, 0, 100);
        // USER START (Optionally insert additional code for further widget initialization)
        // USER END
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id)
        {
        case ID_LISTBOX_0: // Notifications sent by 'Listbox'
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
        case ID_BUTTON_LOOK: // Notifications sent by 'Button'
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
        case ID_BUTTON_DEL: // Notifications sent by 'Button'
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
        case ID_BUTTON_RETURN: // Notifications sent by 'Button'
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                printf("run main menu\r\n");
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
        case ID_BUTTON_EXPALL: // Notifications sent by 'Button'
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

WM_HWIN ui_data_creat(void)
{
    g_ui_msg.hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    GUI_ExecCreatedDialog(g_ui_msg.hWin);
    return g_ui_msg.hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/