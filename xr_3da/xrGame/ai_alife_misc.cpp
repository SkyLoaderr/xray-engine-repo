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
			vfChangeObjectGraphPoint(tpALifeMonsterAbstract->m_tObjectID,tpALifeMonsterAbstract->m_tGraphID,tpALifeMonsterAbstract->m_tNextGraphID);
			tpALifeMonsterAbstract->m_fDistanceToPoint	= tpALifeMonsterAbstract->m_fDistanceFromPoint	= 0.0f;
			tpALifeMonsterAbstract->m_tPrevGraphID		= tpALifeMonsterAbstract->m_tGraphID;
			tpALifeMonsterAbstract->m_tGraphID			= tpALifeMonsterAbstract->m_tNextGraphID;
		}
	}
	if (tpALifeMonsterAbstract->m_tNextGraphID == tpALifeMonsterAbstract->m_tGraphID) {
		CALifeHumanAbstract *tpALifeHumanAbstract = dynamic_cast<CALifeHumanAbstract *>(tpALifeMonsterAbstract);
		if (tpALifeHumanAbstract) {
			if (tpALifeHumanAbstract->m_tpaVertices.size() > ++(tpALifeHumanAbstract->m_dwCurNode)) {
				tpALifeHumanAbstract->m_tNextGraphID		= _GRAPH_ID(tpALifeHumanAbstract->m_tpaVertices[tpALifeHumanAbstract->m_dwCurNode]);
				tpALifeHumanAbstract->m_fCurSpeed			= tpALifeMonsterAbstract->m_fMinSpeed;
			}
			else {
				tpALifeHumanAbstract->m_fCurSpeed			= 0.0f;
				tpALifeHumanAbstract->m_fDistanceToPoint	= 0.0f;
			}
		}
		else {
			_GRAPH_ID			tGraphID		= tpALifeMonsterAbstract->m_tGraphID;
			AI::SGraphVertex	*tpaGraph		= Level().AI.m_tpaGraph;
			u16					wNeighbourCount = (u16)tpaGraph[tGraphID].dwNeighbourCount;
			AI::SGraphEdge		*tpaEdges		= (AI::SGraphEdge *)((BYTE *)tpaGraph + tpaGraph[tGraphID].dwEdgeOffset);
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
			tpALifeMonsterAbstract->m_fDistanceFromPoint	= 0.0f;
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

void CAI_ALife::vfCheckForTheBattle(CALifeMonsterAbstract	*tpALifeMonsterAbstract)
{
}

void CAI_ALife::vfCheckForDeletedEvents(CALifeHumanAbstract	*tpALifeHuman)
{
	PERSONAL_EVENT_IT I = remove_if(tpALifeHuman->m_tpEvents.begin(),tpALifeHuman->m_tpEvents.end(),CRemovePersonalEventPredicate(m_tEventRegistry.m_tpMap));
	tpALifeHuman->m_tpEvents.erase(I,tpALifeHuman->m_tpEvents.end());
}

void CAI_ALife::vfCheckForItems(CALifeHumanAbstract	*tpALifeHumanAbstract)
{
	CALifeHuman *tpALifeHuman = dynamic_cast<CALifeHuman *>(tpALifeHumanAbstract);
	if (tpALifeHuman)
		vfProcessItems(tpALifeHuman->m_tHumanParams,tpALifeHuman->m_tGraphID,tpALifeHuman->m_fMaxItemMass);
	else {
		CALifeHumanGroup *tpALifeHumanGroup = dynamic_cast<CALifeHumanGroup *>(tpALifeHumanAbstract);
		VERIFY(tpALifeHumanGroup);
		HUMAN_PARAMS_IT	I = tpALifeHumanGroup->m_tpMembers.begin();
		HUMAN_PARAMS_IT	E = tpALifeHumanGroup->m_tpMembers.end();
		for ( ; I != E; I++)
			vfProcessItems(*I,tpALifeHumanGroup->m_tGraphID,tpALifeHumanGroup->m_fMaxItemMass);
	}
}

void CAI_ALife::vfProcessItems(CALifeHumanParams &tHumanParams, _GRAPH_ID tGraphID, float fMaxItemMass)
{
	OBJECT_IT	I = m_tpGraphObjects[tGraphID].tpObjectIDs.begin();
	OBJECT_IT	E  = m_tpGraphObjects[tGraphID].tpObjectIDs.end();
	for ( ; I != E; I++) {
		OBJECT_PAIR_IT	i = m_tObjectRegistry.m_tppMap.find(*I);
		VERIFY(i != m_tObjectRegistry.m_tppMap.end());
		CALifeDynamicObject *tpALifeDynamicObject = (*i).second;
		VERIFY(tpALifeDynamicObject);
		CALifeItem *tpALifeItem = dynamic_cast<CALifeItem *>(tpALifeDynamicObject);
		if (tpALifeItem) {
			// adding new item to the item list
			if (tHumanParams.m_fCumulativeItemMass + tpALifeItem->m_fMass < fMaxItemMass) {
				tHumanParams.m_tpItemIDs.push_back(*I);
				m_tpGraphObjects[tGraphID].tpObjectIDs.erase(I);
				tHumanParams.m_fCumulativeItemMass += tpALifeItem->m_fMass;
			}
			else {
				sort(tHumanParams.m_tpItemIDs.begin(),tHumanParams.m_tpItemIDs.end(),CSortItemPredicate(m_tObjectRegistry.m_tppMap));
				OBJECT_IT	I = tHumanParams.m_tpItemIDs.end();
				OBJECT_IT	S = tHumanParams.m_tpItemIDs.begin();
				float		fItemMass = tHumanParams.m_fCumulativeItemMass;
				for ( ; I != S; I--) {
					OBJECT_PAIR_IT II = m_tObjectRegistry.m_tppMap.find((*I));
					VERIFY(II != m_tObjectRegistry.m_tppMap.end());
					CALifeItem *tpALifeItemIn = dynamic_cast<CALifeItem *>((*II).second);
					VERIFY(tpALifeItemIn);
					tHumanParams.m_fCumulativeItemMass -= tpALifeItemIn->m_fMass;
					if (tpALifeItemIn->m_fPrice/tpALifeItemIn->m_fMass >= tpALifeItem->m_fPrice/tpALifeItem->m_fMass)
						break;
					if (tHumanParams.m_fCumulativeItemMass + tpALifeItem->m_fMass < fMaxItemMass)
						break;
				}
				if (tHumanParams.m_fCumulativeItemMass + tpALifeItem->m_fMass < fMaxItemMass) {
					tHumanParams.m_tpItemIDs.erase		(I,tHumanParams.m_tpItemIDs.end());
					tHumanParams.m_tpItemIDs.push_back	(tpALifeItem->m_tObjectID);
					tHumanParams.m_fCumulativeItemMass	+= tpALifeItem->m_fMass;
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