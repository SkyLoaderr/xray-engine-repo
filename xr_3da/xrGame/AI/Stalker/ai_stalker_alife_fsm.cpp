////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_alife_fsm.cpp
//	Created 	: 15.09.2003
//  Modified 	: 15.09.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker ALife FSM
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"

void CAI_Stalker::vfContinueWithALifeGoals(IBaseAI_NodeEvaluator *tpNodeEvaluator)
{
	if (m_bStateChanged && !m_bPlayHumming && m_tpCurrentSound) {
		m_tpCurrentSound->stop();
		m_tpCurrentSound = 0;
	}

	// going via graph nodes
	//WRITE_TO_LOG			("Accomplishing task");
	m_bPlayHumming = true;

	if (m_bStateChanged || AI_Path.Nodes.empty() || (AI_Path.Nodes[AI_Path.Nodes.size() - 1] != AI_Path.DestNode)) {
		m_tActionState = !::Random.randI(1) ? eActionStateWatch : eActionStateDontWatch;
		m_dwActionStartTime = Level().timeServer() + ::Random.randI(30000,50000);
	}

	if (!bfAssignDestinationNode()) {
		switch (m_tTaskState) {
			case eTaskStateGoingToSearchItem : {
				m_tTaskState = eTaskStateAccomplishTask;
				break;
											   }
			case eTaskStateGoingToCustomer : {
				m_tTaskState = eTaskStateTradeWithCustomer;
				break;
											 }
			default : NODEFAULT;
		}
		return;
	}

	if (getAI().m_tpaCrossTable[getAI().m_tpaGraph[m_tNextGraphID].tNodeID].tGraphIndex != m_tNextGraphID) {
		string4096 S;
		sprintf(S,"Graph doesn't correspond to the cross table (graph vertex %d != Cross[Graph[%d].Node(%d)].Vertex(%d)",m_tNextGraphID,m_tNextGraphID,getAI().m_tpaGraph[m_tNextGraphID].tNodeID,getAI().m_tpaCrossTable[getAI().m_tpaGraph[m_tNextGraphID].tNodeID].tGraphIndex);
		R_ASSERT2(false,S);
	}
	
	if (!AI_Path.DestNode) {
		Msg("! Invalid graph point node (graph index %d)",m_tNextGraphID);
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

void CAI_Stalker::vfSearchObject()
{
	if (m_bStateChanged && !m_bPlayHumming && m_tpCurrentSound) {
		m_tpCurrentSound->stop();
		m_tpCurrentSound = 0;
	}

	m_bPlayHumming = false;

	if (AI_NodeID == AI_Path.DestNode) {
		if (!::Random.randI(10)) {
			vfFinishTask();
			return;
		}
		Fvector	l_tDirection;
		l_tDirection.sub(getAI().m_tpaGraph[m_tGraphID].tLocalPoint,Position());
		vfNormalizeSafe(l_tDirection);
		
		float y,p;
		l_tDirection.getHP(y,p);
		l_tDirection.setHP(y + ::Random.randF(-PI_DIV_2,PI_DIV_2),p);
		getAI().vfFindGraphPointNodeInDirection(AI_NodeID,Position(),l_tDirection,AI_Path.DestNode,m_tGraphID);
		if (AI_NodeID == AI_Path.DestNode) {
			if ((getAI().m_tpaGraph[m_tGraphID].tNodeID != AI_NodeID) && !::Random.randI(10))
				AI_Path.DestNode = getAI().m_tpaGraph[m_tGraphID].tNodeID;
			else
				return;
		}
	}

	vfSetParameters	(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeNormal,eLookTypeSearch);
}