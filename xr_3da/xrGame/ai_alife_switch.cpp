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
}

void CAI_ALife::vfReleaseObject(CALifeDynamicObject *tpALifeDynamicObject)
{
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
				//Fvector tTemp;
				//tTemp.set(::Random.randF(0,.35f),0,::Random.randF(0,.35f));
				//(*J).second->o_Position.add(tpALifeDynamicObject->o_Position,tTemp);
				(*J).second->o_Position	= tpALifeDynamicObject->o_Position;
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
	Msg								("- SERVER: Going online [%d] '%s'(%d,%d,%d) as #%d, on '%s'",Device.dwTimeGlobal,tpALifeDynamicObject->s_name_replace, tpALifeDynamicObject->g_team(), tpALifeDynamicObject->g_squad(), tpALifeDynamicObject->g_group(), tpALifeDynamicObject->ID, "*SERVER*");
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
				u32 dwNodeID = getAI().q_LoadSearch(tpALifeMonsterAbstract->o_Position = tpEnemy->o_Position);
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
	Msg								("- SERVER: Going offline [%d] '%s'(%d,%d,%d) as #%d, on '%s'",Device.dwTimeGlobal,tpALifeDynamicObject->s_name_replace, tpALifeDynamicObject->g_team(), tpALifeDynamicObject->g_squad(), tpALifeDynamicObject->g_group(), tpALifeDynamicObject->ID, "*SERVER*");
}

void CAI_ALife::ProcessOnlineOfflineSwitches(CALifeDynamicObject *I)
{
	I->m_tNodeID = getAI().q_Node(I->m_tNodeID,I->o_Position);
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
			if (!(*J).second->m_bOnline)
				vfSwitchObjectOffline(I);
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
			if ((*J).second->m_bOnline)
				vfSwitchObjectOnline(I);
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