////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_misc.cpp
//	Created 	: 14.01.2003
//  Modified 	: 14.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation miscellanious functions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"

void CAI_ALife::vfAssignGraphPosition(CALifeMonsterAbstract	*tpALifeMonsterAbstract)
{
	tpALifeMonsterAbstract->m_tNextGraphID		= tpALifeMonsterAbstract->m_tPrevGraphID = tpALifeMonsterAbstract->m_tGraphID;
	tpALifeMonsterAbstract->m_fDistanceToPoint	= tpALifeMonsterAbstract->m_fDistance;
	_GRAPH_ID									tGraphID		= tpALifeMonsterAbstract->m_tNextGraphID;
	u16											wNeighbourCount = (u16)getAI().m_tpaGraph[tGraphID].tNeighbourCount;
	CALifeGraph::SGraphEdge						*tpaEdges		= (CALifeGraph::SGraphEdge *)((BYTE *)getAI().m_tpaGraph + getAI().m_tpaGraph[tGraphID].dwEdgeOffset);
	for (int i=0; i<wNeighbourCount; i++)
		if (tpaEdges[i].fPathDistance > tpALifeMonsterAbstract->m_fDistance) {
			tpALifeMonsterAbstract->m_fDistanceFromPoint	= tpaEdges[i].fPathDistance - tpALifeMonsterAbstract->m_fDistance;
			break;
		}
}

void CAI_ALife::vfChooseNextRoutePoint(CALifeMonsterAbstract	*tpALifeMonsterAbstract)
{
	if (tpALifeMonsterAbstract->m_tNextGraphID != tpALifeMonsterAbstract->m_tGraphID) {
		_TIME_ID tCurTime = tfGetGameTime();
		tpALifeMonsterAbstract->m_fDistanceFromPoint += float(tCurTime - tpALifeMonsterAbstract->m_tTimeID)/1000.f * tpALifeMonsterAbstract->m_fCurSpeed;
		if (tpALifeMonsterAbstract->m_fDistanceToPoint - tpALifeMonsterAbstract->m_fDistanceFromPoint < EPS_L) {
			tpALifeMonsterAbstract->m_fDistanceToPoint	= tpALifeMonsterAbstract->m_fDistanceFromPoint	= 0.0f;
			tpALifeMonsterAbstract->m_tPrevGraphID		= tpALifeMonsterAbstract->m_tGraphID;
			vfChangeObjectGraphPoint(tpALifeMonsterAbstract,tpALifeMonsterAbstract->m_tGraphID,tpALifeMonsterAbstract->m_tNextGraphID);
			CALifeAbstractGroup							*tpALifeAbstractGroup = dynamic_cast<CALifeAbstractGroup*>(tpALifeMonsterAbstract);
			if (tpALifeAbstractGroup)
				tpALifeAbstractGroup->m_bCreateSpawnPositions = true;
		}
	}
	if (tpALifeMonsterAbstract->m_tNextGraphID == tpALifeMonsterAbstract->m_tGraphID) {
		_GRAPH_ID			tGraphID		= tpALifeMonsterAbstract->m_tNextGraphID;
		u16					wNeighbourCount = (u16)getAI().m_tpaGraph[tGraphID].tNeighbourCount;
		CALifeGraph::SGraphEdge			*tpaEdges		= (CALifeGraph::SGraphEdge *)((BYTE *)getAI().m_tpaGraph + getAI().m_tpaGraph[tGraphID].dwEdgeOffset);
		
		TERRAIN_VECTOR		&tpaTerrain		= tpALifeMonsterAbstract->m_tpaTerrain;
		int					iPointCount		= (int)tpaTerrain.size();
		int					iBranches		= 0;
		for (int i=0; i<wNeighbourCount; i++)
			if (tpaEdges[i].dwVertexNumber != tpALifeMonsterAbstract->m_tPrevGraphID)
				for (int j=0; j<iPointCount; j++)
					if (getAI().bfCheckMask(tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes))
						iBranches++;
		bool				bOk = false;
		if (!iBranches) {
			for (int i=0; i<wNeighbourCount; i++) {
				for (int j=0; j<iPointCount; j++)
					if (getAI().bfCheckMask(tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes)) {
						tpALifeMonsterAbstract->m_tNextGraphID	= (_GRAPH_ID)tpaEdges[i].dwVertexNumber;
						tpALifeMonsterAbstract->m_fDistanceToPoint = tpaEdges[i].fPathDistance;
						//m_dwTimeToChange	= Level().timeServer() + ::Random.randI(tpaTerrain[j].dwMinTime,tpaTerrain[j].dwMaxTime);
						bOk = true;
						break;
					}
				if (bOk)
					break;
			}
		}
		else {
			int iChosenBranch = ::Random.randI(0,iBranches);
			iBranches = 0;
			for (int i=0; i<wNeighbourCount; i++)
				if (tpaEdges[i].dwVertexNumber != tpALifeMonsterAbstract->m_tPrevGraphID) {
					for (int j=0; j<iPointCount; j++)
						if (getAI().bfCheckMask(tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes) && (tpaEdges[i].dwVertexNumber != tpALifeMonsterAbstract->m_tPrevGraphID)) {
							if (iBranches == iChosenBranch) {
								tpALifeMonsterAbstract->m_tNextGraphID	= (_GRAPH_ID)tpaEdges[i].dwVertexNumber;
								tpALifeMonsterAbstract->m_fDistanceToPoint = tpaEdges[i].fPathDistance;
								//m_dwTimeToChange	= Level().timeServer() + ::Random.randI(tpaTerrain[j].dwMinTime,tpaTerrain[j].dwMaxTime);
								bOk = true;
								break;
							}
							iBranches++;
						}
					if (bOk)
						break;
				}
		}
		if (!bOk) {
			tpALifeMonsterAbstract->m_fCurSpeed			= 0.0f;
			tpALifeMonsterAbstract->m_fDistanceToPoint	= 0.0f;
		}
		else {
			tpALifeMonsterAbstract->m_fCurSpeed			= tpALifeMonsterAbstract->m_fGoingSpeed;
		}
	}
}

void CAI_ALife::vfCheckForTheBattle(CALifeMonsterAbstract *tpALifeMonsterAbstract)
{
}

void CAI_ALife::vfCheckForDeletedEvents(CALifeHumanAbstract	*tpALifeHuman)
{
	PERSONAL_EVENT_P_IT I = remove_if(tpALifeHuman->m_tpEvents.begin(),tpALifeHuman->m_tpEvents.end(),CRemovePersonalEventPredicate(m_tEventRegistry));
	tpALifeHuman->m_tpEvents.erase(I,tpALifeHuman->m_tpEvents.end());
}

bool CAI_ALife::bfCheckForItems(xrSE_Human	*tpALifeHumanAbstract)
{
	xrSE_Human *tpALifeHuman = dynamic_cast<xrSE_Human *>(tpALifeHumanAbstract);
	if (tpALifeHuman)
		return(bfProcessItems(*tpALifeHuman,tpALifeHuman->m_tGraphID,tpALifeHuman->m_fMaxItemMass,1.f));//tpALifeHuman->m_tTaskState == eTaskStateSearching ? .25f : .05f));
	else {
//		CALifeHumanGroup *tpALifeHumanGroup = dynamic_cast<CALifeHumanGroup *>(tpALifeHumanAbstract);
//		VERIFY(tpALifeHumanGroup);
//		HUMAN_PARAMS_P_IT	I = tpALifeHumanGroup->m_tpMembers.begin();
//		HUMAN_PARAMS_P_IT	E = tpALifeHumanGroup->m_tpMembers.end();
//		bool bOk = false;
//		for ( ; I != E; I++)
//			bOk = bOk || bfProcessItems(*(*I),tpALifeHumanGroup->m_tGraphID,tpALifeHumanGroup->m_fMaxItemMass,tpALifeHumanGroup->m_tTaskState == eTaskStateSearching ? .75f : .1f);
//		return(bOk);
		return(false);
	}
}

bool CAI_ALife::bfProcessItems(xrServerEntity &tServerEntity, _GRAPH_ID tGraphID, float fMaxItemMass, float fProbability)
{
	CALifeTraderParams *tpALifeTraderParams = dynamic_cast<CALifeTraderParams*>(&tServerEntity);
	VERIFY(tpALifeTraderParams);
	DYNAMIC_OBJECT_P_IT		I = m_tpGraphObjects[tGraphID].tpObjects.begin();
	DYNAMIC_OBJECT_P_IT		E = m_tpGraphObjects[tGraphID].tpObjects.end();
	for ( ; I != E; I++) {
		OBJECT_PAIR_IT	i = m_tObjectRegistry.find((*I)->m_tObjectID);
		VERIFY(i != m_tObjectRegistry.end());
		CALifeDynamicObject *tpALifeDynamicObject = (*i).second;
		VERIFY(tpALifeDynamicObject);
		CALifeItem *tpALifeItem = dynamic_cast<CALifeItem *>(tpALifeDynamicObject);
		if (tpALifeItem) {
			// adding _new item to the item list
			if (tpALifeTraderParams->m_fCumulativeItemMass + tpALifeItem->m_fMass < fMaxItemMass) {
				if (randF(1.0f) < fProbability) {
					vfAttachItem(tServerEntity,tpALifeItem,tGraphID);
					return(true);
				}
			}
			else {
				sort(tServerEntity.children.begin(),tServerEntity.children.end(),CSortItemPredicate(m_tObjectRegistry));
				float		fItemMass = tpALifeTraderParams->m_fCumulativeItemMass;
				u32			dwCount = tServerEntity.children.size();
				int			i;
				for ( i=(int)dwCount - 1; i>=0; i--) {
					OBJECT_PAIR_IT II = m_tObjectRegistry.find(tServerEntity.children[i]);
					VERIFY(II != m_tObjectRegistry.end());
					CALifeItem *tpALifeItemIn = dynamic_cast<CALifeItem *>((*II).second);
					VERIFY(tpALifeItemIn);
					tpALifeTraderParams->m_fCumulativeItemMass -= tpALifeItemIn->m_fMass;
					if (float(tpALifeItemIn->m_dwCost)/tpALifeItemIn->m_fMass >= float(tpALifeItemIn->m_dwCost)/tpALifeItem->m_fMass)
						break;
					if (tpALifeTraderParams->m_fCumulativeItemMass + tpALifeItem->m_fMass < fMaxItemMass)
						break;
				}
				if (tpALifeTraderParams->m_fCumulativeItemMass + tpALifeItem->m_fMass < fMaxItemMass) {
					for (int j=i + 1 ; j < (int)dwCount; j++)
						vfDetachItem(tServerEntity,tpALifeItem,tGraphID);
					vfAttachItem(tServerEntity,tpALifeItem,tGraphID);
					return(true);
				}
				else
					tpALifeTraderParams->m_fCumulativeItemMass	= fItemMass;
			}
		}
	}
	return(false);
}

void CAI_ALife::vfCommunicateWithTrader(CALifeHuman *tpALifeHuman, CALifeTrader *tpTrader)
{
//	// update items
//	TASK_PAIR_IT T = m_tTaskRegistry.find(tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tTaskID);
//	if (T != m_tTaskRegistry.end()) {
//		OBJECT_IT	I;
//		if (bfCheckIfTaskCompleted(tpALifeHuman, I)) {
//			CALifeItem *tpALifeItem = dynamic_cast<CALifeItem *>(m_tObjectRegistry[*I]);
//			if (tpTrader->m_dwMoney >= tpALifeItem->m_dwCost) {
//				tpALifeHuman->m_tpaVertices.clear();
//				tpTrader->m_tpItemIDs.push_back(*I);
//				tpALifeHuman->m_tpItemIDs.erase(I);
//				tpTrader->m_fCumulativeItemMass += tpALifeItem->m_fMass;
//				tpALifeHuman->m_fCumulativeItemMass -= tpALifeItem->m_fMass;
//				tpTrader->m_dwMoney -= tpALifeItem->m_dwCost;
//				tpALifeHuman->m_dwMoney += tpALifeItem->m_dwCost;
//				m_tTaskRegistry.erase(T);
//				tpTrader->m_tpTaskIDs.erase(lower_bound(tpTrader->m_tpTaskIDs.begin(),tpTrader->m_tpTaskIDs.end(),(*T).first));
//				tpALifeHuman->m_tpTaskIDs.erase(lower_bound(tpALifeHuman->m_tpTaskIDs.begin(),tpALifeHuman->m_tpTaskIDs.end(),(*T).first));
//			}
//		}
//	}
//	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//	// TEMPORARY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//	if (!tpTrader->m_tpTaskIDs.size()) {
//		vfCreateNewDynamicObject	(m_tpSpawnPoints.begin() + ::Random.randI(m_tpSpawnPoints.size() - 2),true);
//		vfCreateNewTask				(tpTrader);
//	}
//	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//	// END OF TEMPORARY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
//	// update events
//	
//	// update tasks
//	m_tpBufferTaskIDs.resize(tpALifeHuman->m_tpTaskIDs.size() + tpTrader->m_tpTaskIDs.size());
//	set_union(tpALifeHuman->m_tpTaskIDs.begin(),tpALifeHuman->m_tpTaskIDs.end(),tpTrader->m_tpTaskIDs.begin(),tpTrader->m_tpTaskIDs.end(),m_tpBufferTaskIDs.begin());
//	tpALifeHuman->m_tpTaskIDs = m_tpBufferTaskIDs;
}