////////////////////////////////////////////////////////////////////////////
//	Module 		: problem_solver.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Problem solver
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "operator_abstract.h"
#include "condition_evaluator.h"

template <
	typename _condition_type,
	typename _value_type,
	typename _edge_value_type,
	typename _operator_id_type
>
class CProblemSolver {
public:
	typedef COperatorConditionAbstract<
		_condition_type,
		_value_type
	>											COperatorCondition;
	typedef COperatorAbstract<
		_condition_type,
		_value_type,
		_edge_value_type
	>											COperator;
	typedef CConditionState<
		_condition_type,
		_value_type
	>											CState;
	typedef IConditionEvaluator<_value_type>	CConditionEvaluator;
	typedef CState								_index_type;
	typedef _operator_id_type					_edge_type;
	struct SOperator {
		_operator_id_type	m_operator_id;
		COperator			*m_operator;

		IC					SOperator(const _operator_id_type &operator_id, COperator *_operator) :
								m_operator_id(operator_id),
								m_operator(_operator)
		{
		}

		bool				operator<(const _operator_id_type &operator_id) const
		{
			return			(m_operator_id < operator_id);
		}
	};
	typedef xr_vector<SOperator>									OPERATOR_VECTOR;
	typedef typename OPERATOR_VECTOR::const_iterator				const_iterator;
	typedef xr_map<_condition_type,CConditionEvaluator*>			EVALUATOR_MAP;


protected:
	OPERATOR_VECTOR				m_operators;
	xr_vector<_edge_type>		m_solution;
	EVALUATOR_MAP				m_evaluators;
	mutable CState				m_current_state;
	mutable CState				m_temp;
	mutable bool				m_applied;
	CState						m_target_state;
	_edge_type					m_current_operator;
	
public:
	IC									CProblemSolver			();
	virtual								~CProblemSolver			();
	// graph interface
	IC		u8							get_edge_weight			(const _index_type	&vertex_index0,	const _index_type &vertex_index1,	const const_iterator	&i) const;
	IC		bool						is_accessible			(const _index_type	&vertex_index) const;
	IC		const _index_type			&value					(const _index_type	&vertex_index,	const_iterator		&i) const;
	IC		void						begin					(const _index_type	&vertex_index,	const_iterator		&b,				const_iterator			&e) const;
	IC		bool						is_goal_reached			(const _index_type	&vertex_index) const;
	// operator interface
	IC		void						add_operator			(COperator			*_operator,		const _edge_type	&operator_id);
	IC		void						remove_operator			(const _edge_type	&operator_id);
	IC		const _edge_type			&current_operator		() const;
	IC		const OPERATOR_VECTOR		&operators				() const;
	IC		void						set_current_operator	(const _edge_type	&operator_id);
	// state interface
	IC		void						set_current_state		(const CState		&state);
	IC		void						set_target_state		(const CState		&state);
	IC		const CState				&current_state			() const;
	IC		const CState				&target_state			() const;
	// evaluator interface
	IC		void						add_evaluator			(const _condition_type &condition_id, CConditionEvaluator *evaluator);
	IC		void						remove_evaluator		(const _condition_type &condition_id);
	IC		const CConditionEvaluator	*evaluator				(const _condition_type &condition_id) const;
	IC		const EVALUATOR_MAP			&evaluators				() const;
	IC		void						evaluate_condition		(typename xr_vector<COperatorCondition>::const_iterator &I, typename xr_vector<COperatorCondition>::const_iterator &E, const _condition_type &condition_id) const;
	// solver interface
	IC		void						solve					();
};

#include "problem_solver_inline.h"