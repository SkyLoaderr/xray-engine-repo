////////////////////////////////////////////////////////////////////////////
//	Module 		: board_classic_othello_inline.h
//	Created 	: 07.12.2004
//  Modified 	: 07.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Calssic othello board implementation inline functions
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
