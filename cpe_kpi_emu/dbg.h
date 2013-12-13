#ifndef _DBG_H_
#define _DBG_H_

#define LVL_PNC (1 << 0)
#define LVL_ERR (1 << 1)
#define LVL_INF (1 << 2)
#define LVL_DBG (1 << 3)

extern int set_dbg_lvl(int lvl);
extern void dbg(unsigned char lvl, const char *fmt, ...);

#define DBG_PNC(fmt, ...) dbg(LVL_PNC, fmt, ##__VA_ARGS__)
#define DBG_DBG(fmt, ...) dbg(LVL_DBG, fmt, ##__VA_ARGS__)
#define DBG_INF(fmt, ...) dbg(LVL_INF, fmt, ##__VA_ARGS__)
#define DBG_ERR(fmt, ...) dbg(LVL_ERR, fmt, ##__VA_ARGS__)

#endif /* ifndef _DBG_H_ */
