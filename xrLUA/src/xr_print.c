#include <stdarg.h>
#include "xr_print.h"

char *g_ca_stderr = 0;
char *g_ca_stdout = 0;

void __cdecl xr_printf(FILE *stream, const char *fmt, ...)
{
	va_list marker;

	va_start(marker,fmt);

	if (g_ca_stderr)
		sprintf(g_ca_stderr,fmt,marker);

	va_end( marker );              
}
