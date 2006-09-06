////////////////////////////////////////////////////////////////////////////
//	Module 		: saved_game_wrapper.cpp
//	Created 	: 21.02.2006
//  Modified 	: 21.02.2006
//	Author		: Dmitriy Iassenev
//	Description : saved game wrapper class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "saved_game_wrapper.h"
#include "alife_time_manager.h"
#include "alife_object_registry.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "ai_space.h"
#include "game_graph.h"

extern LPCSTR alife_section;

CSavedGameWrapper::CSavedGameWrapper	(LPCSTR saved_game_name)
{
	string256					temp,file_name;
	strconcat					(temp,saved_game_name,SAVE_EXTENSION);
	FS.update_path				(file_name,"$game_saves$",temp);
	R_ASSERT3					(FS.exist(file_name),"There is no saved game ",file_name);
	
	IReader						*reader = FS.r_open(file_name);

	{
		CALifeTimeManager		time_manager(alife_section);
		time_manager.load		(*reader);
		m_game_time				= time_manager.game_time();
	}

	{
		R_ASSERT2				(reader->find_chunk(OBJECT_CHUNK_DATA),"Can't find chunk OBJECT_CHUNK_DATA!");
		u32						count = reader->r_u32();
		VERIFY					(count > 0);
		CSE_ALifeDynamicObject	*object = CALifeObjectRegistry::get_object(*reader);
		VERIFY					(object->ID == 0);
		CSE_ALifeCreatureActor	*actor = smart_cast<CSE_ALifeCreatureActor*>(object);
		VERIFY					(actor);

		m_actor_health			= actor->g_Health();
		m_level_id				= ai().game_graph().vertex(object->m_tGraphID)->level_id();

		F_entity_Destroy		(object);
	}

	FS.r_close					(reader);
}
