#ifndef DEBUG_STR_H
#define DEBUG_STR_H

extern char *g_caOutBuffer;
extern char *g_caErrorBuffer;

extern int xr_print(FILE *tpStream, const char *caFormat,...);

#endif