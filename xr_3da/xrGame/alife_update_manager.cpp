////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_update_manager.h
//	Created 	: 25.12.2002
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator update manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_update_manager.h"
#include "alife_simulator_header.h"
#include "alife_time_manager.h"
#include "alife_graph_registry.h"
#include "alife_schedule_registry.h"
#include "alife_news_registry.h"
#include "alife_spawn_registry.h"
#include "alife_object_registry.h"
#include "ef_storage.h"
#include "xrserver.h"
#include "level.h"
#include "graph_engine.h"
#include "../x_ray.h"
#include "restriction_space.h"
#include "profiler.h"

using namespace ALife;

class CSwitchPredicate {
private:
	CALifeSwitchManager *m_switch_manager;

public:
	IC			CSwitchPredicate(CALifeSwitchManager *switch_manager)
	{
		m_switch_manager			= switch_manager;
	}

	IC	bool	operator()		(CALifeLevelRegistry::_iterator &i, u64 cycle_count, bool) const
	{
		if ((*i).second->m_switch_counter	== cycle_count)
			return					(false);

		(*i).second->m_switch_counter	= cycle_count;
		return						(true);
	}

	IC	void	operator()		(CALifeLevelRegistry::_iterator &i, u64 cycle_count) const
	{
		m_switch_manager->switch_object((*i).second);
	}
};

CALifeUpdateManager::CALifeUpdateManager	(xrServer *server, LPCSTR section) :
	CALifeSwitchManager		(server,section),
	CALifeSurgeManager		(server,section),
	CALifeStorageManager	(server,section),
	CALifeSimulatorBase		(server,section)
{
	shedule.t_min			= pSettings->r_s32	(section,"schedule_min");
	shedule.t_max			= pSettings->r_s32	(section,"schedule_max");
	shedule_register		();
	m_max_process_time		= pSettings->r_s32	(section,"process_time");
	m_update_monster_factor	= pSettings->r_float(section,"update_monster_factor");
	m_changing_level		= false;
}

CALifeUpdateManager::~CALifeUpdateManager	()
{
	shedule_unregister		();
}

float CALifeUpdateManager::shedule_Scale	()
{
	return					(.5f); // (schedule_min + schedule_max)*0.5f
}

BOOL CALifeUpdateManager::shedule_Ready		()
{
	return					(TRUE);
}

void CALifeUpdateManager::shedule_Update	(u32 dt)
{
	ISheduled::shedule_Update		(dt);

	Device.Statistic.TEST3.Begin	();
	if (!initialized()) {
		Device.Statistic.TEST3.End	();
		return;
	}

	update							(true);

	Device.Statistic.TEST3.End		();
}

void CALifeUpdateManager::set_process_time	(int microseconds)
{
	graph().set_process_time		(u64(float(microseconds) - float(microseconds)*update_monster_factor())*CPU::cycles_per_microsec);
	scheduled().set_process_time	(u64(float(microseconds)*update_monster_factor())*CPU::cycles_per_microsec);
}

void CALifeUpdateManager::init_ef_storage() const
{
	ai().ef_storage().alife_evaluation(true);
}

bool CALifeUpdateManager::change_level	(NET_Packet &net_packet)
{
	if (m_changing_level)
		return					(false);

	prepare_objects_for_save	();

	m_changing_level			= true;
	
	GameGraph::_GRAPH_ID			safe_graph_vertex_id	= graph().actor()->m_tGraphID;
	u32							safe_level_vertex_id	= graph().actor()->m_tNodeID;
	Fvector						safe_position			= graph().actor()->o_Position;
	Fvector						safe_angles				= graph().actor()->o_Angle;
	
	GameGraph::_GRAPH_ID			holder_safe_graph_vertex_id = GameGraph::_GRAPH_ID(-1);
	u32							holder_safe_level_vertex_id = u32(-1);
	Fvector						holder_safe_position = Fvector().set(flt_max,flt_max,flt_max);
	Fvector						holder_safe_angles = Fvector().set(flt_max,flt_max,flt_max);
	CSE_ALifeObject				*holder = 0;

	net_packet.r				(&graph().actor()->m_tGraphID,sizeof(graph().actor()->m_tGraphID));
	net_packet.r				(&graph().actor()->m_tNodeID,sizeof(graph().actor()->m_tNodeID));
	net_packet.r_vec3			(graph().actor()->o_Position);
	net_packet.r_vec3			(graph().actor()->o_Angle);

	if (graph().actor()->m_holderID != 0xffff) {
		holder						= objects().object(graph().actor()->m_holderID);

		holder_safe_graph_vertex_id	= holder->m_tGraphID;
		holder_safe_level_vertex_id	= holder->m_tNodeID;
		holder_safe_position		= holder->o_Position;
		holder_safe_angles			= holder->o_Angle;

		holder->m_tGraphID			= graph().actor()->m_tGraphID;
		holder->m_tNodeID			= graph().actor()->m_tNodeID;
		holder->o_Position			= graph().actor()->o_Position;
		holder->o_Angle				= graph().actor()->o_Angle;
	}

	save							();

	graph().actor()->m_tGraphID		= safe_graph_vertex_id;
	graph().actor()->m_tNodeID		= safe_level_vertex_id;
	graph().actor()->o_Position		= safe_position;
	graph().actor()->o_Angle		= safe_angles;

	if (graph().actor()->m_holderID != 0xffff) {
		VERIFY						(holder);
		holder->m_tGraphID			= holder_safe_graph_vertex_id;
		holder->m_tNodeID			= holder_safe_level_vertex_id;
		holder->o_Position			= holder_safe_position;
		holder->o_Angle				= holder_safe_angles;
	}

	return							(true);
}

void CALifeUpdateManager::update(bool switch_objects)
{
	init_ef_storage						();

	switch (header().state()) {
		case eZoneStateSurge : {
			surge						();
			header().set_state			(eZoneStateBetweenSurges);
			break;
		}
		case eZoneStateBetweenSurges : {
			if (time_manager().game_time() > time_manager().next_surge_time()) {
				header().set_state		(eZoneStateSurge);

				if (switch_objects) {
					Msg						("SURGE started");
					NET_Packet				net_packet;
					net_packet.w_begin		(M_CHANGE_LEVEL);
					net_packet.w			(&graph().actor()->m_tGraphID,sizeof(graph().actor()->m_tGraphID));
					net_packet.w			(&graph().actor()->m_tNodeID,sizeof(graph().actor()->m_tNodeID));
					net_packet.w_vec3		(graph().actor()->o_Position);
					net_packet.w_vec3		(graph().actor()->o_Angle);
					Level().Send			(net_packet,net_flags(TRUE));
					return;
				}
//				for ( ;!graph().level().objects().empty(); )
//					furl_object			((*graph().level().objects().begin()).second);
				break;
			}
			
			START_PROFILE("AI/ALife/switch");
			if (switch_objects)
				graph().level().update	(CSwitchPredicate(this));
			STOP_PROFILE

			START_PROFILE("AI/ALife/spawn");
			spawns().update				();
			STOP_PROFILE

			START_PROFILE("AI/ALife/scheduled");
			scheduled().update			();
			STOP_PROFILE

			break;
		}
		default : NODEFAULT;
	}
}

void CALifeUpdateManager::new_game			(LPCSTR save_name)
{
	Msg									("* Creating new game...");

	unload								();
	reload								(m_section);

	spawns().load						(save_name);

	server().PerformIDgen				(0x0000);

	time_manager().init					(m_section);
	header().set_state					(eZoneStateSurge);

	_TIME_ID							finish_time = time_manager().game_time() + 0*120000;//3*7*24*3600*1000; // 3 weeks in milliseconds
	float								time_factor = time_manager().time_factor();
	
	time_manager().set_time_factor		(1.f);

	do {
		update							(false);
	}
	while (time_manager().game_time() < finish_time);

	time_manager().set_time_factor		(time_factor);

	save								(save_name);

	Msg									("* New game is successfully created!");
}

void CALifeUpdateManager::load			(LPCSTR game_name, bool no_assert, bool new_only)
{
	pApp->LoadTitle						("SERVER: Loading alife simulator...");

	Memory.mem_compact					();
	u32									memory_usage = Memory.mem_usage();

	if (new_only || !CALifeStorageManager::load(game_name)) {
		R_ASSERT3						(new_only || no_assert && xr_strlen(game_name),"Cannot find the specified saved game ",game_name);
		new_game						(game_name);
	}

	Msg									("* Loading alife simulator is successfully completed (%7.3f Mb)",float(Memory.mem_usage() - memory_usage)/1048576.0);
	pApp->LoadTitle						("SERVER: Connecting...");
}

void CALifeUpdateManager::reload		(LPCSTR section)
{
	CALifeSimulatorBase::reload			(section);
	set_process_time					((int)m_max_process_time);
}

bool CALifeUpdateManager::load_game		(LPCSTR game_name, bool no_assert)
{
	{
		string256				temp,file_name;
		strconcat				(temp,game_name,SAVE_EXTENSION);
		FS.update_path			(file_name,"$game_saves$",temp);
		if (!FS.exist(file_name)) {
			R_ASSERT3			(no_assert,"There is no saved game ",file_name);
			return				(false);
		}
	}
	string512					S,S1;
	strcpy						(S,**m_server_command_line);
	LPSTR						temp = strchr(S,'/');
	R_ASSERT2					(temp,"Invalid server options!");
	strconcat					(S1,game_name,temp);
	*m_server_command_line		= S1;
	return						(true);
}

void CALifeUpdateManager::set_switch_online		(ALife::_OBJECT_ID id, bool value)
{
	CSE_ALifeDynamicObject			*object = objects().object(id);
	unregister_object				(object,object->m_bOnline);
	object->m_alife_simulator		= 0;
	object->can_switch_online		(value);
	register_object					(object,true);
}

void CALifeUpdateManager::set_switch_offline	(ALife::_OBJECT_ID id, bool value)
{
	CSE_ALifeDynamicObject			*object = objects().object(id);
	unregister_object				(object,object->m_bOnline);
	object->m_alife_simulator		= 0;
	object->can_switch_offline		(value);
	register_object					(object,true);
}

void CALifeUpdateManager::set_interactive		(ALife::_OBJECT_ID id, bool value)
{
	CSE_ALifeDynamicObject			*object = objects().object(id);
	unregister_object				(object,object->m_bOnline);
	object->m_alife_simulator		= 0;
	object->interactive				(value);
	register_object					(object,true);
}

void CALifeUpdateManager::jump_to_level			(LPCSTR level_name) const
{
	const CGameGraph::SLevel		&level = ai().game_graph().header().level(level_name);
	GameGraph::_GRAPH_ID				dest = GameGraph::_GRAPH_ID(-1);
	GraphEngineSpace::CGameLevelParams	evaluator(level.id());
	bool							failed = !ai().graph_engine().search(ai().game_graph(),graph().actor()->m_tGraphID,GameGraph::_GRAPH_ID(-1),0,evaluator);
	if (failed) {
		Msg							("! Cannot build path via game graph from the current level to the level %s!",level_name);
		float						min_dist = flt_max;
		Fvector						current = ai().game_graph().vertex(graph().actor()->m_tGraphID)->game_point();
		GameGraph::_GRAPH_ID			n = ai().game_graph().header().vertex_count();
		for (GameGraph::_GRAPH_ID i=0; i<n; ++i)
			if (ai().game_graph().vertex(i)->level_id() == level.id()) {
				float				distance = ai().game_graph().vertex(i)->game_point().distance_to_sqr(current);
				if (distance < min_dist) {
					min_dist		= distance;
					dest			= i;
				}
			}
		if (!ai().game_graph().vertex(dest)) {
			Msg						("! There is no game vertices on the level %s, cannot jump to the specified level",level_name);
			return;
		}
	}
	else
		dest						= (GameGraph::_GRAPH_ID)evaluator.selected_vertex_id();
	NET_Packet						net_packet;
	net_packet.w_begin				(M_CHANGE_LEVEL);
	net_packet.w					(&dest,sizeof(dest));
	
	u32								vertex_id = ai().game_graph().vertex(dest)->level_vertex_id();
	net_packet.w					(&vertex_id,sizeof(vertex_id));
	
	Fvector							level_point = ai().game_graph().vertex(dest)->level_point();
	net_packet.w					(&level_point,sizeof(level_point));
	net_packet.w_vec3				(Fvector().set(0.f,0.f,0.f));
	Level().Send					(net_packet,net_flags(TRUE));
}

void CALifeUpdateManager::teleport_object	(ALife::_OBJECT_ID id, GameGraph::_GRAPH_ID game_vertex_id, u32 level_vertex_id, const Fvector &position)
{
	CSE_ALifeDynamicObject					*object = objects().object(id,true);
	if (!object) {
		Msg									("! cannot teleport entity with id %d",id);
		return;
	}
	
	if (object->m_bOnline)
		switch_offline						(object);
	graph().change							(object,object->m_tGraphID,game_vertex_id);
	object->m_tNodeID						= level_vertex_id;
	object->o_Position						= position;
	CSE_ALifeMonsterAbstract				*monster_abstract = smart_cast<CSE_ALifeMonsterAbstract*>(object);
	if (monster_abstract)
		monster_abstract->m_tNextGraphID	= object->m_tGraphID;
}

void CALifeUpdateManager::add_restriction	(ALife::_OBJECT_ID id, ALife::_OBJECT_ID restriction_id, const RestrictionSpace::ERestrictorTypes &restriction_type)
{
	CSE_ALifeDynamicObject					*object = objects().object(id,true);
	if (!object) {
		Msg									("! cannot add restriction with id %d to the entity with id %d, because there is no creature with the specified id",restriction_id,id);
		return;
	}
	
	CSE_ALifeDynamicObject					*object_restrictor = objects().object(restriction_id,true);
	if (!object_restrictor) {
		Msg									("! cannot add restriction with id %d to the entity with id %d, because there is no space restrictor with the specified id",restriction_id,id);
		return;
	}

	CSE_ALifeCreatureAbstract				*creature = smart_cast<CSE_ALifeCreatureAbstract*>(object);
	if (!creature) {
		Msg									("! cannot add restriction with id %d to the entity with id %d, because there is an object with the specified id, but it is not a creature",restriction_id,id);
		return;
	}
	
	CSE_ALifeSpaceRestrictor				*restrictor = smart_cast<CSE_ALifeSpaceRestrictor*>(object_restrictor);
	if (!restrictor) {
		Msg									("! cannot add restriction with id %d to the entity with id %d, because there is an object with the specified id, but it is not a space restrictor",restriction_id,id);
		return;
	}

	switch (restriction_type) {
		case RestrictionSpace::eRestrictorTypeOut : {
			if (std::find(creature->m_dynamic_out_restrictions.begin(),creature->m_dynamic_out_restrictions.end(),restriction_id) != creature->m_dynamic_out_restrictions.end()) {
				Msg							("! cannot add out-restriction with id %d, name %s to the entity with id %d, name %s, because it is already added",restriction_id,restrictor->name_replace(),id,creature->name_replace());
				return;
			}

			creature->m_dynamic_out_restrictions.push_back(restriction_id);

			break;
		}
		case RestrictionSpace::eRestrictorTypeIn : {
			if (std::find(creature->m_dynamic_in_restrictions.begin(),creature->m_dynamic_in_restrictions.end(),restriction_id) != creature->m_dynamic_in_restrictions.end()) {
				Msg							("! cannot add in-restriction with id %d, name %s to the entity with id %d, name %s, because it is already added",restriction_id,restrictor->name_replace(),id,creature->name_replace());
				return;
			}

			creature->m_dynamic_in_restrictions.push_back(restriction_id);

			break;
		}
		default :  {
			Msg								("! Invalid restriction type!");
			return;
		}
	}
}

void CALifeUpdateManager::remove_restriction(ALife::_OBJECT_ID id, ALife::_OBJECT_ID restriction_id, const RestrictionSpace::ERestrictorTypes &restriction_type)
{
	CSE_ALifeDynamicObject					*object = objects().object(id,true);
	if (!object) {
		Msg									("! cannot remove restriction with id %d to the entity with id %d, because there is no creature with the specified id",restriction_id,id);
		return;
	}
	
	CSE_ALifeDynamicObject					*object_restrictor = objects().object(restriction_id,true);
	if (!object_restrictor) {
		Msg									("! cannot remove restriction with id %d to the entity with id %d, because there is no space restrictor with the specified id",restriction_id,id);
		return;
	}

	CSE_ALifeCreatureAbstract				*creature = smart_cast<CSE_ALifeCreatureAbstract*>(object);
	if (!creature) {
		Msg									("! cannot remove restriction with id %d to the entity with id %d, because there is an object with the specified id, but it is not a creature",restriction_id,id);
		return;
	}
	
	CSE_ALifeSpaceRestrictor				*restrictor = smart_cast<CSE_ALifeSpaceRestrictor*>(object_restrictor);
	if (!restrictor) {
		Msg									("! cannot remove restriction with id %d to the entity with id %d, because there is an object with the specified id, but it is not a space restrictor",restriction_id,id);
		return;
	}

	switch (restriction_type) {
		case RestrictionSpace::eRestrictorTypeOut : {
			xr_vector<ALife::_OBJECT_ID>::iterator	I = std::find(creature->m_dynamic_out_restrictions.begin(),creature->m_dynamic_out_restrictions.end(),restriction_id);
			if (I == creature->m_dynamic_out_restrictions.end()) {
				Msg							("! cannot remove restriction with id %d to the entity with id %d, because it is not added",restriction_id,id);
				return;
			}

			creature->m_dynamic_out_restrictions.erase(I);

			break;
		}
		case RestrictionSpace::eRestrictorTypeIn : {
			xr_vector<ALife::_OBJECT_ID>::iterator	I = std::find(creature->m_dynamic_in_restrictions.begin(),creature->m_dynamic_in_restrictions.end(),restriction_id);
			if (I == creature->m_dynamic_in_restrictions.end()) {
				Msg							("! cannot remove restriction with id %d to the entity with id %d, because it is not added",restriction_id,id);
				return;
			}

			creature->m_dynamic_in_restrictions.erase(I);

			break;
		}
		default :  {
			Msg								("! Invalid restriction type!");
			return;
		}
	}
}
