////////////////////////////////////////////////////////////////////////////
//	Module 		: mini_max_inline.h
//	Created 	: 25.12.2004
//  Modified 	: 25.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Mini-max algorithm implementation inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <typename board_type, typename evaluator_type>
#define _minimax				mini_max<board_type,evaluator_type>

TEMPLATE_SPECIALIZATION
IC	_minimax::mini_max					(board_type *board, evaluator_type *evaluator)
{
	m_board		= board;
	m_evaluator	= evaluator;
}

TEMPLATE_SPECIALIZATION
IC	board_type &_minimax::board			() const
{
	VERIFY		(m_board);
	return		(*m_board);
}

IC	evaluator_type &_minimax::evaluator	() const
{
	VERIFY		(m_evaluator);
	return		(*m_evaluator);
}

TEMPLATE_SPECIALIZATION
IC	board::score_type _minimax::search	(int depth)
{
	if (board().terminal_position())
		return						(board().score());

	if (!depth)
		return						(evaluator().evaluate(board()));

	evaluator_type::result_type		result = -evaluator_type::infinity, current;

	board_type::iterator			I = board().moves().begin();
	board_type::iterator			E = board().moves().end();
	for ( ; I != E; ++I) {
		board().move				(*I);

		current						= search(depth - 1);
		if (current > result)
			result					= current;

		board().undo				();
	}

	return							(result);
}

#undef TEMPLATE_SPECIALIZATION
#undef _minimax