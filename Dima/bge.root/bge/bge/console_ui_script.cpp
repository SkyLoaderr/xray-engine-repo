////////////////////////////////////////////////////////////////////////////
//	Module 		: console_ui_script.cpp
//	Created 	: 12.11.2004
//  Modified 	: 12.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Console interface script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "console_ui.h"
#include "ui.h"
#include "script_space.h"

void CConsoleUI::script_register(lus_State *L)
{
	module(L)
	[
		def("ui",&ui),
		class_<CConsoleUI>("CConsoleUI")
	];
}