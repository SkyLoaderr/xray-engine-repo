////////////////////////////////////////////////////////////////////////////
//	Module 		: problem_solver_inline.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Problem solver inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template<\
	typename T1,\
	typename T2,\
	typename T3,\
	typename T4\
>

#define CProblemSolverAbstract CProblemSolver<T1,T2,T3,T4>

TEMPLATE_SPECIALIZATION
IC	CProblemSolverAbstract::CProblemSolver				()
{
}

TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::add_operator			(COperator &state, const T4 &state_id)
{
}

TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::remove_operator		(const T4 &state_id)
{
}

TEMPLATE_SPECIALIZATION
IC	const T4 &CProblemSolverAbstract::current_operator	() const
{
}

//TEMPLATE_SPECIALIZATION
//IC	const xr_vector<COperatorCondition>	&CProblemSolverAbstract::current_state		() const
//{
//}

//TEMPLATE_SPECIALIZATION
//IC	const xr_vector<COperatorCondition>	&CProblemSolverAbstract::target_state		() const
//{
//}

TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::set_current_state		(const xr_vector<COperatorCondition> &state)
{
}

TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::set_target_state		(const xr_vector<COperatorCondition> &state)
{
}

TEMPLATE_SPECIALIZATION
IC	void CProblemSolverAbstract::set_current_operator	(const T4 &state_id)
{
}

#undef TEMPLATE_SPECIALIZATION
#undef CProblemSolverAbstract
