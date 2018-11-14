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

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0   (GUI_ID_USER + 0x00)

#define ID_IMAGE_MEASURE   (GUI_ID_USER + 0x02)
#define ID_IMAGE_VIEW      (GUI_ID_USER + 0x03)
#define ID_IMAGE_USB       (GUI_ID_USER + 0x04)
#define ID_IMAGE_SETTING   (GUI_ID_USER + 0x05)

#define ID_TEXT_MEASURE    (GUI_ID_USER + 0x06)
#define ID_TEXT_VIEW       (GUI_ID_USER + 0x07)
#define ID_TEXT_USB        (GUI_ID_USER + 0x08)
#define ID_TEXT_SETTING    (GUI_ID_USER + 0x09)

// USER START (Optionally insert additional defines)
extern const GUI_BITMAP bmpic_measure_72px;
extern const GUI_BITMAP bmpic_setting_72px;
extern const GUI_BITMAP bmpic_usb_disk_72px;
extern const GUI_BITMAP bmpic_view_72px;

extern const GUI_FONT GUI_FontHZ_Consolas;
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 800, 480, 0, 0x0, 0 },

    { IMAGE_CreateIndirect, "measure", ID_IMAGE_MEASURE, 102, 140, 72, 72, 0, 0, 0 },
    { IMAGE_CreateIndirect, "view", ID_IMAGE_VIEW, 276, 140, 72, 72, 0, 0, 0 },
    { IMAGE_CreateIndirect, "usb", ID_IMAGE_USB, 450, 140, 72, 72, 0, 0, 0 },
    { IMAGE_CreateIndirect, "setting", ID_IMAGE_SETTING, 624, 140, 72, 72, 0, 0, 0 },

    { TEXT_CreateIndirect, "measure", ID_TEXT_MEASURE, 113, 225, 45, 25, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "view", ID_TEXT_VIEW, 289, 225, 45, 25, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "usb", ID_TEXT_USB, 461, 225, 45, 25, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "setting", ID_TEXT_SETTING, 635, 225, 45, 25, 0, 0x0, 0 },
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
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'Framewin'
    //
    hItem = pMsg->hWin;
    FRAMEWIN_SetTitleHeight(hItem, 50);
    FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    FRAMEWIN_SetFont(hItem, &GUI_FontHZ_Consolas);
    FRAMEWIN_SetText(hItem, "主菜单");
    //
    // Initialization of 'Image'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_MEASURE);
    IMAGE_SetBitmap(hItem, &bmpic_measure_72px);

    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_MEASURE);
    TEXT_SetFont(hItem, &GUI_FontHZ_Consolas);
    TEXT_SetText(hItem, "测量");
    //
    // Initialization of 'Image'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_VIEW);
    IMAGE_SetBitmap(hItem, &bmpic_view_72px);

    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_VIEW);
    TEXT_SetFont(hItem, &GUI_FontHZ_Consolas);
    TEXT_SetText(hItem, "数据");
    //
    // Initialization of 'Image'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_USB);
    IMAGE_SetBitmap(hItem, &bmpic_usb_disk_72px);

    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_USB);
    TEXT_SetFont(hItem, &GUI_FontHZ_Consolas);
    TEXT_SetText(hItem, "U盘");
    //
    // Initialization of 'Image'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_SETTING);
    IMAGE_SetBitmap(hItem, &bmpic_setting_72px);

    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SETTING);
    TEXT_SetFont(hItem, &GUI_FontHZ_Consolas);
    TEXT_SetText(hItem, "设置");


    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
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
WM_HWIN main_menu_creat(void) {
  WM_HWIN hWin;

  hWin = GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
