////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie_templates.cpp
//	Created 	: 23.07.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : Templates for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_zombie.h"
#include "..\\ai_monsters_misc.h"

void CAI_Zombie::vfSaveEnemy()
{
	m_tSavedEnemy = m_Enemy.Enemy;
	m_tSavedEnemyPosition = m_Enemy.Enemy->Position();
	m_tpSavedEnemyNode = m_Enemy.Enemy->AI_Node;
	m_dwSavedEnemyNodeID = m_Enemy.Enemy->AI_NodeID;
}

void CAI_Zombie::vfSetFire(bool bFire, CGroup &Group)
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

void CAI_Zombie::vfSetMovementType(char cBodyState, float fSpeed)
{
	StandUp();
	m_fSpeed = m_fCurSpeed = fSpeed;
}

void CAI_Zombie::vfAdjustSpeed()
{
	Fvector tTemp1, tTemp2;
	tTemp1.sub(m_tGoalDir,vPosition);
	tTemp1.normalize_safe();
	tTemp2 = mRotate.k;
	tTemp2.normalize_safe();
	float fAngle = tTemp1.dotproduct(tTemp2);
	clamp(fAngle,0.f,.99999f);
	fAngle = acosf(fAngle);
	
	float fMinASpeed = PI_MUL_2, fNullASpeed = PI_MUL_2, fMaxASpeed = .2f, fAttackASpeed = .15f;
	
	if (fabsf(m_fSpeed - m_fMinSpeed) <= EPS_L)	{
		if (fAngle >= 3*PI_DIV_2) {
			m_fSpeed = 0;
			m_fASpeed = fNullASpeed;
			r_torso_target.yaw = fAngle;
		}
		else 
		{
			m_fSpeed = m_fMinSpeed;
			m_fASpeed = fMinASpeed;
		}
	}
	else
		if (fabsf(m_fSpeed - m_fMaxSpeed) <= EPS_L)	{
			if (fAngle >= 3*PI_DIV_2) {
				m_fSpeed = 0;
				m_fASpeed = fNullASpeed;
				r_torso_target.yaw = fAngle;
			}
			else
				if (fAngle >= PI_DIV_2) {
					m_fSpeed = m_fMinSpeed;
					m_fASpeed = fMinASpeed;
				}
				else {
					m_fSpeed = m_fMaxSpeed;
					m_fASpeed = fMaxASpeed;
				}
		}
		else
			if (fabsf(m_fSpeed - m_fAttackSpeed) <= EPS_L)	{
				if (fAngle >= 3*PI_DIV_2) {
					m_fSpeed = 0;
					m_fASpeed = fNullASpeed;
					r_torso_target.yaw = fAngle;
				}
				else
					if (fAngle >= PI_DIV_2) {
						m_fSpeed = m_fMinSpeed;
						m_fASpeed = fMinASpeed;
					}
					else
						if (fAngle >= PI_DIV_4) {
							m_fSpeed = m_fMaxSpeed;
							m_fASpeed = fMaxASpeed;
						}
						else {
							m_fSpeed = m_fAttackSpeed;
							m_fASpeed = fAttackASpeed;
						}
			}
			else {
				r_torso_target.yaw = fAngle;
				m_fSpeed = 0;
				m_fASpeed = fNullASpeed;
			}
	
	tTemp2 = mRotate.k;
	tTemp2.normalize_safe();
	
	tTemp1 = vPosition;
	tTemp1.mad(tTemp2,1*m_fSpeed*m_fTimeUpdateDelta);
	if (bfCheckIfOutsideAIMap(tTemp1)) {
		tTemp1 = vPosition;
		if (fabsf(m_fSpeed - m_fAttackSpeed) < EPS_L) {
			tTemp1.mad(tTemp2,1*m_fMaxSpeed*m_fTimeUpdateDelta);
			if (bfCheckIfOutsideAIMap(tTemp1)) {
				m_fSpeed = m_fMinSpeed;
				m_fASpeed = fMinASpeed;
			}
			else {
				m_fSpeed = m_fMaxSpeed;
				m_fASpeed = fMaxASpeed;
			}
		}
		else 
		{
			m_fSpeed = m_fMinSpeed;
			m_fASpeed = fMinASpeed;
		}
	}
}

bool CAI_Zombie::bfComputeNewPosition(bool bCanAdjustSpeed, bool bStraightForward)
{
	// saving current parameters
	Fvector tSafeHPB = m_tHPB;
	Fvector tSavedPosition = vPosition;
	SRotation tSavedTorsoTarget = r_torso_target;
	float fSavedDHeading = m_fDHeading;

	if (bCanAdjustSpeed)
		vfAdjustSpeed();

	if (m_fSpeed < EPS_L)
		return(true);
	
	m_fCurSpeed = m_fSpeed;
	
	// Update position and orientation of the planes
	float fAT = m_fASpeed * m_fTimeUpdateDelta;

	Fvector& tDirection = mRotate.k;

	// Tweak orientation based on last position and goal
	Fvector tOffset;
	tOffset.sub(m_tGoalDir,vPosition);

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
	mRotate.setHPB(m_tHPB.x,m_tHPB.y,m_tHPB.z);
	r_target.yaw = r_torso_target.yaw = -m_tHPB.x;
	UpdateTransform();

	// Update position
	pCreator->ObjectSpace.TestNearestObject(cfModel, vPosition, 1.f);
	Level().ObjectSpace.GetNearest(vPosition,1.f);
	if (Level().ObjectSpace.q_nearest.size()) {
		Fvector tAcceleration;
		tAcceleration.setHP(-r_torso_current.yaw,-r_torso_current.pitch);
		tAcceleration.normalize_safe();
		tAcceleration.mul(m_fSpeed*12.f);
		Movement.SetPosition(vPosition);
		Movement.Calculate	(tAcceleration,0,0,m_fTimeUpdateDelta,false);
		Movement.GetPosition(vPosition);
	}
	else {
//		if (feel_touch.size() || true) {
//			Fvector tTemp1;
//			tTemp1.set(vPosition);
//			tTemp1.mad(tDirection,m_fSpeed*m_fTimeUpdateDelta);
//			vPosition.set(tfGetNextCollisionPosition(this,tTemp1));
//		}
//		else
			vPosition.mad(tDirection,m_fSpeed*m_fTimeUpdateDelta);
	}


	u32 dwNewNode = AI_NodeID;
	NodeCompressed *tpNewNode = AI_Node;
	NodePosition	QueryPos;
	Level().AI.PackPosition	(QueryPos,vPosition);
	if (!AI_NodeID || !Level().AI.u_InsideNode(*AI_Node,QueryPos)) {
		dwNewNode = Level().AI.q_Node(AI_NodeID,vPosition);
		tpNewNode = Level().AI.Node(dwNewNode);
	}
	if (dwNewNode && Level().AI.u_InsideNode(*tpNewNode,QueryPos)) {
		vPosition.y = Level().AI.ffGetY(*tpNewNode,vPosition.x,vPosition.z);
		m_tOldPosition.set(tSavedPosition);
		m_bNoWay = false;
	}
	else {
		vPosition.set(m_tOldPosition);
		m_fSafeSpeed = m_fSpeed = EPS_S;
		m_bNoWay = true;
		vPosition = tSavedPosition;
		m_tHPB = tSafeHPB;
		r_torso_target = tSavedTorsoTarget;
		m_fDHeading = fSavedDHeading;
		UpdateTransform();
	}

	UpdateTransform();

	bool m_bResult = false;
	if (!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,PI_DIV_8) || m_bNoWay) {
		m_fSpeed = .1f;
		if (m_bNoWay)
			if ((Level().timeServer() - m_dwLastRangeSearch > TIME_TO_RETURN) || (!m_dwLastRangeSearch)) {
				float fAngle = ::Random.randF(m_fWallMinTurnValue,m_fWallMaxTurnValue);
				r_torso_target.yaw = r_torso_current.yaw + fAngle;
				r_torso_target.yaw = angle_normalize(r_torso_target.yaw);
				Fvector tTemp;
				tTemp.setHP(-r_torso_target.yaw,-r_torso_target.pitch);
				tTemp.mul(100.f);
				m_tGoalDir.add(vPosition,tTemp);
				m_dwLastRangeSearch = Level().timeServer();
			}
		m_bResult = true;
	}

	return(m_bResult);
}