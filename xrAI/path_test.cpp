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
#include "data_storage_priority_queue.h"
#include "data_storage_heap.h"
#include "data_storage_list.h"
#include "boost\\f_heap.hpp"
#include "boost\\p_heap.hpp"
#include "path_manager.h"
#include "a_star.h"
#include "path_test.h"

struct SPathParams{
	SPathParams() {};
	SPathParams(u32 x, u32 y) : x(x), y(y) {};
	u32 x,y;
};

template <
	typename _DataStorage,
	typename _dist_type,
	typename _Graph
>
void test					(_Graph *graph, const xr_vector<SPathParams> &path_params, _dist_type dummy)
{
	typedef CPathManager<_Graph,_DataStorage,_dist_type,u32,u32>			_PathManager;
	typedef CAStar<_DataStorage,_PathManager,_Graph,u32,_dist_type>			_AStarSearch;

	_DataStorage			*data_storage	= xr_new<_DataStorage>			(graph->get_node_count());
	_PathManager			*path_manager	= xr_new<_PathManager>			();
	_AStarSearch			*a_star			= xr_new<_AStarSearch>			();

	xr_vector<u32>			path;
	u64						start, finish;
	u32						test_count = path_params.size();
	
//	SetPriorityClass		(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
//	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
	Sleep					(1);
	
	start					= CPU::GetCycleCount();
	
	xr_vector<SPathParams>::const_iterator	I = path_params.begin();
	xr_vector<SPathParams>::const_iterator	E = path_params.end();
	for ( ; I != E; ++I) {
		path_manager->setup		(
			graph,
			data_storage,
			&path,
			(*I).x,
			(*I).y
			);
		a_star->find		(*data_storage,*path_manager,*graph);
	}

	finish					= CPU::GetCycleCount();
	
	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
	SetPriorityClass		(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);

	xr_delete				(data_storage);
	xr_delete				(path_manager);
	xr_delete				(a_star);

	Msg						("%12.6f (%14.6f) : %s",float(s64(finish - start))*CPU::cycles2milisec/_min((int)path_params.size(),test_count),float(s64(finish - start))*CPU::cycles2microsec,typeid(_DataStorage).name());
}

template <
	typename _DataStorage,
	typename _dist_type,
	typename _Graph
>
void test					(_Graph *graph, const xr_vector<SPathParams> &path_params, _dist_type min_value, _dist_type max_value)
{
	typedef CPathManager<_Graph,_DataStorage,_dist_type,u32,u32>			_PathManager;
	typedef CAStar<_DataStorage,_PathManager,_Graph,u32,_dist_type>			_AStarSearch;

	_DataStorage			*data_storage	= xr_new<_DataStorage>			(graph->get_node_count(),min_value,max_value);
	_PathManager			*path_manager	= xr_new<_PathManager>			();
	_AStarSearch			*a_star			= xr_new<_AStarSearch>			();

	xr_vector<u32>			path;
	u64						start, finish;
	u32						test_count = path_params.size();
	
//	SetPriorityClass		(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
//	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
	Sleep					(1);
	
	start					= CPU::GetCycleCount();
	
	xr_vector<SPathParams>::const_iterator	I = path_params.begin() + 53;
	xr_vector<SPathParams>::const_iterator	E = path_params.end();
	for ( ; I != E; ++I) {
		path_manager->setup		(
			graph,
			data_storage,
			&path,
			(*I).x,
			(*I).y
			);
		a_star->find		(*data_storage,*path_manager,*graph);
	}

	finish					= CPU::GetCycleCount();
	
	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
	SetPriorityClass		(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);

	xr_delete				(data_storage);
	xr_delete				(path_manager);
	xr_delete				(a_star);

	Msg						("%12.6f (%14.6f) : %s",float(s64(finish - start))*CPU::cycles2milisec/_min((int)path_params.size(),test_count),float(s64(finish - start))*CPU::cycles2microsec,typeid(_DataStorage).name());
}

template <
	typename _Graph
>
void init_search			(LPCSTR caLevelName, _Graph *&graph, xr_vector<SPathParams> &path_params, bool bRandom)
{
	graph					= xr_new<_Graph>(caLevelName);
	u32						n = graph->get_node_count();
	path_params.resize		(graph->get_node_count());
	xr_vector<SPathParams>::const_iterator	I = path_params.begin(), B = I;
	xr_vector<SPathParams>::const_iterator	E = path_params.end();
	for ( ; I != E; ++I) {
		(*I).x				= I - B;
		(*I).y				= E - I;
	}
	std::random_shuffle		(path_params.begin(),path_params.end());
}

#define TEST_DIST1	150.f
#define TEST_DIST2	100.f

template <>
void init_search<CAI_Map>	(LPCSTR caLevelName, CAI_Map *&graph, xr_vector<SPathParams> &path_params, bool bRandom)
{
	graph					= xr_new<CAI_Map>(caLevelName);

	if (bRandom) {
		u32					n = graph->get_node_count();
		path_params.resize	(graph->get_node_count() - 2);
		xr_vector<SPathParams>::iterator	I = path_params.begin(), B = I;
		xr_vector<SPathParams>::iterator	E = path_params.end();
		for ( ; I != E; ++I) {
			(*I).x			= ::Random.randI(1,n);
			(*I).y			= ::Random.randI(1,n);
		}
	}
//	else
//		for (int i=1, n=graph->get_node_count(); i<n; ++i) {
//			for (int j=i + 1; j<n; j++)
//				if ((graph->get_edge_weight(i,j) > TEST_DIST2) && (graph->get_edge_weight(i,j) <= TEST_DIST1)) {
//					path_params.push_back(SPathParams(i,j));
//					if (path_params.size() >= TEST_COUNT)
//						break;
//				}
//				if (path_params.size() >= TEST_COUNT)
//					break;
//		}

	std::random_shuffle		(path_params.begin(),path_params.end());
}

template <>
void init_search<CSE_ALifeGraph>(LPCSTR caLevelName, CSE_ALifeGraph *&graph, xr_vector<SPathParams> &path_params, bool bRandom)
{
	string256				fName;
	strconcat				(fName,caLevelName,"level.graph");
	graph					= xr_new<CSE_ALifeGraph>(fName);
	u32						n = graph->get_node_count();
	path_params.resize		(graph->get_node_count());
	
	xr_vector<SPathParams>::iterator	I = path_params.begin(), B = I;
	xr_vector<SPathParams>::iterator	E = path_params.end();
	for ( ; I != E; ++I) {
		(*I).x				= ::Random.randI(0,n);
		(*I).y				= ::Random.randI(0,n);
	}
	
	std::random_shuffle		(path_params.begin(),path_params.end());
}

template <typename _dist_type, u32 rows, u32 columns>
void init_search(LPCSTR caLevelName, CTestTable<_dist_type,rows,columns> *&graph, xr_vector<SPathParams> &path_params, bool bRandom)
{
	graph					= xr_new<CTestTable<_dist_type,rows,columns> >(caLevelName);
	u32						n = rows*columns;
	path_params.resize		(rows*columns);
	
	xr_vector<SPathParams>::iterator	I = path_params.begin(), B = I;
	xr_vector<SPathParams>::iterator	E = path_params.end();
	for (int i; I != E; ++I) {
		i					= ::Random.randI(0,n);
		(*I).x				= (i/columns + 1)*(columns + 2) + i%columns + 1;
		i					= ::Random.randI(0,n);
		(*I).y				= (i/columns + 1)*(columns + 2) + i%columns + 1;
	}
	
	std::random_shuffle		(path_params.begin(),path_params.end());
}

template <
	typename _Graph,
	typename _dist_type
>
void test_all				(LPCSTR caLevelName, u32 test_count, _dist_type min_value, _dist_type max_value)
{
	xr_vector<SPathParams>	path_params;
	_Graph					*graph;
	
	Msg						("\nTest %s",typeid(_Graph).name());
	
	init_search				(caLevelName,graph,path_params,true);

	path_params.resize		(_min(path_params.size(),test_count));
				
	test<CDataStorageUL				<							_dist_type,u32,u32,true,24,8>	>	(graph,path_params,min_value);
	test<CDataStorageDLUL			<							_dist_type,u32,u32,true,24,8>	>	(graph,path_params,min_value);
	test<CDataStorageSL				<							_dist_type,u32,u32,true,24,8>	>	(graph,path_params,min_value);
	test<CDataStorageDLSL			<							_dist_type,u32,u32,true,24,8>	>	(graph,path_params,min_value);
	test<CDataStorageBinaryHeap		<							_dist_type,u32,u32,true,24,8>	>	(graph,path_params,min_value);
	test<CDataStorageBinaryHeapList	<4,							_dist_type,u32,u32,true,24,8>	>	(graph,path_params,min_value);
	test<CDataStorageMultiBinaryHeap<4,							_dist_type,u32,u32,true,24,8>	>	(graph,path_params,min_value);
	test<CDataStorageCheapList		<32,true,true,				_dist_type,u32,u32,true,24,8>	>	(graph,path_params,min_value);
	test<CDataStorageBucketList		<8*1024,false,				_dist_type,u32,u32,true,24,8>	>	(graph,path_params,min_value,max_value);
//	test<CDataStoragePriorityQueue	<boost::fibonacci_heap,		_dist_type,u32,u32,true,24,8>	>	(graph,path_params,min_value);
//	test<CDataStoragePriorityQueue	<boost::pairing_heap,		_dist_type,u32,u32,true,24,8>	>	(graph,path_params,min_value);

	xr_delete				(graph);
}

#define TEST_COUNT 1000

void path_test				(LPCSTR caLevelName)
{
//	test_all<CAI_Map>					(caLevelName,TEST_COUNT,float(0),float(2000));
	test_all<CSE_ALifeGraph>			(caLevelName,TEST_COUNT,float(0),float(2000));
	test_all<CTestTable<u32,30,30> >	(caLevelName,TEST_COUNT,u32(0),u32(60));
//	test_all<CTestTable<u32,300,300> >	(caLevelName,TEST_COUNT,u32(0),u32(600));
//	test_all<CTestTable<u32,900,900> >	(caLevelName,TEST_COUNT,u32(0),u32(1800));
}
