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

typedef CDataStorage<float,int,u32,true,24,8>							CHeapDataStorage;
typedef CPathManager1<CAI_Map,CHeapDataStorage,float,int,u32>			CDistancePathManager;
typedef CAStar<CHeapDataStorage,CDistancePathManager,CAI_Map,int,float>	CAStarSearch;

void path_test(LPCSTR caLevelName)
{
	xr_vector<int>			path;
	CAI_Map					*graph			= xr_new<CAI_Map>				(caLevelName);
	CHeapDataStorage		*data_storage	= xr_new<CHeapDataStorage>		(graph->get_node_count());
	CDistancePathManager	*path_manager	= xr_new<CDistancePathManager>	(graph,data_storage);
	CAStarSearch			*a_star			= xr_new<CAStarSearch>			();
	
	Msg						("Total %d nodes",graph->get_node_count() - 1);

	u64						start, finish;
	Sleep					(1);
	start					= CPU::GetCycleCount();
	for (int i=0; i<100; ++i) {
		path_manager->init		(
			&path,
			1 + i,
			graph->get_node_count() - 1 - i,
			u32(-1),
			6000,
			u32(-1)
			);
		a_star->find		(*data_storage,*path_manager,*graph);
	}
	finish					= CPU::GetCycleCount();
	Msg						("%f microseconds",float(s64(finish - start))*CPU::cycles2microsec);

	xr_delete	(graph);
	xr_delete	(data_storage);
	xr_delete	(path_manager);
	
	path_test_old			(caLevelName);
}
