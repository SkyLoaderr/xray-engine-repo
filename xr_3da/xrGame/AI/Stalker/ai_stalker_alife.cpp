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

void CAI_Stalker::vfAccomplishTask(IBaseAI_NodeEvaluator *tpNodeEvaluator)
{
	if (m_bStateChanged && !m_bPlayHumming && m_tpCurrentSound) {
		m_tpCurrentSound->stop();
		m_tpCurrentSound = 0;
	}

	// going via graph nodes
	//WRITE_TO_LOG			("Accomplishing task");
	m_bPlayHumming = true;
	
	if (getAI().m_tpaGraph[m_tNextGP].tNodeID == AI_NodeID)
		m_dwTimeToChange = 0;

	vfUpdateSearchPosition	();

	if (m_bStateChanged || AI_Path.Nodes.empty() || (AI_Path.Nodes[AI_Path.Nodes.size() - 1] != AI_Path.DestNode)) {
		m_tActionState = !::Random.randI(1) ? eActionStateWatch : eActionStateDontWatch;
		m_dwActionStartTime = Level().timeServer() + ::Random.randI(30000,50000);
	}

	AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
	if (getAI().m_tpaCrossTable[getAI().m_tpaGraph[m_tNextGP].tNodeID].tGraphIndex != m_tNextGP) {
		string4096 S;
		sprintf(S,"Graph doesn't correspond to the cross table (graph vertex %d != Cross[Graph[%d].Node(%d)].Vertex(%d)",m_tNextGP,m_tNextGP,getAI().m_tpaGraph[m_tNextGP].tNodeID,getAI().m_tpaCrossTable[getAI().m_tpaGraph[m_tNextGP].tNodeID].tGraphIndex);
		R_ASSERT2(false,S);
	}
	if (!AI_Path.DestNode) {
		Msg("! Invalid graph point node (graph index %d)",m_tNextGP);
		for (int i=0; i<(int)getAI().GraphHeader().dwVertexCount; i++)
			Msg("%3d : %6d",i,getAI().m_tpaGraph[i].tNodeID);
	}
	float					yaw,pitch;
	GetDirectionAngles		(yaw,pitch);
	yaw						= angle_normalize_signed(-yaw);
	if (!getAI().bfTooSmallAngle(r_torso_current.yaw,yaw,PI_DIV_6))
		vfSetParameters(tpNodeEvaluator,0,false,eWeaponStateIdle,!tpNodeEvaluator ? ePathTypeStraight : ePathTypeCriteria,eBodyStateStand,eMovementTypeWalk,eStateTypeNormal,eLookTypeDirection);
	else
		vfSetParameters(tpNodeEvaluator,0,false,eWeaponStateIdle,!tpNodeEvaluator ? ePathTypeStraight : ePathTypeCriteria,eBodyStateStand,eMovementTypeWalk,eStateTypeNormal,eLookTypeSearch);

	switch (m_tActionState) {
		case eActionStateWatch : {
			if (m_dwActionStartTime < Level().timeServer())
				m_tActionState = eActionStateDontWatch;
			break;
		}
		case eActionStateDontWatch : {
			if (!m_tpCurrentSound) {
				m_tpCurrentSound = &m_tpSoundHumming[::Random.randI((int)m_tpSoundHumming.size())];
				m_tpCurrentSound->play_at_pos(this,eye_matrix.c);
				m_tpCurrentSound->feedback->set_volume(1.f);
			}
			else
				if (m_tpCurrentSound->feedback)
					m_tpCurrentSound->feedback->set_position(eye_matrix.c);
				else {
					m_tpCurrentSound = 0;
					m_tActionState = eActionStateWatch;
					m_dwActionStartTime = Level().timeServer() + ::Random.randI(10000,20000);
				}
			break;
		}
	}
}

void CAI_Stalker::vfChooseTask()
{
}

void CAI_Stalker::vfHealthCare()
{
}

void CAI_Stalker::vfBuySupplies()
{
}

void CAI_Stalker::vfGoToCustomer()
{
}

void CAI_Stalker::vfBringToCustomer()
{
}

void CAI_Stalker::vfGoToSOS()
{
}

void CAI_Stalker::vfSendSOS()
{
}

void CAI_Stalker::vfSearchObject()
{
}
