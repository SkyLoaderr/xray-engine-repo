////////////////////////////////////////////////////////////////////////////
//	Module 		: console_ui.cpp
//	Created 	: 12.11.2004
//  Modified 	: 12.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Console interface
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ui.h"

int __cdecl CConsoleUI::log	(LPCSTR format, ...)
{
	va_list		marker;

	va_start	(marker,format);

	int			result = vfprintf(stdout,format,marker);

	va_end		(marker);

	return		(result);
}

void CConsoleUI::execute	(char argc, char *argv[])
{
}
