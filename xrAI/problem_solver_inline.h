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
	typename _operator_condition,\
	typename _operator,\
	typename _condition_state,\
	typename _condition_evaluator,\
	typename _operator_id_type\
>

#define CProblemSolverAbstract \
	CProblemSolver<\
		_operator_condition,\
		_operator,\
		_condition_state,\
		_condition_evaluator,\
		_operator_id_type\
	>

TEMPLATE_SPECIALIZATION
IC	CProblemSolverAbstract::CProblemSolver				()
{
	init					();
}

TEMPLATE_SPECIALIZATION
CProblemSolverAbstract::~CProblemSolver					()
{
	while (!m_operators.empty())
		remove_operator		(m_operators.back().m_operator_id);

	while (!m_evaluators.empty())
		remove_evaluator	((*m_evaluators.begin()).first);
}

TEMPLATE_SPECIALIZATION
void CProblemSolverAbstract::init						()
{
}

TEMPLATE_SPECIALIZATION
void CProblemSolverAbstract::Load						(LPCSTR section)
{
}

TEMPLATE_SPECIALIZATION
void CProblemSolverAbstract::reinit						(bool clear_all)
{
	m_target_state.clear	();
	m_current_state.clear	();
	m_temp.clear			();
	m_solution.clear		();
	m_applied				= false;
	m_solution_changed		= false;

	if (!clear_all)
		return;

	while (!m_operators.empty())
		remove_operator		(m_operators.back().m_operator_id);

	while (!m_evaluators.empty())
		remove_evaluator	((*m_evaluators.begin()).first);
}

TEMPLATE_SPECIALIZATION
void CProblemSolverAbstract::reload						(LPCSTR section)
{
}

TEMPLATE_SPECIALIZATION
IC	bool CProblemSolverAbstract::actual			() const
{
	return				(m_actuality);
}

// operators
TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::add_operator			(const _edge_type &operator_id, COperator *_operator)
{
	OPERATOR_VECTOR::iterator	I = std::lower_bound(m_operators.begin(), m_operators.end(),operator_id);
	VERIFY					((I == m_operators.end()) || ((*I).m_operator_id != operator_id));
	m_actuality				= false;
	m_operators.insert		(I,SOperator(operator_id,_operator));
}

TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::remove_operator		(const _edge_type &operator_id)
{
	OPERATOR_VECTOR::iterator	I = std::lower_bound(m_operators.begin(), m_operators.end(),operator_id);
	VERIFY					(m_operators.end() != I);
	xr_delete				((*I).m_operator);
	m_actuality				= false;
	m_operators.erase		(I);
}

TEMPLATE_SPECIALIZATION
IC	const typename CProblemSolverAbstract::OPERATOR_VECTOR &CProblemSolverAbstract::operators	() const
{
	return					(m_operators);
}

// states
TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::set_target_state		(const CState &state)
{
	m_actuality				= m_actuality && (m_target_state == state);
	m_target_state			= state;
}

TEMPLATE_SPECIALIZATION
IC	const typename CProblemSolverAbstract::CState &CProblemSolverAbstract::current_state	() const
{
	return					(m_current_state);
}

TEMPLATE_SPECIALIZATION
IC	const typename CProblemSolverAbstract::CState &CProblemSolverAbstract::target_state	() const
{
	return					(m_target_state);
}

TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::add_evaluator				(const _condition_type &condition_id, CConditionEvaluator *evaluator)
{
	VERIFY						(m_evaluators.find(condition_id) == m_evaluators.end());
	m_evaluators.insert			(std::make_pair(condition_id,evaluator));
}

TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::remove_evaluator			(const _condition_type &condition_id)
{
	EVALUATOR_MAP::iterator		I = m_evaluators.find(condition_id);
	VERIFY						(I != m_evaluators.end());
	xr_delete					((*I).second);
	m_evaluators.erase			(I);
	m_actuality					= false;
}

TEMPLATE_SPECIALIZATION
IC	const typename CProblemSolverAbstract::CConditionEvaluator *CProblemSolverAbstract::evaluator	(const _condition_type &condition_id) const
{
	EVALUATOR_MAP::const_iterator	I = evaluators().find(condition_id);
	VERIFY							(evaluators().end() != I);
	return							((*I).second);
}

TEMPLATE_SPECIALIZATION
IC	const typename CProblemSolverAbstract::EVALUATOR_MAP &CProblemSolverAbstract::evaluators() const
{
	return						(m_evaluators);
}

TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::evaluate_condition			(typename xr_vector<COperatorCondition>::const_iterator &I, typename xr_vector<COperatorCondition>::const_iterator &E, const _condition_type &condition_id) const
{
	EVALUATOR_MAP::const_iterator	J = m_evaluators.find(condition_id);
	VERIFY							(J != m_evaluators.end());
	size_t							index = I - m_current_state.conditions().begin();
	m_current_state.add_condition	(I,COperatorCondition(condition_id,(*J).second->evaluate()));
	I								= m_current_state.conditions().begin() + index;
	E								= m_current_state.conditions().end();
}

TEMPLATE_SPECIALIZATION
IC	typename CProblemSolverAbstract::_edge_value_type CProblemSolverAbstract::get_edge_weight	(const _index_type &vertex_index0, const _index_type &vertex_index1, const const_iterator &i) const
{
	return					((*i).m_operator->weight(vertex_index1,vertex_index0));
}

TEMPLATE_SPECIALIZATION
IC	bool CProblemSolverAbstract::is_accessible	(const _index_type &vertex_index) const
{
	return					(m_applied);
}

TEMPLATE_SPECIALIZATION
IC	const typename CProblemSolverAbstract::_index_type &CProblemSolverAbstract::value(const _index_type &vertex_index, const_iterator &i) const
{
	if ((*i).m_operator->applicable((*i).m_operator->effects(),(*i).m_operator->conditions(),vertex_index))
		m_applied			= (*i).m_operator->apply_reverse(vertex_index,(*i).m_operator->effects(),m_temp,(*i).m_operator->conditions());
	else
		m_applied			= false;
	return					(m_temp);
}

TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::begin			(const _index_type &vertex_index, const_iterator &b, const_iterator &e) const
{
	b						= m_operators.begin();
	e						= m_operators.end();
}

TEMPLATE_SPECIALIZATION
IC	bool CProblemSolverAbstract::is_goal_reached(const _index_type &vertex_index) const
{
	xr_vector<COperatorCondition>::const_iterator	I = m_current_state.conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	E = m_current_state.conditions().end();
	xr_vector<COperatorCondition>::const_iterator	i = vertex_index.conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	e = vertex_index.conditions().end();
	for ( ; i != e; ) {
		if (I == E)
			evaluate_condition	(I,E,(*i).condition());
		if ((*I).condition() < (*i).condition())
			++I;
		else
			if ((*I).condition() > (*i).condition()) {
				evaluate_condition	(I,E,(*i).condition());
				if ((*I).value() != (*i).value())
					return		(false);
			}
			else
				if ((*I).value() != (*i).value())
					return		(false);
				else {
					++I;
					++i;
				}
	}
	return						(true);
}

TEMPLATE_SPECIALIZATION
IC	const xr_vector<typename CProblemSolverAbstract::_edge_type> &CProblemSolverAbstract::solution	() const
{
	return						(m_solution);
}

TEMPLATE_SPECIALIZATION
IC	typename CProblemSolverAbstract::COperator *CProblemSolverAbstract::get_operator (const _edge_type &operator_id)
{
	OPERATOR_VECTOR::iterator	I = std::lower_bound(m_operators.begin(), m_operators.end(),operator_id);
	VERIFY						(m_operators.end() != I);
	return						((*I).get_operator());
}

TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::solve			()
{
#ifndef AI_COMPILER
	m_solution_changed			= false;
	if (m_actuality) {
		bool			actual = true;
		xr_vector<COperatorCondition>::const_iterator	I = current_state().conditions().begin();
		xr_vector<COperatorCondition>::const_iterator	E = current_state().conditions().end();
		for ( ; I != E; ++I) {
			EVALUATOR_MAP::const_iterator J = evaluators().find((*I).condition());
			VERIFY			(evaluators().end() != J);
			if ((*J).second->evaluate() != (*I).value()) {
				actual	= false;
				break;
			}
		}
		if (actual)
			return;
	}

	m_solution_changed			= true;
	m_current_state.clear		();
	bool						successful = ai().graph_engine().search(*this,target_state(),CState(),&m_solution,CGraphEngine::CSolverBaseParameters());
	m_actuality					= successful;
#endif
}

#undef TEMPLATE_SPECIALIZATION
#undef CProblemSolverAbstract