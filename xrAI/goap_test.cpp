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

const u32 world_state_dimension = 32;
const u32 min_operator_count	= 16;
const u32 max_operator_count	= 32;
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

CState random_condition(int _max = 100, int _min = 10, int __max = 2, int __min = 1)
{
	CState		result;
	for (u32 i=0; i<world_state_dimension; ++i)
		if (random.randI(_max) < _min)
			result.add_condition(CCondition(i,random.randI(__max) < __min ? 1 : 0));
	return		(result);
}

COperator *random_operator()
{
	return		(xr_new<COperator>(0,random_condition().conditions(),random_condition().conditions()));
}

LPSTR show_condition(const xr_vector<CCondition> &condition, LPSTR s)
{
	char					*s1 = s;
	xr_vector<CCondition>::const_iterator	i = condition.begin();
	xr_vector<CCondition>::const_iterator	e = condition.end();
	for (u32 I=0; (I < world_state_dimension) && (i != e); ++I)
		if (I == u32((*i).condition())) {
			s1				+= sprintf(s1,"%d ",(*i).value());
			++i;
		}
		else
			s1				+= sprintf(s1,"? ");

	for ( ; I < world_state_dimension; ++I)
		s1					+= sprintf(s1,"? ");
	return					(s);
}

void show_condition(const CState &condition, u32 i = 0)
{
	string256				s;
	Msg						("condition %2d : %s",i,show_condition(condition.conditions(),s));
}

void show_operator(const COperator &_operator, u32 i)
{
	string256				s;
	char					*s1;
	show_condition			(_operator.conditions(),s);
	strcat					(s," -> ");
	s1						= s + xr_strlen(s);
	show_condition			(_operator.effects(),s1);
	Msg						("operator  %2d : %s",i,s);
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
		u32						operator_count = random.randI(min_operator_count,max_operator_count);
		for (u32 i=0; i<operator_count; ++i)
			problem_solver.add_operator	(random_operator(),i);

		path.clear						();

		if (ii == 137) {
			ii = ii;
			Msg							("Problem %5d (try %I64d, %f sec)",jj,ii,CPU::cycles2seconds*(CPU::GetCycleCount() - start));
			++jj;
			show_condition				(problem_solver.current_state());
			show_condition				(problem_solver.target_state(),1);

			{
				CSProblemSolver::const_iterator	I = problem_solver.operators().begin();
				CSProblemSolver::const_iterator	E = problem_solver.operators().end();
				for ( ; I != E; ++I)
					show_operator			(*(*I).m_operator,(*I).m_operator_id);
			}
		}
#ifdef INTENSIVE_MEMORY_USAGE
		graph_engine->search			(problem_solver,problem_solver.current_state(),problem_solver.target_state(),&path,CBaseParameters());
#else
		graph_engine->search			(problem_solver,CState(),problem_solver.target_state(),&path,CBaseParameters());
#endif
		if (ii == 137) {
			Msg							("Finished %5d (try %I64d, %f sec, %d vertices)",jj,ii,CPU::cycles2seconds*(CPU::GetCycleCount() - start),graph_engine->solver_algorithm().data_storage().get_visited_node_count());
			FlushLog					();
		}

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
			CSProblemSolver::const_iterator	I = problem_solver.operators().begin();
			CSProblemSolver::const_iterator	E = problem_solver.operators().end();
			for ( ; I != E; ++I)
				show_operator			(*(*I).m_operator,(*I).m_operator_id);
		}

		Msg								("Solution : ");

		CState							world_state = problem_solver.current_state(), temp;
		show_condition					(world_state);
		for ( ; I != E; ++I) {
			CSProblemSolver::const_iterator	i = std::lower_bound(problem_solver.operators().begin(),problem_solver.operators().end(),*I);
			VERIFY						(i != problem_solver.operators().end());
			VERIFY						((*i).m_operator_id == *I);
			show_operator				(*(*i).m_operator,(*i).m_operator_id);
#ifdef INTENSIVE_MEMORY_USAGE
			VERIFY						((*i).m_operator->applicable(world_state));
			world_state					= (*i).m_operator->apply(world_state,temp);
#else
			VERIFY						((*i).m_operator->applicable(world_state,problem_solver.current_state()));
			world_state					= (*i).m_operator->apply(world_state,problem_solver.current_state(),temp);
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