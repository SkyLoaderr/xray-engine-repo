////////////////////////////////////////////////////////////////////////////
//	Module 		: othello_bit_board.h
//	Created 	: 24.12.2004
//  Modified 	: 24.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Othello bit board implementation inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	COthelloBitBoard::COthelloBitBoard	()
{
	start_position	();
}

IC	COthelloBitBoard::cell_index COthelloBitBoard::index (const cell_index &index0, const cell_index &index1) const
{
	return							(BOARD_START + index0*BOARD_LINE + index1);
}

IC	void COthelloBitBoard::index (const cell_index &index, cell_index &index0, cell_index &index1) const
{
	VERIFY							(index >= BOARD_START);
	VERIFY							(((index - BOARD_START) / BOARD_LINE) < 8);
	VERIFY							(((index - BOARD_START) % BOARD_LINE) < 8);
	index0							= (index - BOARD_START) / BOARD_LINE;
	index1							= (index - BOARD_START) % BOARD_LINE;
}

IC	void COthelloBitBoard::cell				(const cell_index &index, const cell_type &value)
{
	m_board[index]					= value;
}

IC	void COthelloBitBoard::cell				(const cell_index &index0, const cell_index &index1, const cell_type &value)
{
	m_board[index(index0,index1)]	= value;
}

IC	const COthelloBitBoard::cell_type &COthelloBitBoard::cell	(const cell_index &index) const
{
	return							(m_board[index]);
}

IC	const COthelloBitBoard::cell_type &COthelloBitBoard::cell	(const cell_index &index0, const cell_index &index1) const
{
	return							(m_board[index(index0,index1)]);
}

IC	const COthelloBitBoard::cell_type &COthelloBitBoard::color_to_move	() const
{
	return							(m_color_to_move);
}

IC	u8	 COthelloBitBoard::empties			() const
{
	return							(m_empties);
}

IC	int	 COthelloBitBoard::difference		() const
{
	return							(m_difference);
}

IC	bool COthelloBitBoard::passed			() const
{
	return							(m_passed);
}

IC	bool COthelloBitBoard::terminal_position	() const
{
	return							(m_passed && !m_flip_stack.empty() && m_flip_stack.top().m_passed);
}

template <COthelloBitBoard::cell_type opponent_color>
IC	void COthelloBitBoard::undo_move		()
{
	VERIFY								(!m_flip_stack.empty());

	CStackCell							&stack_cell = m_flip_stack.top();
	int									flip_count = (int)stack_cell.m_flip_count;
	m_passed							= stack_cell.m_passed;
	m_flip_stack.pop					();
	m_color_to_move						= opponent_color == BLACK ? WHITE : BLACK;
	
	if (flip_count) {
		++m_empties;
		VERIFY							(!m_flip_stack.empty());
		*m_flip_stack.top().m_cell		= EMPTY;
		m_flip_stack.pop				();
		VERIFY							(!m_flip_stack.empty());

		do {
			--flip_count;
			VERIFY						(!m_flip_stack.empty());
			*m_flip_stack.top().m_cell	= opponent_color;
			m_flip_stack.pop			();
		}
		while (flip_count);
	}
}

IC	void COthelloBitBoard::undo_move		()
{
	if (m_flip_stack.empty()) {
		ui().error_log				("Undo stack is empty!\n");
		return;
	}

	if (color_to_move() == BLACK)
		undo_move<BLACK>			();
	else
		undo_move<WHITE>			();
}

IC	void COthelloBitBoard::change_color		()
{
	m_color_to_move					= m_color_to_move == BLACK ? WHITE : BLACK;
}

IC	LPCSTR COthelloBitBoard::move_to_string	(const cell_index &index) const
{
	if (!index) {
		strcpy						(m_temp,"PS");
		return						(m_temp);
	}
	cell_index						index0, index1;
	this->index						(index,index0,index1);
	return							(move_to_string(index0,index1));
}

IC	LPCSTR COthelloBitBoard::move_to_string	(const cell_index &index0, const cell_index &index1) const
{
	if (
		(index0 < 0) ||
		(index0 > 7) ||
		(index1 < 0) ||
		(index1 > 7)
		)
		return						(move_to_string(0));
	m_temp[0]						= index1 + 'A';
	m_temp[1]						= index0 + '1';
	m_temp[2]						= 0;
	return							(m_temp);
}

IC	COthelloBitBoard::cell_index COthelloBitBoard::string_to_move	(LPCSTR move) const
{
	string256						temp;
	xr_strcpy						(temp,move);
	strupr							(temp);
	if (!xr_strcmp(temp,"PASS") || !xr_strcmp(temp,"PS"))
		return						(0);

	if (xr_strlen(move) != 2) {
		ui().error_log				("Move \"%s\" is invalid!\n",move);
		return						(0);
	}
	cell_index						index0 = temp[1] - '1';
	cell_index						index1 = temp[0] - 'A';
	if (
		(index0 < 0) ||
		(index0 > 7) ||
		(index1 < 0) ||
		(index1 > 7)
		) {
		ui().error_log				("Move \"%s\" is invalid!\n",move);
		return						(0);
	}
	return							(index(index0,index1));
}

IC	void COthelloBitBoard::do_move	(LPCSTR move)
{
	cell_index						index = string_to_move(move);

	if (!index) {
		if (can_move()) {
			ui().error_log			("Move \"%s\" is invalid since there are possible moves!\n",move);
			return;
		}
		do_move						(cell_index(0));
	}
	
	if (cell(index) != EMPTY) {
		ui().error_log				("Move \"%s\" is invalid!\n",move);
		return;
	}

	if (!can_move(index)) {
		ui().error_log				("Move \"%s\" is invalid!\n",move);
		return;
	}
	
	do_move							(index);
}

IC	bool COthelloBitBoard::can_move	(LPCSTR move) const
{
	cell_index						index = string_to_move(move);

	if (!index) {
		if (!can_move())
			return					(true);
		return						(false);
	}

	if (cell(index) != EMPTY) {
		ui().error_log				("Move \"%s\" is invalid!\n",move);
		return						(false);
	}

	return							(can_move(index));
}

IC	int	 COthelloBitBoard::compute_difference	(LPCSTR move) const
{
	cell_index						index = string_to_move(move);
	
	if (!index) {
		if (!can_move())
			return					(-difference());
		ui().error_log				("Move \"%s\" is invalid since there are possible moves!\n",move);
		return						(difference());
	}
	
	if (cell(index) != EMPTY) {
		ui().error_log				("Move \"%s\" is invalid!\n",move);
		return						(difference());
	}

	if (!can_move(index)) {
		ui().error_log				("Move \"%s\" is invalid!\n",move);
		return						(difference());
	}
	
	return							(compute_difference(index));
}

IC	void COthelloBitBoard::do_move			(const cell_index &index0, const cell_index &index1)
{
	do_move							(move_to_string(index0,index1));
}

IC	bool COthelloBitBoard::can_move			(const cell_index &index0, const cell_index &index1) const
{
	return							(can_move(move_to_string(index0,index1)));
}

IC	int	 COthelloBitBoard::compute_difference	(const cell_index &index0, const cell_index &index1) const
{
	return							(compute_difference(move_to_string(index0,index1)));
}
