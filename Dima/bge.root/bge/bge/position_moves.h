////////////////////////////////////////////////////////////////////////////
//	Module 		: position_moves.h
//	Created 	: 30.12.2004
//  Modified 	: 30.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Position move
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename move_type,
	int reserve_moves
>
class CPositionMoves {
public:
	typedef move_type				move_type;

public:
	typedef std::vector<move_type>	position_moves;

private:
	position_moves			m_moves;
	bool					m_actual;

public:
	IC						CPositionMoves	();
	IC		void			init			();
	IC		void			add				(const move_type &move);
	IC		void			actual			(bool value);
	IC		bool			actual			() const;
	IC		position_moves	&moves			();
};

#include "position_moves_inline.h"