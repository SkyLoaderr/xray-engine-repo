////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat.cpp
//	Created 	: 25.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_rat.h"
#include "ai_rat_selectors.h"
#include "..\\ai_monsters_misc.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\hudmanager.h"
#include "..\\..\\..\\xr_trims.h"

#define TORSO_ANGLE_DELTA				(PI/30.f)
//#define PRE_THINK_COUNT					4
#define ATTACK_DISTANCE						.5f

void CAI_Rat::Die()
{
	WRITE_TO_LOG("Dying...");

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
	AI_Path.TravelPath.clear();
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);

	//setEnabled	(false);
	
	if (m_bFiring) {
		AI_Path.Calculate(this,vPosition,vPosition,m_fCurSpeed,.1f);
		float fY = ffGetY(*AI_Node,vPosition.x,vPosition.z);
		if (vPosition.y - fY > 0.01f) {
			Fvector tAcceleration;
			tAcceleration.set(0,m_fJumpSpeed,0);
			Movement.SetPosition(vPosition);
			Movement.Calculate	(tAcceleration,0,0,.1f,false);
			Movement.GetPosition(vPosition);
		}
		else
			vPosition.set(vPosition.x,fY,vPosition.z);
		UpdateTransform();
	}
}

void CAI_Rat::AttackFire()
{
	WRITE_TO_LOG("Attacking enemy...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiRatDie)
	
	SelectEnemy(Enemy);
	
	CHECK_IF_GO_TO_PREV_STATE(!(Enemy.Enemy))
		
	CHECK_IF_GO_TO_NEW_STATE((Enemy.Enemy->Position().distance_to(vPosition) > ATTACK_DISTANCE),aiRatAttackRun)

	Fvector tTemp;
	tTemp.sub(Enemy.Enemy->Position(),vPosition);
	vfNormalizeSafe(tTemp);
	SRotation sTemp;
	mk_rotation(tTemp,sTemp);
	
	CHECK_IF_GO_TO_NEW_STATE(!Level().AI.bfTooSmallAngle(r_torso_current.yaw,sTemp.yaw,PI_DIV_6),aiRatAttackRun)
		
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),Enemy.Enemy->Position());

	AI_Path.TravelPath.clear();
	
	vfSaveEnemy();

	vfAimAtEnemy();
	//SetDirectionLook();

	vfSetFire(true,Group);

	vfSetMovementType(m_cBodyState,0);
}

void CAI_Rat::AttackRun()
{
	WRITE_TO_LOG("Attack enemy");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiRatDie)
	
	SelectEnemy(Enemy);
	
	CHECK_IF_GO_TO_PREV_STATE(!(Enemy.Enemy))
		
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),Enemy.Enemy->Position());
	
	Fvector tTemp;
	tTemp.sub(Enemy.Enemy->Position(),vPosition);
	vfNormalizeSafe(tTemp);
	SRotation sTemp;
	mk_rotation(tTemp,sTemp);

	CHECK_IF_GO_TO_NEW_STATE(Level().AI.bfTooSmallAngle(r_torso_target.yaw, sTemp.yaw,PI_DIV_6) && (Enemy.Enemy->Position().distance_to(vPosition) <= ATTACK_DISTANCE),aiRatAttackFire);

	CHECK_IF_SWITCH_TO_NEW_STATE(!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,PI_DIV_6),aiRatTurn)

	INIT_SQUAD_AND_LEADER;
	
	GoToPointViaSubnodes(Enemy.Enemy->Position());
	
	//vfAimAtEnemy();
	SetDirectionLook();
	
	vfSetFire(false,Group);

	vfSetMovementType(m_cBodyState,m_fMaxSpeed);
}

void CAI_Rat::Turn()
{
	WRITE_TO_LOG("Turning...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiRatDie)

	CHECK_IF_GO_TO_PREV_STATE(Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw, PI_DIV_6))
	
	INIT_SQUAD_AND_LEADER
	
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	vfSetFire(false,Group);

	float fTurnAngle = min(fabsf(r_torso_target.yaw - r_torso_current.yaw), PI_MUL_2 - fabsf(r_torso_target.yaw - r_torso_current.yaw));
	r_torso_speed = 3*fTurnAngle;

	vfSetMovementType(BODY_STATE_STAND,0);
}

void CAI_Rat::FreeHunting()
{
	//WRITE_TO_LOG("Free hunting");
	bStopThinking = true;
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiRatDie)

	SelectEnemy(Enemy);
	
	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiRatAttackFire)

	if(m_fGoalChangeTime<=0){
		m_fGoalChangeTime += m_fGoalChangeDelta+m_fGoalChangeDelta*::Random.randF(-0.5f,0.5f);
		Fvector vP;
		vP.set(m_tSpawnPosition.x,m_tSpawnPosition.y+m_fMinHeight,m_tSpawnPosition.z);
		m_tGoalDir.x = vP.x+m_tVarGoal.x*::Random.randF(-0.5f,0.5f); 
		m_tGoalDir.y = vP.y+m_tVarGoal.y*::Random.randF(-0.5f,0.5f);
		m_tGoalDir.z = vP.z+m_tVarGoal.z*::Random.randF(-0.5f,0.5f);
		
		int iRandom = ::Random.randI(0,3);
		switch (iRandom) {
			case 0 : {
				m_fSpeed = m_fMaxSpeed;
				break;
			}
			case 1 : {
				m_fSpeed = m_fMinSpeed;
				break;
			}
			case 2 : {
				if (::Random.randI(0,4) == 0)
					m_fSpeed = EPS_S;
				break;
			}
		}
		m_fSafeSpeed = m_fSpeed;
	}
	m_fGoalChangeTime -= m_fTimeUpdateDelta;
	//if (fabsf(m_fSpeed) > EPS_L) 
	{
		// Update position and orientation of the planes
		float fAT = m_fASpeed * m_fTimeUpdateDelta;

		Fvector& tDirection = mRotate.k;

		// Tweak orientation based on last position and goal
		Fvector tOffset;
		tOffset.sub(m_tGoalDir,vPosition);

		// First, tweak the pitch
		if( tOffset.y > 1.0){			// We're too low
			m_tHPB.y += fAT;
			if( m_tHPB.y > 0.8f )	m_tHPB.y = 0.8f;
		}else if( tOffset.y < -1.0){	// We're too high
			m_tHPB.y -= fAT;
			if( m_tHPB.y < -0.8f )m_tHPB.y = -0.8f;
		}else							// Add damping
			m_tHPB.y *= 0.95f;

		// Now figure out yaw changes
		tOffset.y           = 0.0f;
		tDirection.y		= 0.0f;

		tDirection.normalize();
		tOffset.normalize	();

		float fDot = tDirection.dotproduct(tOffset);
		fDot = (1.0f-fDot)/2.0f * fAT * 10.0f;

		tOffset.crossproduct(tOffset,tDirection);

		if( tOffset.y > 0.01f )		m_fDHeading = ( m_fDHeading * 9.0f + fDot ) * 0.1f;
		else if( tOffset.y < 0.01f )m_fDHeading = ( m_fDHeading * 9.0f - fDot ) * 0.1f;

		m_tHPB.x  +=  m_fDHeading;
		m_tHPB.z  = -m_fDHeading * 9.0f;

		// Build the local matrix for the pplane
		mRotate.setHPB(m_tHPB.x,m_tHPB.y,m_tHPB.z);

		// Update position
		Fvector tTemp;
		tTemp.set(vPosition);
		vPosition.mad(tDirection,m_fSpeed*m_fTimeUpdateDelta);
		DWORD dwNewNode = AI_NodeID;
		NodeCompressed *tpNewNode = AI_Node;
		NodePosition	QueryPos;
		Level().AI.PackPosition	(QueryPos,vPosition);

		if (!AI_NodeID || !Level().AI.u_InsideNode(*AI_Node,QueryPos)) {
			dwNewNode = Level().AI.q_Node(0,vPosition);
			tpNewNode = Level().AI.Node(dwNewNode);
		}
		if (dwNewNode && Level().AI.u_InsideNode(*tpNewNode,QueryPos)) {
			vPosition.y = ffGetY(*tpNewNode,vPosition.x,vPosition.z);
			m_tOldPosition.set(tTemp);
		}
		else {
			vPosition.set(m_tOldPosition);
			m_fSafeSpeed = m_fSpeed = EPS_S;
		}

		SetDirectionLook();

		if (!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,PI_DIV_8))
			m_fSpeed = EPS_S;
		else 
			if (m_fSafeSpeed != m_fSpeed) {
				int iRandom = ::Random.randI(0,2);
				switch (iRandom) {
					case 0 : {
						m_fSpeed = m_fMaxSpeed;
						break;
					}
					case 1 : {
						m_fSpeed = m_fMinSpeed;
						break;
					}
					case 2 : {
						if (::Random.randI(0,4) == 0)
							m_fSpeed = EPS_S;
						break;
					}
				}
				m_fSafeSpeed = m_fSpeed;
			}

		AI_Path.TravelPath.clear();
		if (fabsf(m_fSpeed) < EPS_L) {
	//		AI_Path.TravelPath.resize(2);
	//		AI_Path.TravelStart = 0;
	//		AI_Path.TravelPath[0].floating = FALSE;
	//		AI_Path.TravelPath[0].P = vPosition;
	//		AI_Path.TravelPath[1].floating = FALSE;
	//		AI_Path.TravelPath[1].P.mad(vPosition,tDirection,10.f);
	//		AI_Path.TravelPath.resize(11);
	//		AI_Path.TravelStart = 0;
	//		AI_Path.TravelPath[0].floating = FALSE;
	//		AI_Path.TravelPath[0].P = vPosition;
	//		for (int i=1; i<11; i++) {
	//			AI_Path.TravelPath[i].floating = FALSE;
	//			AI_Path.TravelPath[i].P.mad(vPosition,tDirection,(float)i);
	//		}
		}
	}
}

void CAI_Rat::Think()
{
	bStopThinking = false;
	do {
		m_ePreviousState = eCurrentState;
		switch(eCurrentState) {
			case aiRatDie : {
				Die();
				break;
			}
			case aiRatFreeHunting : {
				FreeHunting();
				break;
			}
			case aiRatAttackFire : {
				AttackFire();
				break;
			}
			case aiRatAttackRun : {
				AttackRun();
				break;
			}
			case aiRatTurn : {
				Turn();
				break;
			}
		}
		m_bStateChanged = m_ePreviousState != eCurrentState;
	}
	while (!bStopThinking);
}
