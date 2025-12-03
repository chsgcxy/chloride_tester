#include "rd-tprinter.h"
#include "report.h"
#include "string.h"
#include "stdio.h"
#include "experiment.h"

/** @notes:
 * Chinese characters are not used directly because it depends on the encoding of the editor.
 * use amo tools function ascii2hex to creat this string.
 * */
#define STRING_TITLE        "检测报告"
#define STRING_TYPE         "实验类型: "
#define STRING_INDEX        "实验编号: "
#define STRING_DATE         "测试日期: "
#define STRING_SSJCDY       "试样检测数据:"
#define STRING_XSYYL        "硝酸银用量: "
#define STRING_XSYYL_10     "试样10mL时硝酸银用量: "
#define STRING_XSYYL_20     "试样20mL时硝酸银用量: "
#define STRING_XSYYL_AGNO3_10 "硝酸银检测10mL时硝酸银用量"
#define STRING_XSYYL_AGNO3_20 "硝酸银检测20mL时硝酸银用量"
#define STRING_XSYYL_AGNO3    "硝酸银检测硝酸银用量"
#define STRING_XSYND        "硝酸银浓度: "
#define STRING_KBSYXSYYL    "空白实验硝酸银用量: "
#define STRING_KBSYXSYYL_10    "空白实验10mL时硝酸银用量: "
#define STRING_KBSYXSYYL_20    "空白实验20mL时硝酸银用量: "
#define STRING_MKSYHLLZ     "试样氯离子质量分数: "
#define STRING_PPM          "       PPM: "
#define STRING_LLZND        "氯离子浓度: "
#define STRING_AGNO3        "AgNO3/mL"
#define STRING_VOLT         "V/mV"
#define STRING_DELTAV       "Δ/mV"

#define STRING_CEMENT_SLIVER_NITRATE "水泥检测硝酸银检测"
#define STRING_CEMENT_BLOCK "水泥检测空白实验"
#define STRING_CEMENT_CHLORIDE_ION "水泥检测氯离子检测"
#define STRING_OTHER_SLIVER_NITRATE "其他测试硝酸银检测"
#define STRING_OTHER_BLOCK "其他测试空白实验"
#define STRING_OTHER_CHLORIDE_ION "其他测试氯离子检测"
#define STRING_MANUAL_TITRATION "手动滴定"
#define STRING_ADMIXTURE_SLIVER_NITRATE1 "外加剂检测硝酸银检测10mL"
#define STRING_ADMIXTURE_SLIVER_NITRATE2 "外加剂检测硝酸银检测20mL"
#define STRING_ADMIXTURE_BLOCK1 "外加剂检测空白实验10mL"
#define STRING_ADMIXTURE_BLOCK2 "外加剂检测空白实验20mL"
#define STRING_ADMIXTURE_CHLORIDE_ION1 "外加剂检测氯离子检测10mL"
#define STRING_ADMIXTURE_CHLORIDE_ION2 "外加剂检测氯离子检测20mL"

uint8_t char_space = 0x20;
uint8_t char_colon = 0x3A;
uint8_t char_percent = 0x25;

static struct tprinter *report_printer;

int report_init(struct tprinter *printer)
{
    report_printer = printer;
    return 0;
}

int report_show(struct result_data *rp)
{
    int total;
    char buf[64];
    
    tprinter_prepare(report_printer);
    tprinter_zoomin(report_printer, 1, 1);

    tprinter_send(report_printer, STRING_DATE, sizeof(STRING_DATE));
    sprintf(buf, "20%02d年%02d月%02d日  %02d:%02d", rp->year, rp->month, rp->day, rp->hour, rp->minute);
    tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
    tprinter_newline(report_printer);
    tprinter_newline(report_printer);

    // final result
    switch (rp->type) {
    case EXPER_TYPE_CEMENT_CHLORIDE_ION:
    case EXPER_TYPE_ADMIXTURE_CHLORIDE_ION2:
        tprinter_send(report_printer, STRING_MKSYHLLZ, sizeof(STRING_MKSYHLLZ));
        sprintf(buf, "%.3f%%", rp->cl_percentage);
        tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
        tprinter_newline(report_printer);
        tprinter_newline(report_printer);
        break;
    case EXPER_TYPE_OTHER_CHLORIDE_ION:
        tprinter_send(report_printer, STRING_PPM, sizeof(STRING_PPM));
        sprintf(buf, "%.1f", rp->ppm);
        tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
        tprinter_newline(report_printer);
        tprinter_newline(report_printer);

        tprinter_send(report_printer, STRING_LLZND, sizeof(STRING_LLZND));
        sprintf(buf, "%fmol/L", rp->cl_dosage);
        tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
        tprinter_newline(report_printer);
        tprinter_newline(report_printer);
        break;
    default:
        break;
    }

    if (rp->type != EXPER_TYPE_ADMIXTURE_SLIVER_NITRATE1) {
        tprinter_send(report_printer, STRING_XSYND, sizeof(STRING_XSYND));
        sprintf(buf, "%.4fmol/L", rp->agno3_dosage);
        tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
        tprinter_newline(report_printer);
        tprinter_newline(report_printer);
        tprinter_newline(report_printer);
    }

    // sliver nitrate used
    switch (rp->type) {
    case EXPER_TYPE_ADMIXTURE_CHLORIDE_ION2:
        sprintf(buf, "%.2f(ml)", rp->cl_agno3_used2);
        tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
        tprinter_newline(report_printer);
        tprinter_send(report_printer, STRING_XSYYL_20, sizeof(STRING_XSYYL_20));
        tprinter_newline(report_printer);
        tprinter_newline(report_printer);
    case EXPER_TYPE_ADMIXTURE_CHLORIDE_ION1:
        sprintf(buf, "%.2f(ml)", rp->cl_agno3_used);
        tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
        tprinter_newline(report_printer);
        tprinter_send(report_printer, STRING_XSYYL_10, sizeof(STRING_XSYYL_10));
        tprinter_newline(report_printer);
        tprinter_newline(report_printer);
        break;
    case EXPER_TYPE_ADMIXTURE_BLOCK2:
        sprintf(buf, "%.2f(ml)", rp->block_agno3_used2);
        tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
        tprinter_newline(report_printer);
        tprinter_send(report_printer, STRING_KBSYXSYYL_20, sizeof(STRING_KBSYXSYYL_20));
        tprinter_newline(report_printer);
        tprinter_newline(report_printer);
    case EXPER_TYPE_ADMIXTURE_BLOCK1:
        sprintf(buf, "%.2f(ml)", rp->block_agno3_used);  
        tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
        tprinter_newline(report_printer);
        tprinter_send(report_printer, STRING_KBSYXSYYL_10, sizeof(STRING_KBSYXSYYL_10));
        tprinter_newline(report_printer);
        tprinter_newline(report_printer);
        break;
    case EXPER_TYPE_CEMENT_CHLORIDE_ION:
    case EXPER_TYPE_OTHER_CHLORIDE_ION:
        tprinter_send(report_printer, STRING_XSYYL, sizeof(STRING_XSYYL));
        sprintf(buf, "%.2f(ml)", rp->cl_agno3_used);
        tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
        tprinter_newline(report_printer);
        tprinter_newline(report_printer);
    case EXPER_TYPE_CEMENT_BLOCK:
    case EXPER_TYPE_OTHER_BLOCK:
        tprinter_send(report_printer, STRING_KBSYXSYYL, sizeof(STRING_KBSYXSYYL));
        sprintf(buf, "%.2f(ml)", rp->block_agno3_used);
        tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
        tprinter_newline(report_printer);
        tprinter_newline(report_printer);
        break;
    case EXPER_TYPE_ADMIXTURE_SLIVER_NITRATE2:
        sprintf(buf, "%.2f(ml)", rp->agno3_agno3_used2);
        tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
        tprinter_newline(report_printer);
        tprinter_send(report_printer, STRING_XSYYL_AGNO3_20, sizeof(STRING_XSYYL_AGNO3_20));
        tprinter_newline(report_printer);
        tprinter_newline(report_printer);
    case EXPER_TYPE_ADMIXTURE_SLIVER_NITRATE1:
        sprintf(buf, "%.2f(ml)", rp->agno3_agno3_used);
        tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
        tprinter_newline(report_printer);
        tprinter_send(report_printer, STRING_XSYYL_AGNO3_10, sizeof(STRING_XSYYL_AGNO3_10));
        tprinter_newline(report_printer);
        tprinter_newline(report_printer);
        break;
    case EXPER_TYPE_CEMENT_SLIVER_NITRATE:
    case EXPER_TYPE_OTHER_SLIVER_NITRATE:
        sprintf(buf, "%.2f(ml)", rp->agno3_agno3_used);
        tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
        tprinter_newline(report_printer);
        tprinter_send(report_printer, STRING_XSYYL_AGNO3, sizeof(STRING_XSYYL_AGNO3));
        tprinter_newline(report_printer);
        tprinter_newline(report_printer);
        break;
    default:
        break;
    }

    for (total = rp->items_cnt - 1; total >= 0; total--) {
        tprinter_prepare(report_printer);
        tprinter_pos(report_printer, 0);
        sprintf(buf, "%.2f", rp->items[total].agno3_used);
        tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));

        tprinter_pos(report_printer, 150);
        sprintf(buf, "%.1f", rp->items[total].volt);
        tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
        tprinter_newline(report_printer);

        if (total > 0) {
            tprinter_pos(report_printer, 300);
            sprintf(buf, "%.1f", rp->items[total].delta_v);
            tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
            tprinter_newline(report_printer);
        }
    }
    tprinter_prepare(report_printer);
    tprinter_newline(report_printer);
    tprinter_send(report_printer, STRING_AGNO3, sizeof(STRING_AGNO3));
    tprinter_pos(report_printer, 150);
    tprinter_send(report_printer, STRING_VOLT, sizeof(STRING_VOLT));
    tprinter_pos(report_printer, 300);
    tprinter_send(report_printer, STRING_DELTAV, sizeof(STRING_DELTAV));
    tprinter_newline(report_printer);
    tprinter_newline(report_printer);
    
    tprinter_prepare(report_printer);
    tprinter_send(report_printer, STRING_SSJCDY, sizeof(STRING_SSJCDY));
    tprinter_newline(report_printer);
    tprinter_newline(report_printer);

    tprinter_send(report_printer, STRING_INDEX, sizeof(STRING_INDEX));
    sprintf(buf, "%03d", rp->index);
    tprinter_send(report_printer, (uint8_t *)buf, strlen(buf));
    tprinter_newline(report_printer);
    tprinter_newline(report_printer);
    tprinter_newline(report_printer);

    switch (rp->type) {
    case EXPER_TYPE_CEMENT_SLIVER_NITRATE:
        tprinter_send(report_printer, STRING_CEMENT_SLIVER_NITRATE, strlen(STRING_CEMENT_SLIVER_NITRATE));
        break;
    case EXPER_TYPE_CEMENT_BLOCK:
        tprinter_send(report_printer, STRING_CEMENT_BLOCK, strlen(STRING_CEMENT_BLOCK));
        break;
    case EXPER_TYPE_CEMENT_CHLORIDE_ION:
        tprinter_send(report_printer, STRING_CEMENT_CHLORIDE_ION, strlen(STRING_CEMENT_CHLORIDE_ION));
        break;
    case EXPER_TYPE_OTHER_SLIVER_NITRATE:
        tprinter_send(report_printer, STRING_OTHER_SLIVER_NITRATE, strlen(STRING_OTHER_SLIVER_NITRATE));
        break;
    case EXPER_TYPE_OTHER_BLOCK:
        tprinter_send(report_printer, STRING_OTHER_BLOCK, strlen(STRING_OTHER_BLOCK));
        break;
    case EXPER_TYPE_OTHER_CHLORIDE_ION:
        tprinter_send(report_printer, STRING_OTHER_CHLORIDE_ION, strlen(STRING_OTHER_CHLORIDE_ION));
        break;
    case EXPER_TYPE_MANUAL_TITRATION:
        tprinter_send(report_printer, STRING_MANUAL_TITRATION, strlen(STRING_MANUAL_TITRATION));
        break;
    case EXPER_TYPE_ADMIXTURE_SLIVER_NITRATE1:
        tprinter_send(report_printer, STRING_ADMIXTURE_SLIVER_NITRATE1, strlen(STRING_ADMIXTURE_SLIVER_NITRATE1));
        break;
    case EXPER_TYPE_ADMIXTURE_SLIVER_NITRATE2:
        tprinter_send(report_printer, STRING_ADMIXTURE_SLIVER_NITRATE2, strlen(STRING_ADMIXTURE_SLIVER_NITRATE2));
        break;
    case EXPER_TYPE_ADMIXTURE_BLOCK1:
        tprinter_send(report_printer, STRING_ADMIXTURE_BLOCK1, strlen(STRING_ADMIXTURE_BLOCK1));
        break;
    case EXPER_TYPE_ADMIXTURE_BLOCK2:
        tprinter_send(report_printer, STRING_ADMIXTURE_BLOCK2, strlen(STRING_ADMIXTURE_BLOCK2));
        break;
    case EXPER_TYPE_ADMIXTURE_CHLORIDE_ION1:
        tprinter_send(report_printer, STRING_ADMIXTURE_CHLORIDE_ION1, strlen(STRING_ADMIXTURE_CHLORIDE_ION1));
        break;
    case EXPER_TYPE_ADMIXTURE_CHLORIDE_ION2:
        tprinter_send(report_printer, STRING_ADMIXTURE_CHLORIDE_ION2, strlen(STRING_ADMIXTURE_CHLORIDE_ION2));
        break;
    default:
        break;
    }
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
