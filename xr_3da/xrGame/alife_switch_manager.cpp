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
#include "level_graph.h"

using namespace ALife;

CALifeSwitchManager::~CALifeSwitchManager	()
{
}

void CALifeSwitchManager::add_online(CSE_ALifeDynamicObject *object, bool update_registries)
{
	VERIFY							((ai().game_graph().vertex(object->m_tGraphID)->level_id() == graph().level().level_id()));

	object->m_bOnline	= true;

	NET_Packet						tNetPacket;
	CSE_Abstract					*l_tpAbstract = dynamic_cast<CSE_Abstract*>(object);
	server().entity_Destroy			(l_tpAbstract);
	object->s_flags.or				(M_SPAWN_UPDATE);
	ClientID clientID;clientID.set(0);
	server().Process_spawn			(tNetPacket,clientID,FALSE,l_tpAbstract);
	object->s_flags.and				(u16(-1) ^ M_SPAWN_UPDATE);
	R_ASSERT3						(ai().level_graph().valid_vertex_id(object->m_tNodeID),"Invalid vertex for object ",object->s_name_replace);

#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg							("[LSS] Spawning object [%s][%s][%d]",object->s_name_replace,object->s_name,object->ID);
	}
#endif

	CSE_ALifeTraderAbstract			*tpTraderParams = dynamic_cast<CSE_ALifeTraderAbstract*>(object);
	if (tpTraderParams) {
		OBJECT_IT					I = object->children.begin();
		OBJECT_IT					E = object->children.end();
		for ( ; I != E; ++I) {
			CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = objects().object(*I);
			CSE_ALifeInventoryItem	*l_tpALifeInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>(l_tpALifeDynamicObject);
			R_ASSERT2				(l_tpALifeInventoryItem,"Non inventory item object has parent?!");
			l_tpALifeInventoryItem->base()->s_flags.or(M_SPAWN_UPDATE);
			CSE_Abstract			*l_tpAbstract = dynamic_cast<CSE_Abstract*>(l_tpALifeInventoryItem);
			server().entity_Destroy(l_tpAbstract);

#ifdef DEBUG
			if (psAI_Flags.test(aiALife)) {
				Msg					("[LSS] Spawning item [%s][%s][%d]",l_tpALifeInventoryItem->base()->s_name_replace,l_tpALifeInventoryItem->base()->s_name,l_tpALifeDynamicObject->ID);
			}
#endif

//			R_ASSERT3								(ai().level_graph().valid_vertex_id(l_tpALifeDynamicObject->m_tNodeID),"Invalid vertex for object ",l_tpALifeInventoryItem->s_name_replace);
			l_tpALifeDynamicObject->o_Position		= object->o_Position;
			l_tpALifeDynamicObject->m_tNodeID		= object->m_tNodeID;
			ClientID clientID;clientID.set(0);
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

void CALifeSwitchManager::remove_online(CSE_ALifeDynamicObject *object, bool update_registries)
{
	object->m_bOnline	= false;

	server().Perform_destroy		(object,net_flags(TRUE,TRUE));
	_OBJECT_ID						l_tObjectID = object->ID;
	object->ID						= server().PerformIDgen(l_tObjectID);
	R_ASSERT2						(l_tObjectID == object->ID,"Can't reserve a particular object identifier");

#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg							("[LSS] Destroying object [%s][%s][%d]",object->s_name_replace,object->s_name,object->ID);
	}
#endif

	CSE_ALifeTraderAbstract			*tpTraderParams = dynamic_cast<CSE_ALifeTraderAbstract*>(object);
	if (tpTraderParams) {
		for (int i=0, n=(int)object->children.size(); i<n; ++i) {
			CSE_ALifeDynamicObject	*dynamic_object = dynamic_cast<CSE_ALifeDynamicObject*>(objects().object(object->children[i],true));
			if (!dynamic_object) {
				CSE_Abstract		*abstract = server().ID_to_entity(object->children[i]);
				VERIFY				(abstract);
				Msg					("ERROR : [%s][%s]",object->s_name,object->s_name_replace);
				R_ASSERT3			(false,abstract->s_name,abstract->s_name_replace);
			}
			VERIFY					(dynamic_object);
			CSE_ALifeInventoryItem	*l_tpALifeInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>(dynamic_object);
			VERIFY2					(l_tpALifeInventoryItem,"Non inventory item object has parent?!");
#ifdef DEBUG
			if (psAI_Flags.test(aiALife)) {
				Msg					("[LSS] Destroying item [%s][%s][%d]",l_tpALifeInventoryItem->base()->s_name_replace,l_tpALifeInventoryItem->base()->s_name,l_tpALifeInventoryItem->base()->ID);
			}
#endif
			_OBJECT_ID				l_tObjectID = l_tpALifeInventoryItem->base()->ID;
			l_tpALifeInventoryItem->base()->ID	= server().PerformIDgen(l_tpALifeInventoryItem->base()->ID);
			VERIFY2					(l_tpALifeInventoryItem->base()->ID == l_tObjectID,"Object ID has changed during ID generation!");
			dynamic_object->m_bOnline = false;

			if (!dynamic_object->can_save()) {
				graph().detach		(*object,dynamic_cast<CSE_ALifeInventoryItem*>(dynamic_object),object->m_tGraphID);
				release				(dynamic_object);
				--i;
				--n;
			}
		}
	}
	else {
		while (!object->children.empty()) {
			CSE_ALifeDynamicObject	*child = objects().object(object->children.back(),true);
			if (child)
				release				(child);
		}
		object->children.clear		();
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
		Msg							("[LSS] Going online [%d][%s][%d] ([%f][%f][%f] : [%f][%f][%f]), on '%s'",Device.TimerAsync(),object->s_name_replace, object->ID,VPUSH(graph().actor()->o_Position),VPUSH(object->o_Position), "*SERVER*");
	}
#endif
	
	CSE_ALifeAnomalousZone			*l_tpAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(object);
	if (l_tpAnomalousZone && (l_tpAnomalousZone->m_maxPower < EPS_L))
		return;

	object->m_bOnline				= true;

	CSE_ALifeGroupAbstract			*tpALifeGroupAbstract = dynamic_cast<CSE_ALifeGroupAbstract*>(object);
	if (tpALifeGroupAbstract) {
		OBJECT_IT					I = tpALifeGroupAbstract->m_tpMembers.begin(), B = I;
		OBJECT_IT					E = tpALifeGroupAbstract->m_tpMembers.end();
		u32							N = (u32)(E - I);
		for ( ; I != E; ++I) {
			CSE_ALifeDynamicObject	*J = objects().object(*I);
			if (tpALifeGroupAbstract->m_bCreateSpawnPositions) {
				J->o_Position		= object->o_Position;
				J->m_tNodeID		= object->m_tNodeID;
				CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(J);
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
		Msg							("[LSS] Going offline [%d][%s][%d] ([%f][%f][%f] : [%f][%f][%f]), on '%s'",Device.TimerAsync(),object->s_name_replace, object->ID,VPUSH(graph().actor()->o_Position),VPUSH(object->o_Position), "*SERVER*");
	}
#endif
	
	CSE_ALifeAnomalousZone			*l_tpAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(object);
	if (l_tpAnomalousZone && (l_tpAnomalousZone->m_maxPower < EPS_L))
		return;

	object->m_bOnline	= false;
	CSE_ALifeGroupAbstract			*tpALifeGroupAbstract = dynamic_cast<CSE_ALifeGroupAbstract*>(object);
	if (tpALifeGroupAbstract) {
		OBJECT_IT					I = tpALifeGroupAbstract->m_tpMembers.begin();
		OBJECT_IT					E = tpALifeGroupAbstract->m_tpMembers.end();
		if (I != E) {
			CSE_ALifeMonsterAbstract	*tpGroupMember	= dynamic_cast<CSE_ALifeMonsterAbstract*>(objects().object(*I));
			CSE_ALifeMonsterAbstract	*tpGroup		= dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeGroupAbstract);
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
	else
		remove_online		(object);
}

// switch object offline and check if it is a group of monsters then separate dead monsters from the group
void CALifeSwitchManager::furl_object	(CSE_ALifeDynamicObject *I)
{
	if (I->m_bOnline)
		if (0xffff == I->ID_Parent) {
			CSE_ALifeGroupAbstract *tpALifeGroupAbstract = dynamic_cast<CSE_ALifeGroupAbstract*>(I);
			if (tpALifeGroupAbstract)
				for (u32 i=0, N = (u32)tpALifeGroupAbstract->m_tpMembers.size(); i<N; ++i) {
					CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(objects().object(tpALifeGroupAbstract->m_tpMembers[i]));
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
	VERIFY3					(ai().level_graph().level_id() == ai().game_graph().vertex(I->m_tGraphID)->level_id(),I->s_name,I->s_name_replace);
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
	CSE_ALifeGroupAbstract	*tpALifeGroupAbstract = dynamic_cast<CSE_ALifeGroupAbstract*>(I);
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
				_GRAPH_ID			tGraphID = ai().cross_table().vertex(I->m_tNodeID).game_vertex_id();
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
			_GRAPH_ID				tGraphID = ai().cross_table().vertex(I->m_tNodeID).game_vertex_id();
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

	// checking if the object is online
	if (I->m_bOnline) {
		// checking if the object is not attached
		if (0xffff == I->ID_Parent) {
			// checking if the object is not a group of objects
			CSE_ALifeGroupAbstract *tpALifeGroupAbstract = dynamic_cast<CSE_ALifeGroupAbstract*>(I);
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
					CSE_ALifeMonsterAbstract	*tpGroupMember = dynamic_cast<CSE_ALifeMonsterAbstract*>(objects().object(tpALifeGroupAbstract->m_tpMembers[i]));
					if (tpGroupMember)
						// check if monster is not dead
						if (tpGroupMember->fHealth <= 0) {
							// detach object from the group
							tpGroupMember->fHealth			= 0.f;
							tpGroupMember->m_bDirectControl	= true;
							tpALifeGroupAbstract->m_tpMembers.erase(tpALifeGroupAbstract->m_tpMembers.begin() + i);
							tpGroupMember->m_bOnline		= false;
							CSE_ALifeInventoryItem			*item = dynamic_cast<CSE_ALifeInventoryItem*>(tpGroupMember);
							if (item && item->attached()) {
								 CSE_ALifeTraderAbstract	*trader = dynamic_cast<CSE_ALifeTraderAbstract*>(objects().object(tpGroupMember->ID_Parent,true));
								 if (trader)
									 trader->detach			(item);
							}
							// store the __new separate object into the registries
							register_object					(tpGroupMember);
							// and remove it from the graph point but do not remove it from the current level map
							CSE_ALifeInventoryItem			*l_tpALifeInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>(tpGroupMember);
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
				CSE_ALifeCreatureAbstract	*l_tpALifeCreatureAbstract = dynamic_cast<CSE_ALifeCreatureAbstract*>(objects().object(I->ID_Parent));
				if (l_tpALifeCreatureAbstract && (l_tpALifeCreatureAbstract->fHealth < EPS_L))
					Msg				("! uncontrolled situation [%d][%d][%s][%f]",I->ID,I->ID_Parent,l_tpALifeCreatureAbstract->s_name_replace,l_tpALifeCreatureAbstract->fHealth);
				R_ASSERT2			(!dynamic_cast<CSE_ALifeCreatureAbstract*>(objects().object(I->ID_Parent)) || (dynamic_cast<CSE_ALifeCreatureAbstract*>(objects().object(I->ID_Parent))->fHealth >= EPS_L),"Parent offline, item online...");
				if (!objects().object(I->ID_Parent)->m_bOnline)
					Msg				("! uncontrolled situation [%d][%d][%s][%f]",I->ID,I->ID_Parent,l_tpALifeCreatureAbstract->s_name_replace,l_tpALifeCreatureAbstract->fHealth);
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
			CSE_ALifeGroupAbstract *tpALifeGroupAbstract = dynamic_cast<CSE_ALifeGroupAbstract*>(I);
			if (tpALifeGroupAbstract && tpALifeGroupAbstract->m_tpMembers.empty()) {
				// release empty group of objects
				release(I);
				return;
			}
			else {
				CSE_ALifeSchedulable	*schedulable = dynamic_cast<CSE_ALifeSchedulable*>(I);
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
				CSE_ALifeCreatureAbstract	*l_tpALifeCreatureAbstract = dynamic_cast<CSE_ALifeCreatureAbstract*>(objects().object(I->ID_Parent));
				if (l_tpALifeCreatureAbstract && (l_tpALifeCreatureAbstract->fHealth < EPS_L))
					Msg				("! uncontrolled situation [%d][%d][%s][%f]",I->ID,I->ID_Parent,l_tpALifeCreatureAbstract->s_name_replace,l_tpALifeCreatureAbstract->fHealth);
				R_ASSERT2			(!l_tpALifeCreatureAbstract || (l_tpALifeCreatureAbstract->fHealth >= EPS_L),"Parent online, item offline...");
				if (objects().object(I->ID_Parent)->m_bOnline)
					Msg				("! uncontrolled situation [%d][%d][%s][%f]",I->ID,I->ID_Parent,l_tpALifeCreatureAbstract->s_name_replace,l_tpALifeCreatureAbstract->fHealth);
			}
#endif
			R_ASSERT2				(!objects().object(I->ID_Parent)->m_bOnline,"Parent online, item offline...");
		}
	}
}
