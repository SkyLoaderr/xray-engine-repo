////////////////////////////////////////////////////////////////////////////
//	Module 		: othello_bit_board.cpp
//	Created 	: 24.12.2004
//  Modified 	: 24.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Othello bit board script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "othello_bit_board.h"

template <int increment, COthelloBitBoard::cell_type _color_to_move, COthelloBitBoard::cell_type opponent_color>
IC	void COthelloBitBoard::try_flip_direction(cell_type *start_cell, int &difference)
{
    COthelloBitBoard::cell_type *current_cell = start_cell + increment;

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

template <COthelloBitBoard::cell_type _color_to_move>
IC	void COthelloBitBoard::do_move		(const cell_index &index)
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

	*start_cell			= color_to_move;
	m_flip_stack.push	(CStackCell(start_cell));
	m_flip_stack.push	(CStackCell((int)m_flip_stack.size() - stack_count - 1,m_passed));
	m_passed			= false;
	m_difference		= difference + (_color_to_move == BLACK ? 1 : -1);
	m_color_to_move		= opponent_color;
	--m_empties;
}

void COthelloBitBoard::do_move			(const cell_index &index)
{
	if (index) {
		if (color_to_move() == BLACK)
			do_move<BLACK>	(index);
		else
			do_move<WHITE>	(index);
		return;
	}
	
	VERIFY				(!can_move());
	
	change_color		();
	m_flip_stack.push	(CStackCell(0,m_passed));
	m_passed			= true;
}
