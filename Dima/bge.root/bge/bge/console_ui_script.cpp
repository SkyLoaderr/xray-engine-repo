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

using namespace luabind;

void _log(CConsoleUI *ui, LPCSTR string)
{
	ui->log(string);
	ui->log("\n");
}

void CConsoleUI::script_register(lua_State *L)
{
	module(L)
	[
		def("ui",&ui),
		class_<CConsoleUI>("CConsoleUI")
			.def("log",&_log)
	];
}