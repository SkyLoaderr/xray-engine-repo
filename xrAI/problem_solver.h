////////////////////////////////////////////////////////////////////////////
//	Module 		: problem_solver.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Problem solver
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "operator_abstract.h"

template <
	typename T1,
	typename T2,
	typename T3,
	typename T4
>
class CProblemSolver {
private:
	typedef COperatorConditionAbstract<T2,T3>	COperatorCondition;
	typedef COperatorEffectAbstract<T2,T3>		COperatorEffect;
	typedef COperatorAbstract<T1,T2,T3>			COperator;

protected:
	xr_map<T4,COperator>			m_operators;
	CConditionState					m_current_state;
	CConditionState					m_target_state;
	T4								m_current_operator;

public:
	IC											CProblemSolver		();
	virtual										~CProblemSolver		();
	IC		void								add_operator		(COperator &state, const T4 &state_id);
	IC		void								remove_operator		(const T4 &state_id);
	IC		const T4							&current_operator	() const;
	IC		const xr_vector<COperatorCondition>	&current_state		() const;
	IC		const xr_vector<COperatorCondition>	&target_state		() const;
	IC		void								set_current_state	(const xr_vector<COperatorCondition> &state);
	IC		void								set_target_state	(const xr_vector<COperatorCondition> &state);
	IC		void								set_current_operator(const T4 &state_id);
};

#include "problem_solver_inline.h"
