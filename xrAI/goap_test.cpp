////////////////////////////////////////////////////////////////////////////
//	Module 		: goap_test.cpp
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Goal Oriented Action Planning tests
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "problem_solver.h"

enum EGoals {
	eBit0 = u32(1) << 0,
	eBit1 = u32(1) << 1,
	eBit2 = u32(1) << 2,
	eBit3 = u32(1) << 3,
	eBit4 = u32(1) << 4,
	eBit5 = u32(1) << 5,
	eBit6 = u32(1) << 6,
	eBit7 = u32(1) << 7,
};

struct COperatorAction {
	void initialize	()		 {};
	void finalize	()		 {};
	void execute	()		 {};
	bool completed	() const {};
};

#include "a_star.h"

//	data storages
#include "data_storage_edge_path.h"

// vertex managers
#include "data_storage_vertex_manager.h"
#include "data_storage_fixed_vertex_manager.h"

// allocators
#include "data_storage_allocator.h"
#include "data_storage_fixed_allocator.h"

//		lists
#include "data_storage_bucket_list.h"
#include "data_storage_cheap_list.h"
#include "data_storage_single_linked_list.h"
#include "data_storage_double_linked_list.h"
//		priority queues
#include "boost/f_heap.hpp"
#include "boost/p_heap.hpp"
#include "data_storage_priority_queue.h"
#include "data_storage_binary_heap.h"
#include "data_storage_binary_heap_list.h"
#include "data_storage_multi_binary_heap.h"

#include "path_manager.h"

typedef CProblemSolver<COperatorAction,u32,bool,u32>	CSProblemSolver;
typedef CConditionState<u32,bool>						CState;
typedef COperatorConditionAbstract<u32,bool>			CCondition;
typedef COperatorAbstract<COperatorAction,u32,bool>		COperator;

CState random_condition(int _max = 100, int _min = 40, int __max = 2, int __min = 1)
{
	CState		result;
	for (u32 i=0; i<8; ++i)
		if (::Random.randI(_max) < _min)
			result.add_condition(CCondition(EGoals(u32(1) << i),::Random.randI(__max) < __min));
	return		(result);
}

COperator random_operator()
{
	return		(COperator(0,0,random_condition().conditions(),random_condition().conditions()));
}

LPSTR show_condition(const CState &condition, LPSTR s)
{
	char					*s1 = s;
	xr_vector<CCondition>::const_iterator	i = condition.conditions().begin();
	xr_vector<CCondition>::const_iterator	e = condition.conditions().end();
	for (u32 I=0; (I < 8) && (i != e); ++I)
		if ((u32(1) << I) == u32((*i).condition())) {
			s1				+= sprintf(s1,"%d ",(*i).value() ? 1 : 0);
			++i;
		}
		else
			s1				+= sprintf(s1,"? ");

	for ( ; I < 8; ++I)
		s1					+= sprintf(s1,"? ");
	return					(s);
}

CState &show_condition(CState &condition, u32 i = 0)
{
	string256				s;
	Msg						("condition %d : %s",i,show_condition(condition,s));
	return					(condition);
}

COperator &show_operator(COperator &_operator, u32 i)
{
	string256				s;
	char					*s1;
	show_condition			(_operator.conditions(),s);
	strcat					(s," -> ");
	s1						= s + xr_strlen(s);
	show_condition			(_operator.effects(),s1);
	Msg						("operator %d  : %s",i,s);
	return					(_operator);
}

void test_goap	()
{
	for (u32 ii=0; ii<10000; ++ii) {
		Msg	("Problem %5d",ii);
		CSProblemSolver					problem_solver;
//		CState							current, target;
//		COperator						operator1, operator2, operator3, operator4;
//		COperatorAction	action;
//		current.add_condition			(CCondition	(eBit0,false));
//		current.add_condition			(CCondition	(eBit1,false));
//		current.add_condition			(CCondition	(eBit2,false));
//		current.add_condition			(CCondition	(eBit3,false));
//
//		target.add_condition			(CCondition	(eBit0,true));
//		target.add_condition			(CCondition	(eBit1,true));
//		target.add_condition			(CCondition	(eBit2,true));
//		target.add_condition			(CCondition	(eBit3,false));
//
//		operator1.add_condition			(CCondition	(eBit1,false));
//		operator1.add_condition			(CCondition	(eBit2,false));
//		operator1.add_effect			(CCondition	(eBit1,true));
//
//		operator2.add_condition			(CCondition	(eBit2,false));
//		operator2.add_effect			(CCondition	(eBit2,true));
//
//		operator3.add_condition			(CCondition	(eBit1,true));
//		operator3.add_condition			(CCondition	(eBit2,true));
//		operator3.add_effect			(CCondition	(eBit0,true));
//		operator3.add_effect			(CCondition	(eBit1,false));
//		operator3.add_effect			(CCondition	(eBit2,true));
//
//		operator4.add_condition			(CCondition	(eBit1,false));
//		operator4.add_condition			(CCondition	(eBit2,true));
//		operator4.add_effect			(CCondition	(eBit1,true));
//
//		problem_solver.add_operator		(operator1,	0);
//		problem_solver.add_operator		(operator2,	1);
//		problem_solver.add_operator		(operator3,	2);
//		problem_solver.add_operator		(operator4,	3);
//
//		problem_solver.set_current_state(current);
//		problem_solver.set_target_state	(target);
//		problem_solver.solve			();

		problem_solver.set_current_state(show_condition(random_condition(100,100,4,1)));
		problem_solver.set_target_state	(show_condition(random_condition(100,100,4,3),1));
		u32								operator_count = ::Random.randI(4,8);
		for (u32 i=0; i<operator_count; ++i)
			problem_solver.add_operator	(show_operator(random_operator(),i),i);

		typedef u8											_dist_type;
		typedef u32											_iteration_type;
		typedef CSProblemSolver::_edge_type					_edge_type;
		typedef CSProblemSolver::CState						_index_type;
		typedef PathManagers::SBaseParameters<_dist_type,_index_type,_iteration_type>		_Parameters;
		typedef CDataStorageAllocator						_VertexAllocator;
		typedef CDataStorageVertexManager<u32,_index_type>	_VertexManager;
		typedef CDataStorageSingleLinkedList<false>			_PriorityQueue;
//		typedef CDataStorageSingleLinkedList<true>					_PriorityQueue;
//		typedef CDataStorageDoubleLinkedList<false>					_PriorityQueue;
//		typedef CDataStorageDoubleLinkedList<true>					_PriorityQueue;
//		typedef CDataStorageCheapList<32,true,true>					_PriorityQueue;
//		typedef CDataStorageBucketList<u32,u32,8*1024,false>		_PriorityQueue;
//		typedef CDataStorageBinaryHeap								_PriorityQueue;
//		typedef CDataStorageBinaryHeapList<4>						_PriorityQueue;
//		typedef CDataStorageMultiBinaryHeap<4>						_PriorityQueue;
//		typedef CDataStoragePriorityQueue<boost::fibonacci_heap>	_PriorityQueue;
//		typedef CDataStoragePriorityQueue<boost::pairing_heap>		_PriorityQueue;

		typedef CAStar<_dist_type,_PriorityQueue,_VertexManager,_VertexAllocator,true,CDataStorageEdgePath<_edge_type,true> > CAStarGeneric;
		typedef CPathManager<CSProblemSolver, CAStarGeneric::CDataStorage, _Parameters, _dist_type,_index_type,_iteration_type>	CPathManagerGeneric;

		CPathManagerGeneric			path_manager;
		CAStarGeneric				a_star(u32(-1));
		xr_vector<_edge_type>		path;
		_Parameters					parameters;

		path_manager.setup			(
			&problem_solver,
			&a_star.data_storage(),
			&path,
			problem_solver.current_state(),
			problem_solver.target_state(),
			parameters
		);
		bool						successfull = a_star.find(path_manager);

		xr_vector<_edge_type>::iterator	I = path.begin(), B = I;
		xr_vector<_edge_type>::iterator	E = path.end();
		
		if (B == E)
			Msg	("Problem cannot be solved!");
		else
			Msg	("Problem is solved!");

		for ( ; I != E; ++I)
			Msg					("%d",*I);

		Msg	("");
	}
}