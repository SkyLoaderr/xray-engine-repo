////////////////////////////////////////////////////////////////////////////
//	Module 		: mini_max.h
//	Created 	: 25.12.2004
//  Modified 	: 25.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Mini-max algorithm implementation
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename board_type,
	typename evaluator_type
>
class mini_max {
public:
	typedef board_type		board_type;
	typedef evaluator_type	evaluator_type;

private:
	board_type				*m_board;
	evaluator_type			*m_evaluator;

public:
	IC						mini_max	(board_type *board, evaluator_type *evaluator);
	IC	board_type			&board		() const;
	IC	evaluator_type		&evaluator	() const;
	IC	board::score_type	search		();
};

#include "mini_max_inline.h"