////////////////////////////////////////////////////////////////////////////
//	Module 		: action_planner.h
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Action planner
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "problem_solver.h"
#include "graph_engine.h"

template <typename _object_type>
class CActionPlanner : public CGraphEngine::CSProblemSolver {
protected:
	typedef CGraphEngine::CSProblemSolver	CProblemSolver;
	typedef CProblemSolver					inherited;

protected:
			void		follow_solution			();

public:
						CActionPlanner			();
	virtual				~CActionPlanner			();
			void		init					();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(_object_type *object, bool clear_all = false);
	virtual	void		reload					(LPCSTR section);
	virtual	void		update					(u32 time_delta);
	IC		COperator	&action					(u32 action_id);
};

#include "action_planner_inline.h"