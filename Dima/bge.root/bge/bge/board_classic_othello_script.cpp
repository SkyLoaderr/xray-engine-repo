////////////////////////////////////////////////////////////////////////////
//	Module 		: board_classic_othello_script.cpp
//	Created 	: 07.12.2004
//  Modified 	: 07.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Classic othello board script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "board_classic_othello.h"
#include "script_space.h"

using namespace luabind;

void CBoardClassicOthello::script_register(lua_State *L)
{
	module(L)
	[
		class_<CBoardClassicOthello>("bco")//("board_classic_othello")
			.def(constructor<>())
			.def("start_position",		&CBoardClassicOthello::start_position)
			.def("show",				&CBoardClassicOthello::show)
			.def("move",				(void (CBoardClassicOthello::*)(const cell_index &, const cell_index &))(&CBoardClassicOthello::do_move))
			.def("undo",				(void (CBoardClassicOthello::*)())(&CBoardClassicOthello::undo_move))
			.def("can_move",			(bool (CBoardClassicOthello::*)(const cell_index &, const cell_index &) const)(&CBoardClassicOthello::can_move))
			.def("can_move",			(bool (CBoardClassicOthello::*)() const)(&CBoardClassicOthello::can_move))
			.def("color_to_move",		&CBoardClassicOthello::color_to_move)
			.def("empties",				&CBoardClassicOthello::empties)
			.def("difference",			&CBoardClassicOthello::difference)
			.def("compute_difference",	(int (CBoardClassicOthello::*)(const cell_index &,const cell_index &) const)(&CBoardClassicOthello::compute_difference))
	];
}
