////////////////////////////////////////////////////////////////////////////
//	Module 		: location_manager.cpp
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Location manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "location_manager.h"
#include "gameobject.h"

CLocationManager::CLocationManager	()
{
	init							();
}

CLocationManager::~CLocationManager	()
{
}

void CLocationManager::init			()
{
	m_vertex_types.clear			();
}

void CLocationManager::setup_location_types(LPCSTR S, LPCSTR section)
{
	m_vertex_types.clear			();
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

void CLocationManager::Load			(LPCSTR section)
{
	setup_location_types			(pSettings->r_string(section,"terrain"),section);
}

void CLocationManager::reinit		()
{
}

void CLocationManager::reload		(LPCSTR section)
{
	CGameObject						*game_object = dynamic_cast<CGameObject*>(this);
	if (!game_object || !game_object->spawn_ini() || !game_object->spawn_ini()->section_exist("alife") || !game_object->spawn_ini()->line_exist("alife","terrain"))
		return;
	setup_location_types			(game_object->spawn_ini()->r_string("alife","terrain"),*game_object->cNameSect());
}