////////////////////////////////////////////////////////////////////////////
//	Module 		: problem_solver.cpp
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Problem solver
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "problem_solver.h"

#define TEMPLATE_SPECIALIZATION template<\
	typename T1,\
	typename T2,\
	typename T3,\
	typename T4\
>

#define CProblemSolverAbstract CProblemSolver<T1,T2,T3,T4>

TEMPLATE_SPECIALIZATION
CProblemSolverAbstract::~CProblemSolver		()
{
}

#undef TEMPLATE_SPECIALIZATION
#undef CProblemSolverAbstract
