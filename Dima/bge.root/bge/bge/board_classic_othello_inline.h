////////////////////////////////////////////////////////////////////////////
//	Module 		: board_classic_othello_inline.h
//	Created 	: 07.12.2004
//  Modified 	: 07.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Classic othello board implementation inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CBoardClassicOthello::CBoardClassicOthello	()
{
	start_position	();
}

IC	CBoardClassicOthello::cell_index CBoardClassicOthello::index (const cell_index &index0, const cell_index &index1) const
{
	return							(BOARD_START + index0*BOARD_LINE + index1);
}

IC	void CBoardClassicOthello::cell				(const cell_index &index, const cell_type &value)
{
	m_board[index]					= value;
}

IC	void CBoardClassicOthello::cell				(const cell_index &index0, const cell_index &index1, const cell_type &value)
{
	m_board[index(index0,index1)]	= value;
}

IC	const CBoardClassicOthello::cell_type &CBoardClassicOthello::cell	(const cell_index &index) const
{
	return							(m_board[index]);
}

IC	const CBoardClassicOthello::cell_type &CBoardClassicOthello::cell	(const cell_index &index0, const cell_index &index1) const
{
	return							(m_board[index(index0,index1)]);
}

IC	bool CBoardClassicOthello::do_move			(const cell_index &index0, const cell_index &index1)
{
	return							(do_move(index(index0,index1)));
}

IC	const CBoardClassicOthello::cell_type &CBoardClassicOthello::color_to_move	() const
{
	return							(m_color_to_move);
}

IC	u8	 CBoardClassicOthello::empties			() const
{
	return							(m_empties);
}

IC	int	 CBoardClassicOthello::difference		() const
{
	return							(m_difference);
}

IC	bool CBoardClassicOthello::passed			() const
{
	return							(m_passed);
}

template <CBoardClassicOthello::cell_type opponent_color>
IC	void CBoardClassicOthello::undo_move		()
{
	VERIFY								(!m_flip_stack.empty());

	CStackCell							&stack_cell = m_flip_stack.top();
	int									flip_count = (int)stack_cell.m_flip_count;
	m_passed							= stack_cell.m_passed;
	m_flip_stack.pop					();
	m_color_to_move						= opponent_color == BLACK ? WHITE : BLACK;
	
	if (flip_count) {
		VERIFY							(!m_flip_stack.empty());
		*m_flip_stack.top().m_cell		= EMPTY;
		m_flip_stack.pop				();

		for ( ; flip_count; ) {
			--flip_count;
			VERIFY						(!m_flip_stack.empty());
			*m_flip_stack.top().m_cell	= opponent_color;
			m_flip_stack.pop			();
		}
	}
}

IC	void CBoardClassicOthello::undo_move		()
{
	if (color_to_move() == BLACK)
		undo_move<BLACK>			();
	else
		undo_move<WHITE>			();
}

IC	bool CBoardClassicOthello::can_move			(const cell_index &index0, const cell_index &index1) const
{
	return							(can_move(index(index0,index1)));
}

IC	int	 CBoardClassicOthello::compute_difference	(const cell_index &index0, const cell_index &index1) const
{
	return							(compute_difference(index(index0,index1)));
}

IC	void CBoardClassicOthello::change_color		()
{
	m_color_to_move					= m_color_to_move == BLACK ? WHITE : BLACK;
}