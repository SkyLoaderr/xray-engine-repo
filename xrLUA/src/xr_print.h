#ifndef XR_PRINT_H
#define XR_PRINT_H

#include <stdio.h>

extern __declspec(dllexport) char *g_ca_stderr;
extern __declspec(dllexport) char *g_ca_stdout;

extern void __cdecl xr_printf(FILE *stream, const char *fmt, ...);

#endif