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
#include "beep.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0   (GUI_ID_USER + 0x00)
#define ID_BUTTON_GET (GUI_ID_USER + 0x01)
#define ID_BUTTON_PUT (GUI_ID_USER + 0x02)
#define ID_BUTTON_RETURN (GUI_ID_USER + 0x03)
#define ID_BUTTON_START_NO3 (GUI_ID_USER + 0x04)
#define ID_BUTTON_START_TEST (GUI_ID_USER + 0x05)
#define ID_BUTTON_START_BLOCK (GUI_ID_USER + 0x06)
#define ID_BUTTON_CLEAR (GUI_ID_USER + 0x07)

#define ID_TEXT_NO3ND (GUI_ID_USER + 0x08)
#define ID_TEXT_NO3YL (GUI_ID_USER + 0x09)
#define ID_TEXT_DJDW (GUI_ID_USER + 0x0A)
#define ID_TEXT_RYCL (GUI_ID_USER + 0x0B)
#define ID_TEXT_NACLND (GUI_ID_USER + 0x0C)
#define ID_TEXT_PERCENTAGE (GUI_ID_USER + 0x0D)
#define ID_TEXT_5 (GUI_ID_USER + 0x0E)

#define ID_PROGBAR_0 (GUI_ID_USER + 0x0F)
#define ID_GRAPH_0   (GUI_ID_USER + 0x10)

#define ID_TEXT_NACLND_VALUE  (GUI_ID_USER + 0x11)
#define ID_TEXT_NO3ND_VALUE   (GUI_ID_USER + 0x12)
#define ID_TEXT_NO3YL_VALUE   (GUI_ID_USER + 0x13)
#define ID_TEXT_DJDW_VALUE    (GUI_ID_USER + 0x14)
#define ID_TEXT_PERCENT_VALUE (GUI_ID_USER + 0x15)

#define ID_TEXT_TEMP          (GUI_ID_USER + 0x16)
#define ID_TEXT_TEMP_VALUE    (GUI_ID_USER + 0x17)
// USER START (Optionally insert additional defines)
extern const GUI_FONT GUI_FontHZ_kaiti_20;
extern const GUI_FONT GUI_Fontfont_spec;
// USER END


extern int diag_err_creat(void);
extern int diag_info_creat(struct ui_exper_info *info);
extern int diag_res_creat(struct ui_exper_res *res);

static struct ui_exper_info ginfo;
static struct ui_exper_res gres;
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
    {WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    
    {TEXT_CreateIndirect, "��Һ����", ID_TEXT_RYCL, 5, 425, 120, 35, 0, 0x64, 0},
    {PROGBAR_CreateIndirect, "Progbar", ID_PROGBAR_0, 135, 425, 150, 30, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "��Һ", ID_BUTTON_GET, 320, 415, 120, 50, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "��Һ", ID_BUTTON_PUT, 490, 415, 120, 50, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "��ϴ", ID_BUTTON_CLEAR, 660, 415, 120, 50, 0, 0x0, 0},
    
    {TEXT_CreateIndirect, "�����ӱ�׼ҺŨ��", ID_TEXT_NACLND, 5, 15, 250, 32, 0, 0x64, 0},
    {TEXT_CreateIndirect, "0.02mol/L", ID_TEXT_NACLND_VALUE, 75, 50, 160, 32, 0, 0x64, 0},

    {TEXT_CreateIndirect, "AgNO3Ũ��", ID_TEXT_NO3ND, 5, 85, 160, 32, 0, 0x64, 0},
    {TEXT_CreateIndirect, "---- mol/L", ID_TEXT_NO3ND_VALUE, 75, 120, 160, 32, 0, 0x64, 0},
    
    {TEXT_CreateIndirect, "AgNO3����", ID_TEXT_NO3YL, 5, 155, 160, 32, 0, 0x64, 0},
    {TEXT_CreateIndirect, "0mL", ID_TEXT_NO3YL_VALUE, 75, 190, 160, 32, 0, 0x64, 0},

    {TEXT_CreateIndirect, "ˮ����������������", ID_TEXT_PERCENTAGE, 5, 225, 250, 32, 0, 0x64, 0},
    {TEXT_CreateIndirect, "----%", ID_TEXT_PERCENT_VALUE, 75, 260, 160, 32, 0, 0x64, 0},

    {TEXT_CreateIndirect, "�¶�", ID_TEXT_TEMP, 5, 310, 160, 32, 0, 0x64, 0},
    {TEXT_CreateIndirect, "37.5", ID_TEXT_TEMP_VALUE, 75, 313, 160, 32, 0, 0x64, 0},

    {BUTTON_CreateIndirect, "���ز˵�", ID_BUTTON_RETURN, 600, 15, 180, 60, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "AgNO3���", ID_BUTTON_START_NO3, 600, 110, 180, 60, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "�հ�ʵ��", ID_BUTTON_START_BLOCK, 600, 205, 180, 60, 0, 0x0, 0},
    {BUTTON_CreateIndirect, "�����Ӽ��", ID_BUTTON_START_TEST, 600, 300, 180, 60, 0, 0x0, 0},
    
    { GRAPH_CreateIndirect, "Graph", ID_GRAPH_0, 265, 15, 310, 340, 0, 0x0, 0 },
    {TEXT_CreateIndirect, "�缫��λ", ID_TEXT_DJDW, 325, 360, 135, 32, 0, 0x64, 0},
    {TEXT_CreateIndirect, "320.1mV", ID_TEXT_DJDW_VALUE, 455, 363, 130, 25, 0, 0x64, 0},
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
static struct exper_msg msg;
static uint8_t put_func = 0;
static uint8_t test_func = 0;
// USER START (Optionally insert additional static code)
static GRAPH_SCALE_Handle hScaleV, hScaleH;
static GRAPH_DATA_Handle pdataGRP;
// USER END

static void ctrl_all_btn(WM_HWIN hWin, int enable)
{
    WM_HWIN hItem;
    int id;

    for (id = ID_BUTTON_GET; id <= ID_BUTTON_CLEAR; id++) {
        hItem = WM_GetDialogItem(hWin, id);
        if (enable)
            WM_EnableWindow(hItem);
        else
            WM_DisableWindow(hItem);
    }
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
    struct exper_stat *stat;
    char buf[16];
    // USER START (Optionally insert additional variables)
    // USER END

    switch (pMsg->MsgId)
    {
    case WM_INIT_DIALOG:
        //
        // Initialization of 'Framewin'
        //
        hItem = pMsg->hWin;
        WINDOW_SetBkColor(hItem, GUI_GRAY);
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_GET);
        BUTTON_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        BUTTON_SetTextColor(hItem, BUTTON_CI_UNPRESSED, GUI_BLUE);
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_PUT);
        BUTTON_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        BUTTON_SetTextColor(hItem, 0, GUI_BLUE);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CLEAR);
        BUTTON_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        BUTTON_SetTextColor(hItem, BUTTON_CI_UNPRESSED, GUI_BLUE);
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_RETURN);
        BUTTON_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        BUTTON_SetTextColor(hItem, 0, GUI_RED);
        //
        // Initialization of 'Button'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_START_BLOCK);
        BUTTON_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        BUTTON_SetTextColor(hItem, 0, GUI_BLUE);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_START_NO3);
        BUTTON_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        BUTTON_SetTextColor(hItem, 0, GUI_BLUE);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_START_TEST);
        BUTTON_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
        //
        // Initialization of 'Text'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_NO3ND);
        TEXT_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        TEXT_SetTextColor(hItem, GUI_BLACK);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_NO3ND_VALUE);
        TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
        TEXT_SetTextColor(hItem, GUI_RED);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TEMP);
        TEXT_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        TEXT_SetTextColor(hItem, GUI_BLACK);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TEMP_VALUE);
        TEXT_SetFont(hItem, GUI_FONT_24_1);
        TEXT_SetTextColor(hItem, GUI_GREEN);
        sprintf(buf, "%.1f", 37.5);
        buf[4] = 0xBA;
        buf[5] = 0x43;
        buf[6] = 0;
        TEXT_SetText(hItem, buf);
        //
        // Initialization of 'Text'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_NO3YL);
        TEXT_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        TEXT_SetTextColor(hItem, GUI_BLACK);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_NO3YL_VALUE);
        TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
        TEXT_SetTextColor(hItem, GUI_RED);
        //
        // Initialization of 'Text'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DJDW);
        TEXT_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        TEXT_SetTextColor(hItem, GUI_BLACK);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DJDW_VALUE);
        TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
        TEXT_SetTextColor(hItem, GUI_GREEN);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_NACLND);
        TEXT_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        TEXT_SetTextColor(hItem, GUI_BLACK);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_NACLND_VALUE);
        TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
        TEXT_SetTextColor(hItem, GUI_BLACK);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_PERCENTAGE);
        TEXT_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        TEXT_SetTextColor(hItem, GUI_BLACK);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_PERCENT_VALUE);
        TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
        TEXT_SetTextColor(hItem, GUI_RED);

        //
        // Initialization of 'Text'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_RYCL);
        TEXT_SetFont(hItem, &GUI_FontHZ_kaiti_20);
        TEXT_SetTextColor(hItem, GUI_BLACK);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_0);
        PROGBAR_SetFont(hItem, GUI_FONT_24B_ASCII);
        PROGBAR_SetBarColor(hItem, 0, GUI_RED);
        PROGBAR_SetSkinClassic(hItem);
        PROGBAR_SetMinMax(hItem, 0, 100);
        

        hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0);
        GRAPH_SetBorder(hItem, 35, 3, 3, 15);
        GRAPH_SetGridVis(hItem, 2);
        GRAPH_SetGridFixedX(hItem, 2);
        GRAPH_SetGridDistY(hItem, 20);
        GRAPH_SetGridDistX(hItem, 50);
        
        hScaleV = GRAPH_SCALE_Create(3, GUI_TA_LEFT, GRAPH_SCALE_CF_VERTICAL, 20);
        GRAPH_SCALE_SetTextColor(hScaleV, GUI_RED);
        GRAPH_SCALE_SetOff(hScaleV, -140);
        GRAPH_AttachScale(hItem, hScaleV);
        
        hScaleH = GRAPH_SCALE_Create(330, GUI_TA_HCENTER, GRAPH_SCALE_CF_HORIZONTAL, 50);
        GRAPH_SCALE_SetTextColor(hScaleH, GUI_DARKGREEN);
        GRAPH_AttachScale(hItem, hScaleH);
        pdataGRP = GRAPH_DATA_XY_Create(GUI_GREEN, 350, 0, 0);
        GRAPH_DATA_XY_SetOffY(pdataGRP, -140);
        GRAPH_AttachData(hItem, pdataGRP);
        GRAPH_DATA_XY_Clear(pdataGRP);
        // USER END
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {
        case ID_BUTTON_GET: // Notifications sent by 'Button'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                beep_clicked();
                hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_GET);
                BUTTON_SetTextColor(hItem, BUTTON_CI_DISABLED, GUI_GREEN);
                BUTTON_SetText(hItem, "��Һ��");
                hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CLEAR);
                BUTTON_SetTextColor(hItem, BUTTON_CI_DISABLED, GUI_BLACK);
                ctrl_all_btn(pMsg->hWin, 0);
                msg.msg = EXPER_MSG_OIL_GET;
                msg.stop = 0;
                exper_msg_set(&msg);
                break;
            default:
                break;
            }
            break;
        case ID_BUTTON_PUT: // Notifications sent by 'Button'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                beep_clicked();
                hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_PUT);
                msg.msg = EXPER_MSG_OIL_PUT;

                if (put_func) {    
                    BUTTON_SetText(hItem, "��Һ");
                    BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
                    ctrl_all_btn(pMsg->hWin, 1);                    
                    put_func = 0;
                    msg.stop = 1;
                } else {
                    BUTTON_SetText(hItem, "ֹͣ");
                    BUTTON_SetTextColor(hItem, 0, GUI_RED);
                    ctrl_all_btn(pMsg->hWin, 0);
                    WM_EnableWindow(hItem);
                    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_GET);
                    BUTTON_SetTextColor(hItem, BUTTON_CI_DISABLED, GUI_BLACK);
                    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CLEAR);
                    BUTTON_SetTextColor(hItem, BUTTON_CI_DISABLED, GUI_BLACK);         
                    put_func = 1;
                    msg.stop = 0;
                }
                exper_msg_set(&msg);
                break;
            default:
                break;
            }
            break;
        case ID_BUTTON_CLEAR: // Notifications sent by 'Button'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                beep_clicked();
                hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CLEAR);
                BUTTON_SetText(hItem, "��ϴ��");
                BUTTON_SetTextColor(hItem, BUTTON_CI_DISABLED, GUI_GREEN);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_GET);
                BUTTON_SetTextColor(hItem, BUTTON_CI_DISABLED, GUI_BLACK);
                ctrl_all_btn(pMsg->hWin, 0);
                msg.msg = EXPER_MSG_OIL_CLEAR;
                msg.stop = 0;
                exper_msg_set(&msg);
                break;
            default:
                break;
            }
            break;
        case ID_BUTTON_RETURN: // Notifications sent by 'Button'
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                beep_clicked();
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
        case ID_BUTTON_START_BLOCK: // Notifications sent by 'Button'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                beep_clicked();
                /* cancel */
                ginfo.func = EXPER_MSG_BLOCK_START;
                ginfo.flag = test_func;
                if (diag_info_creat(&ginfo))
                    break;

                if (test_func) {
                    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_START_BLOCK);
                    BUTTON_SetText(hItem, "�հ�ʵ��");
                    BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
                    ctrl_all_btn(pMsg->hWin, 1);
                    msg.stop = 1;
                    test_func = 0;
                } else {
                    GRAPH_DATA_XY_Clear(pdataGRP);
                    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_START_BLOCK);
                    BUTTON_SetText(hItem, "ֹͣʵ��");
                    BUTTON_SetTextColor(hItem, 0, GUI_RED);
                    ctrl_all_btn(pMsg->hWin, 0);
                    WM_EnableWindow(hItem);
                    msg.stop = 0;
                    test_func = 1;
                }
                msg.msg = EXPER_MSG_BLOCK_START;
                exper_msg_set(&msg);
                break;
            default:
                break;
            }
            break;
        case ID_BUTTON_START_NO3:
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                beep_clicked();
                /* cancel */
                ginfo.func = EXPER_MSG_AGNO3_START;
                ginfo.flag = test_func;
                if (diag_info_creat(&ginfo))
                    break;

                if (test_func) {
                    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_START_NO3);
                    BUTTON_SetText(hItem, "AgNO3���");
                    BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
                    ctrl_all_btn(pMsg->hWin, 1);
                    msg.stop = 1;
                    test_func = 0;
                } else {
                    GRAPH_DATA_XY_Clear(pdataGRP);
                    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_START_NO3);
                    BUTTON_SetText(hItem, "ֹͣʵ��");
                    BUTTON_SetTextColor(hItem, 0, GUI_RED);
                    ctrl_all_btn(pMsg->hWin, 0);
                    WM_EnableWindow(hItem);
                    msg.stop = 0;
                    test_func = 1;
                }
                msg.msg = EXPER_MSG_AGNO3_START;
                exper_msg_set(&msg);
                break;
            default:
                break;
            }
            break;
        case ID_BUTTON_START_TEST:
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                beep_clicked();
                /* cancel */
                ginfo.func = EXPER_MSG_CL_START;
                ginfo.flag = test_func;
                if (diag_info_creat(&ginfo))
                    break;

                if (test_func) {
                    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_START_TEST);
                    BUTTON_SetText(hItem, "�����Ӽ��");
                    BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
                    ctrl_all_btn(pMsg->hWin, 1);
                    msg.stop = 1;
                    test_func = 0;
                } else {
                    GRAPH_DATA_XY_Clear(pdataGRP);
                    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_START_TEST);
                    BUTTON_SetText(hItem, "ֹͣʵ��");
                    BUTTON_SetTextColor(hItem, 0, GUI_RED);
                    ctrl_all_btn(pMsg->hWin, 0);
                    WM_EnableWindow(hItem);
                    msg.stop = 0;
                    test_func = 1;
                }
                msg.msg = EXPER_MSG_CL_START;
                exper_msg_set(&msg);
                break;
            default:
                break;
            }
            break;
        }
        break;
    case WM_USER:
        stat = (struct exper_stat *)pMsg->Data.p;
        switch (stat->stat) {
            case EXPER_STAT_ERR_MOTOR:
                beep_warning();
                diag_err_creat();
                break;
            case EXPER_STAT_UPDATE_PROGRESS:
                hItem = WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_0);
                if (stat->oil_stock < 10)
                    PROGBAR_SetBarColor(hItem, 0, GUI_RED);
                else if (stat->oil_stock < 40)
                    PROGBAR_SetBarColor(hItem, 0, GUI_YELLOW);
                else
                    PROGBAR_SetBarColor(hItem, 0, GUI_GREEN);
                PROGBAR_SetValue(hItem, stat->oil_stock);
                break;
            case EXPER_STAT_UPDATE_GRAPH:
                GRAPH_DATA_XY_AddPoint(pdataGRP, &stat->graph_pos);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DJDW_VALUE);
                sprintf(buf, "%.3fmV", stat->volt);
                TEXT_SetText(hItem, buf);
                
                hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_NO3YL_VALUE);
                sprintf(buf, "%.3fmL", stat->agno3_used);
                TEXT_SetText(hItem, buf);
                break;
            case EXPER_STAT_OIL_GET_FINISHED:
                test_func = 0;
                hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_GET);
                BUTTON_SetText(hItem, "��Һ");
                ctrl_all_btn(pMsg->hWin, 1);
                break;
            case EXPER_STAT_OIL_PUT_FINISHED:
                beep_finished();
                test_func = 0;
                hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_PUT);   
                BUTTON_SetText(hItem, "��Һ");
                BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
                ctrl_all_btn(pMsg->hWin, 1);
                put_func = 0;
                break;
            case EXPER_STAT_OIL_CLEAR_FINISHED:
                beep_finished();
                hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CLEAR);
                BUTTON_SetText(hItem, "��ϴ");
                ctrl_all_btn(pMsg->hWin, 1);
                break;
            case EXPER_STAT_AGNO3_FINISHED:
                beep_finished();
                test_func = 0;
                gres.func = EXPER_STAT_AGNO3_FINISHED;
                gres.agno3_used = stat->agno3_used;
                gres.res = stat->agno3_consistence;
                diag_res_creat(&gres);

                hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_NO3ND_VALUE);
                sprintf(buf, "%.4fmol/L", stat->agno3_consistence);
                TEXT_SetText(hItem, buf);

                hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_NO3YL_VALUE);
                sprintf(buf, "%.3fmL", stat->agno3_used);
                TEXT_SetText(hItem, buf);

                hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_START_NO3);
                BUTTON_SetText(hItem, "AgNO3���");
                BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
                ctrl_all_btn(pMsg->hWin, 1);
                break;
            case EXPER_STAT_BLOCK_FINISHED:
                beep_finished();
                test_func = 0;
                gres.func = EXPER_STAT_BLOCK_FINISHED;
                gres.agno3_used = stat->agno3_used;
                diag_res_creat(&gres);

                hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_NO3YL_VALUE);
                sprintf(buf, "%.3fmL", stat->agno3_used);
                TEXT_SetText(hItem, buf);

                hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_START_BLOCK);
                BUTTON_SetText(hItem, "�հ�ʵ��");
                BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
                ctrl_all_btn(pMsg->hWin, 1);
                break;
            case EXPER_STAT_CL_FINISHED:
                beep_finished();
                test_func = 0;
                gres.func = EXPER_STAT_CL_FINISHED;
                gres.agno3_used = stat->agno3_used;
                gres.res = stat->cl_percentage;
                diag_res_creat(&gres);

                hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_PERCENT_VALUE);
                sprintf(buf, "%.3f%%", stat->cl_percentage);
                TEXT_SetText(hItem, buf);

                hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_NO3YL_VALUE);
                sprintf(buf, "%.3fmL", stat->agno3_used);
                TEXT_SetText(hItem, buf);

                hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_START_TEST);
                BUTTON_SetText(hItem, "�����Ӽ��");
                BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
                ctrl_all_btn(pMsg->hWin, 1);
                break;
            default:
                break;
        }
        break;
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

int ui_blocktest_creat(void)
{
    return GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
