////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_simulator_base.cpp
//	Created 	: 25.12.2002
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator base class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_simulator_base.h"
#include "alife_simulator_header.h"
#include "alife_time_manager.h"
#include "alife_spawn_registry.h"
#include "alife_object_registry.h"
#include "alife_event_registry.h"
#include "alife_task_registry.h"
#include "alife_graph_registry.h"
#include "alife_trader_registry.h"
#include "alife_schedule_registry.h"
#include "alife_anomaly_registry.h"
#include "alife_organization_registry.h"
#include "alife_news_registry.h"
#include "alife_story_registry.h"
#include "alife_registry_container.h"
#include "level.h"
#include "xrserver.h"
#include "level_graph.h"

using namespace ALife;

CALifeSimulatorBase::CALifeSimulatorBase	(xrServer *server, LPCSTR section)
{
	m_server					= server;
	m_initialized				= false;
	m_header					= 0;
	m_time_manager				= 0;
	m_spawns					= 0;
	m_objects					= 0;
	m_events					= 0;
	m_tasks						= 0;
	m_graph_objects				= 0;
	m_traders					= 0;
	m_scheduled					= 0;
	m_anomalies					= 0;
	m_organizations				= 0;
	m_news						= 0;
	m_story_objects				= 0;
	m_registry_container		= 0;
	random().seed						(u32(CPU::GetCycleCount() & 0xffffffff));
}

CALifeSimulatorBase::~CALifeSimulatorBase	()
{
	unload						();
}

void CALifeSimulatorBase::unload			()
{
//	if (initialized()) {
//		ALife::D_OBJECT_P_MAP::const_iterator	I = objects().objects().begin();
//		ALife::D_OBJECT_P_MAP::const_iterator	E = objects().objects().end();
//		for ( ; I != E; ++I)
//			if ((*I).second->m_bOnline) {
//				CSE_Abstract			*object = smart_cast<CSE_Abstract*>((*I).second);
//				VERIFY					(object);
//				server().entity_Destroy	(object);
//			}
//	}

	m_initialized				= false;
	xr_delete					(m_header);
	xr_delete					(m_time_manager);
	xr_delete					(m_spawns);
	xr_delete					(m_objects);
	xr_delete					(m_events); 
	xr_delete					(m_tasks); 
	xr_delete					(m_graph_objects);
	xr_delete					(m_traders);
	xr_delete					(m_scheduled);
	xr_delete					(m_anomalies);
	xr_delete					(m_organizations);
	xr_delete					(m_news);
	xr_delete					(m_story_objects);
	xr_delete					(m_registry_container);
}

void CALifeSimulatorBase::reload			(LPCSTR section)
{
	m_header					= xr_new<CALifeSimulatorHeader>		(section);
	m_time_manager				= xr_new<CALifeTimeManager>			(section);
	m_spawns					= xr_new<CALifeSpawnRegistry>		(section);
	m_objects					= xr_new<CALifeObjectRegistry>		(section);
	m_events					= xr_new<CALifeEventRegistry>		(section); 
	m_tasks						= xr_new<CALifeTaskRegistry>		(section); 
	m_anomalies					= xr_new<CALifeAnomalyRegistry>		(section);
	m_organizations				= xr_new<CALifeOrganizationRegistry>(section);
	m_news						= xr_new<CALifeNewsRegistry>		(section);
	m_graph_objects				= xr_new<CALifeGraphRegistry>		(server_command_line());
	m_traders					= xr_new<CALifeTraderRegistry>		();
	m_scheduled					= xr_new<CALifeScheduleRegistry>	();
	m_story_objects				= xr_new<CALifeStoryRegistry>		();
	m_registry_container		= xr_new<CALifeRegistryContainer>	();
	m_initialized				= true;
}

CSE_Abstract *CALifeSimulatorBase::spawn_item	(LPCSTR section, const Fvector &position, u32 level_vertex_id, _GRAPH_ID game_vertex_id, u16 parent_id)
{
	CSE_Abstract				*abstract = F_entity_Create(section);
	R_ASSERT3					(abstract,"Cannot find item with section %s",section);

	abstract->s_name			= section;
	abstract->s_gameid			= u8(GAME_SINGLE); // GameID()
	abstract->s_RP				= 0xff;
	abstract->ID				= server().PerformIDgen(0xffff);
	abstract->ID_Parent			= parent_id;
	abstract->ID_Phantom		= 0xffff;
	abstract->o_Position		= position;
	
	string256					s_name_replace;
	strcpy						(s_name_replace,*abstract->s_name);
	if (abstract->ID < 1000)
		strcat					(s_name_replace,"0");
	if (abstract->ID < 100)
		strcat					(s_name_replace,"0");
	if (abstract->ID < 10)
		strcat					(s_name_replace,"0");
	string16					S1;
	strcat						(s_name_replace,itoa(abstract->ID,S1,10));
	abstract->set_name_replace	(s_name_replace);


	CSE_ALifeDynamicObject		*dynamic_object = smart_cast<CSE_ALifeDynamicObject*>(abstract);
	VERIFY						(dynamic_object);

	//оружие спавним с полным магазинои
	CSE_ALifeItemWeapon* weapon = smart_cast<CSE_ALifeItemWeapon*>(dynamic_object);
	if(weapon)
		weapon->a_elapsed = weapon->get_ammo_magsize();


	dynamic_object->m_tNodeID	= level_vertex_id;
	dynamic_object->m_tGraphID	= game_vertex_id;
	dynamic_object->m_tSpawnID	= u16(-1);
	register_object				(dynamic_object,true);

	return						(dynamic_object);
}

CSE_Abstract *CALifeSimulatorBase::create(CSE_ALifeGroupAbstract *tpALifeGroupAbstract, CSE_ALifeDynamicObject *j)
{
	NET_Packet					tNetPacket;
	LPCSTR						S = pSettings->r_string(tpALifeGroupAbstract->base()->s_name,"monster_section");
	CSE_Abstract				*l_tpAbstract = F_entity_Create(S);
	R_ASSERT2					(l_tpAbstract,"Can't create entity.");
	CSE_ALifeDynamicObject		*k = smart_cast<CSE_ALifeDynamicObject*>(l_tpAbstract);
	R_ASSERT2					(k,"Non-ALife object in the 'game.spawn'");

	j->Spawn_Write				(tNetPacket,TRUE);
	k->Spawn_Read				(tNetPacket);
	tNetPacket.w_begin			(M_UPDATE);
	j->UPDATE_Write				(tNetPacket);
	u16							id;
	tNetPacket.r_begin			(id);
	k->UPDATE_Read				(tNetPacket);
	k->s_name					= S;
	k->m_tSpawnID				= j->m_tSpawnID;
	k->ID						= server().PerformIDgen(0xffff);
	k->m_bDirectControl			= false;
	k->m_bALifeControl			= true;
	
	string256					s_name_replace;
	strcpy						(s_name_replace,*k->s_name);
	if (k->ID < 1000)
		strcat					(s_name_replace,"0");
	if (k->ID < 100)
		strcat					(s_name_replace,"0");
	if (k->ID < 10)
		strcat					(s_name_replace,"0");
	string16					S1;
	strcat						(s_name_replace,itoa(k->ID,S1,10));
	k->set_name_replace			(s_name_replace);

	register_object				(k,true);
	k->spawn_supplies			();
	return						(k);
}

void CALifeSimulatorBase::create(CSE_ALifeDynamicObject *&i, CSE_ALifeDynamicObject *j, const _SPAWN_ID &tSpawnID)
{
	CSE_Abstract				*tpSE_Abstract = F_entity_Create(*j->s_name);
	R_ASSERT2					(tpSE_Abstract,"Can't create entity.");
	i							= smart_cast<CSE_ALifeDynamicObject*>(tpSE_Abstract);
	R_ASSERT2					(i,"Non-ALife object in the 'game.spawn'");

	NET_Packet					tNetPacket;
	j->Spawn_Write				(tNetPacket,TRUE);
	i->Spawn_Read				(tNetPacket);
	tNetPacket.w_begin			(M_UPDATE);
	j->UPDATE_Write				(tNetPacket);
	u16							id;
	tNetPacket.r_begin			(id);
	i->UPDATE_Read				(tNetPacket);

	R_ASSERT3					(!(i->used_ai_locations()) || (i->m_tNodeID != u32(-1)),"Invalid vertex for object ",i->name_replace());

	i->m_tSpawnID				= tSpawnID;
	if (!graph().actor() && smart_cast<CSE_ALifeCreatureActor*>(i))
		i->ID					= 0;
	else
		i->ID					= server().PerformIDgen(0xffff);

	register_object				(i,true);
	i->m_bALifeControl			= true;

	CSE_ALifeMonsterAbstract	*monster	= smart_cast<CSE_ALifeMonsterAbstract*>(i);
	if (monster)
		graph().assign			(monster);

	CSE_ALifeGroupAbstract		*group = smart_cast<CSE_ALifeGroupAbstract*>(i);
	if (group) {
		group->m_tpMembers.resize(group->m_wCount);
		OBJECT_IT				I = group->m_tpMembers.begin();
		OBJECT_IT				E = group->m_tpMembers.end();
		for ( ; I != E; ++I) {
			CSE_Abstract		*object = create	(group,j);
			*I					= object->ID;
		}
	}
	else
		i->spawn_supplies		();
}

void CALifeSimulatorBase::create	(CSE_ALifeObject *object)
{
	CSE_ALifeDynamicObject		*dynamic_object = smart_cast<CSE_ALifeDynamicObject*>(object);
	if (!dynamic_object)
		return;
	
	VERIFY						(dynamic_object->m_bOnline);

	if (0xffff != dynamic_object->ID_Parent) {
		u16							id = dynamic_object->ID_Parent;
		CSE_ALifeDynamicObject		*parent = objects().object(id);
		VERIFY						(parent);
		dynamic_object->m_tGraphID	= parent->m_tGraphID;
		dynamic_object->o_Position	= parent->o_Position;
		dynamic_object->m_tNodeID	= parent->m_tNodeID;
		dynamic_object->ID_Parent	= 0xffff;
		register_object				(dynamic_object,true);
		dynamic_object->ID_Parent	= id;
	}
	else
		register_object				(dynamic_object,true);
}

void CALifeSimulatorBase::unregister_object	(CSE_ALifeDynamicObject *object, bool alife_query)
{
	CSE_ALifeInventoryItem			*item = smart_cast<CSE_ALifeInventoryItem*>(object);
	if (item && item->attached())
		graph().detach				(*objects().object(item->base()->ID_Parent),item,objects().object(item->base()->ID_Parent)->m_tGraphID,alife_query);

	objects().remove				(object->ID);
	story_objects().remove			(object->m_story_id);

	if (!object->m_bOnline) {
		graph().remove				(object,object->m_tGraphID);
		scheduled().remove			(object);
	}
	else
		if (object->ID_Parent == 0xffff)
			graph().level().remove	(object);
}

void CALifeSimulatorBase::release	(CSE_Abstract *abstract, bool alife_query)
{
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg							("[LSS] Releasing object [%s][%s][%d][%x]",abstract->name_replace(),abstract->s_name,abstract->ID,smart_cast<void*>(abstract));
	}
#endif
	CSE_ALifeDynamicObject			*object = objects().object(abstract->ID);
	VERIFY							(object);
	
	unregister_object				(object,alife_query);
	
	object->m_bALifeControl			= false;

	if (alife_query)
		server().entity_Destroy		(abstract);
}

void CALifeSimulatorBase::register_object	(CSE_ALifeDynamicObject *object, bool add_object)
{
	if (add_object)
		objects().add					(object);
	
	graph().update						(object);
	traders().add						(object);
	scheduled().add						(object);
	story_objects().add					(object->m_story_id,object);

	setup_simulator						(object);
	
	CSE_ALifeInventoryItem				*item = smart_cast<CSE_ALifeInventoryItem*>(object);
	if (item && item->attached()) {
		CSE_ALifeDynamicObject			*II = objects().object(item->base()->ID_Parent);
#ifdef DEBUG
		if (std::find(II->children.begin(),II->children.end(),item->base()->ID) != II->children.end()) {
//			if (psAI_Flags.test(aiALife)) {
				Msg						("[LSS] Specified item [%s][%d] is already attached to the specified object [%s][%d]",item->base()->name_replace(),item->base()->ID,II->name_replace(),II->ID);
//			}
			Debug.fatal					("[LSS] Cannot recover from the previous error!");
		}
#endif
		II->children.push_back			(item->base()->ID);
	}
}

void CALifeSimulatorBase::append_item_vector(OBJECT_VECTOR &tObjectVector, ITEM_P_VECTOR &tItemList)
{
	OBJECT_IT	I = tObjectVector.begin();
	OBJECT_IT	E = tObjectVector.end();
	for ( ; I != E; ++I) {
		CSE_ALifeInventoryItem *l_tpALifeInventoryItem = smart_cast<CSE_ALifeInventoryItem*>(objects().object(*I));
		if (l_tpALifeInventoryItem)
			tItemList.push_back		(l_tpALifeInventoryItem);
	}
}

void CALifeSimulatorBase::assign_death_position(CSE_ALifeCreatureAbstract *tpALifeCreatureAbstract, _GRAPH_ID tGraphID, CSE_ALifeSchedulable *tpALifeSchedulable)
{
	tpALifeCreatureAbstract->fHealth		= 0;
	
	if (tpALifeSchedulable) {
		CSE_ALifeAnomalousZone				*l_tpALifeAnomalousZone = smart_cast<CSE_ALifeAnomalousZone*>(tpALifeSchedulable);
		if (l_tpALifeAnomalousZone) {
			spawns().assign_artefact_position(l_tpALifeAnomalousZone,tpALifeCreatureAbstract);
			CSE_ALifeMonsterAbstract		*l_tpALifeMonsterAbstract = smart_cast<CSE_ALifeMonsterAbstract*>(tpALifeCreatureAbstract);
			if (l_tpALifeMonsterAbstract)
				l_tpALifeMonsterAbstract->m_tPrevGraphID = l_tpALifeMonsterAbstract->m_tNextGraphID = l_tpALifeMonsterAbstract->m_tGraphID;
			return;
		}
	}

	CGameGraph::const_spawn_iterator		i, e;
	ai().game_graph().begin_spawn			(tGraphID,i,e);
	i										+= (e != i) ? random().random(s32(e - i)) : 0;
	tpALifeCreatureAbstract->m_tGraphID		= tGraphID;
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg									("[LSS] Generated death position %s[%f][%f][%f] -> [%f][%f][%f]",tpALifeCreatureAbstract->name_replace(),VPUSH(tpALifeCreatureAbstract->o_Position),VPUSH((*i).level_point()));
	}
#endif
	tpALifeCreatureAbstract->o_Position		= (*i).level_point();
	tpALifeCreatureAbstract->m_tNodeID		= (*i).level_vertex_id();
	R_ASSERT2								((ai().game_graph().vertex(tGraphID)->level_id() != graph().level().level_id()) || ai().level_graph().valid_vertex_id(tpALifeCreatureAbstract->m_tNodeID),"Invalid vertex");
	tpALifeCreatureAbstract->m_fDistance	= (*i).distance();
	CSE_ALifeMonsterAbstract				*l_tpALifeMonsterAbstract = smart_cast<CSE_ALifeMonsterAbstract*>(tpALifeCreatureAbstract);
	if (l_tpALifeMonsterAbstract)
		l_tpALifeMonsterAbstract->m_tPrevGraphID = l_tpALifeMonsterAbstract->m_tNextGraphID = l_tpALifeMonsterAbstract->m_tGraphID;
}
