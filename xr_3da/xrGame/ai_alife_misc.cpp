////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_misc.cpp
//	Created 	: 14.01.2003
//  Modified 	: 14.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation miscellanious functions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "ai_space.h"
#include "ai_alife_predicates.h"

void CSE_ALifeSimulator::vfAssignGraphPosition(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract)
{
	tpALifeMonsterAbstract->m_tNextGraphID		= tpALifeMonsterAbstract->m_tPrevGraphID = tpALifeMonsterAbstract->m_tGraphID;
	tpALifeMonsterAbstract->m_fDistanceToPoint	= tpALifeMonsterAbstract->m_fDistance;
	_GRAPH_ID									tGraphID		= tpALifeMonsterAbstract->m_tNextGraphID;
	u16											wNeighbourCount = (u16)getAI().m_tpaGraph[tGraphID].tNeighbourCount;
	CSE_ALifeGraph::SGraphEdge						*tpaEdges		= (CSE_ALifeGraph::SGraphEdge *)((BYTE *)getAI().m_tpaGraph + getAI().m_tpaGraph[tGraphID].dwEdgeOffset);
	for (int i=0; i<wNeighbourCount; i++)
		if (tpaEdges[i].fPathDistance > tpALifeMonsterAbstract->m_fDistance) {
			tpALifeMonsterAbstract->m_fDistanceFromPoint	= tpaEdges[i].fPathDistance - tpALifeMonsterAbstract->m_fDistance;
			break;
		}
}

void CSE_ALifeSimulator::vfChooseNextRoutePoint(CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract)
{
	if (tpALifeMonsterAbstract->m_tNextGraphID != tpALifeMonsterAbstract->m_tGraphID) {
		_TIME_ID								tCurTime = tfGetGameTime();
		tpALifeMonsterAbstract->m_fDistanceFromPoint += float(tCurTime - tpALifeMonsterAbstract->m_tTimeID)/1000.f * tpALifeMonsterAbstract->m_fCurSpeed;
		if (tpALifeMonsterAbstract->m_fDistanceToPoint - tpALifeMonsterAbstract->m_fDistanceFromPoint < EPS_L) {
			tpALifeMonsterAbstract->m_fDistanceToPoint	= tpALifeMonsterAbstract->m_fDistanceFromPoint	= 0.0f;
			tpALifeMonsterAbstract->m_tPrevGraphID		= tpALifeMonsterAbstract->m_tGraphID;
			vfChangeObjectGraphPoint(tpALifeMonsterAbstract,tpALifeMonsterAbstract->m_tGraphID,tpALifeMonsterAbstract->m_tNextGraphID);
			CSE_ALifeAbstractGroup					*tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(tpALifeMonsterAbstract);
			if (tpALifeAbstractGroup)
				tpALifeAbstractGroup->m_bCreateSpawnPositions = true;
		}
	}
	if (tpALifeMonsterAbstract->m_tNextGraphID == tpALifeMonsterAbstract->m_tGraphID) {
		_GRAPH_ID			tGraphID		= tpALifeMonsterAbstract->m_tNextGraphID;
		u16					wNeighbourCount = (u16)getAI().m_tpaGraph[tGraphID].tNeighbourCount;
		CSE_ALifeGraph::SGraphEdge			*tpaEdges		= (CSE_ALifeGraph::SGraphEdge *)((BYTE *)getAI().m_tpaGraph + getAI().m_tpaGraph[tGraphID].dwEdgeOffset);
		
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
						//m_dwTimeToChange	= Level().timeServer() + randI(tpaTerrain[j].dwMinTime,tpaTerrain[j].dwMaxTime);
						bOk = true;
						break;
					}
				if (bOk)
					break;
			}
		}
		else {
			int iChosenBranch = randI(0,iBranches);
			iBranches = 0;
			for (int i=0; i<wNeighbourCount; i++)
				if (tpaEdges[i].dwVertexNumber != tpALifeMonsterAbstract->m_tPrevGraphID) {
					for (int j=0; j<iPointCount; j++)
						if (getAI().bfCheckMask(tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes) && (tpaEdges[i].dwVertexNumber != tpALifeMonsterAbstract->m_tPrevGraphID)) {
							if (iBranches == iChosenBranch) {
								tpALifeMonsterAbstract->m_tNextGraphID	= (_GRAPH_ID)tpaEdges[i].dwVertexNumber;
								tpALifeMonsterAbstract->m_fDistanceToPoint = tpaEdges[i].fPathDistance;
								//m_dwTimeToChange	= Level().timeServer() + randI(tpaTerrain[j].dwMinTime,tpaTerrain[j].dwMaxTime);
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

void CSE_ALifeSimulator::vfCheckForTheBattle(CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract)
{
}