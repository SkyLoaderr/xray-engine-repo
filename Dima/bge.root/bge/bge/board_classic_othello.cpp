////////////////////////////////////////////////////////////////////////////
//	Module 		: board_classic_othello.cpp
//	Created 	: 07.12.2004
//  Modified 	: 07.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Calssic othello board implementation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "board_classic_othello.h"

const u8 CBoardClassicOthello::flipping_directions[BOARD_SIZE] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 1, 2, 2, 2, 2, 3, 3,
	0, 1, 1, 2, 2, 2, 2, 3, 3,
	0, 4, 4, 5, 5, 5, 5, 6, 6,
	0, 4, 4, 5, 5, 5, 5, 6, 6,
	0, 4, 4, 5, 5, 5, 5, 6, 6,
	0, 4, 4, 5, 5, 5, 5, 6, 6,
	0, 7, 7, 8, 8, 8, 8, 9, 9,
	0, 7, 7, 8, 8, 8, 8, 9, 9,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void CBoardClassicOthello::start_position	()
{
	Memory::mem_fill(m_board,DUMMY,sizeof(m_board));
	
	for (cell_index i=0; i<8; ++i)
		for (cell_index j=0; j<8; ++j)
			cell	(i,j,EMPTY);
	
	cell			(3,3,WHITE);
	cell			(3,4,BLACK);
	cell			(4,3,BLACK);
	cell			(4,4,WHITE);

	m_color_to_move	= BLACK;
	m_empties		= 60;
	m_difference	= 0;
	m_current_flip	= m_flip_stack;
}

void CBoardClassicOthello::show_cell		(const cell_type &value) const
{
	char			output;
	switch (value) {
		case WHITE : {
			output = 'o';
			break;
		}
		case BLACK : {
			output = 'x';
			break;
		}
		case EMPTY : {
			output = '_';
			break;
		}
		default : NODEFAULT;
	}
	ui().log		(" %c",output);
}

void CBoardClassicOthello::show				() const
{
	for (cell_index i=0; i<8; ++i) {
		for (cell_index j=0; j<8; ++j)
			show_cell(cell(i,j));
		ui().log("\n");
	}
}

template <int increment, CBoardClassicOthello::cell_type _color_to_move, CBoardClassicOthello::cell_type opponent_color>
IC	void CBoardClassicOthello::try_flip_direction(cell_type *start_cell, int &difference)
{
    CBoardClassicOthello::cell_type *current_cell = start_cell + increment;
	if (*current_cell == opponent_color) {
		current_cell += increment;
		if (*current_cell == opponent_color) {
			current_cell += increment;
			if (*current_cell == opponent_color) {
				current_cell += increment;
				if (*current_cell == opponent_color) {
					current_cell += increment;
					if (*current_cell == opponent_color) {
						current_cell += increment;
						if (*current_cell == opponent_color) {
                            current_cell += increment;
                        }
                    }
                }
            }
        }
		if (*current_cell == _color_to_move) {
			current_cell		-= increment;
			do {
                *current_cell	= _color_to_move;
				*(m_current_flip++) = current_cell;
                current_cell	-= increment;
                difference		+= 2;
            }
			while (current_cell != start_cell);
        }
    }
}

template <CBoardClassicOthello::cell_type _color_to_move>
IC	bool CBoardClassicOthello::do_move		(const cell_index &index)
{
	const cell_type color_to_move	= _color_to_move;
	const cell_type	opponent_color	= (color_to_move == BLACK ? WHITE : BLACK);
	int				difference		= this->difference();
	cell_type	*	start_cell		= m_board + index;

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
	}

	if (difference != this->difference()) {
		*start_cell			= color_to_move;
		*(m_current_flip++) = start_cell;
		*(m_current_flip++) = reinterpret_cast<cell_type*>((size_t)((this->difference() - difference) >> 1));
		m_difference		= difference + 1;
		m_color_to_move		= opponent_color;
		return				(true);
	}
	return					(false);
}

bool CBoardClassicOthello::do_move			(const cell_index &index)
{
	if (color_to_move() == BLACK)
		return	(do_move<BLACK>(index));
	else
		return	(do_move<WHITE>(index));
}
