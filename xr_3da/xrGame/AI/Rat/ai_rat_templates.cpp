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

using namespace NAI_Rat_Constants;

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

void CAI_Rat::vfComputeNewPosition()
{
	// Update position and orientation of the planes
	float fAT = m_fASpeed * m_fTimeUpdateDelta;

	Fvector& tDirection = mRotate.k;

	// Tweak orientation based on last position and goal
	Fvector tOffset;
	tOffset.sub(m_tGoalDir,vPosition);

	// First, tweak the pitch
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

	// Now figure out yaw changes
	tDirection.normalize();
	tOffset.normalize	();

	float fDot = tDirection.dotproduct(tOffset);
	fDot = (1.0f-fDot)/2.0f * fAT * 10.0f;

	tOffset.crossproduct(tOffset,tDirection);

	if (tOffset.y > 0.01f)		
		m_fDHeading = ( m_fDHeading * 9.0f + fDot )*0.1f;
	else 
		if (tOffset.y < 0.01f)
			m_fDHeading = (m_fDHeading*9.0f - fDot)*0.1f;

	m_tHPB.x  +=  m_fDHeading;
	m_tHPB.z  = -m_fDHeading * 9.0f;

	m_tHPB.x = angle_normalize_signed(m_tHPB.x);
	m_tHPB.y = angle_normalize_signed(m_tHPB.y);
	m_tHPB.z = angle_normalize_signed(m_tHPB.z);

	// Build the local matrix for the pplane
	mRotate.setHPB(m_tHPB.x,m_tHPB.y,m_tHPB.z);
	r_torso_target.yaw = -m_tHPB.x;
	UpdateTransform();

	// Update position
	Fvector tTemp;
	tTemp.set(vPosition);
//	if (feel_touch.size() || true) {
//		Fvector tTemp1;
//		tTemp1.set(vPosition);
//		tTemp1.mad(tDirection,m_fSpeed*m_fTimeUpdateDelta);
//		vPosition.set(tfGetNextCollisionPosition(this,tTemp1));
//	}
//	else
		vPosition.mad(tDirection,m_fSpeed*m_fTimeUpdateDelta);
	
	DWORD dwNewNode = AI_NodeID;
	NodeCompressed *tpNewNode = AI_Node;
	NodePosition	QueryPos;
	Level().AI.PackPosition	(QueryPos,vPosition);
	if (!AI_NodeID || !Level().AI.u_InsideNode(*AI_Node,QueryPos)) {
		dwNewNode = Level().AI.q_Node(AI_NodeID,vPosition);
		tpNewNode = Level().AI.Node(dwNewNode);
	}
	if (dwNewNode && Level().AI.u_InsideNode(*tpNewNode,QueryPos)) {
		vPosition.y = Level().AI.ffGetY(*tpNewNode,vPosition.x,vPosition.z);
		m_tOldPosition.set(tTemp);
		m_bNoWay = false;
	}
	else {
		vPosition.set(m_tOldPosition);
		m_fSafeSpeed = m_fSpeed = EPS_S;
		m_bNoWay = true;
	}
}

void CAI_Rat::vfComputeNextDirectionPosition()
{
	float fAT = m_fASpeed * m_fTimeUpdateDelta;

	Fvector& tDirection = mRotate.k;
	
	// Tweak orientation based on last position and goal
	Fvector tOffset;
	tOffset.sub(m_tGoalDir,vPosition);
	float fDistance = tOffset.magnitude();
	
	// First, tweak the pitch
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

	// Now figure out yaw changes
	tDirection.normalize();
	tOffset.normalize	();

	float fDot = tDirection.dotproduct(tOffset);
	float fSafeDot = fDot;

	fDot = (1.0f-fDot)/2.0f * fAT * 10.0f;
	
	tOffset.crossproduct(tOffset,tDirection);

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

	m_tHPB.x  +=  m_fDHeading;
	m_tHPB.z  = -m_fDHeading * 9.0f;

	m_tHPB.x = angle_normalize_signed(m_tHPB.x);
	m_tHPB.y = angle_normalize_signed(m_tHPB.y);
	m_tHPB.z = angle_normalize_signed(m_tHPB.z);

	// Build the local matrix for the pplane
	mRotate.setHPB(m_tHPB.x,m_tHPB.y,m_tHPB.z);
	r_torso_target.yaw = -m_tHPB.x;
	UpdateTransform();
	// Update position
	Fvector tTemp;
	tTemp.set(vPosition);
	vPosition.mad(tDirection,m_fSpeed*m_fTimeUpdateDelta);
	
	DWORD dwNewNode = AI_NodeID;
	NodeCompressed *tpNewNode = AI_Node;
	NodePosition	QueryPos;
	Level().AI.PackPosition	(QueryPos,vPosition);
	if (!AI_NodeID || !Level().AI.u_InsideNode(*AI_Node,QueryPos)) {
		dwNewNode = Level().AI.q_Node(AI_NodeID,vPosition);
		tpNewNode = Level().AI.Node(dwNewNode);
	}
	if (dwNewNode && Level().AI.u_InsideNode(*tpNewNode,QueryPos)) {
		vPosition.y = Level().AI.ffGetY(*tpNewNode,vPosition.x,vPosition.z);
		m_tOldPosition.set(tTemp);
		m_bNoWay = false;
	}
	else {
		vPosition.set(m_tOldPosition);
		m_fSafeSpeed = m_fSpeed = EPS_S;
		m_bNoWay = true;
	}
}