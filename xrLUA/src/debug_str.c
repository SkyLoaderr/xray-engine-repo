#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>

#include "debug_str.h"

__declspec(dllexport) char* g_caOutBuffer = 0;
__declspec(dllexport) char* g_caErrorBuffer = 0;
int	 g_iReturn;

int xr_print(FILE *tpStream, const char *caFormat,...)
{
	va_list marker;

	if (!g_caOutBuffer || !g_caErrorBuffer)
		return(0);

	va_start(marker,caFormat);

	g_iReturn = 0;

	if (tpStream == stdout)
		g_caOutBuffer += g_iReturn = vsprintf(g_caOutBuffer,caFormat,marker);
	else
		if (tpStream == stderr)
			g_caErrorBuffer += g_iReturn = vsprintf(g_caErrorBuffer,caFormat,marker);
		else
			assert(0);

	va_end(marker);

	return(g_iReturn);
}
