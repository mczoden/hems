#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "dbg.h"

static unsigned char dbg_lvl = LVL_PNC | LVL_ERR | LVL_INF;

void dbg(unsigned char lvl, const char *fmt, ...)
{
    va_list arg;
    time_t t_now = NULL;
    struct tm tm_now = { 0 };

    if ((dbg_lvl & lvl) == 0) {
        return;
    }

    t_now = time(NULL);
    localtime_r(&t_now, &tm_now);
    fprintf(stderr, "[%04d%02d%02d %02d:%02d:%02d] ",
            tm_now.tm_year + 1900,
            tm_now.tm_mon + 1,
            tm_now.tm_mday,
            tm_now.tm_hour,
            tm_now.tm_min,
            tm_now.tm_sec);

    switch (lvl) {
    case LVL_PNC:
        fprintf(stderr, "[PNC] ");
        break;

    case LVL_ERR:
        fprintf(stderr, "[ERR] ");
        break;

    case LVL_INF:
        fprintf(stderr, "[INF] ");
        break;

    case LVL_DBG:
        fprintf(stderr, "[DBG] ");
        break;

    default:
        break;
    }

    va_start(arg, fmt);
    vfprintf(stderr, fmt, arg);
    va_end(arg);

    fprintf(stderr, "\n");
}

int set_dbg_lvl(int lvl)
{
    if (lvl < 0) {
        DBG_ERR("Invalid debug level specified, keep default: panic, error & info.");
        return -1;
    }

    if (lvl > 7) {
        lvl = 7;
    }

    dbg_lvl = 0xff >> (8 - lvl);

    return 0;
}
