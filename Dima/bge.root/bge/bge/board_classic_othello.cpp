////////////////////////////////////////////////////////////////////////////
//	Module 		: board_classic_othello.cpp
//	Created 	: 07.12.2004
//  Modified 	: 07.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Classic othello board implementation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "board_classic_othello.h"
#include "stack_reserve.h"

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
	m_passed		= false;
	m_current_flip	= m_flip_stack;

	stack_reserve<u32>	s;
	s.reserve			(4096);
	s.push				(12);
	s.push				(6);
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
