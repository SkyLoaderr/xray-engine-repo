////////////////////////////////////////////////////////////////////////////
//	Module 		: problem_solver_inline.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Problem solver inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template<\
	typename _operator_condition,\
	typename _operator,\
	typename _condition_state,\
	typename _condition_evaluator,\
	typename _operator_id_type,\
	bool	 _reverse_search,\
	typename _operator_ptr,\
	typename _condition_evaluator_ptr\
>

#define CProblemSolverAbstract \
	CProblemSolver<\
		_operator_condition,\
		_operator,\
		_condition_state,\
		_condition_evaluator,\
		_operator_id_type,\
		_reverse_search,\
		_operator_ptr,\
		_condition_evaluator_ptr\
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
	m_actuality				= true;
	m_failed				= false;

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
IC	void CProblemSolverAbstract::add_operator			(const _edge_type &operator_id, _operator_ptr _operator)
{
	OPERATOR_VECTOR::iterator	I = std::lower_bound(m_operators.begin(), m_operators.end(),operator_id);
	VERIFY						((I == m_operators.end()) || ((*I).m_operator_id != operator_id));
#ifdef DEBUG
	validate_properties			(_operator->conditions());
	validate_properties			(_operator->effects());
#endif
	m_actuality					= false;
	m_operators.insert			(I,SOperator(operator_id,_operator));
}

#ifdef DEBUG
TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::validate_properties	(const CState &conditions) const
{
	xr_vector<COperatorCondition>::const_iterator	I = conditions.conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	E = conditions.conditions().end();
	for ( ; I != E; ++I) {
		VERIFY					(evaluators().find((*I).condition()) != evaluators().end());
	}
}
#endif

TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::remove_operator		(const _edge_type &operator_id)
{
	OPERATOR_VECTOR::iterator	I = std::lower_bound(m_operators.begin(), m_operators.end(),operator_id);
	VERIFY						(m_operators.end() != I);
	delete_data					((*I).m_operator);
	m_actuality					= false;
	m_operators.erase			(I);
}

TEMPLATE_SPECIALIZATION
IC	const typename CProblemSolverAbstract::OPERATOR_VECTOR &CProblemSolverAbstract::operators	() const
{
	return						(m_operators);
}

// states
TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::set_target_state		(const CState &state)
{
	m_actuality					= m_actuality && (m_target_state == state);
	m_target_state				= state;
}

TEMPLATE_SPECIALIZATION
IC	const typename CProblemSolverAbstract::CState &CProblemSolverAbstract::current_state	() const
{
	return						(m_current_state);
}

TEMPLATE_SPECIALIZATION
IC	const typename CProblemSolverAbstract::CState &CProblemSolverAbstract::target_state	() const
{
	return						(m_target_state);
}

TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::add_evaluator				(const _condition_type &condition_id, _condition_evaluator_ptr evaluator)
{
	VERIFY						(m_evaluators.find(condition_id) == m_evaluators.end());
	m_evaluators.insert			(std::make_pair(condition_id,evaluator));
}

TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::remove_evaluator			(const _condition_type &condition_id)
{
	EVALUATOR_MAP::iterator		I = m_evaluators.find(condition_id);
	VERIFY						(I != m_evaluators.end());
	delete_data					((*I).second);
	m_evaluators.erase			(I);
	m_actuality					= false;
}

TEMPLATE_SPECIALIZATION
IC	typename CProblemSolverAbstract::_condition_evaluator_ptr CProblemSolverAbstract::evaluator	(const _condition_type &condition_id) const
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
	_edge_value_type		current, min;
	current					= (*i).m_operator->weight(vertex_index1,vertex_index0);
	min						= (*i).m_operator->min_weight();
	VERIFY					(current >= min);
	return					(current);
}

TEMPLATE_SPECIALIZATION
IC	bool CProblemSolverAbstract::is_accessible	(const _index_type &vertex_index) const
{
	return					(m_applied);
}

TEMPLATE_SPECIALIZATION
IC	const typename CProblemSolverAbstract::_index_type &CProblemSolverAbstract::value(const _index_type &vertex_index, const_iterator &i, bool reverse_search) const
{
	if (reverse_search) {
		if ((*i).m_operator->applicable_reverse((*i).m_operator->effects(),(*i).m_operator->conditions(),vertex_index))
			m_applied			= (*i).m_operator->apply_reverse(vertex_index,(*i).m_operator->effects(),m_temp,(*i).m_operator->conditions());
		else
			m_applied			= false;
	}
	else {
		if ((*i).m_operator->applicable(vertex_index,current_state(),(*i).m_operator->conditions(),*this)) {
			(*i).m_operator->apply(vertex_index,(*i).m_operator->effects(),m_temp,m_current_state,*this);
			m_applied			= true;
		}
		else
			m_applied			= false;
	}
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
	return					(is_goal_reached_impl<reverse_search>(vertex_index));
}

TEMPLATE_SPECIALIZATION
IC	bool CProblemSolverAbstract::is_goal_reached_impl (const _index_type &vertex_index) const
{
	STATIC_CHECK				(!reverse_search,This_function_cannot_be_used_in_the_REVERSE_search);
	xr_vector<COperatorCondition>::const_iterator	I = vertex_index.conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	E = vertex_index.conditions().end();
	xr_vector<COperatorCondition>::const_iterator	i = target_state().conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	e = target_state().conditions().end();
	xr_vector<COperatorCondition>::const_iterator	II = current_state().conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	EE = current_state().conditions().end();
	for ( ; (i != e) && (I != E); ) {
		if ((*I).condition() < (*i).condition()) {
			++I;
		}
		else
			if ((*I).condition() > (*i).condition()) {
				for ( ; (II != EE) && ((*II).condition() < (*i).condition()); )
					++II;
				if ((II == EE) || ((*II).condition() > (*i).condition()))
					evaluate_condition(II,EE,(*i).condition());
				if ((*II).value() != (*i).value())
					return	(false);
				++II;
				++i;
			}
			else {
				if ((*I).value() != (*i).value())
					return		(false);
				++I;
				++i;
			}
	}

	if (I == E) {
		I = II;
		E = EE;
	}
	else
		return	(true);

	for ( ; i != e; ) {
		if ((I == E) || ((*I).condition() > (*i).condition()))
			evaluate_condition(I,E,(*i).condition());

		if ((*I).condition() < (*i).condition())
			++I;
		else {
			VERIFY	((*I).condition() == (*i).condition());
			if ((*I).value() != (*i).value())
				return	(false);
			++I;
			++i;
		}
	}
	return		(true);
}

TEMPLATE_SPECIALIZATION
IC	bool CProblemSolverAbstract::is_goal_reached_impl(const _index_type &vertex_index, bool) const
{
	STATIC_CHECK				(reverse_search,This_function_cannot_be_used_in_the_STRAIGHT_search);
	xr_vector<COperatorCondition>::const_iterator	I = m_current_state.conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	E = m_current_state.conditions().end();
	xr_vector<COperatorCondition>::const_iterator	i = vertex_index.conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	e = vertex_index.conditions().end();
	for ( ; i != e; ) {
		if ((I == E) || ((*I).condition() > (*i).condition()))
			evaluate_condition	(I,E,(*i).condition());

		if ((*I).condition() < (*i).condition())
			++I;
		else {
			if ((*I).value() != (*i).value())
				return		(false);
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
IC	typename CProblemSolverAbstract::_operator_ptr CProblemSolverAbstract::get_operator (const _edge_type &operator_id)
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
		bool					actual = true;
		xr_vector<COperatorCondition>::const_iterator	I = current_state().conditions().begin();
		xr_vector<COperatorCondition>::const_iterator	E = current_state().conditions().end();
		for ( ; I != E; ++I) {
			EVALUATOR_MAP::const_iterator J = evaluators().find((*I).condition());
			VERIFY				(evaluators().end() != J);
			if ((*J).second->evaluate() != (*I).value()) {
				actual			= false;
				break;
			}
		}
		if (actual)
			return;
	}

	m_actuality					= true;
	m_solution_changed			= true;
	m_current_state.clear		();
	bool						successful = 
		!reverse_search ? 
		ai().graph_engine().search(*this,current_state(),target_state(),&m_solution,CGraphEngine::CSolverBaseParameters(CGraphEngine::_solver_dist_type(-1),CGraphEngine::_solver_condition_type(-1),8000))
		:
		ai().graph_engine().search(*this,target_state(),current_state(),&m_solution,CGraphEngine::CSolverBaseParameters(CGraphEngine::_solver_dist_type(-1),CGraphEngine::_solver_condition_type(-1),8000));
	m_failed					= !successful;
#endif
}

TEMPLATE_SPECIALIZATION
IC	typename CProblemSolverAbstract::_edge_value_type CProblemSolverAbstract::estimate_edge_weight	(const _index_type &condition) const
{
	return						(estimate_edge_weight_impl<reverse_search>(condition));
}

TEMPLATE_SPECIALIZATION
IC	typename CProblemSolverAbstract::_edge_value_type CProblemSolverAbstract::estimate_edge_weight_impl	(const _index_type &condition) const
{
	STATIC_CHECK				(!reverse_search,This_function_cannot_be_used_in_the_REVERSE_search);
	_edge_value_type			result = 0;
	xr_vector<COperatorCondition>::const_iterator	I = target_state().conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	E = target_state().conditions().end();
	xr_vector<COperatorCondition>::const_iterator	i = condition.conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	e = condition.conditions().end();
	for ( ; (I != E) && (i != e); )
		if ((*I).condition() < (*i).condition()) {
			++result;
			++I;
		}
		else
			if ((*I).condition() > (*i).condition())
				++i;
			else {
				if ((*I).value() != (*i).value())
					++result;
				++I;
				++i;
			}
	return					(result + _edge_value_type(E - I));
}

TEMPLATE_SPECIALIZATION
IC	typename CProblemSolverAbstract::_edge_value_type CProblemSolverAbstract::estimate_edge_weight_impl	(const _index_type &condition, bool) const
{
	STATIC_CHECK				(reverse_search,This_function_cannot_be_used_in_the_STRAIGHT_search);
	_edge_value_type			result = 0;
	xr_vector<COperatorCondition>::const_iterator	I = current_state().conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	E = current_state().conditions().end();
	xr_vector<COperatorCondition>::const_iterator	i = condition.conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	e = condition.conditions().end();
	for ( ; (i != e); ) {
		if ((I == E) || ((*I).condition() > (*i).condition()))
			evaluate_condition	(I,E,(*i).condition());

		if ((*I).condition() < (*i).condition())
			++I;
		else {
			VERIFY	((*I).condition() == (*i).condition());
			if ((*I).value() != (*i).value())
				++result;
			++I;
			++i;
		}
	}
	return					(result);
}

#undef TEMPLATE_SPECIALIZATION
#undef CProblemSolverAbstract