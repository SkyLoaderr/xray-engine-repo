////////////////////////////////////////////////////////////////////////////
//	Module 		: board_classic_othello_do_move.cpp
//	Created 	: 08.12.2004
//  Modified 	: 08.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Classic othello board do move implementation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "board_classic_othello.h"

template <int increment, CBoardClassicOthello::cell_type _color_to_move, CBoardClassicOthello::cell_type opponent_color>
IC	void CBoardClassicOthello::try_flip_direction(cell_type *start_cell, int &difference)
{
    CBoardClassicOthello::cell_type *current_cell = start_cell + increment;

	if (*current_cell != opponent_color)
		return;

	current_cell		+= increment;
	if (*current_cell != opponent_color)
		goto check;
			
	current_cell		+= increment;
	if (*current_cell != opponent_color)
		goto check;
			
	current_cell		+= increment;
	if (*current_cell != opponent_color)
		goto check;
			
	current_cell		+= increment;
	if (*current_cell != opponent_color)
		goto check;
			
	current_cell		+= increment;
	if (*current_cell != opponent_color)
		goto check;
			
    current_cell		+= increment;

check:
	if (*current_cell != _color_to_move)
		return;

	current_cell		-= increment;
	do {
        *current_cell		= _color_to_move;
		m_flip_stack.push	(CStackCell(current_cell));
        current_cell		-= increment;
		difference			+= _color_to_move == BLACK ? 2 : -2;
    }
	while (current_cell != start_cell);
}

template <CBoardClassicOthello::cell_type _color_to_move>
IC	bool CBoardClassicOthello::do_move		(const cell_index &index)
{
	const cell_type color_to_move	= _color_to_move;
	const cell_type	opponent_color	= (color_to_move == BLACK ? WHITE : BLACK);
	int				difference		= this->difference();
	int				stack_count		= (int)m_flip_stack.size();
	cell_type		*start_cell		= m_board + index;

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

	if (stack_count != (int)m_flip_stack.size()) {
		*start_cell			= color_to_move;
		m_flip_stack.push	(CStackCell(start_cell));
		m_flip_stack.push	(CStackCell((int)m_flip_stack.size() - stack_count - 1,m_passed));
		m_passed			= false;
		m_difference		= difference + 1;
		m_color_to_move		= opponent_color;
		return				(true);
	}
	return					(false);
}

bool CBoardClassicOthello::do_move			(const cell_index &index)
{
	if (index) {
		if (color_to_move() == BLACK)
			return		(do_move<BLACK>(index));
		else
			return		(do_move<WHITE>(index));
	}
	
	if (can_move())
		return			(false);

	change_color		();
	m_flip_stack.push	(CStackCell(0,m_passed));
	m_passed			= true;
	return				(true);
}
