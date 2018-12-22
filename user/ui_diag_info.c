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
#include "main.h"
#include "string.h"
#include "stdio.h"
#include "experiment.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_TEXT_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_0 (GUI_ID_USER + 0x03)
#define ID_TEXT_1 (GUI_ID_USER + 0x04)
#define ID_BUTTON_1 (GUI_ID_USER + 0x05)
#define ID_IMAGE_0 (GUI_ID_USER + 0x06)
#define ID_TEXT_2 (GUI_ID_USER + 0x07)

extern const GUI_FONT GUI_FontHZ_kaiti_20;
extern const GUI_BITMAP bminfor_32px;
extern const GUI_BITMAP bmwarning_32px;

static struct ui_exper_info ginfo;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    {WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 150, 120, 500, 240, 0, 0x0, 0},
    {TEXT_CreateIndirect, "信息", ID_TEXT_0, 52, 14, 80, 25, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "开始实验", ID_BUTTON_0, 20, 185, 140, 40, 0, 0x0, 0},
    {TEXT_CreateIndirect, "line1", ID_TEXT_1, 10, 67, 466, 25, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "取消", ID_BUTTON_1, 340, 185, 140, 40, 0, 0x0, 0},
    {IMAGE_CreateIndirect, "Image", ID_IMAGE_0, 10, 10, 32, 32, 0, 0, 0},
    {TEXT_CreateIndirect, "line2", ID_TEXT_2, 10, 105, 462, 25, 0, 0x0, 0},
    // USER START (Optionally insert additional widgets)
    // USER END
};

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
        hItem = pMsg->hWin;
        WINDOW_SetBkColor(hItem, GUI_BLACK);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        if (ginfo.flag)
            BUTTON_SetText(hItem, "停止实验");
        else
            BUTTON_SetText(hItem, "开始实验");
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
        TEXT_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        TEXT_SetTextColor(hItem, GUI_WHITE);
        if (ginfo.flag)
            TEXT_SetText(hItem, "警告");
        else
            TEXT_SetText(hItem, "信息");

        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
        BUTTON_SetFont(hItem, &GUI_FontHZ_kaiti_20);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
        if (ginfo.flag)
            IMAGE_SetBitmap(hItem, &bmwarning_32px);
        else
            IMAGE_SetBitmap(hItem, &bminfor_32px);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
        TEXT_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        TEXT_SetTextColor(hItem, GUI_WHITE);
        if (ginfo.flag)
            TEXT_SetText(hItem, "实验正在进行中");
        else {
            switch (ginfo.func) {
            case EXPER_MSG_AGNO3_START:
                TEXT_SetText(hItem, "即将进行AgNO3浓度检测");
                break;
            case EXPER_MSG_BLOCK_START:
                TEXT_SetText(hItem, "即将进行空白实验");
                break;
            case EXPER_MSG_CL_START:
                TEXT_SetText(hItem, "即将进行水泥氯离子含量检测");
                break;
            default:
                break;
            }
        }

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
        TEXT_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        TEXT_SetTextColor(hItem, GUI_WHITE);
        if (ginfo.flag)
            TEXT_SetText(hItem, "确定停止实验吗?");
        else
            TEXT_SetText(hItem, "准备好了吗?");
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
        case ID_BUTTON_1: // Notifications sent by 'Button'
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
*       CreateWindow
*/

int diag_info_creat(struct ui_exper_info *info)
{
    memcpy(&ginfo, info, sizeof(struct ui_exper_info));
    return GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
