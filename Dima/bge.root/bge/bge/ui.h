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
			console_ui()
	{
		printf("test::test() called!\n");
	}

	virtual ~console_ui()
	{
		printf("test::~test() called!\n");
	}
};

IC	console_ui &ui()
{
	return	(Loki::SingletonHolder<console_ui>::Instance());
}