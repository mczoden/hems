#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>
#include <curl/curl.h>

#include "main.h"
#include "uld.h"
#include "dbg.h"

enum {
    PROTO_UNKNOWN = -1,
    PROTO_FTP,
    PROTO_HTTP,
    PROTO_HTTPS
};

static int get_proto_by_url(const char *url)
{
    if (strncmp(url, "ftp:", 4) == 0) {
        return PROTO_FTP;
    } else if (strncmp(url, "http:", 5) == 0) {
        return PROTO_HTTP;
    } else if (strncmp(url, "https:", 6) == 0) {
        return PROTO_HTTPS;
    }

    return -1;
}

static int do_curl(const cpe_kpi_emu_ctx_t *ctx, char *filename)
{
    int proto = 0;
    char *p = NULL;
    FILE *fp = NULL;
    CURL *curl = NULL;
    char url[256] = { 0 };
    static char err_buf[CURL_ERROR_SIZE] = { 0 };

    if ((proto = get_proto_by_url(ctx->url)) == PROTO_UNKNOWN) {
        DBG_ERR("Unkonw protocal.");
        return -1;
    }

    p = stpncpy(url, ctx->url, sizeof(url) - 1);
    if (p + strlen(filename) > url + sizeof(url) - 1) {
        DBG_ERR("File name too long, retry at next intval.");
        return -1;
    }
    strcat(url, filename);

    if ((fp = fopen(filename, "r")) == NULL) {
        DBG_ERR("Open file %s failed: %s.", filename, strerror(errno));
        return -1;
    }

    if ((curl = curl_easy_init()) == NULL) {
        DBG_ERR("Initiate CURL failed, retry at next intval.");
        fclose(fp);
        return -1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    if (ctx->port != 0 ) {
        curl_easy_setopt(curl, CURLOPT_PORT, ctx->port);
    }
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_USERNAME, ctx->username);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, ctx->password);
    if (proto == PROTO_HTTP) {
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
        curl_easy_setopt(curl, CURLOPT_UNRESTRICTED_AUTH, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    }
    curl_easy_setopt(curl, CURLOPT_READDATA, fp);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, err_buf);

    if (curl_easy_perform(curl) != CURLE_OK) {
        DBG_ERR("%s", err_buf);
        curl_easy_cleanup(curl);
        return -1;
    }

    curl_easy_cleanup(curl);

    return 0;
}

int uld(const cpe_kpi_emu_ctx_t *ctx)
{
    char date[16] = { 0 };
    char start_time[16] = { 0 }, end_time[16] = { 0 };
    char sn[256] = { 0 };
    DIR* dp = NULL;
    struct dirent *ent = NULL;
    char bak_file[256] = { 0 };

    assert(ctx);

    if ((dp = opendir(".")) == NULL) {
        DBG_ERR("Open directory \".\" failed: %s.", strerror(errno));
        return -1;
    }

    while ((ent = readdir(dp)) != NULL) {
        if (sscanf(ent->d_name,
                   "A%8[0-9].%4[0-9]%*[^-]-%4[0-9]%*[^_]_%s",
                   date, start_time, end_time, sn) == 4) {
            if (do_curl(ctx, ent->d_name) == 0) {
                DBG_INF("Upload kpi file %s successfully.", ent->d_name);
                if (ctx->need_bak) {
                    snprintf(bak_file, sizeof(bak_file), "%s/%s",
                             ctx->bak_dir, ent->d_name);
                    rename(ent->d_name, bak_file);
                } else {
                    unlink(ent->d_name);
                }
            } else {
                DBG_ERR("Upload %s failed", ent->d_name);
                continue;
            }
        }
    }

    closedir(dp);

    return 0;
}
