#include "stdafx.h"

#include "ai_space.h"
#include "object_factory.h"
#include "ai/ai_monster_group.h"
#include "string_table.h"
#include "InfoPortion.h"
#include "entity_alive.h"

void clean_memory_global()
{
	// xml parser options
	XML_CleanUpMemory			();
	
	// destroy ai space
	xr_delete(g_ai_space);
	// destroy object factory
	xr_delete(g_object_factory);
	// destroy monster squad global var
	xr_delete(g_monster_squad);


	//info portions static data
	CInfoPortion::DeleteIdToIndexData();
	//static shader for blood
	CEntityAlive::UnloadBloodyWallmarks	();
	CEntityAlive::UnloadFireParticles ();

	//�������� ������ ������� �����
	CStringTable::Destroy();
}