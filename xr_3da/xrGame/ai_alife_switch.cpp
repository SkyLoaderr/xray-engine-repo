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

void CSE_ALifeSimulator::vfReleaseObject(CSE_Abstract *tpSE_Abstract, bool bForceDelete)
{
	CSE_ALifeDynamicObject			*tpALifeDynamicObject = m_tObjectRegistry[tpSE_Abstract->ID];
	VERIFY							(tpALifeDynamicObject);
	m_tObjectRegistry.erase			(tpSE_Abstract->ID);
	
	if (!dynamic_cast<CSE_ALifeItem *>(tpALifeDynamicObject) || !dynamic_cast<CSE_ALifeItem *>(tpALifeDynamicObject)->bfAttached()) {
		if (!tpALifeDynamicObject->m_bOnline && !dynamic_cast<CSE_ALifeTrader *>(tpALifeDynamicObject))
			vfRemoveObjectFromGraphPoint(tpALifeDynamicObject,tpALifeDynamicObject->m_tGraphID);
		vfRemoveObjectFromCurrentLevel(tpALifeDynamicObject);
	}
	
	CSE_ALifeMonsterAbstract		*tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(tpSE_Abstract);
	if (tpALifeMonsterAbstract && !tpALifeMonsterAbstract->m_bOnline && (m_tpScheduledObjects.find(tpALifeMonsterAbstract->ID) != m_tpScheduledObjects.end()))
		vfRemoveObjectFromScheduled	(tpALifeDynamicObject);

	tpSE_Abstract->m_bALifeControl	= false;

	if (bForceDelete)
		m_tpServer->entity_Destroy	(tpSE_Abstract);
}

void CSE_ALifeSimulator::vfCreateOnlineObject(CSE_ALifeDynamicObject *tpALifeDynamicObject, bool bRemoveFromScheduled)
{
	NET_Packet						tNetPacket;
	
	CSE_Abstract					*l_tpAbstract = dynamic_cast<CSE_Abstract*>(tpALifeDynamicObject);
	m_tpServer->entity_Destroy		(l_tpAbstract);
	tpALifeDynamicObject->s_flags.or(M_SPAWN_UPDATE);
	m_tpServer->Process_spawn		(tNetPacket,0,FALSE,l_tpAbstract);
	tpALifeDynamicObject->s_flags.and(u16(-1) ^ M_SPAWN_UPDATE);
	R_ASSERT3						(tpALifeDynamicObject->m_tNodeID && (tpALifeDynamicObject->m_tNodeID < getAI().Header().count),"Invalid node for object ",tpALifeDynamicObject->s_name_replace);

#ifdef DEBUG_LOG
	Msg("ALife : Spawning object %s",tpALifeDynamicObject->s_name_replace);
#endif

	CSE_ALifeTraderAbstract			*tpTraderParams = dynamic_cast<CSE_ALifeTraderAbstract*>(tpALifeDynamicObject);
	if (tpTraderParams) {
		OBJECT_IT					I = tpALifeDynamicObject->children.begin();
		OBJECT_IT					E = tpALifeDynamicObject->children.end();
		for ( ; I != E; I++) {
			CSE_ALifeItem			*tpItem = dynamic_cast<CSE_ALifeItem*>(m_tObjectRegistry[*I]);
			if (!tpItem)
				continue;
			tpItem->s_flags.or		(M_SPAWN_UPDATE);
			CSE_Abstract			*l_tpAbstract = dynamic_cast<CSE_Abstract*>(tpItem);
			m_tpServer->entity_Destroy(l_tpAbstract);

#ifdef DEBUG_LOG
			Msg						("ALife : Spawning item %s (ID = %d)",tpItem->s_name,tpItem->ID);
#endif

			R_ASSERT3				(tpItem->m_tNodeID && (tpItem->m_tNodeID < getAI().Header().count),"Invalid node for object ",tpItem->s_name_replace);
			m_tpServer->Process_spawn(tNetPacket,0,FALSE,tpItem);
			tpItem->o_Position		= tpALifeDynamicObject->o_Position;
			tpItem->m_tNodeID		= tpALifeDynamicObject->m_tNodeID;
			tpItem->s_flags.and		(u16(-1) ^ M_SPAWN_UPDATE);
			tpItem->m_bOnline		= true;
		}
	}
	
	if (bRemoveFromScheduled) {
		vfRemoveObjectFromScheduled	(tpALifeDynamicObject);
		vfRemoveObjectFromGraphPoint(tpALifeDynamicObject,tpALifeDynamicObject->m_tGraphID);
	}
}

void CSE_ALifeSimulator::vfRemoveOnlineObject(CSE_ALifeDynamicObject *tpALifeDynamicObject, bool bAddToScheduled)
{
	m_tpServer->Perform_destroy		(tpALifeDynamicObject,net_flags(TRUE,TRUE));
	_OBJECT_ID						l_tObjectID = tpALifeDynamicObject->ID;
	tpALifeDynamicObject->ID		= m_tpServer->PerformIDgen(l_tObjectID);
	R_ASSERT2						(l_tObjectID == tpALifeDynamicObject->ID,"Can't reserve a particular object identifier");

#ifdef DEBUG_LOG
	Msg("ALife : Destroying monster %s",tpALifeDynamicObject->s_name_replace);
#endif

	CSE_ALifeTraderAbstract			*tpTraderParams = dynamic_cast<CSE_ALifeTraderAbstract*>(tpALifeDynamicObject);
	if (tpTraderParams) {
		OBJECT_IT					I = tpALifeDynamicObject->children.begin();
		OBJECT_IT					E = tpALifeDynamicObject->children.end();
		for ( ; I != E; I++) {
			CSE_ALifeItem			*tpItem = dynamic_cast<CSE_ALifeItem*>(m_tObjectRegistry[*I]);
			if (!tpItem)
				continue;

#ifdef DEBUG_LOG
			Msg						("ALife : Destroying item %s",tpItem->s_name_replace);
#endif

			m_tpServer->Perform_destroy(tpItem,net_flags(TRUE,TRUE));
			_OBJECT_ID				l_tObjectID = tpItem->ID;
			tpItem->ID				= m_tpServer->PerformIDgen(tpItem->ID);
			R_ASSERT2				(tpItem->ID == l_tObjectID,"Object ID has changed during ID generation!");
			tpItem->m_bOnline		= false;
		}
	}
	
	if (bAddToScheduled) {
		vfAddObjectToScheduled		(tpALifeDynamicObject);
		vfAddObjectToGraphPoint		(tpALifeDynamicObject,tpALifeDynamicObject->m_tGraphID);
	}
}

void CSE_ALifeSimulator::vfSwitchObjectOnline(CSE_ALifeDynamicObject *tpALifeDynamicObject)
{
	VERIFY							(!tpALifeDynamicObject->m_bOnline);
	tpALifeDynamicObject->m_dwLastSwitchTime = 0;
	tpALifeDynamicObject->m_bOnline	= true;

#ifdef DEBUG_LOG
	Msg								("ALife : Going online [%d] '%s' as #%d, on '%s'",Device.TimerAsync(),tpALifeDynamicObject->s_name_replace, tpALifeDynamicObject->ID, "*SERVER*");
#endif
	
	CSE_ALifeAnomalousZone			*l_tpAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(tpALifeDynamicObject);
	if (l_tpAnomalousZone && (l_tpAnomalousZone->m_maxPower < EPS_L))
		return;

	CSE_ALifeAbstractGroup			*tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(tpALifeDynamicObject);
	if (tpALifeAbstractGroup) {
		OBJECT_IT					I = tpALifeAbstractGroup->m_tpMembers.begin(), B = I;
		OBJECT_IT					E = tpALifeAbstractGroup->m_tpMembers.end();
		u32							N = (u32)(E - I);
		for ( ; I != E; I++) {
			OBJECT_PAIR_IT			J = m_tObjectRegistry.find(*I);
			VERIFY					(J != m_tObjectRegistry.end());
			if (tpALifeAbstractGroup->m_bCreateSpawnPositions) {
				(*J).second->o_Position	= tpALifeDynamicObject->o_Position;
				(*J).second->m_tNodeID	= tpALifeDynamicObject->m_tNodeID;
				CSE_ALifeMonsterAbstract	*tpEnemy = dynamic_cast<CSE_ALifeMonsterAbstract*>((*J).second);
				if (tpEnemy)
					tpEnemy->o_torso.yaw = angle_normalize_signed((I - B)/N*PI_MUL_2);
			}
			vfCreateOnlineObject	((*J).second, false);
		}
		tpALifeAbstractGroup->m_bCreateSpawnPositions = false;
		vfRemoveObjectFromScheduled	(tpALifeDynamicObject);
		vfRemoveObjectFromGraphPoint(tpALifeDynamicObject,tpALifeDynamicObject->m_tGraphID);
	}
	else
		vfCreateOnlineObject		(tpALifeDynamicObject);
	
	tpALifeDynamicObject->m_dwLastSwitchTime = Device.TimerAsync();
}

void CSE_ALifeSimulator::vfSwitchObjectOffline(CSE_ALifeDynamicObject *tpALifeDynamicObject)
{
	VERIFY							(tpALifeDynamicObject->m_bOnline);
	tpALifeDynamicObject->m_dwLastSwitchTime = 0;
	tpALifeDynamicObject->m_bOnline	= false;
#ifdef DEBUG_LOG
	Msg								("ALife : Going offline [%d] '%s' as #%d, on '%s'",Device.TimerAsync(),tpALifeDynamicObject->s_name_replace, tpALifeDynamicObject->ID, "*SERVER*");
#endif
	
	CSE_ALifeAnomalousZone			*l_tpAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(tpALifeDynamicObject);
	if (l_tpAnomalousZone && (l_tpAnomalousZone->m_maxPower < EPS_L))
		return;

	CSE_ALifeAbstractGroup				*tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(tpALifeDynamicObject);
	if (tpALifeAbstractGroup) {
		OBJECT_IT I = tpALifeAbstractGroup->m_tpMembers.begin();
		OBJECT_IT E = tpALifeAbstractGroup->m_tpMembers.end();
		if (I != E) {
			OBJECT_PAIR_IT			J = m_tObjectRegistry.find(*I);
			VERIFY					(J != m_tObjectRegistry.end());
			CSE_ALifeMonsterAbstract	*tpGroupMember	= dynamic_cast<CSE_ALifeMonsterAbstract*>((*J).second);
			CSE_ALifeMonsterAbstract	*tpGroup		= dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeAbstractGroup);
			if (tpGroupMember && tpGroup) {
				tpGroup->m_fCurSpeed	= tpGroup->m_fGoingSpeed;
				tpGroup->o_Position		= tpGroupMember->o_Position;
				u32	dwNodeID			= tpGroup->m_tNodeID;
				tpGroup->m_tGraphID		= getAI().m_tpaCrossTable[dwNodeID].tGraphIndex;
				tpGroup->m_fDistanceToPoint = getAI().m_tpaCrossTable[dwNodeID].fDistance;
				tpGroup->m_tNextGraphID	= tpGroup->m_tGraphID;
				u16	wNeighbourCount		= (u16)getAI().m_tpaGraph[tpGroup->m_tGraphID].tNeighbourCount;
				CSE_ALifeGraph::SGraphEdge	*tpaEdges = (CSE_ALifeGraph::SGraphEdge *)((BYTE *)getAI().m_tpaGraph + getAI().m_tpaGraph[tpGroup->m_tGraphID].dwEdgeOffset);
				tpGroup->m_tPrevGraphID	= _GRAPH_ID(tpaEdges[randI(0,wNeighbourCount)].dwVertexNumber);
			}
			vfRemoveOnlineObject	((*J).second,false);
			I++;
		}
		for ( ; I != E; I++) {
			OBJECT_PAIR_IT			J = m_tObjectRegistry.find(*I);
			VERIFY					(J != m_tObjectRegistry.end());
			vfRemoveOnlineObject	((*J).second,false);
		}
		vfAddObjectToScheduled		(tpALifeDynamicObject);
		vfAddObjectToGraphPoint		(tpALifeDynamicObject,tpALifeDynamicObject->m_tGraphID);
	}
	else
		vfRemoveOnlineObject		(tpALifeDynamicObject);
	tpALifeDynamicObject->m_dwLastSwitchTime = Device.TimerAsync();
}

// switch object offline and check if it is a group of monsters then separate dead monsters from the group
void CSE_ALifeSimulator::vfFurlObjectOffline(CSE_ALifeDynamicObject *I)
{
	if (I->m_bOnline)
		if (I->ID_Parent == 0xffff) {
			CSE_ALifeAbstractGroup *tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(I);
			if (tpALifeAbstractGroup)
				for (u32 i=0, N = (u32)tpALifeAbstractGroup->m_tpMembers.size(); i<N; i++) {
					OBJECT_PAIR_IT			J = m_tObjectRegistry.find(tpALifeAbstractGroup->m_tpMembers[i]);
					VERIFY					(J != m_tObjectRegistry.end());
					CSE_ALifeMonsterAbstract	*tpEnemy = dynamic_cast<CSE_ALifeMonsterAbstract*>((*J).second);
					if (tpEnemy && tpEnemy->fHealth <= 0) {
						(*J).second->m_bDirectControl	= true;
						(*J).second->m_bOnline			= true;
						tpALifeAbstractGroup->m_tpMembers.erase(tpALifeAbstractGroup->m_tpMembers.begin() + i);
						vfUpdateDynamicData((*J).second);
						i--;
						N--;
						continue;
					}
				}
				vfSwitchObjectOffline(I);
		}
		else {
			OBJECT_PAIR_IT		J = m_tObjectRegistry.find(I->ID_Parent);
			VERIFY				(J != m_tObjectRegistry.end());
			if (!(*J).second->m_bOnline) {
				VERIFY			(false);
				vfSwitchObjectOffline(I);
			}
		}
}

void CSE_ALifeSimulator::ProcessOnlineOfflineSwitches(CSE_ALifeDynamicObject *I)
{
	// updating node if it is invalid and object is not attached and online
	if ((I->m_bOnline || (I->m_tNodeID <= 0) || (I->m_tNodeID >= getAI().Header().count)) && (I->ID_Parent == 0xffff)) {
		// checking if it is a group of objects
		CSE_ALifeAbstractGroup *tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(I);
		if (tpALifeAbstractGroup) {
			// checking if group is empty then remove it
			if (tpALifeAbstractGroup->m_tpMembers.empty())
				vfReleaseObject(I);
			else {
				// getting pointer to the very first group member via its ID
				OBJECT_PAIR_IT			J = m_tObjectRegistry.find(tpALifeAbstractGroup->m_tpMembers[0]);
				R_ASSERT2				(J != m_tObjectRegistry.end(),"Group member not found in the Object registry!");
				// assign group position to the member position
				I->o_Position			= (*J).second->o_Position;
				if (!getAI().bfInsideNode(getAI().Node(I->m_tNodeID),I->o_Position)) {
					// checking if position is inside the current node
					I->m_tNodeID		= getAI().q_Node(I->m_tNodeID,I->o_Position);
					// validating graph point and changing it if needed
					_GRAPH_ID			tGraphID = getAI().m_tpaCrossTable[I->m_tNodeID].tGraphIndex;
					if ((tGraphID != I->m_tGraphID) && (I->ID_Parent == 0xffff) && !I->m_bOnline)
						vfChangeObjectGraphPoint(I,I->m_tGraphID,tGraphID);
					// validating distance to graph point via graph cross-table
					I->m_fDistance		= getAI().m_tpaCrossTable[I->m_tNodeID].fDistance;
				}
			}
		}
		else {
			// otherwise validate position, graph point and node
			if (!getAI().bfInsideNode(getAI().Node(I->m_tNodeID),I->o_Position)) {
				// checking if position is inside the current node
				I->m_tNodeID			= getAI().q_Node(I->m_tNodeID,I->o_Position);
				// validating graph point and changing it if needed
				_GRAPH_ID				tGraphID = getAI().m_tpaCrossTable[I->m_tNodeID].tGraphIndex;
				if ((tGraphID != I->m_tGraphID) && (I->ID_Parent == 0xffff) && !I->m_bOnline)
					vfChangeObjectGraphPoint(I,I->m_tGraphID,tGraphID);
				// validating distance to graph point via graph cross-table
				I->m_fDistance			= getAI().m_tpaCrossTable[I->m_tNodeID].fDistance;
			}
		}
	}
	
	// checking if the object is online
	if (I->m_bOnline) {
		// checking if the object is not attached
		if (I->ID_Parent == 0xffff) {
			// checking if the object is not a group of objects
			CSE_ALifeAbstractGroup *tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(I);
			if (!tpALifeAbstractGroup) {
				// checking if the object is ready to switch offline
				if (m_tpActor->o_Position.distance_to(I->o_Position) > m_fOnlineDistance) {
					// checking if the object wants to switch offline during the given amount of time
					if (Device.TimerAsync() -  I->m_dwLastSwitchTime > m_dwSwitchDelay)
						// switching the object offline
						vfSwitchObjectOffline(I);
				}
				else
					// object is not ready to switch offline, therefore
					// set its start time to from which it wants to switch offline to the current time
					I->m_dwLastSwitchTime = Device.TimerAsync();
			}
			else {
				// so, we have a group of objects
				// therefore check all the group members if they are ready to switch offline

				// iterating on group members
				for (u32 i=0, N = (u32)tpALifeAbstractGroup->m_tpMembers.size(); i<N; i++) {
					// getting pointer to the group member via its ID
					OBJECT_PAIR_IT			J = m_tObjectRegistry.find(tpALifeAbstractGroup->m_tpMembers[i]);
					R_ASSERT2				(J != m_tObjectRegistry.end(),"Group member is not found in the Object registry!");
					// casting group member to the abstract monster to get access to the Health property
					CSE_ALifeMonsterAbstract	*tpGroupMember = dynamic_cast<CSE_ALifeMonsterAbstract*>((*J).second);
					if (tpGroupMember)
						// check if monster is not dead
						if (tpGroupMember->fHealth <= 0) {
							// detach object from the group
							tpGroupMember->m_bDirectControl	= true;
							tpGroupMember->m_bOnline		= true;
							tpALifeAbstractGroup->m_tpMembers.erase(tpALifeAbstractGroup->m_tpMembers.begin() + i);
							// store the new separate object into the registries
							vfUpdateDynamicData(tpGroupMember);
							i--;
							N--;
							continue;
						}
						else
							// so, monster is not dead
							// checking if the object is _not_ ready to switch offline
							if (m_tpActor->o_Position.distance_to((*J).second->o_Position) <= m_fOnlineDistance)
								// so, it is not ready, breaking a cycle, because we can't 
								// switch group offline since not all the group members are ready
								// to switch offline
								break;
				}
				// checking if group is not empty
				if (tpALifeAbstractGroup->m_tpMembers.size() && (i == N)) {
					// checking if the group wants to switch offline during the given amount of time
					if (Device.TimerAsync() -  I->m_dwLastSwitchTime > m_dwSwitchDelay)
						// switching the group offline
						vfSwitchObjectOffline(I);
				}
				else
					// group is not ready to switch offline, therefore
					// set its start time to from which it wants to switch offline to the current time
					I->m_dwLastSwitchTime = Device.TimerAsync();
			}
		}
		else {
			// getting pointer to the parent object via its ID
			OBJECT_PAIR_IT		J = m_tObjectRegistry.find(I->ID_Parent);
			R_ASSERT2			(J != m_tObjectRegistry.end(),"Invalid parent object ID!");
			// checking if parent is online too
			R_ASSERT2			((*J).second->m_bOnline,"Parent offline, item online...");
		}
	}
	else {
		// so, the object is offline
		// checking if the object is not attached
		if (I->ID_Parent == 0xffff) {
			// checking if the object is not an empty group of objects
			CSE_ALifeAbstractGroup *tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(I);
			if (tpALifeAbstractGroup && (!tpALifeAbstractGroup->m_tpMembers.size())) {
				// relase empty group of objects
				vfReleaseObject(tpALifeAbstractGroup);
				return;
			}
			else {
				CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(I);
				// checking if the abstract monster has just died
				if (tpALifeMonsterAbstract && (tpALifeMonsterAbstract->fHealth <= 0) && !tpALifeMonsterAbstract->m_bOnline && (m_tpScheduledObjects.find(tpALifeMonsterAbstract->ID) != m_tpScheduledObjects.end()))
					vfRemoveObjectFromScheduled(tpALifeMonsterAbstract);
			}
			
			// checking if the object is ready to switch online
			if (m_tpActor->o_Position.distance_to(I->o_Position) <= m_fOnlineDistance) {
				// checking if the object wants to switch online during the given amount of time
				if (Device.TimerAsync() -  I->m_dwLastSwitchTime > m_dwSwitchDelay)
					// switch object online
					vfSwitchObjectOnline(I);
			}
			else
				// object is not ready to switch online, therefore
				// set its start time to from which it wants to switch offline to the current time
				I->m_dwLastSwitchTime = Device.TimerAsync();
		}
		else {
			// so, object is attached
			// getting pointer to the parent object via its ID
			OBJECT_PAIR_IT		J = m_tObjectRegistry.find(I->ID_Parent);
			R_ASSERT2			(J != m_tObjectRegistry.end(),"Invalid parent object ID!");
			// checking if parent is offline too
			R_ASSERT2			(!(*J).second->m_bOnline,"Parent online, item offline...");
		}
	}
}