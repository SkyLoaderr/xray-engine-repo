////////////////////////////////////////////////////////////////////////////
//	Module 		: operator_abstract.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Operator abstract
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "operator_effect.h"
#include "condition_state.h"

template <
	typename T1,
	typename T2,
	typename T3
>
class COperatorAbstract : public CConditionState {
private:
	typedef COperatorConditionAbstract<T2,T3>	COperatorCondition;
	typedef COperatorEffectAbstract<T2,T3>		COperatorEffect;

protected:
	xr_vector<COperatorEffect>					m_effects;
	T1											*m_state;
	u32											m_priority;

public:
	IC											COperatorAbstract	(T1 *state, u32 priority, const xr_vector<COperatorCondition> &conditions, const xr_vector<COperatorEffect> &effects);
	virtual										~COperatorAbstract	();
	IC		T1									*state				();
	IC		u32									priority			() const;
	IC		const xr_vector<COperatorEffect>	&effects			() const;
};

#include "operator_abstract_inline.h"