////////////////////////////////////////////////////////////////////////////
//	Module 		: ui.h
//	Created 	: 12.11.2004
//  Modified 	: 12.11.2004
//	Author		: Dmitriy Iassenev
//	Description : User interface
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <singleton.h>
#include "console_ui.h"

IC	CConsoleUI &ui()
{
	return	(Loki::SingletonHolder<CConsoleUI>::Instance());
}