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
		_TIME_ID tCurTime = tfGetGameTime();
		tpALifeMonsterAbstract->m_fDistanceFromPoint += float(tCurTime - tpALifeMonsterAbstract->m_tTimeID)/1000.f * tpALifeMonsterAbstract->m_fCurSpeed;
		if (tpALifeMonsterAbstract->m_fDistanceToPoint - tpALifeMonsterAbstract->m_fDistanceFromPoint < EPS_L) {
			vfChangeObjectGraphPoint(tpALifeMonsterAbstract,tpALifeMonsterAbstract->m_tGraphID,tpALifeMonsterAbstract->m_tNextGraphID);
			CALifeHumanAbstract *tpALifeHumanAbstract = dynamic_cast<CALifeHumanAbstract *>(tpALifeMonsterAbstract);
			if (tpALifeHumanAbstract && (tpALifeHumanAbstract->m_tTaskState == eTaskStateSearching)) {
				GRAPH_IT	B = m_tpTerrain[tpALifeHumanAbstract->m_tpTasks[tpALifeHumanAbstract->m_dwCurTask]->m_tLocationID].begin();
				GRAPH_IT	E = m_tpTerrain[tpALifeHumanAbstract->m_tpTasks[tpALifeHumanAbstract->m_dwCurTask]->m_tLocationID].end();
				GRAPH_IT	I = B;
				for (; I != E; I++) {
					if (*I == tpALifeHumanAbstract->m_tGraphID)
						tpALifeHumanAbstract->m_baVisitedVertices[I - B] = true;
					else
						if (*I == tpALifeHumanAbstract->m_tNextGraphID)
							tpALifeHumanAbstract->m_baVisitedVertices[I - B] = true;
				}
			}
			tpALifeMonsterAbstract->m_fDistanceToPoint	= tpALifeMonsterAbstract->m_fDistanceFromPoint	= 0.0f;
			tpALifeMonsterAbstract->m_tPrevGraphID		= tpALifeMonsterAbstract->m_tGraphID;
			tpALifeMonsterAbstract->m_tGraphID			= tpALifeMonsterAbstract->m_tNextGraphID;
		}
	}
	if (tpALifeMonsterAbstract->m_tNextGraphID == tpALifeMonsterAbstract->m_tGraphID) {
		CALifeHuman *tpALifeHuman = dynamic_cast<CALifeHuman *>(tpALifeMonsterAbstract);
		if ((tpALifeHuman)
			&&
			((tpALifeHuman->m_tTaskState == eTaskStateSearching)
			 ||
			 (tpALifeHuman->m_tTaskState == eTaskStateGoing)
			 ||
			 (tpALifeHuman->m_tTaskState == eTaskStateGoToTrader))
			&& bfCheckForItems(tpALifeHuman) 
			&& tpALifeHuman->m_tpTaskIDs.size() && bfCheckIfTaskCompleted(tpALifeHuman)) {
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
				tpALifeHumanAbstract->m_fCurSpeed			= tpALifeHumanAbstract->m_fGoingSpeed;
				for (int i=0; i<(int)wNeighbourCount; i++)
					if (tpaEdges[i].dwVertexNumber == tpALifeHumanAbstract->m_tNextGraphID) {
                        tpALifeMonsterAbstract->m_fDistanceToPoint	= tpaEdges[i].fPathDistance;
						break;
					}
				if ((tpALifeHumanAbstract->m_tTaskState == eTaskStateSearching) || (tpALifeHumanAbstract->m_tTaskState == eTaskStateGoing)) {
					switch (tpALifeHumanAbstract->m_tpTasks[tpALifeHumanAbstract->m_dwCurTask]->m_tTaskType) {
						case eTaskTypeSearchForItemCG :
						case eTaskTypeSearchForItemOG : {
							if (tpALifeHumanAbstract->m_tNextGraphID == tpALifeHumanAbstract->m_tpTasks[tpALifeHumanAbstract->m_dwCurTask]->m_tGraphID) {
								tpALifeHumanAbstract->m_tTaskState = eTaskStateSearching;
								tpALifeHumanAbstract->m_fCurSpeed  = tpALifeHumanAbstract->m_fSearchSpeed;
							}
							else {
								tpALifeHumanAbstract->m_tTaskState = eTaskStateGoing;
								tpALifeHumanAbstract->m_fCurSpeed  = tpALifeHumanAbstract->m_fGoingSpeed;
							}
							break;
						}
						case eTaskTypeSearchForItemCL :
						case eTaskTypeSearchForItemOL : {
							VERIFY(m_tpTerrain[tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID].size());
							if (Level().AI.m_tpaGraph[tpALifeHumanAbstract->m_tNextGraphID].tVertexType == tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tLocationID) {
								tpALifeHumanAbstract->m_tTaskState = eTaskStateSearching;
								tpALifeHumanAbstract->m_fCurSpeed  = tpALifeHumanAbstract->m_fSearchSpeed;
							}
							else {
								tpALifeHumanAbstract->m_tTaskState = eTaskStateGoing;
								tpALifeHumanAbstract->m_fCurSpeed  = tpALifeHumanAbstract->m_fGoingSpeed;
							}
							break;
						}
						default : NODEFAULT;
					};
				}
			}
			else {
				tpALifeHumanAbstract->m_fCurSpeed			= 0.0f;
				tpALifeHumanAbstract->m_fDistanceToPoint	= 0.0f;
			}
		}
		else {
			CALifeCreatureSpawnPoint	*tpALifeCreatureSpawnPoint = dynamic_cast<CALifeCreatureSpawnPoint *>(m_tpSpawnPoints[tpALifeMonsterAbstract->m_tSpawnID]);
			VERIFY(tpALifeCreatureSpawnPoint);
			int					iPointCount		= (int)tpALifeCreatureSpawnPoint->m_tpRouteGraphPoints.size();
			GRAPH_VECTOR		&wpaVertexes	= tpALifeCreatureSpawnPoint->m_tpRouteGraphPoints;
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
				tpALifeMonsterAbstract->m_fCurSpeed			= tpALifeMonsterAbstract->m_fGoingSpeed;
			}
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

bool CAI_ALife::bfCheckForItems(CALifeHumanAbstract	*tpALifeHumanAbstract)
{
	CALifeHuman *tpALifeHuman = dynamic_cast<CALifeHuman *>(tpALifeHumanAbstract);
	if (tpALifeHuman)
		return(bfProcessItems(*tpALifeHuman,tpALifeHuman->m_tGraphID,tpALifeHuman->m_fMaxItemMass,tpALifeHuman->m_tTaskState == eTaskStateSearching ? .25f : .05f));
	else {
		CALifeHumanGroup *tpALifeHumanGroup = dynamic_cast<CALifeHumanGroup *>(tpALifeHumanAbstract);
		VERIFY(tpALifeHumanGroup);
		HUMAN_PARAMS_P_IT	I = tpALifeHumanGroup->m_tpMembers.begin();
		HUMAN_PARAMS_P_IT	E = tpALifeHumanGroup->m_tpMembers.end();
		bool bOk = false;
		for ( ; I != E; I++)
			bOk = bOk || bfProcessItems(*(*I),tpALifeHumanGroup->m_tGraphID,tpALifeHumanGroup->m_fMaxItemMass,tpALifeHumanGroup->m_tTaskState == eTaskStateSearching ? .75f : .1f);
		return(bOk);
	}
}

bool CAI_ALife::bfProcessItems(CALifeHumanParams &tHumanParams, _GRAPH_ID tGraphID, float fMaxItemMass, float fProbability)
{
	DYNAMIC_OBJECT_P_IT		I = m_tpGraphObjects[tGraphID].tpObjects.begin();
	DYNAMIC_OBJECT_P_IT		E  = m_tpGraphObjects[tGraphID].tpObjects.end();
	for ( ; I != E; I++) {
		OBJECT_PAIR_IT	i = m_tObjectRegistry.find((*I)->m_tObjectID);
		VERIFY(i != m_tObjectRegistry.end());
		CALifeDynamicObject *tpALifeDynamicObject = (*i).second;
		VERIFY(tpALifeDynamicObject);
		CALifeItem *tpALifeItem = dynamic_cast<CALifeItem *>(tpALifeDynamicObject);
		if (tpALifeItem) {
			// adding _new item to the item list
			if (tHumanParams.m_fCumulativeItemMass + tpALifeItem->m_fMass < fMaxItemMass) {
				if (randF(1.0f) < fProbability) {
					vfAttachItem(tHumanParams,tpALifeItem,tGraphID);
					return(true);
				}
			}
			else {
				sort(tHumanParams.m_tpItemIDs.begin(),tHumanParams.m_tpItemIDs.end(),CSortItemPredicate(m_tObjectRegistry));
				float		fItemMass = tHumanParams.m_fCumulativeItemMass;
				u32			dwCount = tHumanParams.m_tpItemIDs.size();
				int			i;
				for ( i=(int)dwCount - 1; i>=0; i--) {
					OBJECT_PAIR_IT II = m_tObjectRegistry.find(tHumanParams.m_tpItemIDs[i]);
					VERIFY(II != m_tObjectRegistry.end());
					CALifeItem *tpALifeItemIn = dynamic_cast<CALifeItem *>((*II).second);
					VERIFY(tpALifeItemIn);
					tHumanParams.m_fCumulativeItemMass -= tpALifeItemIn->m_fMass;
					if (float(tpALifeItemIn->m_dwCost)/tpALifeItemIn->m_fMass >= float(tpALifeItemIn->m_dwCost)/tpALifeItem->m_fMass)
						break;
					if (tHumanParams.m_fCumulativeItemMass + tpALifeItem->m_fMass < fMaxItemMass)
						break;
				}
				if (tHumanParams.m_fCumulativeItemMass + tpALifeItem->m_fMass < fMaxItemMass) {
					for (int j=i + 1 ; j < (int)dwCount; j++)
						vfDetachItem(tHumanParams,tpALifeItem,tGraphID);
					tHumanParams.m_tpItemIDs.erase		(tHumanParams.m_tpItemIDs.begin() + i,tHumanParams.m_tpItemIDs.end());
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
	return(false);
}

void CAI_ALife::vfCommunicateWithTrader(CALifeHuman *tpALifeHuman, CALifeTrader *tpTrader)
{
	// update items
	TASK_PAIR_IT T = m_tTaskRegistry.find(tpALifeHuman->m_tpTasks[tpALifeHuman->m_dwCurTask]->m_tTaskID);
	if (T != m_tTaskRegistry.end()) {
		OBJECT_IT	I;
		if (bfCheckIfTaskCompleted(tpALifeHuman, I)) {
			CALifeItem *tpALifeItem = dynamic_cast<CALifeItem *>(m_tObjectRegistry[*I]);
			if (tpTrader->m_dwMoney >= tpALifeItem->m_dwCost) {
				tpALifeHuman->m_tpaVertices.clear();
				tpTrader->m_tpItemIDs.push_back(*I);
				tpALifeHuman->m_tpItemIDs.erase(I);
				tpTrader->m_fCumulativeItemMass += tpALifeItem->m_fMass;
				tpALifeHuman->m_fCumulativeItemMass -= tpALifeItem->m_fMass;
				tpTrader->m_dwMoney -= tpALifeItem->m_dwCost;
				tpALifeHuman->m_dwMoney += tpALifeItem->m_dwCost;
				m_tTaskRegistry.erase(T);
				tpTrader->m_tpTaskIDs.erase(lower_bound(tpTrader->m_tpTaskIDs.begin(),tpTrader->m_tpTaskIDs.end(),(*T).first));
				tpALifeHuman->m_tpTaskIDs.erase(lower_bound(tpALifeHuman->m_tpTaskIDs.begin(),tpALifeHuman->m_tpTaskIDs.end(),(*T).first));
			}
		}
	}
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TEMPORARY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (!tpTrader->m_tpTaskIDs.size()) {
		vfCreateNewDynamicObject	(m_tpSpawnPoints.begin() + ::Random.randI(m_tpSpawnPoints.size() - 2),true);
		vfCreateNewTask				(tpTrader);
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