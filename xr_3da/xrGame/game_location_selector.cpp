////////////////////////////////////////////////////////////////////////////
//	Module 		: game_location_selector.cpp
//	Created 	: 02.10.2001
//  Modified 	: 18.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game location selector
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "game_location_selector.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _VertexEvaluator,\
	typename _vertex_id_type\
>

#define CGameLocationSelector CBaseLocationSelector<CGameGraph,_VertexEvaluator,_vertex_id_type>

TEMPLATE_SPECIALIZATION
void CGameLocationSelector::Load(LPCSTR caSection)
{
	m_vertex_types.clear				();
	LPCSTR								S = pSettings->r_string(section,"terrain");
	u32									N = _GetItemCount(S);
	R_ASSERT							(!(N % (LOCATION_TYPE_COUNT + 2)) && N);
	ALife::STerrainPlace				l_terrain_location;
	l_terrain_location.tMask.resize		(LOCATION_TYPE_COUNT);
	string16							I;
	for (u32 i=0; i<N;) {
		for (u32 j=0; j<LOCATION_TYPE_COUNT; ++j, ++i)
			l_terrain_location.tMask[j] = _LOCATION_ID(atoi(_GetItem(S,i,I)));
		l_terrain_location.dwMinTime	= atoi(_GetItem(S,i++,I))*1000;
		l_terrain_location.dwMaxTime	= atoi(_GetItem(S,i++,I))*1000;
		m_vertex_types.push_back		(l_terrain_location);
	}
}

#undef TEMPLATE_SPECIALIZATION
#undef CGameLocationSelector
