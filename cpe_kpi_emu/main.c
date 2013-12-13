#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>

#include "main.h"
#include "gen.h"
#include "uld.h"
#include "dbg.h"

static pthread_mutex_t mtx_gen;
static pthread_mutex_t mtx_uld;
static pthread_t tid_gen;
static pthread_t tid_uld;

static void print_usage(char *argv0)
{
    printf("Usage: %s [OPTIONS]\n", argv0);

    printf("\nCPE information:\n");
    printf("  --sn=SN               serial number\n");
    printf("\nKPI file generation options:\n");
    printf("  --kpi-interval INTVAL interval of kpi generation\n");
    printf("                        upload will begins immediately if KPI file generated\n");
    printf("\nUpload options:\n");
    printf("  --url URL             upload url\n");
    printf("  --port PORT           upload port\n");
    printf("  --username USERNAME   upload username\n");
    printf("  --password PASSWORD   upload password\n");
    printf("  --backup              whether backup the kpi file\n");
    printf("                        after uploading successfully\n");
    printf("\nOther options:\n");
    printf("  --help    display this help and exit\n");
}

static int parser_para(cpe_kpi_emu_ctx_t *ctx, int argc, char *argv[])
{
    int opt = 0;
    struct option long_opts[] = {
        { "help",           no_argument,        0, 'h' },
        { "sn",             required_argument,  0, 's' },
        { "url",            required_argument,  0, 'U' },
        { "port",           required_argument,  0, 'P' },
        { "kpi-interval",   required_argument,  0, 'i' },
        { "username",       required_argument,  0, 'u' },
        { "password",       required_argument,  0, 'p' },
        { "backup",         no_argument,        0, 'b' },
        { 0, 0, 0, 0 }
    };

    do {
        opt = getopt_long(argc, argv, "hs:U:P:i:u:p:b", long_opts, NULL);
        switch (opt) {
        case 'h':
            print_usage(argv[0]);
            exit(EXIT_SUCCESS);
            break;

        case 's':
            strncpy(ctx->sn, optarg, sizeof(ctx->sn) - 1);
            break;

        case 'U':
            strncpy(ctx->url, optarg, sizeof(ctx->url) - 1);
            break;

        case 'P':
            ctx->port = atoi(optarg);
            break;

        case 'i':
            ctx->uld_intval = atoi(optarg);
            break;

        case 'u':
            strncpy(ctx->username, optarg, sizeof(ctx->username) - 1);
            break;

        case 'p':
            strncpy(ctx->password, optarg, sizeof(ctx->password) - 1);
            break;

        case 'b':
            ctx->need_backup = true;
            break;

        case '?':
            print_usage(argv[0]);
            exit(EXIT_FAILURE);
            break;

        case -1:
            break;

        default:
            return -1;
        }
    } while (opt != -1);

    /**
     * TODO:
     *
     * Read upload time from para
     */
    ctx->t_uld_time = 0;

    if (ctx->uld_intval <= 1) {
        DBG_PNC("Upload interval too short!");
        return -1;
    }

    if (strlen(ctx->sn) == 0) {
        DBG_PNC("Serial number should not be empty!");
        return -1;
    }

    if (strlen(ctx->url) == 0) {
        DBG_PNC("Upload URL should not be empty!");
        return -1;
    }

    return 0;
}

static int time_init(cpe_kpi_emu_ctx_t *ctx)
{
    /**
     * TODO:
     * Optimize timezone calculation.
     */
    struct tm tm = { 0 };
    int gmt_h = 0, gmt_m = 0;

    ctx->t_beg_pro = time(NULL);

    localtime_r(&ctx->t_beg_pro, &tm);
#ifdef __USE_BSD
    gmt_h = tm.tm_gmtoff / 3600;
    gmt_m = abs((tm.tm_gmtoff - gmt_h * 3600) / 60);
#else
    gmt_h = tm.__tm_gmtoff / 3600;
    gmt_m = abs((tm.__tm_gmtoff - gmt_h * 3600) / 60);
#endif
    ctx->tz.h = gmt_h;
    ctx->tz.m = gmt_m;

    return 0;
}

static void sig_hdl(int sig)
{
    pthread_cancel(tid_uld);
    pthread_cancel(tid_gen);
    pthread_mutex_destroy(&mtx_gen);
    pthread_mutex_destroy(&mtx_uld);

    /**
     * Wait pthread be cancled!
     */
    sleep(1);

    exit(0);
}

static void *uld_thread_hdl(void *arg)
{
    const cpe_kpi_emu_ctx_t *ctx = ((cpe_kpi_emu_ctx_t *)arg);

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while (1) {
        pthread_mutex_lock(&mtx_uld);
        if (uld(ctx) != 0) {
            DBG_ERR("Upload kpi file failed, retry at next intval.");
        }
    }

    pthread_exit(NULL);
}

static void *gen_thread_hdl(void *arg)
{
    const cpe_kpi_emu_ctx_t *ctx = ((cpe_kpi_emu_ctx_t *)arg);

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while (1) {
        pthread_mutex_lock(&mtx_gen);
        if (gen(ctx) != 0) {
            DBG_ERR("Generate kpi file failed");
        } else {
            pthread_mutex_unlock(&mtx_uld);
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    cpe_kpi_emu_ctx_t ctx = { 0 };
    pthread_attr_t attr;
    bool has_gen = false;

    if (parser_para(&ctx, argc, argv) != 0) {
        DBG_PNC("Parser parameters failed, exit!");
        exit(EXIT_FAILURE);
    }

    if (time_init(&ctx) != 0) {
        DBG_PNC("Initiate time failed, exit!");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_init(&mtx_gen, NULL);
    pthread_mutex_init(&mtx_uld, NULL);
    pthread_mutex_lock(&mtx_gen);
    pthread_mutex_lock(&mtx_uld);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&tid_uld, &attr, uld_thread_hdl, (void *)&ctx) != 0) {
        DBG_PNC("Create upload pthread failed: %s.", strerror(errno));
        pthread_mutex_destroy(&mtx_gen);
        pthread_mutex_destroy(&mtx_uld);
        return -1;
    }

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&tid_gen, &attr, gen_thread_hdl, (void *)&ctx) != 0) {
        DBG_PNC("Create generate pthread failed: %s.", strerror(errno));
        pthread_cancel(tid_uld);
        pthread_mutex_destroy(&mtx_gen);
        pthread_mutex_destroy(&mtx_uld);
        return -1;
    }

    pthread_attr_destroy(&attr);

    signal(SIGINT, sig_hdl);
    signal(SIGTERM, sig_hdl);

    while (1) {
        if ((time(NULL) - ctx.t_uld_time) % ctx.uld_intval == 0) {
            if (!has_gen) {
                pthread_mutex_unlock(&mtx_gen);
                has_gen = true;
            }
        } else {
            has_gen = false;
        }
        usleep(100000);
    }

    return 0;
}
