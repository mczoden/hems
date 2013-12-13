#ifndef _MAIN_H_
#define _MAIN_H_

#include <time.h>
#include <stdbool.h>

typedef struct cpe_kpi_emu_ctx cpe_kpi_emu_ctx_t;
struct cpe_kpi_emu_ctx {
    time_t t_beg_pro;
    
    struct {
        int h;
        int m;
    } tz;

    int uld_intval;
    time_t t_uld_time;

    char sn[256];
    char url[256];
    int port;
    char username[256];
    char password[256];

    bool need_backup;
};

#endif /* ifndef _MAIN_H_ */
