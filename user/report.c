#include "rd-tprinter.h"
#include "report.h"
#include "string.h"
#include "stdio.h"

/** @notes:
 * Chinese characters are not used directly because it depends on the encoding of the editor.
 * use amo tools function ascii2hex to creat this string.
 * */
#define STRING_TITLE    "氯离子检测报告"
#define STRING_DATE     "测试日期: "
#define STRING_SSJCDY   "试样检测电压(mv)"
#define STRING_XSYYL    "硝酸银用量: "
#define STRING_MKSYHLLZ "每克试样含氯离子: "
#define STRING_PPM      "       PPM: "
#define STRING_LLZND    "氯离子浓度: "

uint8_t char_space = 0x20;
uint8_t char_colon = 0x3A;
uint8_t char_percent = 0x25;

static struct tprinter *report_printer;

int report_init(struct tprinter *printer)
{
    report_printer = printer;
    return 0;
}

int report_show(struct report *rp)
{
    int total;
    char buf[64];
    
    tprinter_prepare(report_printer);
    tprinter_zoomin(report_printer, 1, 1);

    tprinter_send(report_printer, STRING_DATE, sizeof(STRING_DATE));
    sprintf(buf, "20%02d年%02d月%02d日  %02d:%02d", rp->year, rp->month, rp->day, rp->hour, rp->minute);
    tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
    tprinter_newline(report_printer);

    switch (rp->type) {
    case REP_TYPE_CL:
        tprinter_send(report_printer, STRING_MKSYHLLZ, sizeof(STRING_MKSYHLLZ));
        sprintf(buf, "%.3f%%", rp->cl_percentage);
        tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
        tprinter_newline(report_printer);
        break;
    case REP_TYPE_STAND:
        tprinter_send(report_printer, STRING_PPM, sizeof(STRING_PPM));
        sprintf(buf, "%.1f", rp->ppm);
        tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
        tprinter_newline(report_printer);

        tprinter_send(report_printer, STRING_LLZND, sizeof(STRING_LLZND));
        sprintf(buf, "%.3fmol/L", rp->cl_dosage);
        tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
        tprinter_newline(report_printer);
        break;
    default:
        break;
    }

    tprinter_send(report_printer, STRING_XSYYL, sizeof(STRING_XSYYL));
    sprintf(buf, "%.2f(ml)", rp->cl_agno3_used);
    tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
    tprinter_newline(report_printer);
    tprinter_newline(report_printer);
    tprinter_newline(report_printer);

    for (total = rp->data_num; total > 0; total -= 2) {  
        tprinter_prepare(report_printer);
        tprinter_pos(report_printer, 30);
        sprintf(buf, "%02d:  %.1f", total - 1, rp->data[total - 2]);
        tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));

        tprinter_pos(report_printer, 210);
        sprintf(buf, "%02d:  %.1f", total, rp->data[total - 1]);
        tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
        tprinter_newline(report_printer);
    }
    tprinter_prepare(report_printer);
    tprinter_newline(report_printer);
    
    tprinter_send(report_printer, STRING_SSJCDY, sizeof(STRING_SSJCDY));
    tprinter_newline(report_printer);
    tprinter_newline(report_printer);
    tprinter_newline(report_printer);

    tprinter_send(report_printer, &char_space, 1);
    tprinter_zoomin(report_printer, 2, 2);
    tprinter_send(report_printer, STRING_TITLE, sizeof(STRING_TITLE));
    tprinter_newline(report_printer);
    tprinter_newline(report_printer);
    tprinter_newline(report_printer);
    tprinter_newline(report_printer);
    tprinter_newline(report_printer);
    tprinter_newline(report_printer);

    return 0;
}
