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

class CActionPlanner : public CGraphEngine::CSProblemSolver {
protected:
	typedef CGraphEngine::CSProblemSolver inherited;
public:
						CActionPlanner			();
	virtual				~CActionPlanner			();
			void		init					();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(bool clear_all = false);
	virtual	void		reload					(LPCSTR section);
	virtual	void		update					(u32 time_delta);
};

#include "action_planner_inline.h"