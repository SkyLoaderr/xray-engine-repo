////////////////////////////////////////////////////////////////////////////
//	Module 		: move_processor.h
//	Created 	: 30.12.2004
//  Modified 	: 30.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Move processor
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "position_moves.h"

template <
	typename move_type,
	int reserve_moves,
	int max_move_count
>
class CMoveProcessor {
public:
	typedef CPositionMoves<move_type,reserve_moves>			CSPositionMoves;
	typedef typename CSPositionMoves::position_moves		move_container;
	typedef typename move_container::iterator				move_iterator;
	typedef typename move_container::const_iterator			const_move_iterator;
	typedef typename move_container::reverse_iterator		reverse_move_iterator;
	typedef typename move_container::const_reverse_iterator	const_reverse_move_iterator;

public:
	typedef std::vector<CSPositionMoves>					game_moves;
	typedef typename game_moves::iterator					position_move_iterator;

protected:
	game_moves				m_position_moves;
	position_move_iterator	m_current_position_moves;

public:
	IC						CMoveProcessor			();
	IC		void			init					();
	IC		void			on_do_move				();
	IC		void			on_undo_move			();
	IC		CSPositionMoves	&current_position_moves	();
	IC		move_container	&moves					();
	IC		bool			actual					() const;
};

#include "move_processor_inline.h"