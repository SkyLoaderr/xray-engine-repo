#include "stdafx.h"
#include "ai_space.h"
#include "object_factory.h"
#include "ai/ai_monster_squad_manager.h"
#include "string_table.h"
#include "InfoPortion.h"
#include "entity_alive.h"
#include "profiler.h"
#include "ui/UIInventoryUtilities.h"
#include "UI/UIXmlInit.h"

void clean_game_globals()
{
	// xml parser options
	XML_CleanUpMemory					();
	
	// destroy ai space
	xr_delete							(g_ai_space);
	// destroy object factory
	xr_delete							(g_object_factory);
	// destroy monster squad global var
	xr_delete							(g_monster_squad);


	InventoryUtilities::DestroyShaders	();

	//info portions static data
	CInfoPortion::DeleteIdToIndexData	();
	//static shader for blood
	CEntityAlive::UnloadBloodyWallmarks	();
	CEntityAlive::UnloadFireParticles	();

	//очищение памяти таблицы строк
	CStringTable::Destroy				();
	
	// Очищение таблицы цветов
	CUIXmlInit::DeleteColorDefs			();

	xr_delete							(g_profiler);
}