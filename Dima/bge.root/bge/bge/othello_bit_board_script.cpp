////////////////////////////////////////////////////////////////////////////
//	Module 		: othello_bit_board.cpp
//	Created 	: 24.12.2004
//  Modified 	: 24.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Othello bit board implementation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "othello_bit_board.h"

#include "script_space.h"

using namespace luabind;

void COthelloBitBoard::script_register(lua_State *L)
{
	module(L)
	[
		class_<COthelloBitBoard>("othello_bit_board")
			.def(constructor<>())
			.def("start_position",		&COthelloBitBoard::start_position)
			.def("show",				&COthelloBitBoard::show)
			.def("move",				(void (COthelloBitBoard::*)(const cell_index &, const cell_index &))(&COthelloBitBoard::do_move))
			.def("move",				(void (COthelloBitBoard::*)(LPCSTR))(&COthelloBitBoard::do_move))
			.def("undo",				(void (COthelloBitBoard::*)())(&COthelloBitBoard::undo_move))
			.def("can_move",			(bool (COthelloBitBoard::*)(const cell_index &, const cell_index &) const)(&COthelloBitBoard::can_move))
			.def("can_move",			(bool (COthelloBitBoard::*)(LPCSTR) const)(&COthelloBitBoard::can_move))
			.def("can_move",			(bool (COthelloBitBoard::*)() const)(&COthelloBitBoard::can_move))
			.def("color_to_move",		&COthelloBitBoard::color_to_move)
			.def("empties",				&COthelloBitBoard::empties)
			.def("difference",			&COthelloBitBoard::difference)
			.def("compute_difference",	(int (COthelloBitBoard::*)(const cell_index &,const cell_index &) const)(&COthelloBitBoard::compute_difference))
			.def("compute_difference",	(int (COthelloBitBoard::*)(LPCSTR) const)(&COthelloBitBoard::compute_difference))
			.def("terminal_position",	&COthelloBitBoard::terminal_position)
	];
}
