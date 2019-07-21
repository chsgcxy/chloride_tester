#ifndef __MAIN_H__
#define __MAIN_H__

#include "usbh_usr.h"

struct ui_msg {
#define MSG_LOAD_UI_MENU         0x01
#define MSG_LOAD_UI_BLOCKTEST    0x02
#define MSG_LOAD_UI_STAND        0x03
#define MSG_LOAD_UI_SETTING      0x04
#define MSG_LOAD_UI_DATA         0x05
#define MSG_LOAD_UI_DETAIL       0x06
#define MSG_LOAD_UI_TOUCH_CALC   0x07
#define MSG_LOAD_UI_DROPPER      0x08
#define MSG_LOAD_UI_EXTEST       0x09
    int msg;
    int param0;
};

extern struct ui_msg g_ui_msg;


struct ui_exper_info {
    int func;
    int flag;
    char *str;
};

struct ui_exper_test {
    int func;
};

extern USBH_HOST USB_Host;
extern USB_OTG_CORE_HANDLE USB_OTG_Core;

#endif
