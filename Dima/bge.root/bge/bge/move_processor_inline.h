////////////////////////////////////////////////////////////////////////////
//	Module 		: move_processor_inline.h
//	Created 	: 30.12.2004
//  Modified 	: 30.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Move processor inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <typename move_type, int reserve_moves, int max_move_count> 
#define CSMoveProcessor			CMoveProcessor<move_type,reserve_moves,max_move_count>

TEMPLATE_SPECIALIZATION
IC	CSMoveProcessor::CMoveProcessor		()
{
	init						();
}

TEMPLATE_SPECIALIZATION
IC	void CSMoveProcessor::init			()
{
	m_position_moves.clear		();
	m_position_moves.resize		(max_move_count);
	m_current_position_moves	= m_position_moves.begin();
}

TEMPLATE_SPECIALIZATION
IC	void CSMoveProcessor::on_do_move	(const move_type &move)
{
	VERIFY								(m_position_moves.end() != m_current_position_moves);
	(*(++m_current_position_moves)).init();
}

TEMPLATE_SPECIALIZATION
IC	void CSMoveProcessor::on_undo_move	(const move_type &move)
{
	VERIFY						(m_position_moves.begin() != m_current_position_moves);
	--m_current_position_moves;
}

TEMPLATE_SPECIALIZATION
IC	typename CSMoveProcessor::CSPositionMoves &CSMoveProcessor::current_position_moves	()
{
	VERIFY						(m_position_moves.end() != m_current_position_moves);
	return						(*m_current_position_moves);
}

TEMPLATE_SPECIALIZATION
IC	typename CSMoveProcessor::move_container &CSMoveProcessor::moves	()
{
	VERIFY						(m_position_moves.end() != m_current_position_moves);
	return						((*m_current_position_moves).moves());
}

TEMPLATE_SPECIALIZATION
IC	bool CSMoveProcessor::actual	() const
{
	VERIFY						(m_position_moves.end() != m_current_position_moves);
	return						((*m_current_position_moves).actual());
}

#undef TEMPLATE_SPECIALIZATION
#undef CSMoveProcessor