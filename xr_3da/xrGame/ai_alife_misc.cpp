////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_misc.cpp
//	Created 	: 14.01.2003
//  Modified 	: 14.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation miscellanious functions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"

void CAI_ALife::vfChooseNextRoutePoint(CALifeMonsterAbstract	*tpALifeMonsterAbstract)
{
	if (tpALifeMonsterAbstract->m_tNextGraphID != tpALifeMonsterAbstract->m_tGraphID) {
		u32 dwCurTime = Level().timeServer();
		tpALifeMonsterAbstract->m_fDistanceFromPoint += float(dwCurTime - tpALifeMonsterAbstract->m_tTimeID)/1000.f * tpALifeMonsterAbstract->m_fCurSpeed;
		if (tpALifeMonsterAbstract->m_fDistanceToPoint - tpALifeMonsterAbstract->m_fDistanceFromPoint < EPS_L) {
			vfChangeObjectGraphPoint(tpALifeMonsterAbstract,tpALifeMonsterAbstract->m_tGraphID,tpALifeMonsterAbstract->m_tNextGraphID);
			tpALifeMonsterAbstract->m_fDistanceToPoint	= tpALifeMonsterAbstract->m_fDistanceFromPoint	= 0.0f;
			tpALifeMonsterAbstract->m_tPrevGraphID		= tpALifeMonsterAbstract->m_tGraphID;
			tpALifeMonsterAbstract->m_tGraphID			= tpALifeMonsterAbstract->m_tNextGraphID;
		}
	}
	if (tpALifeMonsterAbstract->m_tNextGraphID == tpALifeMonsterAbstract->m_tGraphID) {
		CALifeHuman *tpALifeHuman = dynamic_cast<CALifeHuman *>(tpALifeMonsterAbstract);
		if (tpALifeHuman && bfCheckForItems(tpALifeHuman) && (tpALifeHuman->m_tTaskState == eTaskStateGoing) && bfCheckIfTaskCompleted(tpALifeHuman)) {
			tpALifeHuman->m_tpaVertices.clear();
			tpALifeHuman->m_tNextGraphID = tpALifeHuman->m_tGraphID;
			return;
		}
		_GRAPH_ID			tGraphID		= tpALifeMonsterAbstract->m_tGraphID;
		AI::SGraphVertex	*tpaGraph		= Level().AI.m_tpaGraph;
		u16					wNeighbourCount = (u16)tpaGraph[tGraphID].dwNeighbourCount;
		AI::SGraphEdge		*tpaEdges		= (AI::SGraphEdge *)((BYTE *)tpaGraph + tpaGraph[tGraphID].dwEdgeOffset);
		tpALifeMonsterAbstract->m_fDistanceFromPoint	= 0.0f;
		CALifeHumanAbstract *tpALifeHumanAbstract = dynamic_cast<CALifeHumanAbstract *>(tpALifeMonsterAbstract);
		if (tpALifeHumanAbstract) {
			if (tpALifeHumanAbstract->m_tpaVertices.size() > ++(tpALifeHumanAbstract->m_dwCurNode)) {
				tpALifeHumanAbstract->m_tNextGraphID		= _GRAPH_ID(tpALifeHumanAbstract->m_tpaVertices[tpALifeHumanAbstract->m_dwCurNode]);
				tpALifeHumanAbstract->m_fCurSpeed			= tpALifeHumanAbstract->m_fMinSpeed;
				for (int i=0; i<(int)wNeighbourCount; i++)
					if (tpaEdges[i].dwVertexNumber == tpALifeHumanAbstract->m_tNextGraphID) {
                        tpALifeMonsterAbstract->m_fDistanceToPoint	= tpaEdges[i].fPathDistance;
						break;
					}
			}
			else {
				tpALifeHumanAbstract->m_fCurSpeed			= 0.0f;
				tpALifeHumanAbstract->m_fDistanceToPoint	= 0.0f;
			}
		}
		else {
			int					iPointCount		= (int)m_tpSpawnPoints[tpALifeMonsterAbstract->m_tSpawnID].ucRoutePointCount;
			GRAPH_VECTOR		&wpaVertexes	= m_tpSpawnPoints[tpALifeMonsterAbstract->m_tSpawnID].tpRouteGraphPoints;
			int					iBranches		= 0;
			bool bOk = false;
			for (int i=0; i<wNeighbourCount; i++)
				for (int j=0; j<iPointCount; j++)
					if ((tpaEdges[i].dwVertexNumber == wpaVertexes[j]) && (wpaVertexes[j] != tpALifeMonsterAbstract->m_tPrevGraphID))
						iBranches++;
			if (!iBranches) {
				for (int i=0; i<wNeighbourCount; i++) {
					for (int j=0; j<iPointCount; j++)
						if (tpaEdges[i].dwVertexNumber == wpaVertexes[j]) {
							tpALifeMonsterAbstract->m_tNextGraphID = wpaVertexes[j];
							tpALifeMonsterAbstract->m_fDistanceToPoint = tpaEdges[i].fPathDistance;
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
				for (int i=0; i<wNeighbourCount; i++) {
					for (int j=0; j<iPointCount; j++)
						if ((tpaEdges[i].dwVertexNumber == wpaVertexes[j]) && (wpaVertexes[j] != tpALifeMonsterAbstract->m_tPrevGraphID)) {
							if (iBranches == iChosenBranch) {
								tpALifeMonsterAbstract->m_tNextGraphID = wpaVertexes[j];
								tpALifeMonsterAbstract->m_fDistanceToPoint = tpaEdges[i].fPathDistance;
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
				tpALifeMonsterAbstract->m_fCurSpeed			= tpALifeMonsterAbstract->m_fMinSpeed;
			}
		}
	}
}

void CAI_ALife::vfCheckForTheBattle(CALifeMonsterAbstract *tpALifeMonsterAbstract)
{
}

void CAI_ALife::vfCheckForDeletedEvents(CALifeHumanAbstract	*tpALifeHuman)
{
	PERSONAL_EVENT_IT I = remove_if(tpALifeHuman->m_tpEvents.begin(),tpALifeHuman->m_tpEvents.end(),CRemovePersonalEventPredicate(m_tEventRegistry.m_tpMap));
	tpALifeHuman->m_tpEvents.erase(I,tpALifeHuman->m_tpEvents.end());
}

bool CAI_ALife::bfCheckForItems(CALifeHumanAbstract	*tpALifeHumanAbstract)
{
	CALifeHuman *tpALifeHuman = dynamic_cast<CALifeHuman *>(tpALifeHumanAbstract);
	if (tpALifeHuman)
		return(bfProcessItems(tpALifeHuman->m_tHumanParams,tpALifeHuman->m_tGraphID,tpALifeHuman->m_fMaxItemMass));
	else {
		CALifeHumanGroup *tpALifeHumanGroup = dynamic_cast<CALifeHumanGroup *>(tpALifeHumanAbstract);
		VERIFY(tpALifeHumanGroup);
		HUMAN_PARAMS_IT	I = tpALifeHumanGroup->m_tpMembers.begin();
		HUMAN_PARAMS_IT	E = tpALifeHumanGroup->m_tpMembers.end();
		bool bOk = false;
		for ( ; I != E; I++)
			bOk = bOk || bfProcessItems(*I,tpALifeHumanGroup->m_tGraphID,tpALifeHumanGroup->m_fMaxItemMass);
		return(bOk);
	}
}

void CAI_ALife::vfAttachItem(CALifeHumanParams &tHumanParams, CALifeItem *tpALifeItem, _GRAPH_ID tGraphID)
{
	tHumanParams.m_tpItemIDs.push_back(tpALifeItem->m_tObjectID);
	tpALifeItem->m_bAttached = true;
	DYNAMIC_OBJECT_P_IT		I = m_tpGraphObjects[tGraphID].tpObjects.begin();
	DYNAMIC_OBJECT_P_IT		E = m_tpGraphObjects[tGraphID].tpObjects.end();
	for ( ; I != E; I++)
		if (*I == tpALifeItem) {
			m_tpGraphObjects[tGraphID].tpObjects.erase(I);
			break;
		}
	tHumanParams.m_fCumulativeItemMass += tpALifeItem->m_fMass;
}

void CAI_ALife::vfDetachItem(CALifeHumanParams &tHumanParams, CALifeItem *tpALifeItem, _GRAPH_ID tGraphID)
{
	tpALifeItem->m_bAttached = true;
	m_tpGraphObjects[tGraphID].tpObjects.push_back(tpALifeItem);
	tHumanParams.m_fCumulativeItemMass -= tpALifeItem->m_fMass;
}

bool CAI_ALife::bfProcessItems(CALifeHumanParams &tHumanParams, _GRAPH_ID tGraphID, float fMaxItemMass)
{
	DYNAMIC_OBJECT_P_IT		I = m_tpGraphObjects[tGraphID].tpObjects.begin();
	DYNAMIC_OBJECT_P_IT		E  = m_tpGraphObjects[tGraphID].tpObjects.end();
	for ( ; I != E; I++) {
		OBJECT_PAIR_IT	i = m_tObjectRegistry.m_tppMap.find((*I)->m_tObjectID);
		VERIFY(i != m_tObjectRegistry.m_tppMap.end());
		CALifeDynamicObject *tpALifeDynamicObject = (*i).second;
		VERIFY(tpALifeDynamicObject);
		CALifeItem *tpALifeItem = dynamic_cast<CALifeItem *>(tpALifeDynamicObject);
		if (tpALifeItem) {
			// adding new item to the item list
			if (tHumanParams.m_fCumulativeItemMass + tpALifeItem->m_fMass < fMaxItemMass) {
				vfAttachItem(tHumanParams,tpALifeItem,tGraphID);
				return(true);
			}
			else {
				sort(tHumanParams.m_tpItemIDs.begin(),tHumanParams.m_tpItemIDs.end(),CSortItemPredicate(m_tObjectRegistry.m_tppMap));
				OBJECT_IT	E = tHumanParams.m_tpItemIDs.end();
				OBJECT_IT	S = tHumanParams.m_tpItemIDs.begin();
				OBJECT_IT	I = E - 1;
				float		fItemMass = tHumanParams.m_fCumulativeItemMass;
#pragma todo("Reimplement with the reverse iterator because of the possible ERROR!")
				for ( ; I != S; I--) {
					OBJECT_PAIR_IT II = m_tObjectRegistry.m_tppMap.find((*I));
					VERIFY(II != m_tObjectRegistry.m_tppMap.end());
					CALifeItem *tpALifeItemIn = dynamic_cast<CALifeItem *>((*II).second);
					VERIFY(tpALifeItemIn);
					tHumanParams.m_fCumulativeItemMass -= tpALifeItemIn->m_fMass;
					if (float(tpALifeItemIn->m_dwCost)/tpALifeItemIn->m_fMass >= float(tpALifeItemIn->m_dwCost)/tpALifeItem->m_fMass)
						break;
					if (tHumanParams.m_fCumulativeItemMass + tpALifeItem->m_fMass < fMaxItemMass)
						break;
				}
				if (tHumanParams.m_fCumulativeItemMass + tpALifeItem->m_fMass < fMaxItemMass) {
					for ( ; I != E; I++)
						vfDetachItem(tHumanParams,tpALifeItem,tGraphID);
					tHumanParams.m_tpItemIDs.erase		(I,tHumanParams.m_tpItemIDs.end());
					tHumanParams.m_tpItemIDs.push_back	(tpALifeItem->m_tObjectID);
					tHumanParams.m_fCumulativeItemMass	+= tpALifeItem->m_fMass;
					vfAttachItem(tHumanParams,tpALifeItem,tGraphID);
					return(true);
				}
				else
					tHumanParams.m_fCumulativeItemMass	= fItemMass;
			}
		}
	}
}

CALifeHuman *CAI_ALife::tpfGetNearestSuitableTrader(CALifeHuman *tpALifeHuman)
{
	float			fBestDistance = MAX_NODE_ESTIMATION_COST;
	CALifeHuman *	tpBestTrader = 0;
	HUMAN_P_IT		I = m_tpTraders.begin();
	HUMAN_P_IT		E = m_tpTraders.end();
	Fvector			&tPoint = Level().AI.m_tpaGraph[tpALifeHuman->m_tGraphID].tPoint;
	for ( ; I != E; I++) {
		if ((*I)->m_tHumanParams.m_tRank != tpALifeHuman->m_tHumanParams.m_tRank)
			break;
		float fCurDistance = Level().AI.m_tpaGraph[(*I)->m_tGraphID].tPoint.distance_to(tPoint);
		if (fCurDistance < fBestDistance) {
			fBestDistance = fCurDistance;
			tpBestTrader = *I;
		}
	}
	return(tpBestTrader);
}

void CAI_ALife::vfCommunicateWithTrader(CALifeHuman *tpALifeHuman, CALifeHuman *tpTrader)
{
	// update items
	if (tpALifeHuman->m_tTaskState == eTaskStateReturningSuccess) {
		TASK_PAIR_IT T = m_tTaskRegistry.m_tpMap.find(tpALifeHuman->m_tCurTask.tTaskID);
		if (T != m_tTaskRegistry.m_tpMap.end()) {
			OBJECT_IT	I;
			if (bfCheckIfTaskCompleted(tpALifeHuman, I)) {
				CALifeItem *tpALifeItem = dynamic_cast<CALifeItem *>(m_tObjectRegistry.m_tppMap[*I]);
				if (tpTrader->m_tHumanParams.m_dwMoney >= tpALifeItem->m_dwCost) {
					tpALifeHuman->m_tpaVertices.clear();
					tpTrader->m_tHumanParams.m_tpItemIDs.push_back(*I);
					tpALifeHuman->m_tHumanParams.m_tpItemIDs.erase(I);
					tpTrader->m_tHumanParams.m_fCumulativeItemMass += tpALifeItem->m_fMass;
					tpALifeHuman->m_tHumanParams.m_fCumulativeItemMass -= tpALifeItem->m_fMass;
					tpTrader->m_tHumanParams.m_dwMoney -= tpALifeItem->m_dwCost;
					tpALifeHuman->m_tHumanParams.m_dwMoney += tpALifeItem->m_dwCost;
					m_tTaskRegistry.m_tpMap.erase(T);
					tpTrader->m_tpTaskIDs.erase(lower_bound(tpTrader->m_tpTaskIDs.begin(),tpTrader->m_tpTaskIDs.end(),(*T).first));
					tpALifeHuman->m_tpTaskIDs.erase(lower_bound(tpALifeHuman->m_tpTaskIDs.begin(),tpALifeHuman->m_tpTaskIDs.end(),(*T).first));
				}
			}
		}
	}
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TEMPORARY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (!tpTrader->m_tpTaskIDs.size()) {
		vfCreateNewDynamicObject	(m_tpSpawnPoints.begin() + 73+0*::Random.randI(m_tpSpawnPoints.size() - 2),true);
		vfCreateNewTask				(m_tpTraders[0]);
	}
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// END OF TEMPORARY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// update events
	
	// update tasks
	m_tpBufferTaskIDs.resize(tpALifeHuman->m_tpTaskIDs.size() + tpTrader->m_tpTaskIDs.size());
	set_union(tpALifeHuman->m_tpTaskIDs.begin(),tpALifeHuman->m_tpTaskIDs.end(),tpTrader->m_tpTaskIDs.begin(),tpTrader->m_tpTaskIDs.end(),m_tpBufferTaskIDs.begin());
	tpALifeHuman->m_tpTaskIDs = m_tpBufferTaskIDs;
	
}