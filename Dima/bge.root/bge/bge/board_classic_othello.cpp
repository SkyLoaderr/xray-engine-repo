////////////////////////////////////////////////////////////////////////////
//	Module 		: board_classic_othello.cpp
//	Created 	: 07.12.2004
//  Modified 	: 07.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Classic othello board implementation
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
	Memory::mem_fill		(m_board,DUMMY,sizeof(m_board));
	
	for (cell_index i=0; i<8; ++i)
		for (cell_index j=0; j<8; ++j)
			cell			(i,j,EMPTY);
	
	cell					(3,3,WHITE);
	cell					(3,4,BLACK);
	cell					(4,3,BLACK);
	cell					(4,4,WHITE);

	m_color_to_move			= BLACK;
	m_empties				= 60;
	m_difference			= 0;
	m_passed				= false;

	while (!m_flip_stack.empty())
		m_flip_stack.pop();
	m_flip_stack.c.reserve	(4096);
}

IC	void CBoardClassicOthello::show_cell		(const cell_index &index) const
{
	char			output;
	switch (cell(index)) {
		case WHITE : {
			output = 'o';
			break;
		}
		case BLACK : {
			output = 'x';
			break;
		}
		case EMPTY : {
			if (!can_move(index))
				output = '-';
			else
				output = '.';
			break;
		}
		default : NODEFAULT;
	}
	ui().log		(" %c",output);
}

IC	void CBoardClassicOthello::show_letters		() const
{
	ui().log		("  ");
	for (cell_index i=0; i<8; ++i)
		ui().log	(" %c",'A' + i);
	ui().log		("\n");
}

IC	void CBoardClassicOthello::show_digit		(const cell_index &index) const
{
	ui().log		(" %c",'1' + index);
}

void CBoardClassicOthello::show				() const
{
	show_letters		();
	for (cell_index i=0; i<8; ++i) {
		show_digit		(i);
		for (cell_index j=0; j<8; ++j)
			show_cell	(index(i,j));
		show_digit		(i);
		ui().log		("\n");
	}
	show_letters		();
}
