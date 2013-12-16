#ifndef _MAIN_H_
#define _MAIN_H_

#include <time.h>
#include <stdbool.h>

typedef struct cpe_kpi_emu_ctx cpe_kpi_emu_ctx_t;
struct cpe_kpi_emu_ctx {
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
};

#endif /* ifndef _MAIN_H_ */
