#include "stdafx.h"
#include "ai_space.h"
#include "object_factory.h"
#include "ai/monsters/ai_monster_squad_manager.h"
#include "string_table.h"

#include "entity_alive.h"
#include "ui/UIInventoryUtilities.h"
#include "UI/UIXmlInit.h"

#include "InfoPortion.h"
#include "PhraseDialog.h"
#include "GameTask.h"
#include "encyclopedia_article.h"

#include "character_info.h"
#include "specific_character.h"
#include "character_community.h"
#include "character_rank.h"
#include "character_reputation.h"

#include "profiler.h"



extern void show_smart_cast_stats					();
extern void clear_smart_cast_stats					();
extern void release_smart_cast_stats				();

void clean_game_globals()
{
	// xml parser options
	XML_CleanUpMemory								();
	
	// destroy ai space
	xr_delete										(g_ai_space);
	// destroy object factory
	xr_delete										(g_object_factory);
	// destroy monster squad global var
	xr_delete										(g_monster_squad);


	InventoryUtilities::DestroyShaders				();

	//XML indexes
	CInfoPortion::DeleteSharedData					();
	CInfoPortion::DeleteIdToIndexData				();

	CEncyclopediaArticle::DeleteSharedData			();
	CEncyclopediaArticle::DeleteIdToIndexData		();

	CPhraseDialog::DeleteSharedData					();
	CPhraseDialog::DeleteIdToIndexData				();

//	CGameTask::DeleteSharedData						();
//	CGameTask::DeleteIdToIndexData					();


	CCharacterInfo::DeleteSharedData				();
	CCharacterInfo::DeleteIdToIndexData				();
	
	CSpecificCharacter::DeleteSharedData			();
	CSpecificCharacter::DeleteIdToIndexData			();
	
	CHARACTER_COMMUNITY::DeleteIdToIndexData		();
	CHARACTER_RANK::DeleteIdToIndexData				();
	CHARACTER_REPUTATION::DeleteIdToIndexData		();


	//static shader for blood
	CEntityAlive::UnloadBloodyWallmarks				();
	CEntityAlive::UnloadFireParticles				();
	//очищение памяти таблицы строк
	CStringTable::Destroy							();
	// Очищение таблицы цветов
	CUIXmlInit::DeleteColorDefs						();
	// Очищение таблицы идентификаторов рангов и отношений сталкеров
	InventoryUtilities::ClearCharacterInfoStrings	();

#ifdef DEBUG
	xr_delete										(g_profiler);
	release_smart_cast_stats						();
#endif
}