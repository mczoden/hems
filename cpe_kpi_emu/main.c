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
#include <sys/types.h>
#include <sys/stat.h>

#include "main.h"
#include "gen.h"
#include "uld.h"
#include "dbg.h"
#include "cfg.h"

#define DFL_BAK_DIR "./backup/"

static pthread_mutex_t mtx_gen = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mtx_uld = PTHREAD_MUTEX_INITIALIZER;
static pthread_t tid_gen = 0;
static pthread_t tid_uld = 0;

static void print_usage(const char *argv0)
{
    printf("Usage: %s [OPTIONS]\n", argv0);

    printf("  --config CONFIG   specify config file path\n");
    printf("  --help            display this help and exit\n");
}

static int parser_para(cheer_ctx_t *ctx, int argc, char *argv[])
{
    int opt = 0;
    struct option long_opts[] = {
        { "help",           no_argument,        0, 'h' },
        { "config",         required_argument,  0, 'f' },
        { 0, 0, 0, 0 }
    };

    do {
        opt = getopt_long(argc, argv, "hf:", long_opts, NULL);
        switch (opt) {
        case 'h':
            print_usage(argv[0]);
            exit(EXIT_SUCCESS);
            break;

        case 'f':
            strncpy(ctx->cfg, optarg, sizeof(ctx->cfg) - 1);
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

    return 0;
}

static int chk_and_prep_bak_dir(const char *dirname)
{
    struct stat st = { 0 };

    if (lstat(dirname, &st) != 0) {
        if (errno == ENOENT) {
            if (mkdir(dirname, 0777) != 0) {
                DBG_PNC("Create directory: %s failed.");
                return -1;
            }
        } else {
            DBG_PNC("Invoke lstat() failed: %s.", strerror(errno));
            return -1;
        }
    } else {
        if (!S_ISDIR(st.st_mode)) {
            DBG_PNC("%s has been existed but not a directory.", dirname);
            return -1;
        }
    }

    return 0;
}

static bool permit_to_run(cheer_ctx_t *ctx)
{

    if (ctx->interval <= 1) {
        DBG_PNC("Upload interval too short.");
        return false;
    }

    if (strlen(ctx->sn) == 0) {
        DBG_PNC("Serial number should not be empty.");
        return false;
    }

    if (strlen(ctx->url) == 0) {
        DBG_PNC("Upload URL should not be empty.");
        return false;
    }

    if (ctx->need_bak) {
        if (strlen(ctx->bak_dir) == 0) {
            DBG_ERR("Backup directory is not specified, default: %s.",
                    DFL_BAK_DIR);
            strncpy(ctx->bak_dir, DFL_BAK_DIR, sizeof(ctx->bak_dir) - 1);
        }
        if (chk_and_prep_bak_dir(ctx->bak_dir) != 0) {
            DBG_PNC("Backup directory can't be used.");
            return false;
        }
    }

    if (ctx->dbg_lvl == LVL_NON) {
        DBG_INF("Debug level is 0, no dbg will be displayed from now.");
    }
    set_dbg_lvl(ctx->dbg_lvl);

    return true;
}

static int time_init(cheer_ctx_t *ctx)
{
    /**
     * TODO:
     * Optimize timezone calculation.
     */
    struct tm tm = { 0 };
    long int gmt_h = 0, gmt_m = 0;

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
    uld_deinit();

    exit(0);
}

static void *uld_thread_hdl(void *arg)
{
    const cheer_ctx_t *ctx = ((cheer_ctx_t *)arg);

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
    const cheer_ctx_t *ctx = ((cheer_ctx_t *)arg);

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while (1) {
        pthread_mutex_lock(&mtx_gen);
        if (gen(ctx) != 0) {
            DBG_ERR("Generate kpi file failed.");
        } else {
            pthread_mutex_unlock(&mtx_uld);
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    cheer_ctx_t ctx = { 0 };
    pthread_attr_t attr;
    bool has_gen = false;

    if (parser_para(&ctx, argc, argv) != 0) {
        DBG_PNC("Parser parameters failed, exit.");
        exit(EXIT_FAILURE);
    }

    if (strlen(ctx.cfg) == 0) {
        DBG_PNC("Specify the config file with -f.");
        exit(EXIT_FAILURE);
    }

    if (read_cfg(&ctx, ctx.cfg) != 0) {
        DBG_PNC("Read config file: %s failed.");
        exit(EXIT_FAILURE);
    }

    if (time_init(&ctx) != 0) {
        DBG_PNC("Initiate time failed, exit.");
        exit(EXIT_FAILURE);
    }

    if (uld_init() != 0) {
        exit(EXIT_FAILURE);
    }

    if (!permit_to_run(&ctx)) {
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
        sleep(1);
        pthread_mutex_destroy(&mtx_gen);
        pthread_mutex_destroy(&mtx_uld);
        return -1;
    }

    pthread_attr_destroy(&attr);

    signal(SIGINT, sig_hdl);
    signal(SIGTERM, sig_hdl);

    while (1) {
        if ((time(NULL) - ctx.t_uld_time) % ctx.interval == 0) {
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
