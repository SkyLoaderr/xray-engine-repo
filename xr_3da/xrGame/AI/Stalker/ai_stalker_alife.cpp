////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_alife.cpp
//	Created 	: 25.07.2003
//  Modified 	: 25.07.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker ALife online behaviour
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"

void CAI_Stalker::vfChooseNextGraphPoint()
{
	_GRAPH_ID						tGraphID		= m_tNextGP;
	u16								wNeighbourCount = (u16)getAI().m_tpaGraph[tGraphID].tNeighbourCount;
	CSE_ALifeGraph::SGraphEdge			*tpaEdges		= (CSE_ALifeGraph::SGraphEdge *)((BYTE *)getAI().m_tpaGraph + getAI().m_tpaGraph[tGraphID].dwEdgeOffset);

	int								iPointCount		= (int)m_tpaTerrain.size();
	int								iBranches		= 0;
	for (int i=0; i<wNeighbourCount; i++)
		for (int j=0; j<iPointCount; j++)
			if (getAI().bfCheckMask(m_tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes) && (tpaEdges[i].dwVertexNumber != m_tCurGP))
				iBranches++;
	if (!iBranches) {
		for (int i=0; i<wNeighbourCount; i++) {
			for (int j=0; j<iPointCount; j++)
				if (getAI().bfCheckMask(m_tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes)) {
					m_tCurGP		= m_tNextGP;
					m_tNextGP		= (_GRAPH_ID)tpaEdges[i].dwVertexNumber;
					m_dwTimeToChange= Level().timeServer() + ::Random.randI(m_tpaTerrain[j].dwMinTime,m_tpaTerrain[j].dwMaxTime);
					return;
				}
		}
	}
	else {
		int							iChosenBranch = ::Random.randI(0,iBranches);
		iBranches					= 0;
		for (int i=0; i<wNeighbourCount; i++) {
			for (int j=0; j<iPointCount; j++)
				if (getAI().bfCheckMask(m_tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes) && (tpaEdges[i].dwVertexNumber != m_tCurGP)) {
					if (iBranches == iChosenBranch) {
						m_tCurGP	= m_tNextGP;
						m_tNextGP	= (_GRAPH_ID)tpaEdges[i].dwVertexNumber;
						m_dwTimeToChange	= Level().timeServer() + ::Random.randI(m_tpaTerrain[j].dwMinTime,m_tpaTerrain[j].dwMaxTime);
						return;
					}
					iBranches++;
				}
		}
	}
}