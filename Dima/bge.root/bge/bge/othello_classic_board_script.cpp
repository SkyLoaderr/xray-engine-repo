////////////////////////////////////////////////////////////////////////////
//	Module 		: othello_classic_board_script.cpp
//	Created 	: 07.12.2004
//  Modified 	: 07.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Othello classic board script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "othello_classic_board.h"
#include "script_space.h"
#include <luabind/iterator_policy.hpp>

using namespace luabind;

void COthelloClassicBoard::script_register(lua_State *L)
{
	module(L)
	[
		class_<COthelloClassicBoard>("othello_classic_board")
			.def(constructor<>())
			.def("start_position",		&COthelloClassicBoard::start_position)
			.def("show",				&COthelloClassicBoard::show)
			.def("move",				(void (COthelloClassicBoard::*)(const cell_index &, const cell_index &))(&COthelloClassicBoard::do_move))
			.def("move",				(void (COthelloClassicBoard::*)(const cell_index &))(&COthelloClassicBoard::do_move))
			.def("move",				(void (COthelloClassicBoard::*)(LPCSTR))(&COthelloClassicBoard::do_move))
			.def("undo",				&COthelloClassicBoard::script_undo_move)
			.def("can_move",			(bool (COthelloClassicBoard::*)(const cell_index &, const cell_index &) const)(&COthelloClassicBoard::can_move))
			.def("can_move",			(bool (COthelloClassicBoard::*)(LPCSTR) const)(&COthelloClassicBoard::can_move))
			.def("can_move",			(bool (COthelloClassicBoard::*)() const)(&COthelloClassicBoard::can_move))
			.def("color_to_move",		&COthelloClassicBoard::color_to_move)
			.def("empties",				&COthelloClassicBoard::empties)
			.def("difference",			&COthelloClassicBoard::difference)
			.def("compute_difference",	(int (COthelloClassicBoard::*)(const cell_index &,const cell_index &) const)(&COthelloClassicBoard::compute_difference))
			.def("compute_difference",	(int (COthelloClassicBoard::*)(LPCSTR) const)(&COthelloClassicBoard::compute_difference))
			.def("terminal_position",	&COthelloClassicBoard::terminal_position)
			.def("score",				&COthelloClassicBoard::score)
			.def("moves",				&COthelloClassicBoard::moves, return_stl_iterator)
	];
}
