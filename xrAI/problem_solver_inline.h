////////////////////////////////////////////////////////////////////////////
//	Module 		: problem_solver_inline.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Problem solver inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_engine.h"

#define TEMPLATE_SPECIALIZATION template<\
	typename _condition_type,\
	typename _value_type,\
	typename _operator_id_type\
>

#define CProblemSolverAbstract CProblemSolver<_condition_type,_value_type,_operator_id_type>

TEMPLATE_SPECIALIZATION
IC	CProblemSolverAbstract::CProblemSolver				()
{
}

TEMPLATE_SPECIALIZATION
CProblemSolverAbstract::~CProblemSolver					()
{
	while (!m_operators.empty())
		remove_operator((*m_operators.begin()).first);
}

TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::add_operator			(COperator *_operator, const _edge_type &operator_id)
{
	m_operators.insert		(std::make_pair(operator_id,_operator));
}

TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::remove_operator		(const _edge_type &operator_id)
{
	OPERATOR_MAP::iterator	I = m_operators.find(operator_id);
	VERIFY					(m_operators.end() != I);
	xr_delete				((*I).second);
	m_operators.erase		(I);
}

TEMPLATE_SPECIALIZATION
IC	const typename CProblemSolverAbstract::_edge_type &CProblemSolverAbstract::current_operator	() const
{
	return					(m_current_operator);
}

TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::set_current_state		(const CState &state)
{
	m_current_state			= state;
}

TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::set_target_state		(const CState &state)
{
	m_target_state			= state;
}

TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::set_current_operator	(const _edge_type &operator_id)
{
	xr_map<_edge_type,COperator>::const_iterator	I = m_operators.find(operator_id);
	VERIFY					(m_operators.end() != E);
	m_current_operator		operator_id;
}

TEMPLATE_SPECIALIZATION
IC	const typename CProblemSolverAbstract::CState &CProblemSolverAbstract::current_state	() const
{
	return			(m_current_state);
}

TEMPLATE_SPECIALIZATION
IC	const typename CProblemSolverAbstract::CState &CProblemSolverAbstract::target_state	() const
{
	return			(m_target_state);
}

TEMPLATE_SPECIALIZATION
IC	u8	CProblemSolverAbstract::get_edge_weight	(const _index_type &vertex_index0, const _index_type vertex_index1, const const_iterator &i) const
{
	return				(vertex_index0.weight(vertex_index1));
}

TEMPLATE_SPECIALIZATION
IC	bool CProblemSolverAbstract::is_accessible	(const _index_type &vertex_index) const
{
	return				(!vertex_index.conditions().empty());
}

TEMPLATE_SPECIALIZATION
IC	const typename CProblemSolverAbstract::_index_type &CProblemSolverAbstract::value(const _index_type &vertex_index, const_iterator &i) const
{
	if ((*i).second->applicable(vertex_index))
		return			((*i).second->apply(vertex_index,m_temp));
	else {
		m_temp.clear	();
		return			(m_temp);
	}
}

TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::begin			(const _index_type &vertex_index, const_iterator &b, const_iterator &e) const
{
	b					= m_operators.begin();
	e					= m_operators.end();
}

TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::solve			()
{
}

TEMPLATE_SPECIALIZATION
IC	const typename CProblemSolverAbstract::OPERATOR_MAP &CProblemSolverAbstract::operators	() const
{
	return				(m_operators);
}

#undef TEMPLATE_SPECIALIZATION
#undef CProblemSolverAbstract