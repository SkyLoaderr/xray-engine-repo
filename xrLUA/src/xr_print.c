#include "stdafx.h"
#pragma hdrstop

#include "xr_print.h"

#ifndef _EDITOR
char *g_ca_stdout	= 0;
#endif

void __cdecl xr_printf(FILE *stream, const char *fmt, ...)
{
	va_list			marker;

	va_start		(marker,fmt);

#ifndef _EDITOR
	if (g_ca_stdout)
		g_ca_stdout += vsprintf(g_ca_stdout,fmt,marker);
#else
	Msg				(fmt,marker);
#endif

	va_end			(marker);              
}

size_t	__cdecl xr_memusage	()
{
	/*
	mallinfo	_i	= dlmallinfo();
	return		_i.uordblks;
	*/
	return	0;	// don't working at this moment
}
