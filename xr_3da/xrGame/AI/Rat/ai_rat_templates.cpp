////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat_templates.cpp
//	Created 	: 23.07.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : Templates for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_rat.h"
#include "../ai_monsters_misc.h"
#include "../../game_graph.h"

void CAI_Rat::vfSaveEnemy()
{
	m_tSavedEnemy = m_Enemy.m_enemy;
	m_tSavedEnemyPosition = m_Enemy.m_enemy->Position();
	m_tpSavedEnemyNode = m_Enemy.m_enemy->level_vertex();
	m_dwSavedEnemyNodeID = m_Enemy.m_enemy->level_vertex_id();
}

void CAI_Rat::vfSetFire(bool bFire, CGroup &/**Group/**/)
{
	if (bFire) {
		m_bFiring = true;
		m_tAction = eRatActionAttackBegin;
	}
	else {
		m_bFiring = false;
		m_tAction = eRatActionAttackEnd;
	}
}

void CAI_Rat::vfSetMovementType(float fSpeed)
{
//	StandUp();
	m_bMoving = _abs(fSpeed) > EPS_L;
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
			m_body.target.yaw = fAngle;
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
				m_body.target.yaw = fAngle;
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
					m_body.target.yaw = fAngle;
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
				m_body.target.yaw = fAngle;
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
	m_bCanAdjustSpeed	= bCanAdjustSpeed;
	m_bStraightForward	= bStraightForward;
	Fvector tSafeHPB = m_tHPB;
	Fvector tSavedPosition = Position();
	SRotation tSavedTorsoTarget = m_body.target;
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
	m_head.target.yaw	= m_body.target.yaw = -m_tHPB.x;

//	Fvector tAcceleration;
//	tAcceleration.setHP(-m_body.current.yaw,-m_body.current.pitch);
//	tAcceleration.normalize_safe();
//	tAcceleration.mul(m_fSpeed*12.f);
//	m_PhysicMovementControl.SetPosition(Position());
//	m_PhysicMovementControl.Calculate	(tAcceleration,0,0,m_fTimeUpdateDelta,false);
//	m_PhysicMovementControl.GetPosition(Position());

	u32 dwNewNode = level_vertex_id();
	const CLevelGraph::CVertex *tpNewNode = level_vertex();
	CLevelGraph::CPosition	QueryPos;
	ai().level_graph().vertex_position	(QueryPos,Position());
	if (!ai().level_graph().valid_vertex_id(dwNewNode) || !ai().level_graph().inside(*level_vertex(),QueryPos)) {
		dwNewNode = ai().level_graph().vertex(level_vertex_id(),Position());
		tpNewNode = ai().level_graph().vertex(dwNewNode);
	}
	if (ai().level_graph().valid_vertex_id(dwNewNode) && ai().level_graph().inside(*tpNewNode,QueryPos)) {
		Position().y = ai().level_graph().vertex_plane_y(*tpNewNode,Position().x,Position().z);
		m_tOldPosition.set(tSavedPosition);
		m_bNoWay		= false;
	}
	else {
		m_fSafeSpeed	= m_fSpeed = EPS_S;
		m_bNoWay		= true;
		m_tHPB			= tSafeHPB;
		XFORM().setHPB	(m_tHPB.x,m_tHPB.y,m_tHPB.z);
		Position()		= tSavedPosition;
		m_body.target	= tSavedTorsoTarget;
		m_fDHeading		= fSavedDHeading;
	}

	bool m_bResult = false;
	if ((angle_difference(m_body.target.yaw, m_body.current.yaw) > PI_DIV_8) || m_bNoWay) {
		m_fSpeed = .1f;
		if (m_bNoWay)
			if ((Level().timeServer() - m_previous_query_time > TIME_TO_RETURN) || (!m_previous_query_time)) {
				float fAngle = ::Random.randF(m_fWallMinTurnValue,m_fWallMaxTurnValue);
				m_body.target.yaw = m_body.current.yaw + fAngle;
				m_body.target.yaw = angle_normalize(m_body.target.yaw);
				Fvector tTemp;
				tTemp.setHP(-m_body.target.yaw,-m_body.target.pitch);
				tTemp.mul(100.f);
				m_tGoalDir.add(Position(),tTemp);
				m_previous_query_time = Level().timeServer();
			}
		m_bResult = true;
	}

	return(m_bResult);
}

void CAI_Rat::vfChooseNextGraphPoint()
{
//	_GRAPH_ID			tGraphID		= m_tNextGP;
//	CGameGraph::const_iterator	i,e;
//	ai().game_graph().begin		(tGraphID,i,e);
//	int					iPointCount		= (int)m_tpaTerrain.size();
//	int					iBranches		= 0;
//	for ( ; i != e; ++i)
//		for (int j=0; j<iPointCount; ++j)
//			if (ai().game_graph().mask(m_tpaTerrain[j].tMask,ai().game_graph().vertex((*i).vertex_id()).vertex_type()) && ((*i).vertex_id() != m_tCurGP))
//				++iBranches;
//	ai().game_graph().begin		(tGraphID,i,e);
//	if (!iBranches) {
//		for ( ; i != e; ++i) {
//			for (int j=0; j<iPointCount; ++j)
//				if (ai().game_graph().mask(m_tpaTerrain[j].tMask,ai().game_graph().vertex((*i).vertex_id()).vertex_type())) {
//					m_tCurGP	= m_tNextGP;
//					m_tNextGP	= (*i).vertex_id();
//					m_dwTimeToChange	= Level().timeServer() + ::Random.randI(m_tpaTerrain[j].dwMinTime,m_tpaTerrain[j].dwMaxTime);
//					return;
//				}
//		}
//	}
//	else {
//		int iChosenBranch = ::Random.randI(0,iBranches);
//		iBranches = 0;
//		for ( ; i != e; ++i) {
//			for (int j=0; j<iPointCount; ++j)
//				if (ai().game_graph().mask(m_tpaTerrain[j].tMask,ai().game_graph().vertex((*i).vertex_id()).vertex_type()) && ((*i).vertex_id() != m_tCurGP)) {
//					if (iBranches == iChosenBranch) {
//						m_tCurGP			= m_tNextGP;
//						m_tNextGP			= (*i).vertex_id();
//						m_dwTimeToChange	= Level().timeServer() + ::Random.randI(m_tpaTerrain[j].dwMinTime,m_tpaTerrain[j].dwMaxTime);
//						return;
//					}
//					++iBranches;
//				}
//		}
//	}
}