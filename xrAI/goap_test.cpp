////////////////////////////////////////////////////////////////////////////
//	Module 		: goap_test.cpp
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Goal Oriented Action Planning tests
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "problem_solver.h"
#include "graph_engine.h"

typedef u32																		_condition_type;
typedef u32																		_value_type;
typedef u32																		_operator_id_type;
typedef CProblemSolver<_condition_type,_value_type,_operator_id_type>			CSProblemSolver;
typedef CConditionState<_condition_type,_value_type>							CState;
typedef COperatorConditionAbstract<_condition_type,_value_type>					CCondition;
typedef COperatorAbstract<_condition_type,_value_type>							COperator;
typedef u8																		_dist_type;
typedef u32																		_iteration_type;
typedef CSProblemSolver::_edge_type												_edge_type;
typedef CSProblemSolver::CState													_index_type;
typedef SBaseParameters<_dist_type,_index_type,_iteration_type>	CBaseParameters;
CRandom								random;

CState random_condition(int _max = 100, int _min = 20, int __max = 2, int __min = 1)
{
	CState		result;
	for (u32 i=0; i<8; ++i)
		if (random.randI(_max) < _min)
			result.add_condition(CCondition(u32(1) << i,random.randI(__max) < __min ? 1 : 0));
	return		(result);
}

COperator *random_operator()
{
	return		(xr_new<COperator>(0,random_condition().conditions(),random_condition().conditions()));
}

LPSTR show_condition(const CState &condition, LPSTR s)
{
	char					*s1 = s;
	xr_vector<CCondition>::const_iterator	i = condition.conditions().begin();
	xr_vector<CCondition>::const_iterator	e = condition.conditions().end();
	for (u32 I=0; (I < 8) && (i != e); ++I)
		if ((u32(1) << I) == u32((*i).condition())) {
			s1				+= sprintf(s1,"%d ",(*i).value());
			++i;
		}
		else
			s1				+= sprintf(s1,"? ");

	for ( ; I < 8; ++I)
		s1					+= sprintf(s1,"? ");
	return					(s);
}

void show_condition(const CState &condition, u32 i = 0)
{
	string256				s;
	Msg						("condition %d : %s",i,show_condition(condition,s));
}

void show_operator(const COperator &_operator, u32 i)
{
	string256				s;
	char					*s1;
	show_condition			(_operator.conditions(),s);
	strcat					(s," -> ");
	s1						= s + xr_strlen(s);
	show_condition			(_operator.effects(),s1);
	Msg						("operator %d  : %s",i,s);
}

void test_goap	()
{
	CGraphEngine						*graph_engine	= xr_new<CGraphEngine>(0);
	xr_vector<_edge_type>				path;
	u32	max_length						= 0;
	u32 best_test						= 0;
	u32 jj								= 0;
	random.seed							(0);//u32(CPU::GetCycleCount()));
	u64 start							= CPU::GetCycleCount();
	for (u64 ii=0; ; ++ii) {
		CSProblemSolver					problem_solver;
		problem_solver.set_current_state(random_condition(100,100,4,1));
//		problem_solver.set_target_state	(random_condition(100,100,4,3) -= problem_solver.current_state());
		problem_solver.set_target_state	(random_condition(100,100,4,3));
		u32						operator_count = random.randI(4,8);
		for (u32 i=0; i<operator_count; ++i)
			problem_solver.add_operator	(random_operator(),i);

		path.clear						();

//		graph_engine->search			(problem_solver,problem_solver.current_state(),problem_solver.target_state(),&path,CBaseParameters());
#ifdef INTENSIVE_MEMORY_USAGE
		graph_engine->search			(problem_solver,problem_solver.current_state(),problem_solver.target_state(),&path,CBaseParameters());
#else
		if (ii == 216) {
			ii = ii;
		}
		graph_engine->search			(problem_solver,CState(),problem_solver.target_state(),&path,CBaseParameters());
#endif

		xr_vector<_edge_type>::iterator	I = path.begin(), B = I;
		xr_vector<_edge_type>::iterator	E = path.end();
		
//		if ((ii != 14602) &&((B == E) || (path.size() <= max_length)))
		if ((B == E) || (path.size() <= max_length))
			continue;

		max_length						= path.size();
		best_test						= jj;
		
		Msg								("Problem %5d (try %I64d, %f sec)",jj,ii,CPU::cycles2seconds*(CPU::GetCycleCount() - start));
		++jj;
		show_condition					(problem_solver.current_state());
		show_condition					(problem_solver.target_state(),1);

		{
			CSProblemSolver::OPERATOR_MAP::const_iterator	I = problem_solver.operators().begin();
			CSProblemSolver::OPERATOR_MAP::const_iterator	E = problem_solver.operators().end();
			for ( ; I != E; ++I)
				show_operator			(*(*I).second,(*I).first);
		}

		Msg								("Solution : ");

		CState							world_state = problem_solver.current_state(), temp;
		show_condition					(world_state);
		for ( ; I != E; ++I) {
			CSProblemSolver::OPERATOR_MAP::const_iterator	i = problem_solver.operators().find(*I);
			show_operator				(*(*i).second,(*i).first);
#ifdef INTENSIVE_MEMORY_USAGE
			VERIFY						((*i).second->applicable(world_state));
			world_state					= (*i).second->apply(world_state,temp);
#else
			VERIFY						((*i).second->applicable(world_state,problem_solver.current_state()));
			world_state					= (*i).second->apply(world_state,problem_solver.current_state(),temp);
#endif
			show_condition				(world_state);
		}

#ifdef INTENSIVE_MEMORY_USAGE
		VERIFY							(world_state.includes(problem_solver.target_state()));
#else
		VERIFY							(world_state.includes(problem_solver.target_state(),problem_solver.current_state()));
#endif
		Msg								("Max solution length : %d (test %d)",max_length,best_test);
		Msg								("");
		FlushLog						();
	}
	xr_delete							(graph_engine);
}