////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_switch.cpp
//	Created 	: 25.12.2002
//  Modified 	: 29.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A-Life Online/Offline switches
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "ai_space.h"
#include "GameObject.h"
#include "level_graph.h"
#include "game_graph.h"
#include "game_level_cross_table.h"
#include "xrserver_objects_alife_monsters.h"
using namespace ALife;

#define USE_SWITCH_OPTION

void CSE_ALifeSimulator::vfReleaseObject(CSE_Abstract *tpSE_Abstract, bool bALifeRequest)
{
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg							("[LSS] Releasing object [%s][%d]",tpSE_Abstract->s_name_replace,tpSE_Abstract->ID);
	}
#endif
	CSE_ALifeDynamicObject			*tpALifeDynamicObject = tpfGetObjectByID(tpSE_Abstract->ID);
	VERIFY							(tpALifeDynamicObject);
	CSE_ALifeInventoryItem			*l_tpInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>(tpALifeDynamicObject);
	if (l_tpInventoryItem && l_tpInventoryItem->bfAttached())
		vfDetachItem				(*tpfGetObjectByID(l_tpInventoryItem->ID_Parent),l_tpInventoryItem,tpfGetObjectByID(l_tpInventoryItem->ID_Parent)->m_tGraphID,bALifeRequest);

	m_tObjectRegistry.erase			(tpSE_Abstract->ID);
	
	if (!tpALifeDynamicObject->m_bOnline) {
		vfRemoveObjectFromGraphPoint(tpALifeDynamicObject,tpALifeDynamicObject->m_tGraphID);
		vfRemoveObjectFromScheduled	(tpALifeDynamicObject);
	}
	else
		vfRemoveObjectFromCurrentLevel(tpALifeDynamicObject);

	tpSE_Abstract->m_bALifeControl	= false;

	if (bALifeRequest)
		m_tpServer->entity_Destroy	(tpSE_Abstract);
}

void CSE_ALifeSimulator::vfCreateOnlineObject(CSE_ALifeDynamicObject *tpALifeDynamicObject, bool bRemoveFromRegistries)
{
	tpALifeDynamicObject->m_bOnline	= true;

	NET_Packet						tNetPacket;
	CSE_Abstract					*l_tpAbstract = dynamic_cast<CSE_Abstract*>(tpALifeDynamicObject);
	m_tpServer->entity_Destroy		(l_tpAbstract);
	tpALifeDynamicObject->s_flags.or(M_SPAWN_UPDATE);
	m_tpServer->Process_spawn		(tNetPacket,0,FALSE,l_tpAbstract);
	tpALifeDynamicObject->s_flags.and(u16(-1) ^ M_SPAWN_UPDATE);
	R_ASSERT3						(ai().level_graph().valid_vertex_id(tpALifeDynamicObject->m_tNodeID),"Invalid vertex for object ",tpALifeDynamicObject->s_name_replace);

#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg							("[LSS] Spawning object [%s][%d]",tpALifeDynamicObject->s_name_replace,tpALifeDynamicObject->ID);
	}
#endif

	CSE_ALifeTraderAbstract			*tpTraderParams = dynamic_cast<CSE_ALifeTraderAbstract*>(tpALifeDynamicObject);
	if (tpTraderParams) {
		OBJECT_IT					I = tpALifeDynamicObject->children.begin();
		OBJECT_IT					E = tpALifeDynamicObject->children.end();
		for ( ; I != E; ++I) {
			CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = tpfGetObjectByID(*I);
			CSE_ALifeInventoryItem	*l_tpALifeInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>(l_tpALifeDynamicObject);
			R_ASSERT2				(l_tpALifeInventoryItem,"Non inventory item object has parent?!");
			l_tpALifeInventoryItem->s_flags.or(M_SPAWN_UPDATE);
			CSE_Abstract			*l_tpAbstract = dynamic_cast<CSE_Abstract*>(l_tpALifeInventoryItem);
			m_tpServer->entity_Destroy(l_tpAbstract);

#ifdef DEBUG
			if (psAI_Flags.test(aiALife)) {
				Msg					("[LSS] Spawning item [%s][%d]",l_tpALifeInventoryItem->s_name_replace,l_tpALifeDynamicObject->ID);
			}
#endif

//			R_ASSERT3								(ai().level_graph().valid_vertex_id(l_tpALifeDynamicObject->m_tNodeID),"Invalid vertex for object ",l_tpALifeInventoryItem->s_name_replace);
			m_tpServer->Process_spawn				(tNetPacket,0,FALSE,l_tpALifeInventoryItem);
			l_tpALifeDynamicObject->o_Position		= tpALifeDynamicObject->o_Position;
			l_tpALifeDynamicObject->m_tNodeID		= tpALifeDynamicObject->m_tNodeID;
			l_tpALifeDynamicObject->s_flags.and		(u16(-1) ^ M_SPAWN_UPDATE);
			l_tpALifeDynamicObject->m_bOnline		= true;
		}
	}
	
	if (bRemoveFromRegistries) {
		vfRemoveObjectFromScheduled	(tpALifeDynamicObject);
		vfRemoveObjectFromGraphPoint(tpALifeDynamicObject,tpALifeDynamicObject->m_tGraphID,false);
	}
}

void CSE_ALifeSimulator::vfRemoveOnlineObject(CSE_ALifeDynamicObject *tpALifeDynamicObject, bool bAddToRegistries)
{
	tpALifeDynamicObject->m_bOnline	= false;

	m_tpServer->Perform_destroy		(tpALifeDynamicObject,net_flags(TRUE,TRUE));
	_OBJECT_ID						l_tObjectID = tpALifeDynamicObject->ID;
	tpALifeDynamicObject->ID		= m_tpServer->PerformIDgen(l_tObjectID);
	R_ASSERT2						(l_tObjectID == tpALifeDynamicObject->ID,"Can't reserve a particular object identifier");

#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg							("[LSS] Destroying object [%s][%d]",tpALifeDynamicObject->s_name_replace,tpALifeDynamicObject->ID);
	}
#endif

	CSE_ALifeTraderAbstract			*tpTraderParams = dynamic_cast<CSE_ALifeTraderAbstract*>(tpALifeDynamicObject);
	if (tpTraderParams) {
		OBJECT_IT					I = tpALifeDynamicObject->children.begin();
		OBJECT_IT					E = tpALifeDynamicObject->children.end();
		for ( ; I != E; ++I) {
			CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = tpfGetObjectByID(*I);
			CSE_ALifeInventoryItem	*l_tpALifeInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>(l_tpALifeDynamicObject);
			R_ASSERT2				(l_tpALifeInventoryItem,"Non inventory item object has parent?!");
#ifdef DEBUG
			if (psAI_Flags.test(aiALife)) {
				Msg					("[LSS] Destroying item [%s]",l_tpALifeInventoryItem->s_name_replace,l_tpALifeInventoryItem->ID);
			}
#endif

			m_tpServer->Perform_destroy(l_tpALifeInventoryItem,net_flags(TRUE,TRUE));
			_OBJECT_ID				l_tObjectID = l_tpALifeInventoryItem->ID;
			l_tpALifeInventoryItem->ID	= m_tpServer->PerformIDgen(l_tpALifeInventoryItem->ID);
			R_ASSERT2				(l_tpALifeInventoryItem->ID == l_tObjectID,"Object ID has changed during ID generation!");
			l_tpALifeDynamicObject->m_bOnline = false;
		}
	}
	
	if (bAddToRegistries) {
		vfAddObjectToScheduled		(tpALifeDynamicObject);
		vfAddObjectToGraphPoint		(tpALifeDynamicObject,tpALifeDynamicObject->m_tGraphID,false);
	}
}

void CSE_ALifeSimulator::vfSwitchObjectOnline(CSE_ALifeDynamicObject *tpALifeDynamicObject)
{
	R_ASSERT						(!tpALifeDynamicObject->m_bOnline);

#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg							("[LSS] Going online [%d][%s][%d] ([%f][%f][%f] : [%f][%f][%f]), on '%s'",Device.TimerAsync(),tpALifeDynamicObject->s_name_replace, tpALifeDynamicObject->ID,VPUSH(m_tpActor->o_Position),VPUSH(tpALifeDynamicObject->o_Position), "*SERVER*");
	}
#endif
	
	CSE_ALifeAnomalousZone			*l_tpAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(tpALifeDynamicObject);
	if (l_tpAnomalousZone && (l_tpAnomalousZone->m_maxPower < EPS_L))
		return;

	tpALifeDynamicObject->m_bOnline	= true;

	CSE_ALifeGroupAbstract			*tpALifeGroupAbstract = dynamic_cast<CSE_ALifeGroupAbstract*>(tpALifeDynamicObject);
	if (tpALifeGroupAbstract) {
		OBJECT_IT					I = tpALifeGroupAbstract->m_tpMembers.begin(), B = I;
		OBJECT_IT					E = tpALifeGroupAbstract->m_tpMembers.end();
		u32							N = (u32)(E - I);
		for ( ; I != E; ++I) {
			CSE_ALifeDynamicObject	*J = tpfGetObjectByID(*I);
			if (tpALifeGroupAbstract->m_bCreateSpawnPositions) {
				J->o_Position		= tpALifeDynamicObject->o_Position;
				J->m_tNodeID		= tpALifeDynamicObject->m_tNodeID;
				CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(J);
				if (l_tpALifeMonsterAbstract)
					l_tpALifeMonsterAbstract->o_torso.yaw = angle_normalize_signed((I - B)/N*PI_MUL_2);
			}
			vfCreateOnlineObject	(J, false);
		}
		tpALifeGroupAbstract->m_bCreateSpawnPositions = false;
		vfRemoveObjectFromScheduled	(tpALifeDynamicObject);
		vfRemoveObjectFromGraphPoint(tpALifeDynamicObject,tpALifeDynamicObject->m_tGraphID,false);
	}
	else
		vfCreateOnlineObject		(tpALifeDynamicObject);
}

void CSE_ALifeSimulator::vfSwitchObjectOffline(CSE_ALifeDynamicObject *tpALifeDynamicObject)
{
	R_ASSERT						(tpALifeDynamicObject->m_bOnline);
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg							("[LSS] Going offline [%d][%s][%d] ([%f][%f][%f] : [%f][%f][%f]), on '%s'",Device.TimerAsync(),tpALifeDynamicObject->s_name_replace, tpALifeDynamicObject->ID,VPUSH(m_tpActor->o_Position),VPUSH(tpALifeDynamicObject->o_Position), "*SERVER*");
	}
#endif
	
	CSE_ALifeAnomalousZone			*l_tpAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(tpALifeDynamicObject);
	if (l_tpAnomalousZone && (l_tpAnomalousZone->m_maxPower < EPS_L))
		return;

	tpALifeDynamicObject->m_bOnline	= false;
	CSE_ALifeGroupAbstract			*tpALifeGroupAbstract = dynamic_cast<CSE_ALifeGroupAbstract*>(tpALifeDynamicObject);
	if (tpALifeGroupAbstract) {
		OBJECT_IT					I = tpALifeGroupAbstract->m_tpMembers.begin();
		OBJECT_IT					E = tpALifeGroupAbstract->m_tpMembers.end();
		if (I != E) {
			CSE_ALifeMonsterAbstract	*tpGroupMember	= dynamic_cast<CSE_ALifeMonsterAbstract*>(tpfGetObjectByID(*I));
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
			vfRemoveOnlineObject	(tpGroupMember,false);
			++I;
		}
		for ( ; I != E; ++I)
			vfRemoveOnlineObject	(tpfGetObjectByID(*I),false);
		vfAddObjectToScheduled		(tpALifeDynamicObject);
		vfAddObjectToGraphPoint		(tpALifeDynamicObject,tpALifeDynamicObject->m_tGraphID,false);
	}
	else
		vfRemoveOnlineObject		(tpALifeDynamicObject);
}

// switch object offline and check if it is a group of monsters then separate dead monsters from the group
void CSE_ALifeSimulator::vfFurlObjectOffline(CSE_ALifeDynamicObject *I)
{
	if (I->m_bOnline)
		if (0xffff == I->ID_Parent) {
			CSE_ALifeGroupAbstract *tpALifeGroupAbstract = dynamic_cast<CSE_ALifeGroupAbstract*>(I);
			if (tpALifeGroupAbstract)
				for (u32 i=0, N = (u32)tpALifeGroupAbstract->m_tpMembers.size(); i<N; ++i) {
					CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(tpfGetObjectByID(tpALifeGroupAbstract->m_tpMembers[i]));
					if (l_tpALifeMonsterAbstract && l_tpALifeMonsterAbstract->fHealth <= 0) {
						l_tpALifeMonsterAbstract->m_bDirectControl	= true;
						l_tpALifeMonsterAbstract->m_bOnline			= false;
						tpALifeGroupAbstract->m_tpMembers.erase(tpALifeGroupAbstract->m_tpMembers.begin() + i);
						vfUpdateDynamicData(l_tpALifeMonsterAbstract);
						--i;
						--N;
						continue;
					}
				}
				vfSwitchObjectOffline(I);
		}
		else
			R_ASSERT2	(tpfGetObjectByID(I->ID_Parent)->m_bOnline,"Object online - parent offline...");
}

void CSE_ALifeSimulator::vfValidatePosition(CSE_ALifeDynamicObject *I)
{
//	Msg("Validating position");
	// updating vertex if it is invalid and object is not attached and online
	if (I->used_ai_locations() && (I->m_bOnline || ai().level_graph().valid_vertex_id(I->m_tNodeID)) && (0xffff == I->ID_Parent)) {
		// checking if it is a group of objects
		CSE_ALifeGroupAbstract *tpALifeGroupAbstract = dynamic_cast<CSE_ALifeGroupAbstract*>(I);
		if (tpALifeGroupAbstract) {
			// checking if group is empty then remove it
			if (tpALifeGroupAbstract->m_tpMembers.empty())
				vfReleaseObject(I);
			else {
				// assign group position to the member position
				I->o_Position			= tpfGetObjectByID(tpALifeGroupAbstract->m_tpMembers[0])->o_Position;
				if (!ai().level_graph().inside(ai().level_graph().vertex(I->m_tNodeID),I->o_Position)) {
					// checking if position is inside the current vertex
					I->m_tNodeID		= ai().level_graph().vertex(I->m_tNodeID,I->o_Position);
					// validating graph point and changing it if needed
					_GRAPH_ID			tGraphID = ai().cross_table().vertex(I->m_tNodeID).game_vertex_id();
					if ((tGraphID != I->m_tGraphID) && (0xffff == I->ID_Parent))
						if (!I->m_bOnline)
							vfChangeObjectGraphPoint(I,I->m_tGraphID,tGraphID);
						else
							I->m_tGraphID = tGraphID;

					// validating distance to graph point via graph cross-table
					I->m_fDistance		= ai().cross_table().vertex(I->m_tNodeID).distance();
				}
			}
		}
		else {
			// otherwise validate position, graph point and vertex
			if (!ai().level_graph().inside(ai().level_graph().vertex(I->m_tNodeID),I->o_Position)) {
				// checking if position is inside the current vertex
				I->m_tNodeID			= ai().level_graph().vertex(I->m_tNodeID,I->o_Position);
				// validating graph point and changing it if needed
				_GRAPH_ID				tGraphID = ai().cross_table().vertex(I->m_tNodeID).game_vertex_id();
				if ((tGraphID != I->m_tGraphID) && (0xffff == I->ID_Parent))
					if (!I->m_bOnline)
						vfChangeObjectGraphPoint(I,I->m_tGraphID,tGraphID);
					else
						I->m_tGraphID = tGraphID;
				// validating distance to graph point via graph cross-table
				I->m_fDistance			= ai().cross_table().vertex(I->m_tNodeID).distance();
			}
		}
	}
}

void CSE_ALifeSimulator::ProcessOnlineOfflineSwitches(CSE_ALifeDynamicObject *I)
{
	// updating vertex if it is invalid and object is not attached and online
	vfValidatePosition			(I);

	// checking if the object is online
	if (I->m_bOnline) {
		// checking if the object is not attached
		if (0xffff == I->ID_Parent) {
			// checking if the object is not a group of objects
			CSE_ALifeGroupAbstract *tpALifeGroupAbstract = dynamic_cast<CSE_ALifeGroupAbstract*>(I);
			if (!tpALifeGroupAbstract) {
				// checking if the object is ready to switch offline
#ifndef USE_SWITCH_OPTION
				if (m_tpActor->o_Position.distance_to(I->o_Position) > m_fOfflineDistance)
#else
				if (I->can_switch_offline() && (!I->can_switch_online() || (m_tpActor->o_Position.distance_to(I->o_Position) > m_fOfflineDistance)))
#endif
					vfSwitchObjectOffline(I);
			}
			else {
				// so, we have a group of objects
				// therefore check all the group members if they are ready to switch offline

				// iterating on group members
				for (u32 i=0, N = (u32)tpALifeGroupAbstract->m_tpMembers.size(); i<N; ++i) {
					// casting group member to the abstract monster to get access to the Health property
					CSE_ALifeMonsterAbstract	*tpGroupMember = dynamic_cast<CSE_ALifeMonsterAbstract*>(tpfGetObjectByID(tpALifeGroupAbstract->m_tpMembers[i]));
					if (tpGroupMember)
						// check if monster is not dead
						if (tpGroupMember->fHealth <= 0) {
							// detach object from the group
							tpGroupMember->fHealth			= 0.f;
							tpGroupMember->m_bDirectControl	= true;
							tpALifeGroupAbstract->m_tpMembers.erase(tpALifeGroupAbstract->m_tpMembers.begin() + i);
							tpGroupMember->m_bOnline		= false;
							// store the __new separate object into the registries
							vfUpdateDynamicData				(tpGroupMember);
							// and remove it from the graph point but do not remove it from the current level map
							CSE_ALifeInventoryItem			*l_tpALifeInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>(tpGroupMember);
							if (!l_tpALifeInventoryItem || !l_tpALifeInventoryItem->bfAttached())
								vfRemoveObjectFromGraphPoint(tpGroupMember,tpGroupMember->m_tGraphID,false);
							tpGroupMember->m_bOnline		= true;
							--(tpALifeGroupAbstract->m_wCount);
							--i;
							--N;
							continue;
						}
						else
							// so, monster is not dead
							// checking if the object is _not_ ready to switch offline
							if (tpGroupMember->can_switch_offline() && (!tpGroupMember->can_switch_online() || (m_tpActor->o_Position.distance_to(tpGroupMember->o_Position) <= m_fOfflineDistance)))
								// so, it is not ready, breaking a cycle, because we can't 
								// switch group offline since not all the group members are ready
								// to switch offline
								break;
				}
				// checking if group is not empty
				if (tpALifeGroupAbstract->m_tpMembers.size()) {
#ifndef USE_SWITCH_OPTION
					if (i == N)
#else
					if (I->can_switch_offline() && (!I->can_switch_online() || (i == N)))
#endif
						vfSwitchObjectOffline(I);
				}
				else
					vfReleaseObject(I);
			}
		}
		else {
#ifdef DEBUG
			if (psAI_Flags.test(aiALife)) {
				// checking if parent is online too
				CSE_ALifeCreatureAbstract	*l_tpALifeCreatureAbstract = dynamic_cast<CSE_ALifeCreatureAbstract*>(tpfGetObjectByID(I->ID_Parent));
				if (l_tpALifeCreatureAbstract && (l_tpALifeCreatureAbstract->fHealth < EPS_L))
					Msg				("! uncontrolled situation [%d][%d][%s][%f]",I->ID,I->ID_Parent,l_tpALifeCreatureAbstract->s_name_replace,l_tpALifeCreatureAbstract->fHealth);
				R_ASSERT2			(!dynamic_cast<CSE_ALifeCreatureAbstract*>(tpfGetObjectByID(I->ID_Parent)) || (dynamic_cast<CSE_ALifeCreatureAbstract*>(tpfGetObjectByID(I->ID_Parent))->fHealth >= EPS_L),"Parent offline, item online...");
				if (!tpfGetObjectByID(I->ID_Parent)->m_bOnline)
					Msg				("! uncontrolled situation [%d][%d][%s][%f]",I->ID,I->ID_Parent,l_tpALifeCreatureAbstract->s_name_replace,l_tpALifeCreatureAbstract->fHealth);
			}
#endif
			R_ASSERT2			(tpfGetObjectByID(I->ID_Parent)->m_bOnline,"Parent offline, item online...");
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
				vfReleaseObject(tpALifeGroupAbstract);
				return;
			}
			else {
				CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(I);
				// checking if the abstract monster has just died
				if (tpALifeMonsterAbstract && (tpALifeMonsterAbstract->fHealth <= 0) && !tpALifeMonsterAbstract->m_bOnline && (m_tpScheduledObjects.find(tpALifeMonsterAbstract->ID) != m_tpScheduledObjects.end()))
					vfRemoveObjectFromScheduled(tpALifeMonsterAbstract);
			}
			
			// checking if the object is ready to switch online
#ifndef USE_SWITCH_OPTION
			if (m_tpActor->o_Position.distance_to(I->o_Position) <= m_fOnlineDistance)
#else
			if (I->can_switch_online() && (!I->can_switch_offline() || (m_tpActor->o_Position.distance_to(I->o_Position) <= m_fOnlineDistance)))
#endif
				vfSwitchObjectOnline(I);
		}
		else {
			// so, object is attached
			// checking if parent is offline too
#ifdef DEBUG
			if (psAI_Flags.test(aiALife)) {
				CSE_ALifeCreatureAbstract	*l_tpALifeCreatureAbstract = dynamic_cast<CSE_ALifeCreatureAbstract*>(tpfGetObjectByID(I->ID_Parent));
				if (l_tpALifeCreatureAbstract && (l_tpALifeCreatureAbstract->fHealth < EPS_L))
					Msg				("! uncontrolled situation [%d][%d][%s][%f]",I->ID,I->ID_Parent,l_tpALifeCreatureAbstract->s_name_replace,l_tpALifeCreatureAbstract->fHealth);
				R_ASSERT2			(!l_tpALifeCreatureAbstract || (l_tpALifeCreatureAbstract->fHealth >= EPS_L),"Parent online, item offline...");
				if (tpfGetObjectByID(I->ID_Parent)->m_bOnline)
					Msg				("! uncontrolled situation [%d][%d][%s][%f]",I->ID,I->ID_Parent,l_tpALifeCreatureAbstract->s_name_replace,l_tpALifeCreatureAbstract->fHealth);
			}
#endif
			R_ASSERT2			(!tpfGetObjectByID(I->ID_Parent)->m_bOnline,"Parent online, item offline...");
		}
	}
}