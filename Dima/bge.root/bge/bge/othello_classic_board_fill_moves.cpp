////////////////////////////////////////////////////////////////////////////
//	Module 		: othello_classic_board_fill_moves.cpp
//	Created 	: 08.12.2004
//  Modified 	: 08.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Othello classic board fill moves implementation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "othello_classic_board.h"

template <COthelloClassicBoard::cell_type _color_to_move>
void COthelloClassicBoard::fill_moves		()
{
	for (cell_index i=0; i<8; ++i)
		for (cell_index j=0; j<8; ++j) {
			if (((const COthelloClassicBoard*)(this))->cell(i,j) != EMPTY)
				continue;

			if (can_move<_color_to_move>(index(i,j)))
				m_move_processor.current_position_moves().add(index(i,j));
		}

	if (!m_move_processor.moves().empty()) {
		m_move_processor.current_position_moves().actual(true);
		return;
	}

	m_move_processor.current_position_moves().add	(MOVE_PASS);
	m_move_processor.current_position_moves().actual(true);
}

void COthelloClassicBoard::fill_moves		()
{
	if (color_to_move() == BLACK)
		fill_moves<BLACK>		();
	else
		fill_moves<WHITE>		();
}
