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
#include "beep.h"
#include "stdio.h"
#include "string.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_BUTTON_NUM7 (GUI_ID_USER + 0x01)
#define ID_BUTTON_NUM8 (GUI_ID_USER + 0x02)
#define ID_EDIT_0 (GUI_ID_USER + 0x03)
#define ID_BUTTON_NUM9 (GUI_ID_USER + 0x04)
#define ID_BUTTON_NUM4 (GUI_ID_USER + 0x05)
#define ID_BUTTON_NUM5 (GUI_ID_USER + 0x06)
#define ID_BUTTON_NUM6 (GUI_ID_USER + 0x07)
#define ID_BUTTON_NUM1 (GUI_ID_USER + 0x08)
#define ID_BUTTON_NUM2 (GUI_ID_USER + 0x09)
#define ID_BUTTON_NUM3 (GUI_ID_USER + 0x0A)
#define ID_BUTTON_NUM0 (GUI_ID_USER + 0x0B)
#define ID_BUTTON_POINT (GUI_ID_USER + 0x0C)
#define ID_BUTTON_DIACARD (GUI_ID_USER + 0x0D)
#define ID_BUTTON_SET (GUI_ID_USER + 0x0E)
#define ID_BUTTON_BACK (GUI_ID_USER + 0x0F)

// USER START (Optionally insert additional defines)
// USER END
extern const GUI_FONT GUI_FontHZ_kaiti_28;
extern const GUI_FONT GUI_FontHZ_kaiti_20;
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
    {WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 150, 100, 368, 260, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "Button", ID_BUTTON_NUM7, 2, 55, 80, 35, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "Button", ID_BUTTON_NUM8, 102, 55, 80, 35, 0, 0x0, 0},
    {EDIT_CreateIndirect, "Edit", ID_EDIT_0, 2, 2, 284, 35, 0, 0x64, 0},
    {BUTTON_CreateIndirect, "Button", ID_BUTTON_NUM9, 202, 55, 80, 35, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "Button", ID_BUTTON_NUM4, 2, 110, 80, 35, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "Button", ID_BUTTON_NUM5, 102, 110, 80, 35, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "Button", ID_BUTTON_NUM6, 202, 110, 80, 35, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "Button", ID_BUTTON_NUM1, 2, 165, 80, 35, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "Button", ID_BUTTON_NUM2, 102, 165, 80, 35, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "Button", ID_BUTTON_NUM3, 202, 165, 80, 35, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "Button", ID_BUTTON_NUM0, 2, 220, 179, 35, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "Button", ID_BUTTON_POINT, 202, 220, 80, 35, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "Button", ID_BUTTON_DIACARD, 305, 55, 60, 90, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "Button", ID_BUTTON_SET, 305, 165, 60, 90, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "Button", ID_BUTTON_BACK, 305, 2, 60, 35, 0, 0x0, 0},
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
static char numpad_buf[32];
static int count = 0;
// USER END

char *numpad_get(void)
{
    return numpad_buf;
}

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
        // Initialization of 'Window'
        //
        hItem = pMsg->hWin;
        WINDOW_SetBkColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_NUM7);
        BUTTON_SetFont(hItem, GUI_FONT_32B_ASCII);
        BUTTON_SetText(hItem, "7");
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_NUM8);
        BUTTON_SetFont(hItem, GUI_FONT_32B_ASCII);
        BUTTON_SetText(hItem, "8");
        //
        // Initialization of 'Edit'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0);
        EDIT_SetText(hItem, "0");
        EDIT_SetFont(hItem, GUI_FONT_32B_ASCII);
        EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_NUM9);
        BUTTON_SetFont(hItem, GUI_FONT_32B_ASCII);
        BUTTON_SetText(hItem, "9");
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_NUM4);
        BUTTON_SetFont(hItem, GUI_FONT_32B_ASCII);
        BUTTON_SetText(hItem, "4");
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_NUM5);
        BUTTON_SetFont(hItem, GUI_FONT_32B_ASCII);
        BUTTON_SetText(hItem, "5");
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_NUM6);
        BUTTON_SetFont(hItem, GUI_FONT_32B_ASCII);
        BUTTON_SetText(hItem, "6");
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_NUM1);
        BUTTON_SetFont(hItem, GUI_FONT_32B_ASCII);
        BUTTON_SetText(hItem, "1");
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_NUM2);
        BUTTON_SetFont(hItem, GUI_FONT_32B_ASCII);
        BUTTON_SetText(hItem, "2");
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_NUM3);
        BUTTON_SetFont(hItem, GUI_FONT_32B_ASCII);
        BUTTON_SetText(hItem, "3");
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_NUM0);
        BUTTON_SetFont(hItem, GUI_FONT_32B_ASCII);
        BUTTON_SetText(hItem, "0");
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_POINT);
        BUTTON_SetFont(hItem, GUI_FONT_32B_ASCII);
        BUTTON_SetText(hItem, ".");
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_DIACARD);
        BUTTON_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        BUTTON_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
        BUTTON_SetText(hItem, "ȡ\n��");
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SET);
        BUTTON_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        BUTTON_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
        BUTTON_SetText(hItem, "��\n��");
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_BACK);
        BUTTON_SetFont(hItem, GUI_FONT_32B_ASCII);
        BUTTON_SetText(hItem, "X");
        // USER START (Optionally insert additional code for further widget initialization)
        count = 0;
        numpad_buf[0] = '0';
        numpad_buf[1] = 0;
        // USER END
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id)
        {
        case ID_BUTTON_NUM7: // Notifications sent by 'Button'
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                beep_clicked();
                numpad_buf[count++] = '7';
                numpad_buf[count] = 0;
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
        case ID_BUTTON_NUM8: // Notifications sent by 'Button'
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                beep_clicked();
                numpad_buf[count++] = '8';
                numpad_buf[count] = 0;
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
        case ID_EDIT_0: // Notifications sent by 'Edit'
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
            case WM_NOTIFICATION_VALUE_CHANGED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        case ID_BUTTON_NUM9: // Notifications sent by 'Button'
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                beep_clicked();
                numpad_buf[count++] = '9';
                numpad_buf[count] = 0;
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
        case ID_BUTTON_NUM4: // Notifications sent by 'Button'
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                beep_clicked();
                numpad_buf[count++] = '4';
                numpad_buf[count] = 0;
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
        case ID_BUTTON_NUM5: // Notifications sent by 'Button'
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                beep_clicked();
                numpad_buf[count++] = '5';
                numpad_buf[count] = 0;
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
        case ID_BUTTON_NUM6: // Notifications sent by 'Button'
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                beep_clicked();
                numpad_buf[count++] = '6';
                numpad_buf[count] = 0;
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
        case ID_BUTTON_NUM1: // Notifications sent by 'Button'
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                beep_clicked();
                numpad_buf[count++] = '1';
                numpad_buf[count] = 0;
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
        case ID_BUTTON_NUM2: // Notifications sent by 'Button'
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                beep_clicked();
                numpad_buf[count++] = '2';
                numpad_buf[count] = 0;
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
        case ID_BUTTON_NUM3: // Notifications sent by 'Button'
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                beep_clicked();
                numpad_buf[count++] = '3';
                numpad_buf[count] = 0;
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
        case ID_BUTTON_NUM0: // Notifications sent by 'Button'
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                beep_clicked();
                numpad_buf[count++] = '0';
                numpad_buf[count] = 0;
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
        case ID_BUTTON_POINT: // Notifications sent by 'Button'
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                beep_clicked();
                numpad_buf[count++] = '.';
                numpad_buf[count] = 0;
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
        case ID_BUTTON_DIACARD: // Notifications sent by 'Button'
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                beep_clicked();
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
        case ID_BUTTON_SET: // Notifications sent by 'Button'
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                beep_clicked();
                GUI_EndDialog(pMsg->hWin, 1);
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
        case ID_BUTTON_BACK: // Notifications sent by 'Button'
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                beep_clicked();
                if (count == 1)
                    numpad_buf[--count] = '0';
                else if (count > 1)
                    numpad_buf[--count] = 0;
                else
                    
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
        hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0);
        EDIT_SetText(hItem, numpad_buf);
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
*       CreateWindow
*/
int numpad_creat(void)
{
    return GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
