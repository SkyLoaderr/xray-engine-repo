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
	typename _condition_type,
	typename _value_type
>
class COperatorAbstract : public CConditionState<_condition_type,_value_type> {
protected:
	typedef CConditionState<_condition_type,_value_type>			inherited;
	typedef inherited												CSConditionState;
	typedef COperatorConditionAbstract<_condition_type,_value_type>	COperatorCondition;

protected:
	xr_vector<COperatorCondition>				m_effects;
	u32											m_priority;

public:
	IC											COperatorAbstract	();
	IC											COperatorAbstract	(u32 priority, const xr_vector<COperatorCondition> &conditions, const xr_vector<COperatorCondition> &effects);
	virtual										~COperatorAbstract	();
	IC		u32									priority			() const;
	IC		const xr_vector<COperatorCondition>	&effects			() const;
	IC		void								add_condition		(const COperatorCondition &condition);
	IC		void								add_effect			(const COperatorCondition &effect);
	virtual bool								applicable			(const CSConditionState &condition) const;
	virtual const CSConditionState				&apply				(const CSConditionState &condition, CSConditionState &result) const;
	virtual void								initialize			();
	virtual void								execute				();
	virtual void								finalize			();
	virtual bool								completed			() const;
};

#include "operator_abstract_inline.h"