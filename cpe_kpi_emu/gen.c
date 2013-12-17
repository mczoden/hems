#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <assert.h>

#include "main.h"
#include "gen.h"
#include "dbg.h"

int gen(const cheer_ctx_t *ctx)
{
    FILE *fp = NULL;
    static time_t t_beg = 0, t_end = 0;
    struct tm tm_beg = { 0 };
    struct tm tm_end = { 0 };
    char filename[256] = { 0 };

    assert(ctx);

    if (t_beg == 0) {
        t_beg = ctx->t_beg_pro;
    }
    t_end = time(NULL);

    localtime_r(&t_beg, &tm_beg);
    localtime_r(&t_end, &tm_end);

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
    fprintf(fp, "<measType p=\"1\">cpe-pm-cell-serv-tm</measType>\n");
    fprintf(fp, "<measType p=\"2\">cpe-pm-rrc-req-nr</measType>\n");
    fprintf(fp, "<measType p=\"3\">cpe-pm-rrc-succ-nr</measType>\n");
    fprintf(fp, "<measType p=\"4\">cpe-pm-rrc-fail-nr</measType>\n");
    fprintf(fp, "<measType p=\"5\">cpe-pm-rab-req-nr</measType>\n");
    fprintf(fp, "<measType p=\"6\">cpe-pm-rab-succ-nr</measType>\n");
    fprintf(fp, "<measType p=\"7\">cpe-pm-rab-fail-nr</measType>\n");
    fprintf(fp, "<measType p=\"8\">cpe-pm-rab-release-mme-nr</measType>\n");
    fprintf(fp, "<measType p=\"9\">cpe-pm-rab-release-enb-nr</measType>\n");
    fprintf(fp, "<measType p=\"10\">cpe-pm-rab-release-abr-nr</measType>\n");
    fprintf(fp, "<measType p=\"11\">cpe-pm-cur-rab-nr</measType>\n");
    fprintf(fp, "<measType p=\"12\">cpe-pm-inc-ho-rab-nr</measType>\n");
    fprintf(fp, "<measType p=\"13\">cpe-pm-ue-conn-avg-nr</measType>\n");
    fprintf(fp, "<measType p=\"14\">cpe-pm-inc-ho-succ-nr</measType>\n");
    fprintf(fp, "<measType p=\"15\">cpe-pm-inc-ho-fail-nr</measType>\n");
    fprintf(fp, "<measType p=\"16\">cpe-pm-pdcp-ul-loss-nr</measType>\n");
    fprintf(fp, "<measType p=\"17\">cpe-pm-pdcp-dl-loss-nr</measType>\n");
    fprintf(fp, "<measType p=\"18\">cpe-pm-pdcp-dl-disc-nr</measType>\n");
    fprintf(fp, "<measType p=\"19\">cpe-pm-pdcp-ul-nr</measType>\n");
    fprintf(fp, "<measType p=\"20\">cpe-pm-pdcp-dl-nr</measType>\n");
    fprintf(fp, "<measType p=\"21\">cpe-pm-pdcp-ul-avg-rt</measType>\n");
    fprintf(fp, "<measType p=\"22\">cpe-pm-pdcp-dl-avg-rt</measType>\n");
    fprintf(fp, "<measType p=\"23\">cpe-pm-pusch-prb-used-nr</measType>\n");
    fprintf(fp, "<measType p=\"24\">cpe-pm-pusch-prb-avail-nr</measType>\n");
    fprintf(fp, "<measType p=\"25\">cpe-pm-pdsch-prb-used-nr</measType>\n");
    fprintf(fp, "<measType p=\"26\">cpe-pm-pdsch-prb-avail-nr</measType>\n");
    fprintf(fp, "<measType p=\"27\">cpe-pm-rrc-rel-nr</measType>\n");
    fprintf(fp, "<measType p=\"28\">cpe-pm-rrc-rel-abr-nr</measType>\n");
    fprintf(fp, "<measType p=\"29\">cpe-pm-paging-nr</measType>\n");
    fprintf(fp, "<measType p=\"30\">cpe-pm-paging-loss-nr</measType>\n");
    fprintf(fp, "<measType p=\"31\">cpe-pm-redir-wcdma-nr</measType>\n");
    fprintf(fp, "<measType p=\"32\">cpe-pm-redir-gsm-nr</measType>\n");
    fprintf(fp, "<measType p=\"33\">cpe-pm-ue-conn-nr</measType>\n");
    fprintf(fp, "<measType p=\"34\">cpe-pm-gtp-rx-pkt</measType>\n");
    fprintf(fp, "<measType p=\"35\">cpe-pm-gtp-tx-pkt</measType>\n");
    fprintf(fp, "<measType p=\"36\">cpe-pm-gtp-rx-bytes</measType>\n");
    fprintf(fp, "<measType p=\"37\">cpe-pm-gtp-tx-bytes</measType>\n");
    fprintf(fp, "<measType p=\"38\">cpe-pm-ip-rx-pkt</measType>\n");
    fprintf(fp, "<measType p=\"39\">cpe-pm-ip-tx-pkt</measType>\n");
    fprintf(fp, "<measType p=\"40\">cpe-pm-ip-rx-bytes</measType>\n");
    fprintf(fp, "<measType p=\"41\">cpe-pm-ip-tx-bytes</measType>\n");
    fprintf(fp, "<measType p=\"42\">cpe-pm-rab-max-nr</measType>\n");
    fprintf(fp, "<measType p=\"43\">cpe-pm-ue-conn-max-nr</measType>\n");
    fprintf(fp, "<measValue measObjLdn=\"HeNBFunction=1\">\n");
    fprintf(fp, "<r p=\"1\">%ld</r>\n", t_end - ctx->t_beg_pro);
    fprintf(fp, "<r p=\"2\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"3\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"4\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"5\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"6\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"7\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"8\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"9\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"10\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"11\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"12\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"13\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"14\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"15\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"16\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"17\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"18\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"19\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"20\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"21\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"22\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"23\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"24\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"25\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"26\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"27\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"28\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"29\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"30\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"31\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"32\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"33\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"34\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"35\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"36\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"37\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"38\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"39\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"40\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"41\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"42\">%ld</r>\n", random() % 100);
    fprintf(fp, "<r p=\"43\">%ld</r>\n", random() % 100);
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
