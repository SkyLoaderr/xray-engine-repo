////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat_templates.cpp
//	Created 	: 23.07.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : Templates for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_rat.h"
#include "..\\ai_monsters_misc.h"
#include "..\\..\\ai_alife_graph.h"

void CAI_Rat::vfSaveEnemy()
{
	m_tSavedEnemy = m_Enemy.Enemy;
	m_tSavedEnemyPosition = m_Enemy.Enemy->Position();
	m_tpSavedEnemyNode = m_Enemy.Enemy->AI_Node;
	m_dwSavedEnemyNodeID = m_Enemy.Enemy->AI_NodeID;
}

void CAI_Rat::vfSetFire(bool bFire, CGroup &Group)
{
	if (bFire) {
		m_bFiring = true;
		q_action.setup(AI::AIC_Action::AttackBegin);
	}
	else {
		m_bFiring = false;
		q_action.setup(AI::AIC_Action::AttackEnd);
	}
}

void CAI_Rat::vfSetMovementType(char cBodyState, float fSpeed)
{
	StandUp();
	m_fSpeed = m_fCurSpeed = fSpeed;
}

void CAI_Rat::vfAdjustSpeed()
{
	Fvector tTemp1, tTemp2;
	tTemp1.sub(m_tGoalDir,Position());
	tTemp1.normalize_safe();
	tTemp2 = XFORM().k;
	tTemp2.normalize_safe();
	float fAngle = tTemp1.dotproduct(tTemp2);
	clamp(fAngle,0.f,.99999f);
	fAngle = acosf(fAngle);
	
//	float fMinASpeed = PI_MUL_2, fNullASpeed = PI_MUL_2, fMaxASpeed = .2f, fAttackASpeed = .15f;
	
	if (_abs(m_fSpeed - m_fMinSpeed) <= EPS_L)	{
		if (fAngle >= 3*PI_DIV_2) {
			m_fSpeed = 0;
			m_fASpeed = m_fNullASpeed;
			r_torso_target.yaw = fAngle;
		}
		else 
		{
			m_fSpeed = m_fMinSpeed;
			m_fASpeed = m_fMinASpeed;
		}
	}
	else
		if (_abs(m_fSpeed - m_fMaxSpeed) <= EPS_L)	{
			if (fAngle >= 3*PI_DIV_2) {
				m_fSpeed = 0;
				m_fASpeed = m_fNullASpeed;
				r_torso_target.yaw = fAngle;
			}
			else
				if (fAngle >= PI_DIV_2) {
					m_fSpeed = m_fMinSpeed;
					m_fASpeed = m_fMinASpeed;
				}
				else {
					m_fSpeed = m_fMaxSpeed;
					m_fASpeed = m_fMaxASpeed;
				}
		}
		else
			if (_abs(m_fSpeed - m_fAttackSpeed) <= EPS_L)	{
				if (fAngle >= 3*PI_DIV_2) {
					m_fSpeed = 0;
					m_fASpeed = m_fNullASpeed;
					r_torso_target.yaw = fAngle;
				}
				else
					if (fAngle >= PI_DIV_2) {
						m_fSpeed = m_fMinSpeed;
						m_fASpeed = m_fMinASpeed;
					}
					else
						if (fAngle >= PI_DIV_4) {
							m_fSpeed = m_fMaxSpeed;
							m_fASpeed = m_fMaxASpeed;
						}
						else {
							m_fSpeed = m_fAttackSpeed;
							m_fASpeed = m_fAttackASpeed;
						}
			}
			else {
				r_torso_target.yaw = fAngle;
				m_fSpeed = 0;
				m_fASpeed = m_fNullASpeed;
			}
	
	tTemp2 = XFORM().k;
	tTemp2.normalize_safe();
	
	tTemp1 = Position();
	tTemp1.mad(tTemp2,1*m_fSpeed*m_fTimeUpdateDelta);
	if (bfCheckIfOutsideAIMap(tTemp1)) {
		tTemp1 = Position();
		if (_abs(m_fSpeed - m_fAttackSpeed) < EPS_L) {
			tTemp1.mad(tTemp2,1*m_fMaxSpeed*m_fTimeUpdateDelta);
			if (bfCheckIfOutsideAIMap(tTemp1)) {
				m_fSpeed = m_fMinSpeed;
				m_fASpeed = m_fMinASpeed;
			}
			else {
				m_fSpeed = m_fMaxSpeed;
				m_fASpeed = m_fMaxASpeed;
			}
		}
		else 
		{
			m_fSpeed = m_fMinSpeed;
			m_fASpeed = m_fMinASpeed;
		}
	}
}

bool CAI_Rat::bfComputeNewPosition(bool bCanAdjustSpeed, bool bStraightForward)
{
	// saving current parameters
	Fvector tSafeHPB = m_tHPB;
	Fvector tSavedPosition = Position();
	SRotation tSavedTorsoTarget = r_torso_target;
	float fSavedDHeading = m_fDHeading;

	if (bCanAdjustSpeed)
		vfAdjustSpeed();

	if (m_fSpeed < EPS_L)
		return(true);
	
	m_fCurSpeed = m_fSpeed;
	
	// Update position and orientation of the planes
	float fAT = m_fASpeed * m_fTimeUpdateDelta;

	Fvector& tDirection = XFORM().k;

	// Tweak orientation based on last position and goal
	Fvector tOffset;
	tOffset.sub(m_tGoalDir,Position());

	if (!bStraightForward) {
		if (tOffset.y > 1.0) {			// We're too low
			m_tHPB.y += fAT;
			if (m_tHPB.y > 0.8f)	
				m_tHPB.y = 0.8f;
		}
		else 
			if (tOffset.y < -1.0) {	// We're too high
				m_tHPB.y -= fAT;
				if (m_tHPB.y < -0.8f)
					m_tHPB.y = -0.8f;
			}
			else							// Add damping
				m_tHPB.y *= 0.95f;
	}
	
	tDirection.normalize();
	tOffset.normalize	();

	float fDot = tDirection.dotproduct(tOffset);
	float fSafeDot = fDot;

	fDot = (1.0f-fDot)/2.0f * fAT * 10.0f;

	tOffset.crossproduct(tOffset,tDirection);

	if (bStraightForward) {
		if (tOffset.y > 0.01f) {
			if (fSafeDot > .95f)
				m_fDHeading = 0.f;
			else
				if (fSafeDot > 0.75f)
					m_fDHeading = .10f;
			m_fDHeading = ( m_fDHeading * 9.0f + fDot )*0.1f;
		}
		else 
			if (tOffset.y < 0.01f) {
				if (fSafeDot > .95f)
					m_fDHeading = 0.f;
				else
					if (fSafeDot > 0.75f)
						m_fDHeading = -.10f;
				m_fDHeading = (m_fDHeading*9.0f - fDot)*0.1f;
			}
	}
	else {
		if (tOffset.y > 0.01f)		
			m_fDHeading = ( m_fDHeading * 9.0f + fDot )*0.1f;
		else 
			if (tOffset.y < 0.01f)
				m_fDHeading = (m_fDHeading*9.0f - fDot)*0.1f;
	}


	m_tHPB.x  +=  m_fDHeading;

	m_tHPB.x = angle_normalize_signed(m_tHPB.x);
	m_tHPB.y = angle_normalize_signed(m_tHPB.y);

	// Build the local matrix for the pplane
	XFORM().setHPB	(m_tHPB.x,m_tHPB.y,m_tHPB.z);
	Position()		= tSavedPosition;
	Position().mad	(tDirection,m_fSpeed*m_fTimeUpdateDelta);
//	Msg				("[%f][%f][%f]",VPUSH(Position()));
	r_target.yaw	= r_torso_target.yaw = -m_tHPB.x;

//	Fvector tAcceleration;
//	tAcceleration.setHP(-r_torso_current.yaw,-r_torso_current.pitch);
//	tAcceleration.normalize_safe();
//	tAcceleration.mul(m_fSpeed*12.f);
//	Movement.SetPosition(Position());
//	Movement.Calculate	(tAcceleration,0,0,m_fTimeUpdateDelta,false);
//	Movement.GetPosition(Position());

	u32 dwNewNode = AI_NodeID;
	NodeCompressed *tpNewNode = AI_Node;
	NodePosition	QueryPos;
	getAI().PackPosition	(QueryPos,Position());
	if (!AI_NodeID || !getAI().u_InsideNode(*AI_Node,QueryPos)) {
		dwNewNode = getAI().q_Node(AI_NodeID,Position());
		tpNewNode = getAI().Node(dwNewNode);
	}
	if (dwNewNode && getAI().u_InsideNode(*tpNewNode,QueryPos)) {
		Position().y = getAI().ffGetY(*tpNewNode,Position().x,Position().z);
		m_tOldPosition.set(tSavedPosition);
		m_bNoWay		= false;
	}
	else {
		m_fSafeSpeed	= m_fSpeed = EPS_S;
		m_bNoWay		= true;
		m_tHPB			= tSafeHPB;
		XFORM().setHPB	(m_tHPB.x,m_tHPB.y,m_tHPB.z);
		Position()		= tSavedPosition;
		r_torso_target	= tSavedTorsoTarget;
		m_fDHeading		= fSavedDHeading;
	}

	bool m_bResult = false;
	if (!getAI().bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,PI_DIV_8) || m_bNoWay) {
		m_fSpeed = .1f;
		if (m_bNoWay)
			if ((Level().timeServer() - m_dwLastRangeSearch > TIME_TO_RETURN) || (!m_dwLastRangeSearch)) {
				float fAngle = ::Random.randF(m_fWallMinTurnValue,m_fWallMaxTurnValue);
				r_torso_target.yaw = r_torso_current.yaw + fAngle;
				r_torso_target.yaw = angle_normalize(r_torso_target.yaw);
				Fvector tTemp;
				tTemp.setHP(-r_torso_target.yaw,-r_torso_target.pitch);
				tTemp.mul(100.f);
				m_tGoalDir.add(Position(),tTemp);
				m_dwLastRangeSearch = Level().timeServer();
			}
		m_bResult = true;
	}

	return(m_bResult);
}

void CAI_Rat::vfChooseNextGraphPoint()
{
	_GRAPH_ID			tGraphID		= m_tNextGP;
	u16					wNeighbourCount = (u16)getAI().m_tpaGraph[tGraphID].tNeighbourCount;
	CSE_ALifeGraph::SGraphEdge			*tpaEdges		= (CSE_ALifeGraph::SGraphEdge *)((BYTE *)getAI().m_tpaGraph + getAI().m_tpaGraph[tGraphID].dwEdgeOffset);
	
	int					iPointCount		= (int)m_tpaTerrain.size();
	int					iBranches		= 0;
	for (int i=0; i<wNeighbourCount; i++)
		for (int j=0; j<iPointCount; j++)
			if (getAI().bfCheckMask(m_tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes) && (tpaEdges[i].dwVertexNumber != m_tCurGP))
				iBranches++;
	if (!iBranches) {
		for (int i=0; i<wNeighbourCount; i++) {
			for (int j=0; j<iPointCount; j++)
				if (getAI().bfCheckMask(m_tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes)) {
					m_tCurGP	= m_tNextGP;
					m_tNextGP	= (_GRAPH_ID)tpaEdges[i].dwVertexNumber;
					m_dwTimeToChange	= Level().timeServer() + ::Random.randI(m_tpaTerrain[j].dwMinTime,m_tpaTerrain[j].dwMaxTime);
					return;
				}
		}
	}
	else {
		int iChosenBranch = ::Random.randI(0,iBranches);
		iBranches = 0;
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