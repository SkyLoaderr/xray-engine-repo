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
	VERIFY							(m_current_flip > m_flip_stack);
	
	--m_current_flip;
	int								flip_count = (int)reinterpret_cast<size_t>(*m_current_flip);

	VERIFY							(m_current_flip > m_flip_stack);
	**(--m_current_flip)			= EMPTY;

	for ( ; flip_count; ) {
		--flip_count;
        **(--m_current_flip)		= opponent_color;
		VERIFY						(m_current_flip >= m_flip_stack);
    }

	m_color_to_move					= opponent_color == BLACK ? WHITE : BLACK;
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
