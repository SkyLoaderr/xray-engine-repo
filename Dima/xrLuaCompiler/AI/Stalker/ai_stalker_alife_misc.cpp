////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_alife_misc.cpp
//	Created 	: 15.09.2003
//  Modified 	: 15.09.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker ALife miscellanious functions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "..\\..\\ai_alife.h"
#include "..\\..\\a_star.h"

bool CAI_Stalker::bfHealthIsGood			()
{
	return(true);
}

bool CAI_Stalker::bfItemCanTreat			(CInventoryItem *tpInventoryItem)
{
	return(false);
}

void CAI_Stalker::vfUseItem					(CInventoryItem *tpInventoryItem)
{
}

bool CAI_Stalker::bfCanTreat				()
{
//	xr_vector<u16>::iterator	I = children.begin();
//	xr_vector<u16>::iterator	E = children.end();
//	for ( ; I != E; I++) {
//		CSE_ALifeInventoryItem	*l_tpALifeInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>(m_tpALife->tpfGetObjectByID(*I));
//		R_ASSERT2				(l_tpALifeInventoryItem,"Non inventory item ihas a parent?!");
//		if (bfItemCanTreat(l_tpALifeInventoryItem))
//			vfUseItem			(l_tpALifeInventoryItem);
//	}
	return						(bfHealthIsGood());
}

bool CAI_Stalker::bfEnoughMoneyToTreat		()
{
	return(true);
}

bool CAI_Stalker::bfEnoughTimeToTreat		()
{
	return(true);
}

bool CAI_Stalker::bfEnoughEquipmentToGo	()
{
	return(true);
}

bool CAI_Stalker::bfDistanceToTraderIsDanger()
{
	return(false);
}

bool CAI_Stalker::bfEnoughMoneyToEquip		()
{
	return(true);
}

bool CAI_Stalker::bfCheckIfTaskCompleted()
{
	if (int(m_tTaskID) < 0)
		return		(false);

	CSE_ALifeTask	&tTask = *m_tpALife->tpfGetTaskByID(m_tTaskID);
	switch (tTask.m_tTaskType) {
		case eTaskTypeSearchForItemCL :
		case eTaskTypeSearchForItemCG : {
			if (m_inventory.dwfGetSameItemCount(tTask.m_caSection))
				return(true);
			break;
		}
		case eTaskTypeSearchForItemOL :
		case eTaskTypeSearchForItemOG : {
			if (m_inventory.bfCheckForObject(tTask.m_tObjectID))
				return(true);
			break;
		}
	}
	return			(false);
}

void CAI_Stalker::vfChooseHumanTask()
{
	OBJECT_IT					I = m_tpKnownCustomers.begin();
	OBJECT_IT					E = m_tpKnownCustomers.end();
	for ( ; I != E; I++) {
		OBJECT_TASK_PAIR_IT		J = m_tpALife->m_tTaskCrossMap.find(*I);
		R_ASSERT2				(J != m_tpALife->m_tTaskCrossMap.end(),"Can't find a specified customer in the Task registry!\nPossibly, there is no traders at all or there is no anomalous zones.");

		u32						l_dwMinTryCount = u32(-1);
		_TASK_ID				l_tBestTaskID = _TASK_ID(-1);
		TASK_SET_IT				i = (*J).second.begin();
		TASK_SET_IT				e = (*J).second.end();
		for ( ; i != e; i++) {
			CSE_ALifeTask		*l_tpTask = m_tpALife->tpfGetTaskByID(*i);
			if (!l_tpTask->m_dwTryCount) {
				l_tBestTaskID = l_tpTask->m_tTaskID;
				break;
			}
			else
				if (l_tpTask->m_dwTryCount < l_dwMinTryCount) {
					l_dwMinTryCount = l_tpTask->m_dwTryCount;
					l_tBestTaskID = l_tpTask->m_tTaskID;
				}
		}

		if (l_tBestTaskID != _TASK_ID(-1)) {
			vfSetCurrentTask	(l_tBestTaskID);
			break;
		}
	}
}

void CAI_Stalker::vfSetCurrentTask(_TASK_ID &tTaskID)
{
	m_tTaskID				= m_tpALife->tpfGetTaskByID(tTaskID)->m_tTaskID;
}

bool CAI_Stalker::bfAssignDestinationNode()
{
//	if ((AI_NodeID == AI_Path.DestNode) && (getAI().m_tpaCrossTable[AI_NodeID].tGraphIndex == m_tNextGraphID)) {
	if (m_tpALife) {
		if (getAI().m_tpaCrossTable[AI_NodeID].tGraphIndex == m_tNextGraphID) {
			m_tGraphID = m_tNextGraphID;
			if (++m_dwCurGraphPathNode < m_tpGraphPath.size()) {
				m_tNextGraphID	= _GRAPH_ID(m_tpGraphPath[m_dwCurGraphPathNode]);
			}
			else
				return(false);
		}

		AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGraphID].tNodeID;
		return	(true);
	}
	else {
		if (AI_NodeID != getAI().m_tpaGraph[m_tNextGraphID].tNodeID) {
			AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGraphID].tNodeID;
			return(true);
		}
		_GRAPH_ID						tGraphID		= m_tNextGraphID;
		u16								wNeighbourCount = (u16)getAI().m_tpaGraph[tGraphID].tNeighbourCount;
		CSE_ALifeGraph::SGraphEdge		*tpaEdges		= (CSE_ALifeGraph::SGraphEdge *)((BYTE *)getAI().m_tpaGraph + getAI().m_tpaGraph[tGraphID].dwEdgeOffset);

		int								iPointCount		= (int)m_tpaTerrain.size();
		int								iBranches		= 0;
		for (int i=0; i<wNeighbourCount; i++)
			for (int j=0; j<iPointCount; j++)
				if (getAI().bfCheckMask(m_tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes) && (tpaEdges[i].dwVertexNumber != m_tGraphID))
					iBranches++;
		if (!iBranches) {
			for (int i=0; i<wNeighbourCount; i++) {
				for (int j=0; j<iPointCount; j++)
					if (getAI().bfCheckMask(m_tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes)) {
						m_tGraphID		= m_tNextGraphID;
						m_tNextGraphID	= (_GRAPH_ID)tpaEdges[i].dwVertexNumber;
						AI_Path.DestNode= getAI().m_tpaGraph[m_tNextGraphID].tNodeID;
						return(true);
					}
			}
		}
		else {
			int							iChosenBranch = ::Random.randI(0,iBranches);
			iBranches					= 0;
			for (int i=0; i<wNeighbourCount; i++) {
				for (int j=0; j<iPointCount; j++)
					if (getAI().bfCheckMask(m_tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes) && (tpaEdges[i].dwVertexNumber != m_tGraphID)) {
						if (iBranches == iChosenBranch) {
							m_tGraphID	= m_tNextGraphID;
							m_tNextGraphID	= (_GRAPH_ID)tpaEdges[i].dwVertexNumber;
							AI_Path.DestNode= getAI().m_tpaGraph[m_tNextGraphID].tNodeID;
							return(true);
						}
						iBranches++;
					}
			}
		}
	}
	return(true);
}
