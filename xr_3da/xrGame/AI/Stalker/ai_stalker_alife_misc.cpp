////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_alife_misc.cpp
//	Created 	: 15.09.2003
//  Modified 	: 15.09.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker ALife miscellanious functions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../../ai_alife.h"
#include "../../graph_engine.h"
#include "../../game_level_cross_table.h"
#include "../../game_graph.h"

bool CAI_Stalker::bfHealthIsGood			()
{
	return(true);
}

bool CAI_Stalker::bfItemCanTreat			(CInventoryItem * /**tpInventoryItem/**/)
{
	return(false);
}

void CAI_Stalker::vfUseItem					(CInventoryItem * /**tpInventoryItem/**/)
{
}

bool CAI_Stalker::bfCanTreat				()
{
//	xr_vector<u16>::iterator	I = children.begin();
//	xr_vector<u16>::iterator	E = children.end();
//	for ( ; I != E; ++I) {
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
	for ( ; I != E; ++I) {
		OBJECT_TASK_PAIR_IT		J = m_tpALife->m_tTaskCrossMap.find(*I);
		R_ASSERT2				(m_tpALife->m_tTaskCrossMap.end() != J,"Can't find a specified customer in the Task registry!\nPossibly, there is no traders at all or there is no anomalous zones.");

		u32						l_dwMinTryCount = u32(-1);
		_TASK_ID				l_tBestTaskID = _TASK_ID(-1);
		TASK_SET_IT				i = (*J).second.begin();
		TASK_SET_IT				e = (*J).second.end();
		for ( ; i != e; ++i) {
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

		if (_TASK_ID(-1) != l_tBestTaskID) {
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
//	if ((level_vertex_id() == m_level_dest_vertex_id) && (ai().cross_table().vertex(level_vertex_id()) == m_tNextGraphID)) {
	if (m_tpALife) {
		if (ai().cross_table().vertex(level_vertex_id()).game_vertex_id() == m_tNextGraphID) {
			m_tGraphID = m_tNextGraphID;
			if (++m_dwCurGraphPathNode < m_tpGraphPath.size()) {
				m_tNextGraphID	= _GRAPH_ID(m_tpGraphPath[m_dwCurGraphPathNode]);
			}
			else
				return(false);
		}

		set_level_dest_vertex(ai().game_graph().vertex(m_tNextGraphID)->level_vertex_id());
		return	(true);
	}
	else {
		if (ai().game_graph().vertex(m_tNextGraphID)->level_vertex_id() != level_vertex_id()) {
			set_level_dest_vertex(ai().game_graph().vertex(m_tNextGraphID)->level_vertex_id());
			return(true);
		}
		_GRAPH_ID						tGraphID		= m_tNextGraphID;
		CGameGraph::const_iterator		i,e;
		ai().game_graph().begin			(tGraphID,i,e);
		int								iPointCount		= (int)m_tpaTerrain.size();
		int								iBranches		= 0;
		for ( ; i != e; ++i)
			for (int j=0; j<iPointCount; ++j)
				if (ai().game_graph().mask(m_tpaTerrain[j].tMask,ai().game_graph().vertex((*i).vertex_id())->vertex_type()) && ((*i).vertex_id() != m_tGraphID))
					++iBranches;

		ai().game_graph().begin			(tGraphID,i,e);

		if (!iBranches) {
			for ( ; i != e; ++i) {
				for (int j=0; j<iPointCount; ++j)
					if (ai().game_graph().mask(m_tpaTerrain[j].tMask,ai().game_graph().vertex((*i).vertex_id())->vertex_type())) {
						m_tGraphID		= m_tNextGraphID;
						m_tNextGraphID	= (*i).vertex_id();
						set_level_dest_vertex(ai().game_graph().vertex(m_tNextGraphID)->level_vertex_id());
						return(true);
					}
			}
		}
		else {
			int							iChosenBranch = ::Random.randI(0,iBranches);
			iBranches					= 0;
			for ( ; i != e; ++i) {
				for (int j=0; j<iPointCount; ++j)
					if (ai().game_graph().mask(m_tpaTerrain[j].tMask,ai().game_graph().vertex((*i).vertex_id())->vertex_type()) && ((*i).vertex_id() != m_tGraphID)) {
						if (iBranches == iChosenBranch) {
							m_tGraphID	= m_tNextGraphID;
							m_tNextGraphID	= (*i).vertex_id();
							set_level_dest_vertex(ai().game_graph().vertex(m_tNextGraphID)->level_vertex_id());
							return(true);
						}
						++iBranches;
					}
			}
		}
	}
	return(true);
}
