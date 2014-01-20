#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <assert.h>

#include "main.h"
#include "gen.h"
#include "dbg.h"

#define KPI_V_CHR_MAX_LEN 32
#define KPI_V_NUM_RAND_RANGE 10

enum {
    IDX_CPE_PM_START = 1,
    IDX_CPE_PM_CELL_SERV_TM = IDX_CPE_PM_START,
    IDX_CPE_PM_RRC_REQ_NR,
    IDX_CPE_PM_RRC_SUCC_NR,
    IDX_CPE_PM_RRC_FAIL_NR,
    IDX_CPE_PM_RAB_REQ_NR,
    IDX_CPE_PM_RAB_SUCC_NR,
    IDX_CPE_PM_RAB_FAIL_NR,
    IDX_CPE_PM_RAB_RELEASE_MME_NR,
    IDX_CPE_PM_RAB_RELEASE_ENB_NR,
    IDX_CPE_PM_RAB_RELEASE_ABR_NR,
    IDX_CPE_PM_CUR_RAB_NR,
    IDX_CPE_PM_INC_HO_RAB_NR,
    IDX_CPE_PM_UE_CONN_AVG_NR,
    IDX_CPE_PM_INC_HO_SUCC_NR,
    IDX_CPE_PM_INC_HO_FAIL_NR,
    IDX_CPE_PM_PDCP_UL_LOSS_NR,
    IDX_CPE_PM_PDCP_DL_LOSS_NR,
    IDX_CPE_PM_PDCP_DL_DISC_NR,
    IDX_CPE_PM_PDCP_UL_NR,
    IDX_CPE_PM_PDCP_DL_NR,
    IDX_CPE_PM_PDCP_UL_AVG_RT,
    IDX_CPE_PM_PDCP_DL_AVG_RT,
    IDX_CPE_PM_PUSCH_PRB_USED_NR,
    IDX_CPE_PM_PUSCH_PRB_AVAIL_NR,
    IDX_CPE_PM_PDSCH_PRB_USED_NR,
    IDX_CPE_PM_PDSCH_PRB_AVAIL_NR,
    IDX_CPE_PM_RRC_REL_NR,
    IDX_CPE_PM_RRC_REL_ABR_NR,
    IDX_CPE_PM_PAGING_NR,
    IDX_CPE_PM_PAGING_LOSS_NR,
    IDX_CPE_PM_REDIR_WCDMA_NR,
    IDX_CPE_PM_REDIR_GSM_NR,
    IDX_CPE_PM_UE_CONN_NR,
    IDX_CPE_PM_GTP_RX_PKT,
    IDX_CPE_PM_GTP_TX_PKT,
    IDX_CPE_PM_GTP_RX_BYTES,
    IDX_CPE_PM_GTP_TX_BYTES,
    IDX_CPE_PM_IP_RX_PKT,
    IDX_CPE_PM_IP_TX_PKT,
    IDX_CPE_PM_IP_RX_BYTES,
    IDX_CPE_PM_IP_TX_BYTES,
    IDX_CPE_PM_RAB_MAX_NR,
    IDX_CPE_PM_UE_CONN_MAX_NR
};

typedef struct kpi_entry kpi_entry_t;
struct kpi_entry {
    char *k;
    enum {
        T_NUM,
        T_CHR
    } t;
    union {
        int num;
        char chr[KPI_V_CHR_MAX_LEN];
    } v;
};

static kpi_entry_t kpi_tbl[] = {
    { "", 0, { 0 } },
    { "cpe-pm-cell-serv-tm", T_NUM, { 0 } },
    { "cpe-pm-rrc-req-nr", T_NUM, { 0 } },
    { "cpe-pm-rrc-succ-nr", T_NUM, { 0 } },
    { "cpe-pm-rrc-fail-nr", T_NUM, { 0 } },
    { "cpe-pm-rab-req-nr", T_NUM, { 0 } },
    { "cpe-pm-rab-succ-nr", T_NUM, { 0 } },
    { "cpe-pm-rab-fail-nr", T_NUM, { 0 } },
    { "cpe-pm-rab-release-mme-nr", T_NUM, { 0 } },
    { "cpe-pm-rab-release-enb-nr", T_NUM, { 0 } },
    { "cpe-pm-rab-release-abr-nr", T_NUM, { 0 } },
    { "cpe-pm-cur-rab-nr", T_NUM, { 0 } },
    { "cpe-pm-inc-ho-rab-nr", T_NUM, { 0 } },
    { "cpe-pm-ue-conn-avg-nr", T_NUM, { 0 } },
    { "cpe-pm-inc-ho-succ-nr", T_NUM, { 0 } },
    { "cpe-pm-inc-ho-fail-nr", T_NUM, { 0 } },
    { "cpe-pm-pdcp-ul-loss-nr", T_NUM, { 0 } },
    { "cpe-pm-pdcp-dl-loss-nr", T_NUM, { 0 } },
    { "cpe-pm-pdcp-dl-disc-nr", T_NUM, { 0 } },
    { "cpe-pm-pdcp-ul-nr", T_NUM, { 0 } },
    { "cpe-pm-pdcp-dl-nr", T_NUM, { 0 } },
    { "cpe-pm-pdcp-ul-avg-rt", T_NUM, { 0 } },
    { "cpe-pm-pdcp-dl-avg-rt", T_NUM, { 0 } },
    { "cpe-pm-pusch-prb-used-nr", T_NUM, { 0 } },
    { "cpe-pm-pusch-prb-avail-nr", T_NUM, { 0 } },
    { "cpe-pm-pdsch-prb-used-nr", T_NUM, { 0 } },
    { "cpe-pm-pdsch-prb-avail-nr", T_NUM, { 0 } },
    { "cpe-pm-rrc-rel-nr", T_NUM, { 0 } },
    { "cpe-pm-rrc-rel-abr-nr", T_NUM, { 0 } },
    { "cpe-pm-paging-nr", T_NUM, { 0 } },
    { "cpe-pm-paging-loss-nr", T_NUM, { 0 } },
    { "cpe-pm-redir-wcdma-nr", T_NUM, { 0 } },
    { "cpe-pm-redir-gsm-nr", T_NUM, { 0 } },
    { "cpe-pm-ue-conn-nr", T_NUM, { 0 } },
    { "cpe-pm-gtp-rx-pkt", T_NUM, { 0 } },
    { "cpe-pm-gtp-tx-pkt", T_NUM, { 0 } },
    { "cpe-pm-gtp-rx-bytes", T_NUM, { 0 } },
    { "cpe-pm-gtp-tx-bytes", T_NUM, { 0 } },
    { "cpe-pm-ip-rx-pkt", T_NUM, { 0 } },
    { "cpe-pm-ip-tx-pkt", T_NUM, { 0 } },
    { "cpe-pm-ip-rx-bytes", T_NUM, { 0 } },
    { "cpe-pm-ip-tx-bytes", T_NUM, { 0 } },
    { "cpe-pm-rab-max-nr", T_NUM, { 0 } },
    { "cpe-pm-ue-conn-max-nr", T_NUM, { 0 } }
};
#define KPI_TOTAL_NUM (sizeof(kpi_tbl) / (sizeof(kpi_tbl[0])) - 1)

int gen(const cheer_ctx_t *ctx)
{
    FILE *fp = NULL;
    static time_t t_beg = 0, t_end = 0;
    struct tm tm_beg, tm_end;
    char filename[256] = { 0 };
    int i = 0;

    assert(ctx);

    if (t_beg == 0) {
        t_beg = ctx->t_beg_pro;
    }
    t_end = time(NULL);

    memcpy(&tm_beg, localtime(&t_beg), sizeof(tm_beg));
    memcpy(&tm_end, localtime(&t_end), sizeof(tm_end));

    snprintf(filename, sizeof(filename),
             "A%04d%02d%02d.%02d%02d%+03d%02d-%02d%02d%+03d%02d_%s",
             tm_beg.tm_year + 1900,
             tm_beg.tm_mon + 1,
             tm_beg.tm_mday,
             tm_beg.tm_hour,
             tm_beg.tm_min,
             ctx->tz.h,
             ctx->tz.m,
             tm_end.tm_hour,
             tm_end.tm_min,
             ctx->tz.h,
             ctx->tz.m,
             ctx->sn);

    if ((fp = fopen(filename, "w")) == NULL) {
        DBG_ERR("Open file %s failed: %s.", filename, strerror(errno));
        t_beg = t_end;
        return -1;
    }
    DBG_DBG("Create kpi file: %s.", filename);

    fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(fp, "<?xml-stylesheet type=\"text/xsl\" href=\"MeasDataCollection.xsl\"?>\n");
    fprintf(fp, "<measCollecFile xmlns=\"http://www.3gpp.org/ftp/specs/archive/32_series/32.435#measCollec\"\n");
    fprintf(fp, "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n");
    fprintf(fp, "xsi:schemaLocation=\"http://www.3gpp.org/ftp/specs/archive/32_series/32.435#measCollec\">\n");
    fprintf(fp, "<fileHeader fileFormatVersion=\"32.435 V7.0\" vendorName=\"Sercomm\">\n");
    fprintf(fp, "<fileSender elementType=\"LTE_Femto\"/>\n");
    fprintf(fp, "<measCollec beginTime=\"%04d-%02d-%02dT%02d:%02d:%02d%+03d:%02d\"/>\n",
            tm_beg.tm_year + 1900,
            tm_beg.tm_mon + 1,
            tm_beg.tm_mday,
            tm_beg.tm_hour,
            tm_beg.tm_min,
            tm_beg.tm_sec,
            ctx->tz.h,
            ctx->tz.m);
    fprintf(fp, "</fileHeader>\n");
    fprintf(fp, "<measData>\n");
    fprintf(fp, "<managedElement userLabel=\"LTE_Femto\"/>\n");
    fprintf(fp, "<measInfo>\n");
    fprintf(fp, "<granPeriod duration=\"PT%dS\" endTime=\"%04d-%02d-%02dT%02d:%02d:%02d%+03d:%02d\"/>\n",
            ctx->interval,
            tm_end.tm_year + 1900,
            tm_end.tm_mon + 1,
            tm_end.tm_mday,
            tm_end.tm_hour,
            tm_end.tm_min,
            tm_end.tm_sec,
            ctx->tz.h,
            ctx->tz.m);
    fprintf(fp, "<repPeriod duration=\"PT%dS\"/>\n", ctx->interval);
    for (i = IDX_CPE_PM_START; i <= KPI_TOTAL_NUM; i++) {
        fprintf(fp, "<measType p=\"%d\">%s</measType>\n", i, kpi_tbl[i].k);
    }
    fprintf(fp, "<measValue measObjLdn=\"HeNBFunction=1\">\n");

    srand((unsigned int)t_beg);
    for (i = IDX_CPE_PM_START; i <= KPI_TOTAL_NUM; i++) {
        if (kpi_tbl[i].t == T_NUM) {
            kpi_tbl[i].v.num = rand() % KPI_V_NUM_RAND_RANGE;
        }
    }

    /**
     * Modify some numeric values to be valid.
     */
    kpi_tbl[IDX_CPE_PM_CELL_SERV_TM].v.num = rand() % (t_end - t_beg);

    for (i = IDX_CPE_PM_START; i <= KPI_TOTAL_NUM; i++) {
        switch (kpi_tbl[i].t) {
        case T_NUM:
            fprintf(fp, "<r p=\"%d\">%d</r>\n", i, kpi_tbl[i].v.num);
            break;
        case T_CHR:
            fprintf(fp, "<r p=\"%d\">%s</r>\n", i, kpi_tbl[i].v.chr);
            break;
        default:
            break;
        }
    }

    fprintf(fp, "</measValue>\n");
    fprintf(fp, "</measInfo>\n");
    fprintf(fp, "</measData>\n");
    fprintf(fp, "<fileFooter>\n");
    fprintf(fp, "<measCollec endTime=\"%04d-%02d-%02dT%02d:%02d:%02d%+03d:%02d\"/>\n",
            tm_end.tm_year + 1900,
            tm_end.tm_mon + 1,
            tm_end.tm_mday,
            tm_end.tm_hour,
            tm_end.tm_min,
            tm_end.tm_sec,
            ctx->tz.h,
            ctx->tz.m);
    fprintf(fp, "</fileFooter>\n");
    fprintf(fp, "</measCollecFile>\n");

    fclose(fp);

    t_beg = t_end;

    return 0;
}
