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

typedef CDataStorage<float,u32,u32,true,24,8>							CHeapDataStorage;
typedef CPathManager<CAI_Map,CHeapDataStorage,float,u32,u32>			CDistancePathManager;
typedef CAStar<CHeapDataStorage,CDistancePathManager,CAI_Map,u32,float>	CAStarSearch;

void path_test(LPCSTR caLevelName)
{
	xr_vector<u32>			path;
	CAI_Map					*graph			= xr_new<CAI_Map>				(caLevelName);
	CHeapDataStorage		*data_storage	= xr_new<CHeapDataStorage>		(graph->m_header.count);
	CDistancePathManager	*path_manager	= xr_new<CDistancePathManager>	();
	CAStarSearch			*a_star			= xr_new<CAStarSearch>			();
	
	path_manager->init		(
		graph,
		data_storage,
		&path,
		2,
		10,
		u32(-1),
		6000,
		u32(-1)
	);

	a_star->find			(*data_storage,*path_manager,*graph);

	xr_delete	(graph);
	xr_delete	(data_storage);
	xr_delete	(path_manager);
}
