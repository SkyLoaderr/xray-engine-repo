////////////////////////////////////////////////////////////////////////////
//	Module 		: othello_classic_board_compute_result.cpp
//	Created 	: 08.12.2004
//  Modified 	: 08.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Othello classic board compute result implementation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "othello_classic_board.h"

template <
	int increment, 
	COthelloClassicBoard::cell_type _color_to_move, 
	COthelloClassicBoard::cell_type opponent_color
>
IC	void COthelloClassicBoard::compute_direction(cell_type const *start_cell, int &result) const
{
	if (start_cell[1*increment] != opponent_color)
		return;

	if (start_cell[2*increment] != opponent_color) {
		if (start_cell[2*increment] == _color_to_move)
			result	+= _color_to_move == BLACK ? 2 : -2;
		return;
	}
			
	if (start_cell[3*increment] != opponent_color) {
		if (start_cell[3*increment] == _color_to_move)
			result	+= _color_to_move == BLACK ? 4 : -4;
		return;
	}
			
	if (start_cell[4*increment] != opponent_color) {
		if (start_cell[4*increment] == _color_to_move)
			result	+= _color_to_move == BLACK ? 6 : -6;
		return;
	}
			
	if (start_cell[5*increment] != opponent_color) {
		if (start_cell[5*increment] == _color_to_move)
			result	+= _color_to_move == BLACK ? 8 : -8;
		return;
	}
			
	if (start_cell[6*increment] != opponent_color) {
		if (start_cell[6*increment] == _color_to_move)
			result	+= _color_to_move == BLACK ? 10 : -10;
		return;
	}
	
	if (start_cell[7*increment] == _color_to_move)
		result		+= _color_to_move == BLACK ? 12 : -12;
}
	
template <COthelloClassicBoard::cell_type _color_to_move>
IC	int COthelloClassicBoard::compute_difference	(const cell_index &index) const
{
	const cell_type color_to_move	= _color_to_move;
	const cell_type	opponent_color	= (color_to_move == BLACK ? WHITE : BLACK);
	cell_type const *start_cell		= m_board + index;
	int				result			= difference();

	switch (flipping_directions[index]) {
		case 1 : {
			compute_direction<  1, color_to_move, opponent_color>(start_cell, result);
			compute_direction<  9, color_to_move, opponent_color>(start_cell, result);
			compute_direction< 10, color_to_move, opponent_color>(start_cell, result);
			break;			   
		}					   
		case 2 : {			   
			compute_direction< -1, color_to_move, opponent_color>(start_cell, result);
			compute_direction<  1, color_to_move, opponent_color>(start_cell, result);
			compute_direction<  8, color_to_move, opponent_color>(start_cell, result);
			compute_direction<  9, color_to_move, opponent_color>(start_cell, result);
			compute_direction< 10, color_to_move, opponent_color>(start_cell, result);
			break;			   
		}					   
		case 3 : {			   
			compute_direction< -1, color_to_move, opponent_color>(start_cell, result);
			compute_direction<  8, color_to_move, opponent_color>(start_cell, result);
			compute_direction<  9, color_to_move, opponent_color>(start_cell, result);
			break;			   
		}					   
		case 4 : {			   
			compute_direction< -9, color_to_move, opponent_color>(start_cell, result);
			compute_direction< -8, color_to_move, opponent_color>(start_cell, result);
			compute_direction<  1, color_to_move, opponent_color>(start_cell, result);
			compute_direction<  9, color_to_move, opponent_color>(start_cell, result);
			compute_direction< 10, color_to_move, opponent_color>(start_cell, result);
			break;			   
		}					   
		case 5 : {			   
			compute_direction<-10, color_to_move, opponent_color>(start_cell, result);
			compute_direction< -9, color_to_move, opponent_color>(start_cell, result);
			compute_direction< -8, color_to_move, opponent_color>(start_cell, result);
			compute_direction< -1, color_to_move, opponent_color>(start_cell, result);
			compute_direction<  1, color_to_move, opponent_color>(start_cell, result);
			compute_direction<  8, color_to_move, opponent_color>(start_cell, result);
			compute_direction<  9, color_to_move, opponent_color>(start_cell, result);
			compute_direction< 10, color_to_move, opponent_color>(start_cell, result);
			break;			   
		}					   
		case 6 : {			   
			compute_direction<-10, color_to_move, opponent_color>(start_cell, result);
			compute_direction< -9, color_to_move, opponent_color>(start_cell, result);
			compute_direction< -1, color_to_move, opponent_color>(start_cell, result);
			compute_direction<  8, color_to_move, opponent_color>(start_cell, result);
			compute_direction<  9, color_to_move, opponent_color>(start_cell, result);
			break;			   
		}					   
		case 7 : {			   
			compute_direction< -9, color_to_move, opponent_color>(start_cell, result);
			compute_direction< -8, color_to_move, opponent_color>(start_cell, result);
			compute_direction<  1, color_to_move, opponent_color>(start_cell, result);
			break;			   
		}					   
		case 8 : {			   
			compute_direction<-10, color_to_move, opponent_color>(start_cell, result);
			compute_direction< -9, color_to_move, opponent_color>(start_cell, result);
			compute_direction< -8, color_to_move, opponent_color>(start_cell, result);
			compute_direction< -1, color_to_move, opponent_color>(start_cell, result);
			compute_direction<  1, color_to_move, opponent_color>(start_cell, result);
			break;			   
		}					   
		case 9 : {			   
			compute_direction<-10, color_to_move, opponent_color>(start_cell, result);
			compute_direction< -9, color_to_move, opponent_color>(start_cell, result);
			compute_direction< -1, color_to_move, opponent_color>(start_cell, result);
			break;
		}
		default : NODEFAULT;
	}
	
	return	(result + (_color_to_move == BLACK ? 1 : -1));
}

template <COthelloClassicBoard::cell_type _color_to_move>
IC	int	 COthelloClassicBoard::compute_difference	(const cell_index &index, bool) const
{
	VERIFY			(can_move(index));
	if (index != MOVE_PASS) {
		VERIFY		(can_move(index) == EMPTY);
		if (color_to_move() == BLACK)
			return	(compute_difference<BLACK>(index));
		else
			return	(-compute_difference<WHITE>(index));
	}

	VERIFY			(!can_move());

	if (color_to_move() == BLACK)
		return		(difference());
	else
		return		(-difference());
}

int	 COthelloClassicBoard::compute_difference	(const cell_index &index) const
{
	if (color_to_move() == BLACK)
		return		(compute_difference<BLACK>(index,true));
	else
		return		(-compute_difference<WHITE>(index,true));
}
