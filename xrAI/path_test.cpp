////////////////////////////////////////////////////////////////////////////
//	Module 		: path_test.cpp
//	Created 	: 18.10.2003
//  Modified 	: 18.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Path tests
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "path_test.h"
#include "ai_map.h"
#include "a_star.h"
#include "data_storage.h"
#include "path_manager.h"
#include "path_test_old.h"

typedef CHeapDataStorage<float,u32,u32,true,24,8>						CDataStorage;
typedef CPathManager<CAI_Map,CDataStorage,float,u32,u32>				CDistancePathManager;
typedef CAStar<CDataStorage,CDistancePathManager,CAI_Map,u32,float>		CAStarSearch;

#define TIME_TEST

void path_test(LPCSTR caLevelName)
{
	xr_vector<u32>			path, path1;
	CAI_Map					*graph			= xr_new<CAI_Map>				(caLevelName);
	CDataStorage			*data_storage	= xr_new<CDataStorage>			(graph->get_node_count());
	CDistancePathManager	*path_manager	= xr_new<CDistancePathManager>	();
	CAStarSearch			*a_star			= xr_new<CAStarSearch>			();
	
	Msg						("Total %d nodes",graph->get_node_count() - 1);

#ifdef TIME_TEST
	u64						start, finish;
	SetPriorityClass		(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
	Sleep					(1);
	start					= CPU::GetCycleCount();
	for (int i=0; i<100; ++i) {
		path_manager->setup		(
			graph,
			data_storage,
			&path,
			1 + i,
			graph->get_node_count() - 1 - i
		);
		a_star->find		(*data_storage,*path_manager,*graph);
	}
	finish					= CPU::GetCycleCount();
	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
	SetPriorityClass		(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
	Msg						("%f microseconds",float(s64(finish - start))*CPU::cycles2microsec);
#else
	for (int I=0, N = graph->get_node_count() - 2; I<N; ++I) {
		path_manager->setup		(
			graph,
			data_storage,
			&path,
			1 + I,
			graph->get_node_count() - 1 - I,
			u32(-1),
			6000,
			u32(-1)
			);
		a_star->find		(*data_storage,*path_manager,*graph);
		u32 v = data_storage->get_visited_node_count(), v1;
		float f;
		path_test_old		(graph,path1,1 + I,graph->get_node_count() - 1 - I,f,v1);
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
			Msg				("[%6d] Path lengths : %d[%f][%d] -> %d[%f][%d] (real path lengths are %f -> %f)",I,path.size(),data_storage->get_best().f(),v,path1.size(),f,v1,v_,v_1);
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
#ifdef TIME_TEST	
	path_test_old			(caLevelName);
#endif
}
