////////////////////////////////////////////////////////////////////////////
//	Module 		: location_manager.cpp
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Location manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "location_manager.h"

CLocationManager::CLocationManager	()
{
	Init							();
}

CLocationManager::~CLocationManager	()
{
}

void CLocationManager::Init			()
{
	m_vertex_types.clear			();
}

void CLocationManager::Load			(LPCSTR section)
{
	m_vertex_types.clear			();
	LPCSTR							S = pSettings->r_string(section,"terrain");
	u32								N = _GetItemCount(S);
	R_ASSERT3						(!(N % (LOCATION_TYPE_COUNT + 2)) && N,"Terrain locations are incorrectly specified for the monster",section);
	ALife::STerrainPlace			terrain_mask;
	terrain_mask.tMask.resize		(LOCATION_TYPE_COUNT);
	m_vertex_types.reserve			(32);
	string16						I;
	for (u32 i=0; i<N;) {
		for (u32 j=0; j<LOCATION_TYPE_COUNT; ++j, ++i)
			terrain_mask.tMask[j]	= ALife::_LOCATION_ID(atoi(_GetItem(S,i,I)));
		terrain_mask.dwMinTime		= atoi(_GetItem(S,i++,I))*1000;
		terrain_mask.dwMaxTime		= atoi(_GetItem(S,i++,I))*1000;
		m_vertex_types.push_back	(terrain_mask);
	}
}

void CLocationManager::reinit			()
{
}

void CLocationManager::reload			(LPCSTR section)
{
}