////////////////////////////////////////////////////////////////////////////
//	Module 		: operator_abstract.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Operator abstract
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "condition_state.h"
#include "intrusive_ptr.h"

template <
	typename _world_property,
	typename _edge_value_type
>
class COperatorAbstract : virtual public intrusive_base {
public:
	typedef _edge_value_type					_edge_value_type;

protected:
	typedef CConditionState<_world_property>	inherited;
	typedef inherited							CSConditionState;
	typedef _world_property						COperatorCondition;

protected:
	CSConditionState				m_conditions;
	CSConditionState				m_effects;
	mutable bool					m_actual;
	mutable _edge_value_type		m_min_weight;

public:
	IC								COperatorAbstract	();
	IC								COperatorAbstract	(const CSConditionState &conditions, const CSConditionState &effects);
	virtual							~COperatorAbstract	();
	virtual	void					Load				(LPCSTR section);
	virtual void					reinit				(bool clear_all);
	virtual void					reload				(LPCSTR section);
	IC		const CSConditionState	&conditions			() const;
	IC		const CSConditionState	&effects			() const;
	IC		void					add_condition		(const COperatorCondition &condition);
	IC		void					add_effect			(const COperatorCondition &effect);
	IC		void					remove_condition	(const typename COperatorCondition::_condition_type &condition);
	IC		void					remove_effect		(const typename COperatorCondition::_condition_type &effect);
	IC		_edge_value_type		min_weight			() const;
	
	template <typename T>
			bool					applicable			(const CSConditionState &condition, const CSConditionState &start, const CSConditionState &self_condition, T &problem_solver) const;
	
	virtual bool					applicable_reverse	(const CSConditionState &condition, const CSConditionState &start, const CSConditionState &self_condition) const;
	
	template <typename T>
			const CSConditionState	&apply				(const CSConditionState &condition, const CSConditionState &self_condition, CSConditionState &result, CSConditionState &current, T &problem_solver) const;
	virtual const CSConditionState	&apply				(const CSConditionState &condition, const CSConditionState &self_condition, CSConditionState &result) const;
	
	virtual bool					apply_reverse		(const CSConditionState &condition, const CSConditionState &start, CSConditionState &result, const CSConditionState &self_condition) const;
	virtual _edge_value_type		weight				(const CSConditionState &condition0, const CSConditionState &condition1) const;
};

#include "operator_abstract_inline.h"