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

void CSE_ALifeSimulator::vfRemoveObject(CSE_Abstract *tpSE_Abstract)
{
	CSE_ALifeDynamicObject			*tpALifeDynamicObject = m_tObjectRegistry[tpSE_Abstract->ID];
	VERIFY							(tpALifeDynamicObject);
	m_tObjectRegistry.erase			(tpSE_Abstract->ID);
	
	vfRemoveObjectFromGraphPoint	(tpALifeDynamicObject,tpALifeDynamicObject->m_tGraphID);
	vfRemoveObjectFromCurrentLevel	(tpALifeDynamicObject);
	vfRemoveObjectFromScheduled		(tpALifeDynamicObject);

	tpSE_Abstract->m_bALifeControl = false;
}

void CSE_ALifeSimulator::vfCreateObject(CSE_ALifeDynamicObject *tpALifeDynamicObject, bool bRemoveFromScheduled)
{
	NET_Packet						tNetPacket;
	
	CSE_Abstract					*l_tpAbstract = dynamic_cast<CSE_Abstract*>(tpALifeDynamicObject);
	m_tpServer->entity_Destroy		(l_tpAbstract);
	tpALifeDynamicObject->s_flags.or(M_SPAWN_UPDATE);
	m_tpServer->Process_spawn		(tNetPacket,0,FALSE,l_tpAbstract);
	tpALifeDynamicObject->s_flags.and(u16(-1) ^ M_SPAWN_UPDATE);
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

void CSE_ALifeSimulator::vfReleaseObject(CSE_ALifeDynamicObject *tpALifeDynamicObject, bool bAddToScheduled)
{
	//VERIFY(tpALifeDynamicObject->ID_Parent == 0xffff);
	m_tpServer->Perform_destroy		(tpALifeDynamicObject,net_flags(TRUE,TRUE));
	_OBJECT_ID						l_tObjectID = tpALifeDynamicObject->ID;
	tpALifeDynamicObject->ID		= m_tpServer->PerformIDgen(l_tObjectID);
	R_ASSERT2						(l_tObjectID == tpALifeDynamicObject->ID,"Can't reserve a particular object identifier");

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
#ifdef DEBUG_LOG
	Msg("ALife : Destroying monster %s",tpALifeDynamicObject->s_name_replace);
#endif
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
			vfCreateObject		((*J).second, false);
		}
		tpALifeAbstractGroup->m_bCreateSpawnPositions = false;
	}
	else
		vfCreateObject					(tpALifeDynamicObject);
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
			CSE_ALifeMonsterAbstract	*tpEnemy = dynamic_cast<CSE_ALifeMonsterAbstract*>((*J).second);
			CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeAbstractGroup);
			if (tpEnemy && tpALifeMonsterAbstract) {
				tpALifeMonsterAbstract->m_fCurSpeed		= tpALifeMonsterAbstract->m_fGoingSpeed;
				tpALifeMonsterAbstract->o_Position		= tpEnemy->o_Position;
				u32					dwNodeID			= tpALifeMonsterAbstract->m_tNodeID;
				tpALifeMonsterAbstract->m_tGraphID		= getAI().m_tpaCrossTable[dwNodeID].tGraphIndex;
				tpALifeMonsterAbstract->m_fDistanceToPoint = getAI().m_tpaCrossTable[dwNodeID].fDistance;
				tpALifeMonsterAbstract->m_tNextGraphID	= tpALifeMonsterAbstract->m_tGraphID;
				u16					wNeighbourCount		= (u16)getAI().m_tpaGraph[tpALifeMonsterAbstract->m_tGraphID].tNeighbourCount;
				CSE_ALifeGraph::SGraphEdge				*tpaEdges = (CSE_ALifeGraph::SGraphEdge *)((BYTE *)getAI().m_tpaGraph + getAI().m_tpaGraph[tpALifeMonsterAbstract->m_tGraphID].dwEdgeOffset);
				tpALifeMonsterAbstract->m_tPrevGraphID	= _GRAPH_ID(tpaEdges[randI(0,wNeighbourCount)].dwVertexNumber);
			}
			vfReleaseObject			((*J).second, false);
			I++;
		}
		for ( ; I != E; I++) {
			OBJECT_PAIR_IT			J = m_tObjectRegistry.find(*I);
			VERIFY					(J != m_tObjectRegistry.end());
			vfReleaseObject			((*J).second);
		}
	}
	else
		vfReleaseObject				(tpALifeDynamicObject);
}

void CSE_ALifeSimulator::ProcessOnlineOfflineSwitches(CSE_ALifeDynamicObject *I)
{
	if ((I->m_bOnline || (I->m_tNodeID < 0) || (I->m_tNodeID >= getAI().Header().count)) && (I->ID_Parent == 0xffff)) {
		if (!getAI().bfInsideNode(getAI().Node(I->m_tNodeID),I->o_Position)) {
			I->m_tNodeID = getAI().q_Node(I->m_tNodeID,I->o_Position);
			_GRAPH_ID tGraphID = getAI().m_tpaCrossTable[I->m_tNodeID].tGraphIndex;
			if ((tGraphID != I->m_tGraphID) && (I->ID_Parent == 0xffff) && !I->m_bOnline)
				vfChangeObjectGraphPoint(I,I->m_tGraphID,tGraphID);
			I->m_fDistance = getAI().m_tpaCrossTable[I->m_tNodeID].fDistance;
		}
	}
	if (I->m_bOnline)
		if (I->ID_Parent == 0xffff) {
			if (I->m_dwLastSwitchTime) {
				CSE_ALifeAbstractGroup *tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(I);
				if (!tpALifeAbstractGroup) {
					if (m_tpActor->o_Position.distance_to(I->o_Position) > m_fOnlineDistance) {
						if (Device.TimerAsync() -  I->m_dwLastSwitchTime > m_dwSwitchDelay)
							vfSwitchObjectOffline(I);
					}
					else
						I->m_dwLastSwitchTime = 0;
				}
				else {
					u32		  dwCount = 0;
					for (u32 i=0, N = (u32)tpALifeAbstractGroup->m_tpMembers.size(); i<N; i++) {
						OBJECT_PAIR_IT			J = m_tObjectRegistry.find(tpALifeAbstractGroup->m_tpMembers[i]);
						VERIFY					(J != m_tObjectRegistry.end());
						CSE_ALifeMonsterAbstract	*tpEnemy = dynamic_cast<CSE_ALifeMonsterAbstract*>((*J).second);
						if (tpEnemy)
							if (tpEnemy->fHealth <= 0) {
								(*J).second->m_bDirectControl	= true;
								(*J).second->m_bOnline			= true;
								tpALifeAbstractGroup->m_tpMembers.erase(tpALifeAbstractGroup->m_tpMembers.begin() + i);
								vfUpdateDynamicData((*J).second);
								i--;
								N--;
								continue;
							}
							else
								if (m_tpActor->o_Position.distance_to((*J).second->o_Position) > m_fOnlineDistance)
									dwCount++;
					}
					if (tpALifeAbstractGroup->m_tpMembers.size() && (dwCount == tpALifeAbstractGroup->m_tpMembers.size())) {
						if (Device.TimerAsync() -  I->m_dwLastSwitchTime > m_dwSwitchDelay)
							vfSwitchObjectOffline(I);
					}
					else
						I->m_dwLastSwitchTime = 0;
				}
			}
			else
				I->m_dwLastSwitchTime = Device.TimerAsync();
		}
		else {
			OBJECT_PAIR_IT		J = m_tObjectRegistry.find(I->ID_Parent);
			VERIFY				(J != m_tObjectRegistry.end());
			if (!(*J).second->m_bOnline) {
				VERIFY			(false);
				vfSwitchObjectOffline(I);
			}
		}
	else
		if (I->ID_Parent == 0xffff) {
			if (I->m_dwLastSwitchTime) {
				CSE_ALifeAbstractGroup *tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(I);
				if (tpALifeAbstractGroup && (!tpALifeAbstractGroup->m_tpMembers.size()))
					return;
				
				if (m_tpActor->o_Position.distance_to(I->o_Position) <= m_fOnlineDistance) {
					if (Device.TimerAsync() -  I->m_dwLastSwitchTime > m_dwSwitchDelay)
						vfSwitchObjectOnline(I);
				}
				else
					I->m_dwLastSwitchTime = 0;
			}
			else
				I->m_dwLastSwitchTime = Device.TimerAsync();
		}
		else {
			OBJECT_PAIR_IT		J = m_tObjectRegistry.find(I->ID_Parent);
			VERIFY				(J != m_tObjectRegistry.end());
			if ((*J).second->m_bOnline) {
				CSE_ALifeDynamicObject	*tpALifeDynamicObject = (*J).second;
				OBJECT_IT				i = tpALifeDynamicObject->children.begin();
				OBJECT_IT				e = tpALifeDynamicObject->children.end();
				for ( ; i != e; i++)
					if (*i == I->ID) {
						i=i;
					}
				
				VERIFY				(false);
				vfSwitchObjectOnline(I);
			}
		}
	
	CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(I);
	if (tpALifeMonsterAbstract && (tpALifeMonsterAbstract->fHealth <= 0) && (m_tpScheduledObjects.find(tpALifeMonsterAbstract->ID) != m_tpScheduledObjects.end()))
		vfRemoveObjectFromScheduled(tpALifeMonsterAbstract);
}

// switch object offline an check if it is a group of monsters then separate dead monsters from the group
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