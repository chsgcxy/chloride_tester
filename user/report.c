#include "rd-tprinter.h"
#include "report.h"
#include "string.h"
#include "stdio.h"

/** @notes:
 * Chinese characters are not used directly because it depends on the encoding of the editor.
 * use amo tools function ascii2hex to creat this string.
 * */

/* 氯离子检测报告 */
uint8_t str_llzjcbg[] = {0xC2,0xC8,0xC0,0xEB,0xD7,0xD3,0xBC,0xEC,0xB2,0xE2,0xB1,0xA8,0xB8,0xE6};
/* 试样检测电压 */
uint8_t str_ssjcdy[] = {0xCA,0xD4,0xD1,0xF9,0xBC,0xEC,0xB2,0xE2,0xB5,0xE7,0xD1,0xB9};
/* 硝酸银用量 */
uint8_t str_xsyyl[] = {0xCF,0xF5,0xCB,0xE1,0xD2,0xF8,0xD3,0xC3,0xC1,0xBF};
/* 每克试样含氯离子 */
uint8_t str_mksyhllz[] = {0xC3,0xBF,0xBF,0xCB,0xCA,0xD4,0xD1,0xF9,0xBA,0xAC,0xC2,0xC8,0xC0,0xEB,0xD7,0xD3};
/* 测试日期 */
uint8_t str_csrq[] = {0xB2,0xE2,0xCA,0xD4,0xC8,0xD5,0xC6,0xDA};
/* 年 */
uint8_t str_year[] = {0xC4, 0xEA};
/* 月 */
uint8_t str_month[] = {0xD4, 0xC2};
/* 日 */
uint8_t str_day[] = {0xC8, 0xD5};

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
    char buf[16];
    
    tprinter_prepare(report_printer);
    tprinter_zoomin(report_printer, 1, 1);

    tprinter_send(report_printer, str_csrq, sizeof(str_csrq));
    tprinter_send(report_printer, &char_colon, 1);
    tprinter_send(report_printer, &char_space, 1);
    sprintf(buf, "20%02d", rp->year);
    tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
    tprinter_send(report_printer, str_year, 2);
    sprintf(buf, "%02d", rp->month);
    tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
    tprinter_send(report_printer, str_month, 2);
    sprintf(buf, "%02d", rp->day);
    tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
    tprinter_send(report_printer, str_day, 2);
    tprinter_send(report_printer, &char_space, 1);
    sprintf(buf, "%02d:%02d", rp->hour, rp->minute);
    tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
    tprinter_newline(report_printer);

    tprinter_send(report_printer, str_mksyhllz, sizeof(str_mksyhllz));
    tprinter_send(report_printer, &char_colon, 1);
    tprinter_send(report_printer, &char_space, 1);

    sprintf(buf, "%.2f", rp->percentage);
    tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
    tprinter_send(report_printer, &char_percent, 1);
    tprinter_newline(report_printer);

    tprinter_send(report_printer, str_xsyyl, sizeof(str_xsyyl));
    tprinter_send(report_printer, &char_colon, 1);
    tprinter_send(report_printer, &char_space, 1);

    sprintf(buf, "%.2f(ml)", rp->nitrate_dosage);
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
    
    tprinter_send(report_printer, str_ssjcdy, sizeof(str_ssjcdy));
    tprinter_send(report_printer, "(mv)", 4);
    tprinter_newline(report_printer);

    tprinter_newline(report_printer);
    tprinter_newline(report_printer);

    tprinter_send(report_printer, &char_space, 1);
    tprinter_zoomin(report_printer, 2, 2);
    tprinter_send(report_printer, str_llzjcbg, sizeof(str_llzjcbg));
    tprinter_newline(report_printer);
    tprinter_newline(report_printer);
    tprinter_newline(report_printer);
    tprinter_newline(report_printer);
    tprinter_newline(report_printer);
    tprinter_newline(report_printer);

    return 0;
}
