////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_switch.cpp
//	Created 	: 25.12.2002
//  Modified 	: 29.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A-Life Online/Offline switches
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "GameObject.h"

void CAI_ALife::vfCreateObject(CALifeDynamicObject *tpALifeDynamicObject)
{
	NET_Packet						tNetPacket;
	
	tpALifeDynamicObject->s_flags.or(M_SPAWN_UPDATE);
	m_tpServer->Process_spawn		(tNetPacket,0,FALSE,tpALifeDynamicObject);
	tpALifeDynamicObject->s_flags.and(u16(-1) ^ M_SPAWN_UPDATE);
	Msg("ALife : Spawning object %s",tpALifeDynamicObject->s_name_replace);

	CALifeTraderParams				*tpTraderParams = dynamic_cast<CALifeTraderParams*>(tpALifeDynamicObject);
	if (tpTraderParams) {
		m_tpChildren				= tpALifeDynamicObject->children;
		tpALifeDynamicObject->children.clear();
		OBJECT_IT					I = m_tpChildren.begin();
		OBJECT_IT					E = m_tpChildren.end();
		for ( ; I != E; I++) {
			xrServerEntity			*t = dynamic_cast<xrServerEntity*>(m_tObjectRegistry[*I]);
			CALifeItem				*tpItem = dynamic_cast<CALifeItem*>(m_tObjectRegistry[*I]);
			if (!tpItem)
				continue;
			tpItem->s_flags.or		(M_SPAWN_UPDATE);
			Msg("ALife : Spawning item %s",tpItem->s_name_replace);
			m_tpServer->Process_spawn(tNetPacket,0,FALSE,tpItem);
			tpItem->s_flags.and		(u16(-1) ^ M_SPAWN_UPDATE);
			tpItem->m_bOnline		= true;
		}
	}
}

void CAI_ALife::vfReleaseObject(CALifeDynamicObject *tpALifeDynamicObject)
{
	CALifeTraderParams				*tpTraderParams = dynamic_cast<CALifeTraderParams*>(tpALifeDynamicObject);
	if (tpTraderParams) {
		m_tpChildren				= tpALifeDynamicObject->children;
		OBJECT_IT					I = tpALifeDynamicObject->children.begin();
		OBJECT_IT					E = tpALifeDynamicObject->children.end();
		for ( ; I != E; I++) {
			xrServerEntity			*t = dynamic_cast<xrServerEntity*>(m_tObjectRegistry[*I]);
			CALifeItem				*tpItem = dynamic_cast<CALifeItem*>(m_tObjectRegistry[*I]);
			if (!tpItem)
				continue;
			Msg("ALife : Destroying item %s",tpItem->s_name_replace);
			m_tpServer->Perform_destroy(tpItem,net_flags(TRUE,TRUE));
			tpItem->m_bOnline		= false;
		}
		tpALifeDynamicObject->children = m_tpChildren;
	}
	Msg("ALife : Destroying monster %s",tpALifeDynamicObject->s_name_replace);
	m_tpServer->Perform_destroy		(tpALifeDynamicObject,net_flags(TRUE,TRUE));
}

void CAI_ALife::vfSwitchObjectOnline(CALifeDynamicObject *tpALifeDynamicObject)
{
	VERIFY							(!tpALifeDynamicObject->m_bOnline);
	CALifeAbstractGroup				*tpALifeAbstractGroup = dynamic_cast<CALifeAbstractGroup*>(tpALifeDynamicObject);
	if (tpALifeAbstractGroup) {
		OBJECT_IT					I = tpALifeAbstractGroup->m_tpMembers.begin(), B = I;
		OBJECT_IT					E = tpALifeAbstractGroup->m_tpMembers.end();
		u32							N = E - I;
		for ( ; I != E; I++) {
			OBJECT_PAIR_IT			J = m_tObjectRegistry.find(*I);
			VERIFY					(J != m_tObjectRegistry.end());
			if (tpALifeAbstractGroup->m_bCreateSpawnPositions) {
				(*J).second->o_Position	= tpALifeDynamicObject->o_Position;
				(*J).second->m_tNodeID	= tpALifeDynamicObject->m_tNodeID;
				xrSE_Enemy				*tpEnemy = dynamic_cast<xrSE_Enemy*>((*J).second);
				if (tpEnemy)
					tpEnemy->o_torso.yaw = angle_normalize_signed((I - B)/N*PI_MUL_2);
			}
			vfCreateObject		((*J).second);
		}
		tpALifeAbstractGroup->m_bCreateSpawnPositions = false;
	}
	else
		vfCreateObject					(tpALifeDynamicObject);
	tpALifeDynamicObject->m_dwLastSwitchTime = 0;
	tpALifeDynamicObject->m_bOnline	= true;
	Msg								("ALife : Going online [%d] '%s'(%d,%d,%d) as #%d, on '%s'",Device.dwTimeGlobal,tpALifeDynamicObject->s_name_replace, tpALifeDynamicObject->g_team(), tpALifeDynamicObject->g_squad(), tpALifeDynamicObject->g_group(), tpALifeDynamicObject->ID, "*SERVER*");
}

void CAI_ALife::vfSwitchObjectOffline(CALifeDynamicObject *tpALifeDynamicObject)
{
	VERIFY							(tpALifeDynamicObject->m_bOnline);
	CALifeAbstractGroup				*tpALifeAbstractGroup = dynamic_cast<CALifeAbstractGroup*>(tpALifeDynamicObject);
	if (tpALifeAbstractGroup) {
		OBJECT_IT I = tpALifeAbstractGroup->m_tpMembers.begin();
		OBJECT_IT E = tpALifeAbstractGroup->m_tpMembers.end();
		if (I != E) {
			OBJECT_PAIR_IT			J = m_tObjectRegistry.find(*I);
			VERIFY					(J != m_tObjectRegistry.end());
			CALifeMonsterAbstract	*tpEnemy = dynamic_cast<CALifeMonsterAbstract*>((*J).second);
			CALifeMonsterAbstract	*tpALifeMonsterAbstract = dynamic_cast<CALifeMonsterAbstract*>(tpALifeAbstractGroup);
			if (tpEnemy && tpALifeMonsterAbstract) {
				tpALifeMonsterAbstract->m_fCurSpeed		= tpALifeMonsterAbstract->m_fGoingSpeed;
				tpALifeMonsterAbstract->o_Position		= tpEnemy->o_Position;
				u32 dwNodeID = tpALifeMonsterAbstract->m_tNodeID;
				tpALifeMonsterAbstract->m_tGraphID		= getAI().m_tpaCrossTable[dwNodeID].tGraphIndex;
				tpALifeMonsterAbstract->m_fDistanceToPoint = getAI().m_tpaCrossTable[dwNodeID].fDistance;
				tpALifeMonsterAbstract->m_tNextGraphID	= tpALifeMonsterAbstract->m_tGraphID;
				u16					wNeighbourCount = (u16)getAI().m_tpaGraph[tpALifeMonsterAbstract->m_tGraphID].tNeighbourCount;
				CALifeGraph::SGraphEdge			*tpaEdges		= (CALifeGraph::SGraphEdge *)((BYTE *)getAI().m_tpaGraph + getAI().m_tpaGraph[tpALifeMonsterAbstract->m_tGraphID].dwEdgeOffset);
				tpALifeMonsterAbstract->m_tPrevGraphID	= _GRAPH_ID(tpaEdges[::Random.randI(0,wNeighbourCount)].dwVertexNumber);
			}
			vfReleaseObject			((*J).second);
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
	tpALifeDynamicObject->m_dwLastSwitchTime = 0;
	tpALifeDynamicObject->m_bOnline	= false;
	Msg								("ALife : Going offline [%d] '%s'(%d,%d,%d) as #%d, on '%s'",Device.dwTimeGlobal,tpALifeDynamicObject->s_name_replace, tpALifeDynamicObject->g_team(), tpALifeDynamicObject->g_squad(), tpALifeDynamicObject->g_group(), tpALifeDynamicObject->ID, "*SERVER*");
}

void CAI_ALife::ProcessOnlineOfflineSwitches(CALifeDynamicObject *I)
{
	if (I->m_bOnline || !I->m_tNodeID || (I->m_tNodeID >= getAI().Header().count)) {
		I->m_tNodeID = getAI().q_Node(I->m_tNodeID,I->o_Position);
		I->m_tGraphID = getAI().m_tpaCrossTable[I->m_tNodeID].tGraphIndex;
		I->m_fDistance = getAI().m_tpaCrossTable[I->m_tNodeID].fDistance;
	}
	if (!I->m_tNodeID || (I->m_tNodeID >= getAI().Header().count))
		Msg("! ALife : Corresponding node hasn't been found for object %s",I->s_name_replace);
	if (I->m_bOnline)
		if (I->ID_Parent == 0xffff) {
			if (I->m_dwLastSwitchTime) {
				CALifeAbstractGroup *tpALifeAbstractGroup = dynamic_cast<CALifeAbstractGroup*>(I);
				if (!tpALifeAbstractGroup) {
					if (m_tpActor->o_Position.distance_to(I->o_Position) > m_fOnlineDistance) {
						if (Device.dwTimeGlobal -  I->m_dwLastSwitchTime > m_dwSwitchDelay)
							vfSwitchObjectOffline(I);
					}
					else
						I->m_dwLastSwitchTime = 0;
				}
				else {
					u32		  dwCount = 0;
					for (u32 i=0, N = tpALifeAbstractGroup->m_tpMembers.size(); i<N; i++) {
						OBJECT_PAIR_IT			J = m_tObjectRegistry.find(tpALifeAbstractGroup->m_tpMembers[i]);
						VERIFY					(J != m_tObjectRegistry.end());
						xrSE_Enemy				*tpEnemy = dynamic_cast<xrSE_Enemy*>((*J).second);
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
						if (Device.dwTimeGlobal -  I->m_dwLastSwitchTime > m_dwSwitchDelay)
							vfSwitchObjectOffline(I);
					}
					else
						I->m_dwLastSwitchTime = 0;
				}
			}
			else
				I->m_dwLastSwitchTime = Device.dwTimeGlobal;
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
				CALifeAbstractGroup *tpALifeAbstractGroup = dynamic_cast<CALifeAbstractGroup*>(I);
				if (tpALifeAbstractGroup && (!tpALifeAbstractGroup->m_tpMembers.size()))
					return;
				
				if (m_tpActor->o_Position.distance_to(I->o_Position) <= m_fOnlineDistance) {
					if (Device.dwTimeGlobal -  I->m_dwLastSwitchTime > m_dwSwitchDelay)
						vfSwitchObjectOnline(I);
				}
				else
					I->m_dwLastSwitchTime = 0;
			}
			else
				I->m_dwLastSwitchTime = Device.dwTimeGlobal;
		}
		else {
			OBJECT_PAIR_IT		J = m_tObjectRegistry.find(I->ID_Parent);
			VERIFY				(J != m_tObjectRegistry.end());
			if ((*J).second->m_bOnline) {
				CALifeDynamicObject *tpALifeDynamicObject = (*J).second;
				OBJECT_IT					i = tpALifeDynamicObject->children.begin();
				OBJECT_IT					e = tpALifeDynamicObject->children.end();
				for ( ; i != e; i++)
					if (*i == I->ID) {
						i=i;
					}
				
				VERIFY				(false);
				vfSwitchObjectOnline(I);
			}
		}
	
	CALifeMonsterAbstract *tpALifeMonsterAbstract = dynamic_cast<CALifeMonsterAbstract*>(I);
	if (tpALifeMonsterAbstract && (tpALifeMonsterAbstract->fHealth <= 0)) {
		ALIFE_MONSTER_P_IT	i = m_tpScheduledObjects.begin();
		ALIFE_MONSTER_P_IT	e = m_tpScheduledObjects.end();
		_OBJECT_ID			tObjectID = tpALifeMonsterAbstract->m_tObjectID;
		for ( ; i != e; i++)
			if ((*i)->m_tObjectID == tObjectID) {
				m_tpScheduledObjects.erase(i);
				break;
			}
	}
}