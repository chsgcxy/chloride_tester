#ifndef __MAIN_H__
#define __MAIN_H__

struct ui_msg {
#define MSG_LOAD_UI_MENU         0x01
#define MSG_LOAD_UI_BLOCKTEST    0x02
#define MSG_LOAD_UI_TEST         0x03
#define MSG_LOAD_UI_SETTING      0x04
#define MSG_LOAD_UI_DATA         0x05
    int msg;
    WM_HWIN hWin;
};

extern struct ui_msg g_ui_msg;


#endif
