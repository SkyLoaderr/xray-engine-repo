////////////////////////////////////////////////////////////////////////////
//	Module 		: path_test.cpp
//	Created 	: 18.10.2003
//  Modified 	: 18.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Path tests
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_map.h"
#include "ai_alife_graph.h"
#include "test_table.h"

#include "boost\\d_heap.hpp"
#include "boost\\f_heap.hpp"
#include "boost\\l_heap.hpp"
#include "boost\\p_heap.hpp"
//#include "boost\\r_heap.hpp"
#include "boost\\s_heap.hpp"

#include "data_storage_priority_queue.h"
#include "data_storage_heap.h"
#include "data_storage_list.h"
//#include "data_storage.h"
#include "path_manager.h"
#include "a_star.h"

#include "path_test_old.h"
#include "path_test.h"

#define TEST_COUNT 1000

#define ROWS	300
#define COLUMNS 300
typedef u32	_dist_type;

//typedef CAI_Map																CGraph;
//typedef CSE_ALifeGraph														CGraph;
typedef CTestTable<_dist_type,ROWS,COLUMNS>									CGraph;	

//typedef CDataStorageMultiBinaryHeap<4,_dist_type,u32,u32,true,24,8>			CDataStorage;
//typedef CDataStorageBinaryHeap<_dist_type,u32,u32,true,24,8>					CDataStorage;
//typedef CDataStorageDLSL<_dist_type,u32,u32,true,24,8>						CDataStorage;
//typedef CDataStorageCheapList<35,true,true,_dist_type,u32,u32,true,24,8>	CDataStorage;
//typedef CDataStoragePriorityQueue<boost::lazy_fibonacci_heap,_dist_type,u32,u32,true,24,8>CDataStorage;
typedef CDataStorageBucketList<8*1024,_dist_type,u32,u32,true,24,8>				CDataStorage;
typedef CPathManager<CGraph,CDataStorage,_dist_type,u32,u32>				CDistancePathManager;
typedef CAStar<CDataStorage,CDistancePathManager,CGraph,u32,_dist_type>		CAStarSearch;

#define TIME_TEST

void path_test(LPCSTR caLevelName)
{
	xr_vector<u32>			path, path1;
	string256				fName;
	if (false)
		strconcat			(fName,caLevelName,"level.graph");
	else
		strcpy				(fName,caLevelName);
	CGraph					*graph			= xr_new<CGraph>				(fName);
	CDataStorage			*data_storage	= xr_new<CDataStorage>			(graph->get_node_count(),_dist_type(0),_dist_type(2000));
	CDistancePathManager	*path_manager	= xr_new<CDistancePathManager>	();
	CAStarSearch			*a_star			= xr_new<CAStarSearch>			();
	
	Msg						("Total %d nodes",graph->get_node_count() - 1);

#ifdef TIME_TEST
	xr_vector<u32>			a;

	a.resize				(ROWS*COLUMNS);
	for (int i=0, n = ROWS*COLUMNS; i<n; ++i)
		a[i] = (i/COLUMNS + 1)*(COLUMNS + 2) + i%COLUMNS + 1;

//	a.resize				(graph->get_node_count());
//	for (int i=0, n = (int)a.size(); i<n; ++i)
//		a[i] = i;
	
	std::random_shuffle		(a.begin(),a.end());
	Msg						("%d times",_min((int)a.size(),TEST_COUNT));

	u64						start, finish;
	SetPriorityClass		(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
	Sleep					(1);
	start					= CPU::GetCycleCount();
//	for (int i=0, n = graph->get_node_count(); i<n; ++i) {
	for (int i=0, n=_min((int)a.size(),TEST_COUNT); i<n; i++) {
		path_manager->setup		(
			graph,
			data_storage,
			&path,
			a[i],
			a[n - 1 - i]
		);
		a_star->find		(*data_storage,*path_manager,*graph);
	}
	finish					= CPU::GetCycleCount();
	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
	SetPriorityClass		(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
	Msg						("%f microseconds",float(s64(finish - start))*CPU::cycles2microsec);
	Msg						("%f microseconds per search",float(s64(finish - start))*CPU::cycles2microsec/_min((int)a.size(),TEST_COUNT));
#else
	for (int I=1, N = graph->get_node_count() - 1; I<N; ++I) {
//		int _i = I/(COLUMNS + 2), _j = I % (COLUMNS + 2);
//		if (!_i || !_j || (_i == ROWS + 1) || (_j == COLUMNS + 1))
//			continue;
		path_manager->setup		(
			graph,
			data_storage,
			&path,
			I,
			graph->get_node_count() - I,
			u32(-1),
			6000,
			u32(-1)
			);
		a_star->find		(*data_storage,*path_manager,*graph);
		u32 v = data_storage->get_visited_node_count(), v1;
		float f;
		path_test_old		(graph,path1,I,graph->get_node_count() - I,f,v1);
//		if ((path.size() != path1.size()) || (_abs(data_storage->get_best().f() - f) > EPS_S) || (v != v1))
		if (_abs(data_storage->get_best().f() - f) > EPS_L) {
			float			v_ = 0, v_1 = 0;
			{
				xr_vector<u32>::const_iterator	I = path.begin(), J = I;
				xr_vector<u32>::const_iterator	E = path.end();
				if (I != E)
					for (++I ; I != E; ++I, ++J)
						v_ += graph->get_edge_weight(*J,*I);
			}
			{
				xr_vector<u32>::const_iterator	I = path1.begin(), J = I;
				xr_vector<u32>::const_iterator	E = path1.end();
				if (I != E)
					for (++I ; I != E; ++I, ++J)
						v_1 += graph->get_edge_weight(*J,*I);
			}
			Msg				("[%6d] Path lengths : %d[%f][%d] -> %d[%f][%d] (%f -> %f)",I,path.size(),data_storage->get_best().f(),v,path1.size(),f,v1,v_,v_1);
		}
		else
			if (I % 100 == 0)
				Msg			("%6d",I);
//		u32					n = _min(path.size(),path1.size());
//		for (u32 i=0; i<n; ++i)
//			if (path[i] != path1[i])
//				Msg			("[%6d] %4d : %6d -> %6d",I,i,path[i],path1[i]);
	}
#endif

	xr_delete	(graph);
	xr_delete	(data_storage);
	xr_delete	(path_manager);
	xr_delete	(a_star);
#ifdef TIME_TEST	
//	path_test_old			(caLevelName);
#endif
}
