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
class COperatorAbstract : public CConditionState<_world_property> {
public:
	typedef _edge_value_type					_edge_value_type;

protected:
	typedef CConditionState<_world_property>	inherited;
	typedef inherited							CSConditionState;
	typedef _world_property						COperatorCondition;

protected:
	xr_vector<COperatorCondition>				m_effects;

public:
	IC											COperatorAbstract	();
	IC											COperatorAbstract	(const xr_vector<COperatorCondition> &conditions, const xr_vector<COperatorCondition> &effects);
	virtual										~COperatorAbstract	();
	virtual	void								Load				(LPCSTR section);
	virtual void								reinit				(bool clear_all);
	virtual void								reload				(LPCSTR section);
	IC		const xr_vector<COperatorCondition>	&effects			() const;
	IC		void								add_effect			(const COperatorCondition &effect);
	virtual bool								applicable			(const xr_vector<COperatorCondition> &condition, const xr_vector<COperatorCondition> &start, const xr_vector<COperatorCondition> &self_condition) const;
	virtual const CSConditionState				&apply				(const CSConditionState &condition, const xr_vector<COperatorCondition> &self_condition, CSConditionState &result) const;
	virtual bool								apply_reverse		(const CSConditionState &condition, const xr_vector<COperatorCondition> &start, CSConditionState &result, const xr_vector<COperatorCondition> &self_condition) const;
	virtual _edge_value_type					weight				() const;
};

#include "operator_abstract_inline.h"