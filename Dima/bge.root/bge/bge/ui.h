////////////////////////////////////////////////////////////////////////////
//	Module 		: ui.h
//	Created 	: 12.11.2004
//  Modified 	: 12.11.2004
//	Author		: Dmitriy Iassenev
//	Description : User interface
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <singleton.h>

struct console_ui {
	IC		console_ui	()
	{
		log("test::test() called!\n");
	}

	virtual ~console_ui	()
	{
		log("test::~test() called!\n");
	}

	int __cdecl log		(LPCSTR format, ...)
	{
		va_list marker;

		va_start(marker,format);

		int		result = vfprintf(stdout,format,marker);

		va_end	(marker);

		return	(result);
	}
};

IC	console_ui &ui()
{
	return	(Loki::SingletonHolder<console_ui>::Instance());
}