#ifndef XR_PRINT_H
#define XR_PRINT_H

#include <stdio.h>

#ifndef _EDITOR
extern	__declspec(dllexport) char *g_ca_stdout;
#endif

extern void		__cdecl xr_printf	(FILE *stream, const char *fmt, ...);
extern size_t	__cdecl xr_memusage	();

#endif