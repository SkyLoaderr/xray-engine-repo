////////////////////////////////////////////////////////////////////////////
//	Module 		: position_move_inline.h
//	Created 	: 30.12.2004
//  Modified 	: 30.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Position move inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <typename move_type, int reserve_moves> 
#define CSPositionMoves			CPositionMoves<move_type,reserve_moves>

TEMPLATE_SPECIALIZATION
IC	CSPositionMoves::CPositionMoves			()
{
	init				();
	m_moves.reserve		(reserve_moves);
}

TEMPLATE_SPECIALIZATION
IC	void CSPositionMoves::init				()
{
	m_actual			= false;
	m_moves.clear		();
}

TEMPLATE_SPECIALIZATION
IC	void CSPositionMoves::add				(const move_type &move)
{
	VERIFY				(std::find(m_moves.begin(),m_moves.end(),move) == m_moves.end());
	m_moves.push_back	(move);
}

TEMPLATE_SPECIALIZATION
IC	void CSPositionMoves::actual			(bool value)
{
	VERIFY				(!m_actual);
	m_actual			= true;
}

TEMPLATE_SPECIALIZATION
IC	bool CSPositionMoves::actual			() const
{
	return				(m_actual);
}

TEMPLATE_SPECIALIZATION
IC	typename CSPositionMoves::position_moves &CSPositionMoves::moves	()
{
	VERIFY				(actual());
	return				(m_moves);
}

#undef TEMPLATE_SPECIALIZATION
#undef CSPositionMoves