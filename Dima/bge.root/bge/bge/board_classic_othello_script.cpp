////////////////////////////////////////////////////////////////////////////
//	Module 		: board_classic_othello_script.cpp
//	Created 	: 07.12.2004
//  Modified 	: 07.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Calssic othello board script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "board_classic_othello.h"
#include "script_space.h"

using namespace luabind;

void CBoardClassicOthello::script_register(lua_State *L)
{
	module(L)
	[
		class_<CBoardClassicOthello>("board_classic_othello")
			.def(constructor<>())
			.def("move", (bool (CBoardClassicOthello::*)(const u8 &,const u8 &))(&CBoardClassicOthello::do_move))
			.def("undo", (void (CBoardClassicOthello::*)())(&CBoardClassicOthello::undo_move))
			.def("show", &CBoardClassicOthello::show)
	];
}