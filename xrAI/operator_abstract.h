////////////////////////////////////////////////////////////////////////////
//	Module 		: operator_abstract.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Operator abstract
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "condition_state.h"

template <
	typename _world_property,
	typename _edge_value_type
>
class COperatorAbstract {
public:
	typedef _edge_value_type					_edge_value_type;

protected:
	typedef CConditionState<_world_property>	inherited;
	typedef inherited							CSConditionState;
	typedef _world_property						COperatorCondition;

protected:
	CSConditionState				m_conditions;
	CSConditionState				m_effects;

public:
	IC								COperatorAbstract	();
	IC								COperatorAbstract	(const CSConditionState &conditions, const CSConditionState &effects);
	virtual							~COperatorAbstract	();
	virtual	void					Load				(LPCSTR section);
	virtual void					reinit				(bool clear_all);
	virtual void					reload				(LPCSTR section);
	IC		const CSConditionState	&conditions			() const;
	IC		void					add_condition		(const COperatorCondition &condition);
	IC		const CSConditionState	&effects			() const;
	IC		void					add_effect			(const COperatorCondition &effect);
	virtual bool					applicable			(const CSConditionState &condition, const CSConditionState &start, const CSConditionState &self_condition) const;
	virtual const CSConditionState	&apply				(const CSConditionState &condition, const CSConditionState &self_condition, CSConditionState &result) const;
	virtual bool					apply_reverse		(const CSConditionState &condition, const CSConditionState &start, CSConditionState &result, const CSConditionState &self_condition) const;
	virtual _edge_value_type		weight				() const;
};

#include "operator_abstract_inline.h"