////////////////////////////////////////////////////////////////////////////
//	Module 		: console_ui.cpp
//	Created 	: 12.11.2004
//  Modified 	: 12.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Console interface
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ui.h"

LPCSTR log_file_name = "vo.log";

CConsoleUI::CConsoleUI		()
{
	m_log		= fopen(log_file_name,"at");
	if (!m_log) {
		fprintf	(stderr,"Cannot open file %s!\n",log_file_name);
		exit	(1);
	}
}

CConsoleUI::~CConsoleUI		()
{
	if (!m_log) {
		fprintf	(stderr,"Cannot close file %s!\n",log_file_name);
		exit	(2);
	}
	fclose		(m_log);
}

int __cdecl CConsoleUI::log	(LPCSTR format, ...)
{
	va_list		marker;

	va_start	(marker,format);

	int			result = vfprintf(stdout,format,marker);
	vfprintf	(m_log,format,marker);

	va_end		(marker);

	return		(result);
}

void CConsoleUI::execute	(char argc, char *argv[])
{
}
