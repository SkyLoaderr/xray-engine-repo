////////////////////////////////////////////////////////////////////////////
//	Module 		: game_spawn_constructor.h
//	Created 	: 16.10.2004
//  Modified 	: 16.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Game spawn constructor
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"
#include "xr_graph_merge.h"
#include "xrthread.h"
#include "graph_abstract.h"
#include "xrServer_Object_Base.h"
#include "spawn_constructor_space.h"
#include "server_entity_wrapper.h"

class CSE_Abstract;
class CLevelSpawnConstructor;

class CGameSpawnConstructor {
	friend class CSpawnMerger;
public:
	typedef SpawnConstructorSpace::LEVEL_POINT_STORAGE							LEVEL_POINT_STORAGE;
	typedef SpawnConstructorSpace::LEVEL_CHANGER_STORAGE						LEVEL_CHANGER_STORAGE;
	typedef CGraphAbstract<CServerEntityWrapper*,float,ALife::_SPAWN_ID,u32>	SPAWN_GRAPH;
	typedef xr_vector<CLevelSpawnConstructor*>									LEVEL_SPAWN_STORAGE;
	typedef xr_set<CLevelInfo>													LEVEL_INFO_STORAGE;

private:
	struct CSpawnHeader {
		u32							m_version;
		u32							m_spawn_count;
		u32							m_level_count;
	};

private:
	xrCriticalSection				m_critical_section;
	ALife::_SPAWN_ID				m_spawn_id;
	CThreadManager					m_thread_manager;
	CSpawnHeader					m_spawn_header;
	ALife::STORY_P_MAP				m_story_objects;
	LEVEL_INFO_STORAGE				m_levels;
	LEVEL_SPAWN_STORAGE				m_level_spawns;
	LEVEL_CHANGER_STORAGE			m_level_changers;
	LEVEL_POINT_STORAGE				m_level_points;
	CGameGraph						*m_game_graph;
	SPAWN_GRAPH						*m_spawn_graph;
	CInifile						*m_game_info;

protected:
	IC		shared_str				actor_level_name		();
	IC		shared_str				spawn_name				(LPCSTR output);
			void					save_spawn				(LPCSTR name, LPCSTR output);
			void					verify_level_changers	();
			void					process_spawns			();
			void					load_spawns				(LPCSTR name);
	IC		SPAWN_GRAPH				&spawn_graph			();
	IC		ALife::_SPAWN_ID		spawn_id				();

public:
									CGameSpawnConstructor	(LPCSTR name, LPCSTR output);
	virtual							~CGameSpawnConstructor	();
			void					add_story_object		(ALife::_STORY_ID id,CSE_ALifeDynamicObject *object, LPCSTR level_name);
			void					add_object				(CSE_Abstract *object);
	IC		void					add_level_changer		(CSE_ALifeLevelChanger *level_changer);
	IC		void					add_level_points		(const LEVEL_POINT_STORAGE &level_points);
	IC		u32						level_id				(LPCSTR level_name);
	IC		const CGameGraph		&game_graph				();
	IC		CInifile				&game_info				();
	IC		void					add_edge				(ALife::_SPAWN_ID id0, ALife::_SPAWN_ID id1, float weight);
	IC		u32						level_point_count		() const;
	IC		LEVEL_CHANGER_STORAGE	&level_changers			();
};

#include "game_spawn_constructor_inline.h"