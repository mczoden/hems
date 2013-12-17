#ifndef _MAIN_H_
#define _MAIN_H_

#include <time.h>
#include <stdbool.h>

typedef struct cheer_ctx cheer_ctx_t;
struct cheer_ctx {
    time_t t_beg_pro;
    char cfg[256];
    
    struct {
        int h;
        int m;
    } tz;

    int interval;
    time_t t_uld_time;

    char sn[256];
    char url[256];
    int port;
    char username[256];
    char password[256];

    bool need_bak;
    char bak_dir[256];

    int dbg_lvl;
};

#endif /* ifndef _MAIN_H_ */
