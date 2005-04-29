////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_switch_manager.cpp
//	Created 	: 25.12.2002
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator switch manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_switch_manager.h"
#include "xrServer_Objects_ALife.h"
#include "alife_graph_registry.h"
#include "alife_object_registry.h"
#include "alife_schedule_registry.h"
#include "game_level_cross_table.h"
#include "xrserver.h"
#include "ai_space.h"
#include "level_navigation_graph.h"

using namespace ALife;

//	TODO: 
//
//	add add_online method to CSE_ALifeObject
//	declare add_online method in CSE_ALifeTraderAbstract
//	override add_online method in child classes from CSE_ALifeTraderAbstract
//
//	add remove_online method to CSE_ALifeObject
//	declare remove_online method in CSE_ALifeTraderAbstract
//	override remove_online method in child classes from CSE_ALifeTraderAbstract
//
//	add switch_online method to CSE_ALifeObject
//	declare switch_online method in CSE_ALifeGroupAbstract
//	override switch_online method in child classes from CSE_ALifeGroupAbstract
//
//	add switch_offline method to CSE_ALifeObject
//	declare switch_offline method in CSE_ALifeGroupAbstract
//	override switch_offline method in child classes from CSE_ALifeGroupAbstract
//
//	add synchronize_location method to CSE_ALifeObject
//	declare synchronize_location method in CSE_ALifeGroupAbstract
//	override synchronize_location method in child classes from CSE_ALifeGroupAbstract
//
//	add can_switch_offline method to CSE_ALifeObject
//	declare can_switch_offline method in CSE_ALifeGroupAbstract
//	override can_switch_offline method in child classes from CSE_ALifeGroupAbstract
//
//	add can_switch_online method to CSE_ALifeObject
//	declare can_switch_online method in CSE_ALifeGroupAbstract
//	override can_switch_online method in child classes from CSE_ALifeGroupAbstract
//
//	Expected result : 
//		no dynamic_cast's, 
//		code is readable,
//		code is scalable,
//		CALifeSwitchManager looks simple 

CALifeSwitchManager::~CALifeSwitchManager	()
{
}

void CALifeSwitchManager::add_online(CSE_ALifeDynamicObject *object, bool update_registries)
{
	VERIFY							((ai().game_graph().vertex(object->m_tGraphID)->level_id() == graph().level().level_id()));

	object->m_bOnline	= true;

	NET_Packet						tNetPacket;
	CSE_Abstract					*l_tpAbstract = smart_cast<CSE_Abstract*>(object);
	server().entity_Destroy			(l_tpAbstract);
	object->s_flags.or				(M_SPAWN_UPDATE);
	ClientID						clientID;
	clientID.set					(server().GetServerClient() ? server().GetServerClient()->ID.value() : 0);
	server().Process_spawn			(tNetPacket,clientID,FALSE,l_tpAbstract);
	object->s_flags.and				(u16(-1) ^ M_SPAWN_UPDATE);
	R_ASSERT3						(!object->used_ai_locations() || ai().level_graph().valid_vertex_id(object->m_tNodeID),"Invalid vertex for object ",object->name_replace());

#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg							("[LSS] Spawning object [%s][%s][%d]",object->name_replace(),*object->s_name,object->ID);
	}
#endif

	CSE_ALifeTraderAbstract			*tpTraderParams = smart_cast<CSE_ALifeTraderAbstract*>(object);
	if (tpTraderParams) {
		OBJECT_IT					I = object->children.begin();
		OBJECT_IT					E = object->children.end();
		for ( ; I != E; ++I) {
			if (*I == graph().actor()->ID)
				continue;

			CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = objects().object(*I);
			CSE_ALifeInventoryItem	*l_tpALifeInventoryItem = smart_cast<CSE_ALifeInventoryItem*>(l_tpALifeDynamicObject);
			R_ASSERT2				(l_tpALifeInventoryItem,"Non inventory item object has parent?!");
			l_tpALifeInventoryItem->base()->s_flags.or(M_SPAWN_UPDATE);
			CSE_Abstract			*l_tpAbstract = smart_cast<CSE_Abstract*>(l_tpALifeInventoryItem);
			server().entity_Destroy(l_tpAbstract);

#ifdef DEBUG
			if (psAI_Flags.test(aiALife)) {
				Msg					("[LSS] Spawning item [%s][%s][%d]",l_tpALifeInventoryItem->base()->name_replace(),*l_tpALifeInventoryItem->base()->s_name,l_tpALifeDynamicObject->ID);
			}
#endif

//			R_ASSERT3								(ai().level_graph().valid_vertex_id(l_tpALifeDynamicObject->m_tNodeID),"Invalid vertex for object ",l_tpALifeInventoryItem->name_replace());
			l_tpALifeDynamicObject->o_Position		= object->o_Position;
			l_tpALifeDynamicObject->m_tNodeID		= object->m_tNodeID;
			server().Process_spawn					(tNetPacket,clientID,FALSE,l_tpALifeInventoryItem->base());
			l_tpALifeDynamicObject->s_flags.and		(u16(-1) ^ M_SPAWN_UPDATE);
			l_tpALifeDynamicObject->m_bOnline		= true;
		}
	}
	
	if (update_registries) {
		scheduled().remove	(object);
		graph().remove		(object,object->m_tGraphID,false);
	}
}

struct remove_non_savable_predicate {
	xrServer			*m_server;

	IC		 remove_non_savable_predicate(xrServer *server)
	{
		VERIFY			(server);
		m_server		= server;
	}

	IC	bool operator()	(const ALife::_OBJECT_ID &id) const
	{
		CSE_Abstract	*object = m_server->game->get_entity_from_eid(id);
		VERIFY			(object);
		CSE_ALifeObject	*alife_object = smart_cast<CSE_ALifeObject*>(object);
		VERIFY			(alife_object);
		return			(!alife_object->can_save());
	}
};

void CALifeSwitchManager::remove_online(CSE_ALifeDynamicObject *object, bool update_registries)
{
	object->m_bOnline			= false;
	
	m_saved_chidren				= object->children;
	CSE_ALifeTraderAbstract		*inventory_owner = smart_cast<CSE_ALifeTraderAbstract*>(object);
	if (inventory_owner) {
		OBJECT_VECTOR::iterator	I = remove_if(m_saved_chidren.begin(),m_saved_chidren.end(),remove_non_savable_predicate(&server()));
		m_saved_chidren.erase	(I,m_saved_chidren.end());
	}

	server().Perform_destroy	(object,net_flags(TRUE,TRUE));
	VERIFY						(object->children.empty());

	_OBJECT_ID					object_id = object->ID;
	object->ID					= server().PerformIDgen(object_id);

#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg							("[LSS] Destroying object [%s][%s][%d]",object->name_replace(),*object->s_name,object->ID);
	}
#endif

	if (inventory_owner) {
		for (u32 i=0, n=m_saved_chidren.size(); i<n; ++i) {
			CSE_ALifeDynamicObject	*child = smart_cast<CSE_ALifeDynamicObject*>(objects().object(m_saved_chidren[i],true));
			R_ASSERT				(child);
			child->m_bOnline		= false;

			CSE_ALifeInventoryItem	*inventory_item = smart_cast<CSE_ALifeInventoryItem*>(child);
			VERIFY2					(inventory_item,"Non inventory item object has parent?!");
#ifdef DEBUG
			if (psAI_Flags.test(aiALife)) {
				Msg					("[LSS] Destroying item [%s][%s][%d]",inventory_item->base()->name_replace(),*inventory_item->base()->s_name,inventory_item->base()->ID);
			}
#endif
			
			_OBJECT_ID					item_id = inventory_item->base()->ID;
			inventory_item->base()->ID	= server().PerformIDgen(item_id);

			if (!child->can_save()) {
				release				(child);
				--i;
				--n;
				continue;
			}
			graph().add				(child,child->m_tGraphID,false);
			graph().attach			(*object,inventory_item,child->m_tGraphID,true);//false);
		}
	}
	else {
		for (u32 i=0, n=m_saved_chidren.size(); i<n; ++i) {
			CSE_ALifeDynamicObject	*child = objects().object(m_saved_chidren[i],true);
			if (child) {
				release				(child);
				--i;
				--n;
			}
		}
	}

	if (update_registries) {
		scheduled().add		(object);
		graph().add			(object,object->m_tGraphID,false);
	}
}

void CALifeSwitchManager::switch_online(CSE_ALifeDynamicObject *object)
{
	R_ASSERT						(!object->m_bOnline);

#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg							("[LSS] Going online [%d][%s][%d] ([%f][%f][%f] : [%f][%f][%f]), on '%s'",Device.TimerAsync(),object->name_replace(), object->ID,VPUSH(graph().actor()->o_Position),VPUSH(object->o_Position), "*SERVER*");
	}
#endif
	
	CSE_ALifeAnomalousZone			*l_tpAnomalousZone = smart_cast<CSE_ALifeAnomalousZone*>(object);
	if (l_tpAnomalousZone && (l_tpAnomalousZone->m_maxPower < EPS_L))
		return;

	object->m_bOnline				= true;

	CSE_ALifeGroupAbstract			*tpALifeGroupAbstract = smart_cast<CSE_ALifeGroupAbstract*>(object);
	if (tpALifeGroupAbstract) {
		OBJECT_IT					I = tpALifeGroupAbstract->m_tpMembers.begin(), B = I;
		OBJECT_IT					E = tpALifeGroupAbstract->m_tpMembers.end();
		u32							N = (u32)(E - I);
		for ( ; I != E; ++I) {
			CSE_ALifeDynamicObject	*J = objects().object(*I);
			if (tpALifeGroupAbstract->m_bCreateSpawnPositions) {
				J->o_Position		= object->o_Position;
				J->m_tNodeID		= object->m_tNodeID;
				CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = smart_cast<CSE_ALifeMonsterAbstract*>(J);
				if (l_tpALifeMonsterAbstract)
					l_tpALifeMonsterAbstract->o_torso.yaw = angle_normalize_signed((I - B)/N*PI_MUL_2);
			}
			add_online				(J, false);
		}
		tpALifeGroupAbstract->m_bCreateSpawnPositions = false;
		scheduled().remove			(object);
		graph().remove				(object,object->m_tGraphID,false);
	}
	else
		add_online					(object);
}

void CALifeSwitchManager::switch_offline(CSE_ALifeDynamicObject *object)
{
	R_ASSERT						(object->m_bOnline);
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg							("[LSS] Going offline [%d][%s][%d] ([%f][%f][%f] : [%f][%f][%f]), on '%s'",Device.TimerAsync(),object->name_replace(), object->ID,VPUSH(graph().actor()->o_Position),VPUSH(object->o_Position), "*SERVER*");
	}
#endif
	
	CSE_ALifeAnomalousZone			*l_tpAnomalousZone = smart_cast<CSE_ALifeAnomalousZone*>(object);
	if (l_tpAnomalousZone && (l_tpAnomalousZone->m_maxPower < EPS_L))
		return;

	object->m_bOnline	= false;
	CSE_ALifeGroupAbstract			*tpALifeGroupAbstract = smart_cast<CSE_ALifeGroupAbstract*>(object);
	if (tpALifeGroupAbstract) {
		OBJECT_IT					I = tpALifeGroupAbstract->m_tpMembers.begin();
		OBJECT_IT					E = tpALifeGroupAbstract->m_tpMembers.end();
		if (I != E) {
			CSE_ALifeMonsterAbstract	*tpGroupMember	= smart_cast<CSE_ALifeMonsterAbstract*>(objects().object(*I));
			CSE_ALifeMonsterAbstract	*tpGroup		= smart_cast<CSE_ALifeMonsterAbstract*>(tpALifeGroupAbstract);
			if (tpGroupMember && tpGroup) {
				tpGroup->m_fCurSpeed	= tpGroup->m_fGoingSpeed;
				tpGroup->o_Position		= tpGroupMember->o_Position;
				u32	dwNodeID			= tpGroup->m_tNodeID;
				tpGroup->m_tGraphID		= ai().cross_table().vertex(dwNodeID).game_vertex_id();
				tpGroup->m_fDistanceToPoint = ai().cross_table().vertex(dwNodeID).distance();
				tpGroup->m_tNextGraphID	= tpGroup->m_tGraphID;
				u16	wNeighbourCount		= ai().game_graph().vertex(tpGroup->m_tGraphID)->edge_count();
				CGameGraph::const_iterator	i,e;
				ai().game_graph().begin	(tpGroup->m_tGraphID,i,e);
				tpGroup->m_tPrevGraphID	= (*(i + randI(0,wNeighbourCount))).vertex_id();
			}
			remove_online	(tpGroupMember,false);
			++I;
		}
		for ( ; I != E; ++I)
			remove_online	(objects().object(*I),false);
		scheduled().add		(object);
		graph().add			(object,object->m_tGraphID,false);
	}
	else {
	//	if (!xr_strcmp(object->name(),"bolt")) {
	//		object = object;
	//	}
		remove_online		(object);
	}
}

// switch object offline and check if it is a group of monsters then separate dead monsters from the group
void CALifeSwitchManager::furl_object	(CSE_ALifeDynamicObject *I)
{
	if (I->m_bOnline)
		if (0xffff == I->ID_Parent) {
			CSE_ALifeGroupAbstract *tpALifeGroupAbstract = smart_cast<CSE_ALifeGroupAbstract*>(I);
			if (tpALifeGroupAbstract)
				for (u32 i=0, N = (u32)tpALifeGroupAbstract->m_tpMembers.size(); i<N; ++i) {
					CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = smart_cast<CSE_ALifeMonsterAbstract*>(objects().object(tpALifeGroupAbstract->m_tpMembers[i]));
					if (l_tpALifeMonsterAbstract && l_tpALifeMonsterAbstract->fHealth <= 0) {
						l_tpALifeMonsterAbstract->m_bDirectControl	= true;
						l_tpALifeMonsterAbstract->m_bOnline			= false;
						tpALifeGroupAbstract->m_tpMembers.erase(tpALifeGroupAbstract->m_tpMembers.begin() + i);
						register_object	(l_tpALifeMonsterAbstract);
						--i;
						--N;
						continue;
					}
				}
			switch_offline(I);
		}
		else
			R_ASSERT2	(objects().object(I->ID_Parent)->m_bOnline,"Object online - parent offline...");
}

bool CALifeSwitchManager::synchronize_location(CSE_ALifeDynamicObject *I)
{
#ifdef DEBUG
	VERIFY3					(ai().level_graph().level_id() == ai().game_graph().vertex(I->m_tGraphID)->level_id(),*I->s_name,I->name_replace());
	xr_vector<u16>			test = I->children;
	std::sort				(test.begin(),test.end());
	for (u32 i=1, n=test.size(); i<n; ++i)
		if (test[i - 1] == test[i]) {
			VERIFY			(false);
		}
#endif

	// check if we do not use ai locations
	if (!I->used_ai_locations())
		return				(true);

	// check if we are not attached
	if (0xffff != I->ID_Parent)
		return				(true);

	// check if we are not online and have an invalid level vertex id
	if	(!I->m_bOnline && !ai().level_graph().valid_vertex_id(I->m_tNodeID))
		return				(true);

	// checking if it is a group of objects
	CSE_ALifeGroupAbstract	*tpALifeGroupAbstract = smart_cast<CSE_ALifeGroupAbstract*>(I);
	if (tpALifeGroupAbstract) {
		// checking if group is empty then remove it
		if (tpALifeGroupAbstract->m_tpMembers.empty()) {
			release	(I);
			return			(false);
		}
		else {
			// assign group position to the member position
			I->o_Position			= objects().object(tpALifeGroupAbstract->m_tpMembers[0])->o_Position;
			if (ai().level_graph().valid_vertex_position(I->o_Position) && !ai().level_graph().inside(ai().level_graph().vertex(I->m_tNodeID),I->o_Position)) {
				// checking if position is inside the current vertex
				I->m_tNodeID		= ai().level_graph().vertex(I->m_tNodeID,I->o_Position);
				// validating graph point and changing it if needed
				GameGraph::_GRAPH_ID	tGraphID = ai().cross_table().vertex(I->m_tNodeID).game_vertex_id();
				if ((tGraphID != I->m_tGraphID) && (0xffff == I->ID_Parent))
					if (!I->m_bOnline)
						graph().change	(I,I->m_tGraphID,tGraphID);
					else
						I->m_tGraphID = tGraphID;

				// validating distance to graph point via graph cross-table
				I->m_fDistance		= ai().cross_table().vertex(I->m_tNodeID).distance();
			}
		}
	}
	else {
		// otherwise validate position, graph point and vertex
		if (ai().level_graph().valid_vertex_position(I->o_Position) && !ai().level_graph().inside(ai().level_graph().vertex(I->m_tNodeID),I->o_Position)) {
			// checking if position is inside the current vertex
			I->m_tNodeID			= ai().level_graph().vertex(I->m_tNodeID,I->o_Position);
			// validating graph point and changing it if needed
			GameGraph::_GRAPH_ID	tGraphID = ai().cross_table().vertex(I->m_tNodeID).game_vertex_id();
			if ((tGraphID != I->m_tGraphID) && (0xffff == I->ID_Parent))
				if (!I->m_bOnline)
					graph().change	(I,I->m_tGraphID,tGraphID);
				else {
					VERIFY			(ai().game_graph().vertex(tGraphID)->level_id() == graph().level().level_id());
					I->m_tGraphID	= tGraphID;
				}
			// validating distance to graph point via graph cross-table
			I->m_fDistance			= ai().cross_table().vertex(I->m_tNodeID).distance();
		}
	}
	return							(true);
}

void CALifeSwitchManager::switch_object	(CSE_ALifeDynamicObject	*I)
{
	// updating vertex if it is invalid and object is not attached and online
	if (!synchronize_location(I))
		return;

	// checking if object is online
	if (I->m_bOnline) {
		// checking if the object is not attached
		if (0xffff == I->ID_Parent) {
			// checking if the object is not a group of objects
			CSE_ALifeGroupAbstract *tpALifeGroupAbstract = smart_cast<CSE_ALifeGroupAbstract*>(I);
			if (!tpALifeGroupAbstract) {
				// checking if the object is ready to switch offline
				if (I->can_switch_offline() && (!I->can_switch_online() || (graph().actor()->o_Position.distance_to(I->o_Position) > offline_distance())))
					switch_offline(I);
			}
			else {
				// so, we have a group of objects
				// therefore check all the group members if they are ready to switch offline

				// iterating on group members
				for (u32 i=0, N = (u32)tpALifeGroupAbstract->m_tpMembers.size(); i<N; ++i) {
					// casting group member to the abstract monster to get access to the Health property
					CSE_ALifeMonsterAbstract	*tpGroupMember = smart_cast<CSE_ALifeMonsterAbstract*>(objects().object(tpALifeGroupAbstract->m_tpMembers[i]));
					if (tpGroupMember)
						// check if monster is not dead
						if (tpGroupMember->fHealth <= 0) {
							// detach object from the group
							tpGroupMember->fHealth			= 0.f;
							tpGroupMember->m_bDirectControl	= true;
							tpALifeGroupAbstract->m_tpMembers.erase(tpALifeGroupAbstract->m_tpMembers.begin() + i);
							tpGroupMember->m_bOnline		= false;
							CSE_ALifeInventoryItem			*item = smart_cast<CSE_ALifeInventoryItem*>(tpGroupMember);
							if (item && item->attached()) {
								 CSE_ALifeTraderAbstract	*trader = smart_cast<CSE_ALifeTraderAbstract*>(objects().object(tpGroupMember->ID_Parent,true));
								 if (trader)
									 trader->detach			(item);
							}
							// store the __new separate object into the registries
							register_object					(tpGroupMember);
							// and remove it from the graph point but do not remove it from the current level map
							CSE_ALifeInventoryItem			*l_tpALifeInventoryItem = smart_cast<CSE_ALifeInventoryItem*>(tpGroupMember);
							if (!l_tpALifeInventoryItem || !l_tpALifeInventoryItem->attached())
								graph().remove				(tpGroupMember,tpGroupMember->m_tGraphID,false);
							tpGroupMember->m_bOnline		= true;
							--(tpALifeGroupAbstract->m_wCount);
							--i;
							--N;
							continue;
						}
						else
							// so, monster is not dead
							// checking if the object is _not_ ready to switch offline
							if (tpGroupMember->can_switch_offline() && (!tpGroupMember->can_switch_online() || (graph().actor()->o_Position.distance_to(tpGroupMember->o_Position) <= offline_distance())))
								// so, it is not ready, breaking a cycle, because we can't 
								// switch group offline since not all the group members are ready
								// to switch offline
								break;
				}
				// checking if group is not empty
				if (tpALifeGroupAbstract->m_tpMembers.size()) {
					if (I->can_switch_offline() && (!I->can_switch_online() || (i == N)))
						switch_offline(I);
				}
				else
					release(I);
			}
		}
		else {
#ifdef DEBUG
			if (psAI_Flags.test(aiALife)) {
				// checking if parent is online too
				CSE_ALifeCreatureAbstract	*l_tpALifeCreatureAbstract = smart_cast<CSE_ALifeCreatureAbstract*>(objects().object(I->ID_Parent));
				if (l_tpALifeCreatureAbstract && (l_tpALifeCreatureAbstract->fHealth < EPS_L))
					Msg				("! uncontrolled situation [%d][%d][%s][%f]",I->ID,I->ID_Parent,l_tpALifeCreatureAbstract->name_replace(),l_tpALifeCreatureAbstract->fHealth);
				R_ASSERT2			(!smart_cast<CSE_ALifeCreatureAbstract*>(objects().object(I->ID_Parent)) || (smart_cast<CSE_ALifeCreatureAbstract*>(objects().object(I->ID_Parent))->fHealth >= EPS_L),"Parent offline, item online...");
				if (!objects().object(I->ID_Parent)->m_bOnline)
					Msg				("! uncontrolled situation [%d][%d][%s][%f]",I->ID,I->ID_Parent,l_tpALifeCreatureAbstract->name_replace(),l_tpALifeCreatureAbstract->fHealth);
			}
#endif
			R_ASSERT2			(objects().object(I->ID_Parent)->m_bOnline,"Parent offline, item online...");
		}
	}
	else {
		// so, the object is offline
		// checking if the object is not attached
		if (0xffff == I->ID_Parent) {
			// checking if the object is not an empty group of objects
			CSE_ALifeGroupAbstract *tpALifeGroupAbstract = smart_cast<CSE_ALifeGroupAbstract*>(I);
			if (tpALifeGroupAbstract && tpALifeGroupAbstract->m_tpMembers.empty()) {
				// release empty group of objects
				release(I);
				return;
			}
			else {
				CSE_ALifeSchedulable	*schedulable = smart_cast<CSE_ALifeSchedulable*>(I);
				// checking if the abstract monster has just died
				if (schedulable) {
					if (!schedulable->need_update(I)) {
						if (scheduled().object(I->ID,true))
							scheduled().remove	(I);
					}
					else
						if (!scheduled().object(I->ID,true))
							scheduled().add		(I);
				}
			}
			
			// checking if the object is ready to switch online
			if (I->can_switch_online() && (!I->can_switch_offline() || (graph().actor()->o_Position.distance_to(I->o_Position) <= online_distance())))
				switch_online(I);
		}
		else {
			// so, object is attached
			// checking if parent is offline too
#ifdef DEBUG
			if (psAI_Flags.test(aiALife)) {
				CSE_ALifeCreatureAbstract	*l_tpALifeCreatureAbstract = smart_cast<CSE_ALifeCreatureAbstract*>(objects().object(I->ID_Parent));
				if (l_tpALifeCreatureAbstract && (l_tpALifeCreatureAbstract->fHealth < EPS_L))
					Msg				("! uncontrolled situation [%d][%d][%s][%f]",I->ID,I->ID_Parent,l_tpALifeCreatureAbstract->name_replace(),l_tpALifeCreatureAbstract->fHealth);
				R_ASSERT2			(!l_tpALifeCreatureAbstract || (l_tpALifeCreatureAbstract->fHealth >= EPS_L),"Parent online, item offline...");
				if (objects().object(I->ID_Parent)->m_bOnline)
					Msg				("! uncontrolled situation [%d][%d][%s][%f]",I->ID,I->ID_Parent,l_tpALifeCreatureAbstract->name_replace(),l_tpALifeCreatureAbstract->fHealth);
			}
#endif
			R_ASSERT2				(!objects().object(I->ID_Parent)->m_bOnline,"Parent online, item offline...");
		}
	}
}
