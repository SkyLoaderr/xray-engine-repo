////////////////////////////////////////////////////////////////////////////
//	Module 		: othello_classic_board_try_move.cpp
//	Created 	: 08.12.2004
//  Modified 	: 08.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Othello classic board try move implementation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "othello_classic_board.h"

template <COthelloClassicBoard::cell_type _color_to_move>
IC	bool COthelloClassicBoard::try_move		(const cell_index &index)
{
	const cell_type color_to_move		= _color_to_move;
	const cell_type	opponent_color		= (color_to_move == BLACK ? WHITE : BLACK);
	int				difference			= this->difference();
	int				stack_count			= (int)m_flip_stack.size();
	cell_type		*start_cell			= m_board + index;

	switch (flipping_directions[index]) {
		case 1 : {
			try_flip_direction<  1, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction<  9, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction< 10, color_to_move, opponent_color>(start_cell, difference);
			break;			   
		}					   
		case 2 : {			   
			try_flip_direction< -1, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction<  1, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction<  8, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction<  9, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction< 10, color_to_move, opponent_color>(start_cell, difference);
			break;			   
		}					   
		case 3 : {			   
			try_flip_direction< -1, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction<  8, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction<  9, color_to_move, opponent_color>(start_cell, difference);
			break;			   
		}					   
		case 4 : {			   
			try_flip_direction< -9, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction< -8, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction<  1, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction<  9, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction< 10, color_to_move, opponent_color>(start_cell, difference);
			break;			   
		}					   
		case 5 : {			   
			try_flip_direction<-10, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction< -9, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction< -8, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction< -1, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction<  1, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction<  8, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction<  9, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction< 10, color_to_move, opponent_color>(start_cell, difference);
			break;			   
		}					   
		case 6 : {			   
			try_flip_direction<-10, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction< -9, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction< -1, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction<  8, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction<  9, color_to_move, opponent_color>(start_cell, difference);
			break;			   
		}					   
		case 7 : {			   
			try_flip_direction< -9, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction< -8, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction<  1, color_to_move, opponent_color>(start_cell, difference);
			break;			   
		}					   
		case 8 : {			   
			try_flip_direction<-10, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction< -9, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction< -8, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction< -1, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction<  1, color_to_move, opponent_color>(start_cell, difference);
			break;			   
		}					   
		case 9 : {			   
			try_flip_direction<-10, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction< -9, color_to_move, opponent_color>(start_cell, difference);
			try_flip_direction< -1, color_to_move, opponent_color>(start_cell, difference);
			break;
		}
		default : NODEFAULT;
	}

	if (this->difference() == difference)
		return			(false);

	*start_cell			= color_to_move;
	m_flip_stack.push	(CStackCell(start_cell));
	m_flip_stack.push	(CStackCell((int)m_flip_stack.size() - stack_count - 1,m_passed));
	m_passed			= false;
	m_difference		= difference + (_color_to_move == BLACK ? 1 : -1);
	m_color_to_move		= opponent_color;
	--m_empties;
	return				(true);
}

bool COthelloClassicBoard::try_move			(const cell_index &index)
{
	if (index != MOVE_PASS) {
		if (color_to_move() == BLACK) {
			if (!try_move<BLACK>(index))
				return	(false);
		}
		else {
			if (!try_move<WHITE>(index))
				return	(false);
		}
		m_move_processor.on_do_move	(index);

		return			(true);
	}
	
	if (can_move())
		return			(false);
	
	m_move_processor.on_do_move		(index);
	change_color		();
	m_flip_stack.push	(CStackCell(0,m_passed));
	m_passed			= true;
	return				(true);
}
