////////////////////////////////////////////////////////////////////////////
//	Module 		: othello_classic_board.cpp
//	Created 	: 07.12.2004
//  Modified 	: 07.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Othello classic board implementation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "othello_classic_board.h"

const u8 COthelloClassicBoard::flipping_directions[BOARD_SIZE] = {
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

void COthelloClassicBoard::start_position	()
{
	Memory::mem_fill		(m_board,DUMMY,sizeof(m_board));

//	m_free_cells.clear		();

	for (cell_index i=0; i<8; ++i)
		for (cell_index j=0; j<8; ++j) {
//			m_free_cells.insert(index(i,j));
			cell			(i,j,EMPTY);
		}
	
	cell					(3,3,WHITE);
//	m_free_cells.erase		(index(3,3));

	cell					(3,4,BLACK);
//	m_free_cells.erase		(index(3,4));

	cell					(4,3,BLACK);
//	m_free_cells.erase		(index(4,3));

	cell					(4,4,WHITE);
//	m_free_cells.erase		(index(4,4));

	m_color_to_move			= BLACK;
	m_empties				= 60;
	m_difference			= 0;
	m_passed				= false;

	m_move_processor.init	();

	while (!m_flip_stack.empty())
		m_flip_stack.pop();
	m_flip_stack.c.reserve	(4096);
}

IC	void COthelloClassicBoard::show_cell		(const cell_index &index) const
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

IC	int	COthelloClassicBoard::move_cell(const cell_index &index) const
{
	flip_stack		temp = m_flip_stack;
	for (int j= 60 - empties(); !temp.empty(); --j) {
		int			n = temp.top().m_flip_count;
		
		temp.pop	();
		
		if (!n) {
			++j;
			continue;
		}

		if (index == (temp.top().m_cell - m_board))
			return	(j);

		temp.pop	();
		
		for (int i=0; i<n; ++i)
			temp.pop();
	}
	return			(0);
}

IC	void COthelloClassicBoard::show_move_cell	(const cell_index &index) const
{
	string16	output;
	switch (cell(index)) {
		case BLACK :
		case WHITE : {
			int		id = move_cell(index);
			if (id)
				sprintf	(output,"|%2d",id);
			else
				strcpy	(output,"|##");
			break;
		}
		case EMPTY : {
			strcpy		(output,"|--");
			break;
		}
		default : NODEFAULT;
	}
	ui().log		("%s",output);
}

IC	void COthelloClassicBoard::show_letters		() const
{
	ui().log		("  ");
	for (cell_index i=0; i<8; ++i)
		ui().log	("%2c",'A' + i);
	
	int				count = ui().display_width() - (20 + 20 + 8);
	Memory::mem_fill(m_temp,' ',count*sizeof(char));
	m_temp[count]	= 0;
	ui().log		(m_temp);
	
	ui().log		("  ");

	ui().log		("  ");
	for (cell_index i=0; i<8; ++i)
		ui().log	("%3c",'A' + i);
	
	ui().log		("\n");
}

IC	void COthelloClassicBoard::show_digit		(const cell_index &index) const
{
	ui().log		("%2c",'1' + index);
}

IC	void COthelloClassicBoard::show_move_digit	(const cell_index &index) const
{
	ui().log		("|%c",'1' + index);
}

void COthelloClassicBoard::show				() const
{
	show_letters		();
	for (cell_index i=0; i<8; ++i) {
		show_digit		(i);
		for (cell_index j=0; j<8; ++j)
			show_cell	(index(i,j));
		show_digit		(i);

		ui().log		(m_temp);

		show_digit		(i);
		for (cell_index j=0; j<8; ++j)
			show_move_cell	(index(i,j));
		show_move_digit	(i);
		
		ui().log		("\n");
	}
	show_letters		();
}
