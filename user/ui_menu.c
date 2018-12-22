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
#include "beep.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0   (GUI_ID_USER + 0x00)

#define ID_IMAGE_BLOCKTEST   (GUI_ID_USER + 0x02)
#define ID_IMAGE_STAND       (GUI_ID_USER + 0x03)
#define ID_IMAGE_DATA        (GUI_ID_USER + 0x04)
#define ID_IMAGE_SETTING     (GUI_ID_USER + 0x05)

#define ID_TEXT_BLOCKTEST    (GUI_ID_USER + 0x06)
#define ID_TEXT_STAND        (GUI_ID_USER + 0x07)
#define ID_TEXT_DATA         (GUI_ID_USER + 0x08)
#define ID_TEXT_SETTING      (GUI_ID_USER + 0x09)

// USER START (Optionally insert additional defines)
extern const GUI_BITMAP bmprocess_running_72px;
extern const GUI_BITMAP bmchemical_72px;
extern const GUI_BITMAP bmpic_view_72px;
extern const GUI_BITMAP bmstand_72px;

extern const GUI_FONT GUI_FontHZ_kaiti_28;
extern const GUI_FONT GUI_FontHZ_kaiti_20;
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 800, 480, 0, 0x0, 0 },

    { IMAGE_CreateIndirect, "block", ID_IMAGE_BLOCKTEST, 102, 140, 72, 72, 0, 0, 0 },
    { IMAGE_CreateIndirect, "stand", ID_IMAGE_STAND, 274, 140, 72, 72, 0, 0, 0 },
    { IMAGE_CreateIndirect, "data", ID_IMAGE_DATA, 446, 140, 72, 72, 0, 0, 0 },
    { IMAGE_CreateIndirect, "setting", ID_IMAGE_SETTING, 620, 140, 72, 72, 0, 0, 0 },
    
    { TEXT_CreateIndirect, "block", ID_TEXT_BLOCKTEST, 82, 212, 120, 45, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "stand", ID_TEXT_STAND, 254, 212, 120, 45, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "data", ID_TEXT_DATA, 426, 212, 120, 45, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "setting", ID_TEXT_SETTING, 600, 212, 120, 45, 0, 0x0, 0 },
    // USER START (Optionally insert additional widgets)
    // USER END
};

// USER START (Optionally insert additional static code)
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
    WM_HWIN      hItem;
    int     NCode;
    int     Id;

    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        //
        // Initialization of 'Framewin'
        //
        hItem = pMsg->hWin;
        FRAMEWIN_SetTitleHeight(hItem, 50);
        FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
        FRAMEWIN_SetFont(hItem, &GUI_FontHZ_kaiti_28);
        FRAMEWIN_SetTextColor(hItem, GUI_BLACK);
        FRAMEWIN_SetClientColor(hItem, GUI_GRAY);
        FRAMEWIN_SetText(hItem, "全自动氯离子检测仪");
        //
        // Initialization of 'Image'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_BLOCKTEST);
        IMAGE_SetBitmap(hItem, &bmchemical_72px);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_BLOCKTEST);
        TEXT_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        TEXT_SetTextColor(hItem, GUI_BLACK);
        TEXT_SetText(hItem, "水泥检测");  

        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_STAND);
        IMAGE_SetBitmap(hItem, &bmstand_72px);



        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_STAND);
        TEXT_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        TEXT_SetTextColor(hItem, GUI_BLACK);
        TEXT_SetText(hItem, "其他检测");
        //
        // Initialization of 'Image'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_DATA);
        IMAGE_SetBitmap(hItem, &bmpic_view_72px);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DATA);
        TEXT_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        TEXT_SetTextColor(hItem, GUI_BLACK);
        TEXT_SetText(hItem, "数据处理");
        //
        // Initialization of 'Image'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_SETTING);
        IMAGE_SetBitmap(hItem, &bmprocess_running_72px);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SETTING);
        TEXT_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        TEXT_SetTextColor(hItem, GUI_BLACK);
        TEXT_SetText(hItem, "系统设置");

    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch(Id) {
        case ID_IMAGE_BLOCKTEST:
        case ID_TEXT_BLOCKTEST:
            switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                beep_clicked();
                g_ui_msg.msg = MSG_LOAD_UI_BLOCKTEST;
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            default:
                break;
            }
            break;
        case ID_IMAGE_DATA:
        case ID_TEXT_DATA:
            switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                beep_clicked();
                g_ui_msg.msg = MSG_LOAD_UI_DATA;
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            default:
                break;
            }
            break;
        case ID_IMAGE_SETTING:
        case ID_TEXT_SETTING:
            switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                beep_clicked();
                g_ui_msg.msg = MSG_LOAD_UI_SETTING;
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
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
int main_menu_creat(void)
{
    return GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
