#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#include "dbg.h"
#include "main.h"

static char *ltrim(char *src)
{
    size_t i = 0;
    char *p = src;
    size_t len = strlen(src);

    for (; (i < len) && (isspace(*p)); p++) {
        ;
    }

    return p;
}

static char *rtrim(char *src)
{
    char *p = src + strlen(src) - 1;

    for (; (p != src - 1) && (isspace(*p)); p--) {
        ;
    }
    *(p + 1) = '\0';

    return src;
}

static int split_cfg_line(char *line, char **name, char **value)
{
    char *p = NULL;

    if ((p = strchr(line, '#')) != NULL) {
        *p = '\0';
    }

    if ((p = strchr(line, '=')) == NULL) {
        return -1;
    }

    *value = rtrim(ltrim(p + 1));
    *p = '\0';
    *name = rtrim(ltrim(line));

    return (strlen(*name) != 0) ? 0 : -1;
}

static int parser_cfg(cheer_ctx_t *ctx,
                      const char *name, const char *value, size_t line_no)
{
    if (strncmp(name, "sn", strlen("sn")) == 0) {
        strncpy(ctx->sn, value, sizeof(ctx->sn) - 1);
    } else if (strncmp(name, "url", strlen("url")) == 0) {
        strncpy(ctx->url, value, sizeof(ctx->url) - 1);
    } else if (strncmp(name, "port", strlen("port")) == 0) {
        ctx->port = atoi(value);
    } else if (strncmp(name, "username", strlen("username")) == 0) {
        strncpy(ctx->username, value, sizeof(ctx->username) - 1);
    } else if (strncmp(name, "password", strlen("password")) == 0) {
        strncpy(ctx->password, value, sizeof(ctx->password) - 1);
    } else if (strncmp(name, "need_bak", strlen("password")) == 0) {
        switch (value[0]) {
        case 'Y':
        case 'y':
            ctx->need_bak = true;
            break;
        case 'N':
        case 'n':
            ctx->need_bak = false;
            break;
        default:
            DBG_ERR("%s LINE %u: need_bak = <Y|y|N|n>", ctx->cfg, line_no);
            break;
        }
    } else if (strncmp(name, "bak_dir", strlen("bak_dir")) == 0) {
        strncpy(ctx->bak_dir, value, sizeof(ctx->bak_dir) - 1);
    } else if (strncmp(name, "interval", strlen("interval")) == 0) {
        ctx->interval = atoi(value);
    } else if (strncmp(name, "debug_level", strlen("dbg_level")) == 0) {
        ctx->dbg_lvl = atoi(value);
    } else {
        DBG_ERR("%s LINE %u: Unrecognized iterm: \"%s\".",
                ctx->cfg, line_no, name);
    }
    /**
     * TODO:
     *
     * Read upload time from para
     */

    return 0;
}

int read_cfg(cheer_ctx_t *ctx, const char *filename)
{
    static char line[BUFSIZ] = { 0 };
    FILE *fp = NULL;
    char *name = NULL, *value = NULL;
    size_t line_no = 0;

    assert(filename);

    if ((fp = fopen(filename, "r")) == NULL) {
        return -1;
    }

    while (1) {
        if (fgets(line, sizeof(line), fp) == NULL) {
            break;
        }
        line_no++;

        if (split_cfg_line(line, &name, &value) == 0) {
            parser_cfg(ctx, name, value, line_no);
        }
    }

    fclose(fp);

    return 0;
}
