////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_human_brain.cpp
//	Created 	: 06.10.2005
//  Modified 	: 06.10.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife human brain class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_human_brain.h"
#include "alife_human_object_handler.h"
#include "alife_human_movement_manager.h"
#include "alife_human_detail_path_manager.h"
#include "alife_human_patrol_path_manager.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "ai_space.h"
#include "ef_storage.h"
#include "ef_primary.h"
#include "object_broker.h"
#include "alife_simulator.h"
#include "alife_graph_registry.h"
#include "movement_manager_space.h"
#include "alife_smart_terrain_registry.h"
#include "alife_time_manager.h"
#include "date_time.h"

#ifdef DEBUG
#	include "level.h"
#	include "map_location.h"
#	include "map_manager.h"
#endif

#define MAX_ITEM_FOOD_COUNT			3
#define MAX_ITEM_MEDIKIT_COUNT		3
#define MAX_AMMO_ATTACH_COUNT		10

CALifeHumanBrain::CALifeHumanBrain			(object_type *object)
{
	VERIFY							(object);
	m_object						= object;
	m_object_handler				= xr_new<CALifeHumanObjectHandler>(object);
	m_movement_manager				= xr_new<CALifeHumanMovementManager>(object);
	m_last_search_time				= 0;
	
	u32								hours,minutes,seconds;
	sscanf							(pSettings->r_string(this->object().name(),"smart_terrain_choose_interval"),"%d:%d:%d",&hours,&minutes,&seconds);
	m_time_interval					= (u32)generate_time(1,1,1,hours,minutes,seconds);

	m_dwTotalMoney					= 0;
	m_tpKnownCustomers.clear		();
	m_cpEquipmentPreferences.resize	(5);
	m_cpMainWeaponPreferences.resize(4);

#ifdef XRGAME_EXPORTS
	m_cpEquipmentPreferences.resize	(iFloor(ai().ef_storage().m_pfEquipmentType->ffGetMaxResultValue() + .5f));
	m_cpMainWeaponPreferences.resize(iFloor(ai().ef_storage().m_pfMainWeaponType->ffGetMaxResultValue() + .5f));
	R_ASSERT2						((iFloor(ai().ef_storage().m_pfEquipmentType->ffGetMaxResultValue() + .5f) == 5) && (iFloor(ai().ef_storage().m_pfMainWeaponType->ffGetMaxResultValue() + .5f) == 4),"Recompile Level Editor and xrAI and rebuild file \"game.spawn\"!");
#endif

	for (int i=0, n=m_cpEquipmentPreferences.size(); i<n; ++i)
		m_cpEquipmentPreferences[i]	= u8(::Random.randI(3));

	for (int i=0, n=m_cpMainWeaponPreferences.size(); i<n; ++i)
		m_cpMainWeaponPreferences[i]= u8(::Random.randI(3));

//	m_fSearchSpeed					= pSettings->r_float(object->name(), "search_speed");
//	m_fGoingSuccessProbability		= pSettings->r_float(object->name(), "going_item_detect_probability");
//	m_fSearchSuccessProbability		= pSettings->r_float(object->name(), "search_item_detect_probability");
}

CALifeHumanBrain::~CALifeHumanBrain			()
{
	xr_delete						(m_object_handler);
	xr_delete						(m_movement_manager);
}

void CALifeHumanBrain::on_state_write		(NET_Packet &packet)
{
	save_data						(m_tpKnownCustomers,packet);
	save_data						(m_cpEquipmentPreferences,packet);
	save_data						(m_cpMainWeaponPreferences,packet);
}

void CALifeHumanBrain::on_state_read		(NET_Packet &packet)
{
	if (object().m_wVersion <= 19)
		return;

	if (object().m_wVersion < 110) {
		{
			DWORD_VECTOR			temp;
			load_data				(temp,packet);
		}
		{
			xr_vector<bool>			temp;
			load_data				(temp,packet);
		}
	}

	if (object().m_wVersion <= 35)
		return;

	if (object().m_wVersion < 110) {
		shared_str					temp;
		packet.r_stringZ			(temp);
	}

	load_data						(m_tpKnownCustomers,packet);
	load_data						(m_cpEquipmentPreferences,packet);
	load_data						(m_cpMainWeaponPreferences,packet);
}

#ifdef XRGAME_EXPORTS

bool CALifeHumanBrain::perform_attack		()
{
	return							(false);
}

ALife::EMeetActionType CALifeHumanBrain::action_type	(CSE_ALifeSchedulable *tpALifeSchedulable, const int &iGroupIndex, const bool &bMutualDetection)
{
	return							(ALife::eMeetActionTypeIgnore);
}

void CALifeHumanBrain::on_surge				()
{
}

void CALifeHumanBrain::on_register			()
{
}

void CALifeHumanBrain::on_unregister		()
{
}

void CALifeHumanBrain::on_location_change	()
{
}

IC	CSE_ALifeSmartZone &CALifeHumanBrain::smart_terrain	()
{
	VERIFY							(object().m_smart_terrain_id != 0xffff);
	if (m_smart_terrain && (object().m_smart_terrain_id == m_smart_terrain->ID))
		return						(*m_smart_terrain);

	m_smart_terrain					= ai().alife().smart_terrains().object(object().m_smart_terrain_id);
	VERIFY							(m_smart_terrain);
	return							(*m_smart_terrain);
}

void CALifeHumanBrain::process_task			()
{
	CALifeSmartTerrainTask			*task = smart_terrain().task(&object());
	VERIFY							(task);
	movement().path_type			(MovementManager::ePathTypeGamePath);
	movement().detail().target		(*task);
}

void CALifeHumanBrain::select_task			()
{
	if (object().m_smart_terrain_id != 0xffff)
		return;

	ALife::_TIME_ID					current_time = ai().alife().time_manager().game_time();

	if (m_last_search_time + m_time_interval > current_time)
		return;

	m_last_search_time				= current_time;

	float							best_value = -1.f;
	CALifeSmartTerrainRegistry::OBJECTS::const_iterator	I = ai().alife().smart_terrains().objects().begin();
	CALifeSmartTerrainRegistry::OBJECTS::const_iterator	E = ai().alife().smart_terrains().objects().end();
	for ( ; I != E; ++I) {
		if (!(*I).second->enabled(&object()))
			continue;

		float						value = (*I).second->suitable(&object());
		if (value > best_value) {
			best_value				= value;
			object().m_smart_terrain_id	= (*I).second->ID;
		}
	}
}

void CALifeHumanBrain::update				()
{
#if 0//def DEBUG
	if (!Level().MapManager().HasMapLocation("debug_stalker",object().ID)) {
		CMapLocation				*map_location = 
			Level().MapManager().AddMapLocation(
				"debug_stalker",
				object().ID
			);

		map_location->SetHint		(object().name_replace());
	}
#endif

	select_task						();
	
	if (object().m_smart_terrain_id != 0xffff)
		process_task				();
	else
		default_behaviour			();

	movement().update				();
}

void CALifeHumanBrain::default_behaviour	()
{
//	movement().path_type			(MovementManager::ePathTypeGamePath);
//	movement().detail().target		();
	movement().path_type			(MovementManager::ePathTypeNoPath);
}

#endif