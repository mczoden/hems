#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>
#include <signal.h>


#include <curl/curl.h>

#define KPI_CONTENT_FILE "kpi"
#define UPLOAD_PORT 9191l
#define UPLOAD_INTVAL_MIN 15

static struct tm tm_now = { 0 };
static pthread_mutex_t mtx;
static pthread_t tid;
static int cpe_pm_cell_serv_tm = 0;

static void sig_hdl(int sig)
{
    pthread_cancel(tid);
    pthread_mutex_destroy(&mtx);

    exit(0);
}

static void *clock_thread_hdl(void *unused)
{
    time_t epoch = NULL;
    bool first_get = true;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while (1) {
        epoch = time(NULL);
        localtime_r(&epoch, &tm_now);
        if (tm_now.tm_min % UPLOAD_INTVAL_MIN == 0) {
            if (first_get) {
                first_get = false;
                pthread_mutex_unlock(&mtx);
            }
        } else {
            first_get = true;
        }
        sleep(1);
        cpe_pm_cell_serv_tm++;
    }

    pthread_exit(0);
}

static int gen_kpi(void)
{
    FILE *fp = NULL;
    struct tm tm_beg = { 0 };
    time_t epoch = mktime(&tm_now);
    long random_val = 0;

    epoch -= UPLOAD_INTVAL_MIN * 60;
    localtime_r(&epoch, &tm_beg);

    if ((fp = fopen(KPI_CONTENT_FILE, "w")) == NULL) {
        return -1;
    }

    fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(fp, "<?xml-stylesheet type=\"text/xsl\" href=\"MeasDataCollection.xsl\"?>\n");
    fprintf(fp, "<measCollecFile xmlns=\"http://www.3gpp.org/ftp/specs/archive/32_series/32.435#measCollec\"\n");
    fprintf(fp, "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n");
    fprintf(fp, "xsi:schemaLocation=\"http://www.3gpp.org/ftp/specs/archive/32_series/32.435#measCollec\">\n");
    fprintf(fp, "<fileHeader fileFormatVersion=\"32.435 V7.0\" vendorName=\"Sercomm\">\n");
    fprintf(fp, "<fileSender elementType=\"LTE_Femto\"/>\n");
    fprintf(fp, "<measCollec beginTime=\"%04d-%02d-%02dT%02d:%02d:00+08:00\"/>\n",
            tm_beg.tm_year + 1900,
            tm_beg.tm_mon + 1,
            tm_beg.tm_mday,
            tm_beg.tm_hour,
            tm_beg.tm_min);
    fprintf(fp, "</fileHeader>\n");
    fprintf(fp, "<measData>\n");
    fprintf(fp, "<managedElement userLabel=\"LTE_Femto\"/>\n");
    fprintf(fp, "<measInfo>\n");
    fprintf(fp, "<granPeriod duration=\"PT900S\" endTime=\"%04d-%02d-%02dT%02d:%02d:00+08:00\"/>\n",
            tm_now.tm_year + 1900,
            tm_now.tm_mon + 1,
            tm_now.tm_mday,
            tm_now.tm_hour,
            tm_now.tm_min);
    fprintf(fp, "<repPeriod duration=\"PT900S\"/>\n");
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
    fprintf(fp, "<r p=\"1\">%d</r>\n", cpe_pm_cell_serv_tm);
    srandom((unsigned int)epoch);
    fprintf(fp, "<r p=\"2\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"3\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"4\">%ld/r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"5\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"6\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"7\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"8\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"9\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"10\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"11\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"12\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"13\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"14\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"15\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"16\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"17\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"18\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"19\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"20\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"21\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"22\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"23\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"24\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"25\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"26\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"27\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"28\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"29\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"30\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"31\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"32\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"33\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"34\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"35\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"36\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"37\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"38\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"39\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"40\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"41\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"42\">%ld</r>\n", (random_val = random() % 100));
    srandom((unsigned int)random_val);
    fprintf(fp, "<r p=\"43\">%ld</r>\n", (random_val = random() % 100));
    fprintf(fp, "</measValue>\n");
    fprintf(fp, "</measInfo>\n");
    fprintf(fp, "</measData>\n");
    fprintf(fp, "<fileFooter>\n");
    fprintf(fp, "<measCollec endTime=\"%04d-%02d-%02dT%02d:%02d:00+08:00\"/>\n",
            tm_now.tm_year + 1900,
            tm_now.tm_mon + 1,
            tm_now.tm_mday,
            tm_now.tm_hour,
            tm_now.tm_min);
    fprintf(fp, "</fileFooter>\n");
    fprintf(fp, "</measCollecFile>\n");

    fclose(fp);

    return 0;
}

static CURLcode upload(void)
{
    FILE *fp = NULL;
    CURL *curl = NULL;
    CURLcode ret = CURLE_OK;
    char url[256] = { 0 };
    struct tm tm_beg = { 0 };
    time_t epoch = mktime(&tm_now);

    epoch -= UPLOAD_INTVAL_MIN * 60;
    localtime_r(&epoch, &tm_beg);
    snprintf(url, sizeof(url),
            "http://172.21.16.112/omskpi/kpi/upload?kpi=/A%04d%02d%02d.%02d%02d+0800-%02d%02d+0800_L.ARW3AI0005",
            tm_beg.tm_year + 1900,
            tm_beg.tm_mon + 1,
            tm_beg.tm_mday,
            tm_beg.tm_hour,
            tm_beg.tm_min,
            tm_now.tm_hour,
            tm_now.tm_min);
    printf("upload url: %s\n", url);

    if ((fp = fopen(KPI_CONTENT_FILE, "r")) == NULL) {
        goto error;
    }

    if ((curl = curl_easy_init()) == NULL) {
        goto error;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_PORT, UPLOAD_PORT);
    curl_easy_setopt(curl, CURLOPT_USERNAME, "upload");
    curl_easy_setopt(curl, CURLOPT_PASSWORD, "upload");
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
    curl_easy_setopt(curl, CURLOPT_UNRESTRICTED_AUTH, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    curl_easy_setopt(curl, CURLOPT_READDATA, fp);

    ret = curl_easy_perform(curl);

error:
    if (fp != NULL) {
        fclose(fp);
    }

    if (curl != NULL) {
        curl_easy_cleanup(curl);
    }

    return ret;
}

int main(void)
{
    signal(SIGINT, sig_hdl);
    signal(SIGTERM, sig_hdl);

    pthread_mutex_init(&mtx, NULL);
    pthread_mutex_lock(&mtx);

    if (pthread_create(&tid, NULL, clock_thread_hdl, NULL) != 0) {
        fprintf(stderr, "%s", strerror(errno));
        pthread_mutex_destroy(&mtx);
        exit(EXIT_FAILURE);
    }

    while (1) {
        pthread_mutex_lock(&mtx);
        printf("%04d-%02d-%02dT%02d:%02d:00\n",
               tm_now.tm_year + 1900,
               tm_now.tm_mon + 1,
               tm_now.tm_mday,
               tm_now.tm_hour,
               tm_now.tm_min);
        if (gen_kpi() != 0) {
            fprintf(stderr, "Generate kpi file failed\n");
            continue;
        }
        if (upload() != CURLE_OK) {
            fprintf(stderr, "Upload file failed\n");
            continue;
        }
    }

    return 0;
}
