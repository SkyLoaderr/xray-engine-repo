////////////////////////////////////////////////////////////////////////////
//	Module 		: problem_solver.h
//	Created 	: 24.02.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Problem solver
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "operator_abstract.h"

template <
	typename _operator_condition,
	typename _condition_state,
	typename _operator,
	typename _condition_evaluator,
	typename _operator_id_type
>
class CProblemSolver {
public:
	typedef _operator_condition								COperatorCondition;
	typedef _operator										COperator;
	typedef _condition_state								CState;
	typedef _condition_evaluator							CConditionEvaluator;
	typedef typename _operator_condition::_condition_type	_condition_type;
	typedef typename _operator_condition::_value_type		_value_type;
	typedef typename _operator::_edge_value_type			_edge_value_type;
	typedef CState											_index_type;
	typedef _operator_id_type								_edge_type;

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

		COperator			*get_operator()
		{
			return			(m_operator);
		}
	};
	typedef xr_vector<SOperator>									OPERATOR_VECTOR;
	typedef typename OPERATOR_VECTOR::const_iterator				const_iterator;
	typedef xr_map<_condition_type,CConditionEvaluator*>			EVALUATOR_MAP;


protected:
	OPERATOR_VECTOR				m_operators;
	EVALUATOR_MAP				m_evaluators;
	xr_vector<_edge_type>		m_solution;
	CState						m_target_state;
	mutable CState				m_current_state;
	mutable CState				m_temp;
	mutable bool				m_applied;
	bool						m_actuality;
	bool						m_solution_changed;

public:
	// common interface
	IC									CProblemSolver			();
	virtual								~CProblemSolver			();
			void						init					();
	virtual	void						Load					(LPCSTR section);
	virtual void						reinit					(bool clear_all);
	virtual void						reload					(LPCSTR section);
	IC		bool						actual					() const;

	// graph interface
	IC		_edge_value_type			get_edge_weight			(const _index_type	&vertex_index0,	const _index_type &vertex_index1,	const const_iterator	&i) const;
	IC		bool						is_accessible			(const _index_type	&vertex_index) const;
	IC		const _index_type			&value					(const _index_type	&vertex_index,	const_iterator		&i) const;
	IC		void						begin					(const _index_type	&vertex_index,	const_iterator		&b,				const_iterator			&e) const;
	IC		bool						is_goal_reached			(const _index_type	&vertex_index) const;

	// operator interface
	IC		void						add_operator			(const _edge_type	&operator_id,	COperator			*_operator);
	IC		void						remove_operator			(const _edge_type	&operator_id);
	IC		COperator					*get_operator			(const _operator_id_type &operator_id);
	IC		const OPERATOR_VECTOR		&operators				() const;

	// state interface
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
	IC		const xr_vector<_edge_type>	&solution				() const;
};

#include "problem_solver_inline.h"