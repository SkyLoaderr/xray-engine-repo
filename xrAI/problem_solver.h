////////////////////////////////////////////////////////////////////////////
//	Module 		: problem_solver.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Problem solver
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "operator_abstract.h"

//#define INTENSIVE_MEMORY_USAGE

template <
	typename _condition_type,
	typename _value_type,
	typename _operator_id_type
>
class CProblemSolver {
public:
	typedef COperatorConditionAbstract<_condition_type,_value_type>	COperatorCondition;
	typedef COperatorAbstract<_condition_type,_value_type>			COperator;
	typedef CConditionState<_condition_type,_value_type>			CState;
	typedef CState													_index_type;
	typedef _operator_id_type										_edge_type;
//	typedef xr_map<_operator_id_type,COperator*>					OPERATOR_MAP;
//	typedef typename OPERATOR_MAP::const_iterator					const_iterator;
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

protected:
	OPERATOR_VECTOR				m_operators;
	CState						m_current_state;
	CState						m_target_state;
	_edge_type					m_current_operator;
	xr_vector<_edge_type>		m_plan;
	mutable CState				m_temp;
	
public:
	IC							CProblemSolver		();
	virtual						~CProblemSolver		();
	IC		void				add_operator		(COperator			*_operator,		const _edge_type	&operator_id);
	IC		void				remove_operator		(const _edge_type	&operator_id);
	IC		const _edge_type	&current_operator	() const;
	IC		void				set_current_state	(const CState		&state);
	IC		void				set_target_state	(const CState		&state);
	IC		void				set_current_operator(const _edge_type	&operator_id);
	IC		void				solve				();
	IC		const CState		&current_state		() const;
	IC		const CState		&target_state		() const;
	IC		const OPERATOR_VECTOR &operators		() const;
	// graph interface
	IC		u8					get_edge_weight		(const _index_type	&vertex_index0,	const _index_type &vertex_index1,	const const_iterator	&i) const;
	IC		bool				is_accessible		(const _index_type	&vertex_index) const;
	IC		const _index_type	&value				(const _index_type	&vertex_index,	const_iterator		&i) const;
	IC		void				begin				(const _index_type	&vertex_index,	const_iterator		&b,				const_iterator			&e) const;
};

#include "problem_solver_inline.h"
